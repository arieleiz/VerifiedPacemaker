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
#include "W32Thread.hpp"
#include <stdarg.h>
#include <stdlib.h>
#include <thread>

void error(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buf[1024];
	int amount = vsprintf_s(buf, _countof(buf), format, ap);
	for (int i = 0; i < amount; ++i)
	{
		AESIM::serial_putc(NULL, buf[i]);
	}
	AESIM::terminate_sim();
}

void NVIC_SystemReset(void)
{
	AESIM::trigger_reset();
}

DWORD __alertable_wait(HANDLE handle, uint32_t millisec)
{
	LARGE_INTEGER eta, now;
	GetSystemTimeAsFileTime((FILETIME*)&eta);
	eta.QuadPart += (millisec * 10000UI64);
	for (;;)
	{
		DWORD status = WaitForSingleObjectEx(handle, millisec, TRUE);
		if (status != WAIT_IO_COMPLETION)
			return status;

		if (millisec != INFINITE && millisec != 0)
		{
			GetSystemTimeAsFileTime((FILETIME*)&now);
			LONGLONG diff = (eta.QuadPart - now.QuadPart) / 10000;
			if (diff < 0) // very very very very important this is < and not <=!
				return WAIT_TIMEOUT;
			millisec = (uint32_t)diff;
		}
	}
}

osThreadId osThreadCreate(osThreadDef_t *thread_def, void *argument)
{
	return (osThreadId)(new Win32Thread(thread_def, argument));

}

/// Return the thread ID of the current running thread.
/// \return thread ID for reference by other functions or NULL in case of error.
/// \note MUST REMAIN UNCHANGED: \b osThreadGetId shall be consistent in every CMSIS-RTOS.
osThreadId osThreadGetId(void)
{
	return (osThreadId)TlsGetValue(Win32Thread::getThreadTlsId());
};

/// Terminate execution of a thread and remove it from Active Threads.
/// \param[in]     thread_id   thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osThreadTerminate shall be consistent in every CMSIS-RTOS.
osStatus osThreadTerminate(osThreadId thread_id)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	thread->terminate();

	return osOK;
}

/// Pass control to next thread that is in state \b READY.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osThreadYield shall be consistent in every CMSIS-RTOS.
osStatus osThreadYield(void)
{
	Sleep(0);
	return osOK;
}


/// Change priority of an active thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
/// \param[in]     priority      new priority value for the thread function.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osThreadSetPriority shall be consistent in every CMSIS-RTOS.
osStatus osThreadSetPriority(osThreadId thread_id, osPriority priority)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	thread->set_priority(priority);
	return osOK;
}


/// Get current priority of an active thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
/// \return current priority value of the thread function.
/// \note MUST REMAIN UNCHANGED: \b osThreadGetPriority shall be consistent in every CMSIS-RTOS.
osPriority osThreadGetPriority(osThreadId thread_id)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	return thread->get_priority();

}

//  ==== Generic Wait Functions ====

/// Wait for Timeout (Time Delay).
/// \param[in]     millisec      time delay value
/// \return status code that indicates the execution status of the function.
osStatus osDelay(uint32_t millisec)
{
	Sleep(millisec);
	return osOK;
}

int32_t osSignalSet(osThreadId thread_id, int32_t signals)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	return thread->signal_set(signals);
}


/// Clear the specified Signal Flags of an active thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
/// \param[in]     signals       specifies the signal flags of the thread that shall be cleared.
/// \return previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters.
/// \note MUST REMAIN UNCHANGED: \b osSignalClear shall be consistent in every CMSIS-RTOS.
int32_t osSignalClear(osThreadId thread_id, int32_t signals)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	return thread->signal_clear(signals);
}


/// Get Signal Flags status of an active thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
/// \return previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters.
/// \note MUST REMAIN UNCHANGED: \b osSignalGet shall be consistent in every CMSIS-RTOS.
int32_t osSignalGet(osThreadId thread_id)
{
	Win32Thread* thread = (Win32Thread*)thread_id;
	return thread->signal_get();
}


/// Wait for one or more Signal Flags to become signaled for the current \b RUNNING thread.
/// \param[in]     signals       wait until all specified signal flags set or 0 for any single signal flag.
/// \param[in]     millisec      timeout value or 0 in case of no time-out.
/// \return event flag information or error code.
/// \note MUST REMAIN UNCHANGED: \b osSignalWait shall be consistent in every CMSIS-RTOS.
os_InRegs osEvent osSignalWait(int32_t signals, uint32_t millisec)
{
	return Win32Thread::signal_wait(signals, millisec);
}

void wait(float s)
{
	int ms = (int)(s * 1000);
	Sleep(ms);
}

void wait_ms(int ms)
{
	Sleep(ms);

}

void wait_us(int us)
{
	Sleep(us / 1000);
}


osMutexId osMutexCreate(osMutexDef_t *mutex_def)
{
	UNREFERENCED_PARAMETER(mutex_def);
	HANDLE hnd = CreateMutex(NULL, FALSE, NULL);
	AESIM::register_mutex(hnd);
	return  (osMutexId)hnd;
}

osStatus osMutexWait(osMutexId mutex_id, uint32_t millisec)
{
	DWORD status = __alertable_wait((HANDLE)mutex_id, millisec);
	switch (status)
	{
	case WAIT_OBJECT_0:
		return osOK;

	case WAIT_TIMEOUT:
		return osEventTimeout;
	}
	return osErrorOS;

}

osStatus osMutexRelease(osMutexId mutex_id)
{
	ReleaseMutex((HANDLE)mutex_id);
	return osOK;
}

/// Delete a Mutex that was created by \ref osMutexCreate.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexCreate.
/// \return status code that indicates the execution status of the function.
/// \note MUST REMAIN UNCHANGED: \b osMutexDelete shall be consistent in every CMSIS-RTOS.
osStatus osMutexDelete(osMutexId mutex_id)
{
	AESIM::unregister_mutex((HANDLE)mutex_id);
	return osOK;
}