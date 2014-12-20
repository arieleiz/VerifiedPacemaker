/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include "mbed.h"
#include "rtos.h"
#include <mutex>
#include "Thread.h"
#include "simhw.hpp"

extern DWORD __alertable_wait(HANDLE handle, uint32_t millisec);

class Win32Thread {
public:
	static bool SYSTEM_SHUTDOWN;

	static int getThreadTlsId()
	{
		static std::mutex mtx;
		static int _tls = -1;
		if (_tls == -1)
		{
			mtx.lock();
			if (_tls == -1)
				_tls = TlsAlloc();
			mtx.unlock();
		}
		return _tls;
	}

	Win32Thread(osThreadDef_t *thread_def, const void* arg)
	{
		_thread_def = thread_def;

		_task = thread_def->pthread;
		_thread_def->tcb.state = Thread::Ready;
		_argument = arg;
		_event = CreateEvent(NULL, FALSE, FALSE, NULL);
		_mutex = CreateMutex(NULL, FALSE, NULL);
		_signals_set = _signals_wait = 0;
		_wait_any = false;
		AESIM::register_thread(this);
		_thread = CreateThread(NULL, 0, thread_main, this, 0, NULL);
	}

	osStatus terminate() {
		if (SYSTEM_SHUTDOWN)
			return osOK;

		HANDLE thread = InterlockedExchangePointer(&_thread, NULL);
		if (thread != NULL)
		{
			_thread_def->tcb.state = Thread::Inactive;
			TerminateThread(thread, 0);
			AESIM::unregister_thread(this); // this order is important
			CloseHandle(thread);
		}
		return osOK;
	}

	osStatus set_priority(osPriority priority) { SetThreadPriority(_thread, priority); return osOK; }
	osPriority get_priority() { return (osPriority)GetThreadPriority(_thread); }
	int32_t signal_set(int32_t signals)
	{
		WaitForSingleObject(_mutex, INFINITE);
		int old = _signals_set;
		_signals_set |= signals;
		if ((_signals_set & _signals_wait) == _signals_wait)
			SetEvent(_event);
		ReleaseMutex(_mutex);
		return old;
	}

	int32_t signal_clear(int32_t signals)
	{
		WaitForSingleObject(_mutex, INFINITE);
		int old = _signals_set;
		_signals_set &= ~signals;
		ReleaseMutex(_mutex);
		return old;
	}

	int32_t signal_get()
	{
		WaitForSingleObject(_mutex, INFINITE);
		int old = _signals_set;
		ReleaseMutex(_mutex);
		return old;
	}

		static osEvent signal_wait(int32_t signals, uint32_t millisec = osWaitForever)
		{
			Win32Thread* thread = (Win32Thread*)TlsGetValue(getThreadTlsId());
			if (thread == NULL)
			{
				osEvent res;
				memset(&res, 0, sizeof(res));
				res.status = osErrorOS;
				return res;
			}

			return thread->signal_wait_imp(signals, millisec);
		}

		static osStatus wait(uint32_t millisec)
		{
			Sleep(millisec);
			return osOK;
		}

		static osStatus yield()
		{
			Sleep(0);
			return osOK;
		}

		virtual ~Win32Thread()
		{
			CloseHandle(_event);
			CloseHandle(_mutex);
			terminate();
		}

private:
	osEvent signal_wait_imp(int32_t signals, uint32_t millisec)
	{
		WaitForSingleObject(_mutex, INFINITE);
		if (signals == 0)
		{
			DWORD set;
			if (_BitScanForward(&set, _signals_set))
			{
				_signals_set &= ~(DWORD)(1 << set);
				ReleaseMutex(_mutex);
				return wait_success(1 << set);
			}
			else
			{
				_wait_any = true;
			}
		}
		else
		{
			DWORD already_set = _signals_set & signals;
			signals &= ~already_set;
			_signals_set &= ~already_set;
			if (signals == 0)
			{
				ReleaseMutex(_mutex);
				return wait_success(already_set);
			}
			_signals_wait = signals;
			_wait_any = false;
		}
		ReleaseMutex(_mutex);

		DWORD set = 0;
		for (;;)
		{
			DWORD status = __alertable_wait(_event, millisec);
			if (status == WAIT_TIMEOUT)
			{
				osEvent res;
				memset(&res, 0, sizeof(res));
				res.status = osEventTimeout;
				return res;
			}
			WaitForSingleObject(_mutex, INFINITE);
			DWORD already_set = _signals_set & _signals_wait;
			_signals_wait &= ~already_set;
			_signals_set &= ~already_set;
			set |= already_set;
			bool done = _signals_wait == 0;
			ReleaseMutex(_mutex);
			if (done)
				break;
		}

		return wait_success(set);
	}

	static osEvent wait_success(DWORD signals)
	{
		osEvent res;
		memset(&res, 0, sizeof(res));
		res.status = osOK;
		res.value.signals = signals;
		return res;
	}

	static DWORD WINAPI thread_main(void* arg)
	{
		try
		{
			Win32Thread* thread = (Win32Thread*)arg;
			TlsSetValue(getThreadTlsId(), thread);
			SetThreadPriority(GetCurrentThread(), thread->_thread_def->tpriority);

			thread->_thread_def->tcb.state = Thread::Running;
			thread->_task(thread->_argument);
			thread->_thread_def->tcb.state = Thread::Inactive;
			TlsSetValue(getThreadTlsId(), NULL);
		}
		catch (...)
		{
			error("*** SEH Exception caught ***\n");
		}
		return 0;
	}
private:
	osThreadDef_t *_thread_def;

	void(*_task)(void const *argument);
	void const *_argument;
	HANDLE _thread;
	HANDLE _event;
	HANDLE _mutex;
	DWORD _signals_set, _signals_wait;
	bool _wait_any;
};

