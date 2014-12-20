/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include <Windows.h>
#include "W32Thread.hpp"
#include <vector>

// support just one TextLCD & serial per mbed right now

#include "SIMHW.hpp"
#include "Serial.h"
#include <mbedsimulatorctr/CTRAPI.h>
#include <set>
#include <mutex>

extern int main(void);
bool Win32Thread::SYSTEM_SHUTDOWN = false;

BOOL WINAPI DllMain(HMODULE dll, DWORD reason, DWORD reserved)
{
	UNREFERENCED_PARAMETER(dll);
	UNREFERENCED_PARAMETER(reason);
	UNREFERENCED_PARAMETER(reserved);

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

namespace AESIM
{
	static bool STATE_DESTROYED = false;

	struct GState
	{
		GState();
		~GState() {
			STATE_DESTROYED = true;
		}
		ComponentAPI* api;
		std::set<Win32Thread*> threads;
		std::set<HANDLE> timers;
		std::set<HANDLE> mutexes;
		std::recursive_mutex mutex;
		volatile long running = false;
		osThreadDef_t fake_def;
		mbed::SerialBase *serial = NULL;
	};

	static GState& GET_STATE()
	{
		static GState st;
		return st;
	}
#define STATE GET_STATE()

	struct StateForceLoader
	{
		StateForceLoader()
		{
			GET_STATE();
		}

	} StateForceLoaderInst;

	BOOL sim_run();
	void sim_stop();
	void sim_serial_event(int mode);

	DLLInitStackCanary* findCanary()
	{
		MEMORY_BASIC_INFORMATION mbi;
		char* bottom = (char*)((((ULONG_PTR)_AddressOfReturnAddress()) & ~sizeof(void*)) + sizeof(DLLInitStackCanary));
		if (!VirtualQuery(bottom, &mbi, sizeof(mbi)))
			return NULL;
		char* top = ((char*)mbi.BaseAddress + mbi.RegionSize);
		while (bottom <= top)
		{
			DLLInitStackCanary* ptr = (DLLInitStackCanary*)bottom;
			if (ptr->magic1 == DLLINITSTACKCANARY_MAGIC
				&&  ptr->magic2 == DLLINITSTACKCANARY_MAGIC
				&&  ptr->magic_xor_address1 == (DLLINITSTACKCANARY_MAGIC ^ (ULONG_PTR)&ptr->magic_xor_address1)
				&& ptr->magic_xor_address2 == (DLLINITSTACKCANARY_MAGIC ^ (ULONG_PTR)&ptr->magic_xor_address2)
				)
			{
				return ptr;
			}

			bottom += sizeof(void*);
		}
		return NULL;
	}

	static void sim_main(const void* arg)
	{
		UNREFERENCED_PARAMETER(arg);

		main();
		sim_stop();
	}

	GState::GState()
	{
		// this is very ugly, but we need to make sure we are initialized before static and dynamic initializors
		// execute, otherwise we do not keep the original semantics
		DLLInitStackCanary* canary = findCanary();
		if (canary == NULL)
			return; // Crash

		us_ticker_read(); // init ticker

		AESIM::STATE.api = canary->api;
		canary->cb->stop = AESIM::sim_stop;
		canary->cb->serial_event = AESIM::sim_serial_event;

		memset(&STATE.fake_def, 0, sizeof(STATE.fake_def));
		STATE.fake_def.pthread = sim_main;

		running = true;

		new Win32Thread(&STATE.fake_def, NULL);
	}


	void sim_stop()
	{
		STATE.mutex.lock();
		std::set<Win32Thread*> threads(STATE.threads);
		for (Win32Thread* thread : threads)
		{
			thread->terminate();
		}
		Win32Thread::SYSTEM_SHUTDOWN = true;
		std::set<HANDLE> timers(STATE.timers);
		for (HANDLE timer : timers)
		{
			CancelWaitableTimer(timer);
		}
		STATE.threads.clear();
		STATE.timers.clear();
		STATE.mutex.unlock();
		STATE.running = 0;
	}

	void terminate_sim()
	{
		STATE.api->terminate_sim(STATE.api);
	}

	void trigger_reset()
	{
		STATE.api->trigger_reset(STATE.api);
	}

	void sim_serial_event(int mode)
	{
		// ignore race
		if (STATE.serial != NULL)
		{
			STATE.serial->on_event((mbed::SerialBase::IrqType)mode);
		}
	}

	void register_thread(Win32Thread* thread)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		STATE.threads.insert(thread);
		STATE.mutex.unlock();
	}

	void unregister_thread(Win32Thread* thread)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		STATE.threads.erase(thread);
		delete thread;
		STATE.mutex.unlock();
	}

	void register_waitable_timer(HANDLE timer)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		STATE.timers.insert(timer);
		STATE.mutex.unlock();
	}

	void unregister_waitable_timer(HANDLE timer)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		CancelWaitableTimer(timer);
		CloseHandle(timer);
		STATE.timers.erase(timer);
		STATE.mutex.unlock();
	}

	void register_mutex(HANDLE mutex)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		STATE.mutexes.insert(mutex);
		STATE.mutex.unlock();
	}

	void unregister_mutex(HANDLE mutex)
	{
		if (STATE_DESTROYED)
			return;
		STATE.mutex.lock();
		CloseHandle(mutex);
		STATE.mutexes.erase(mutex);
		STATE.mutex.unlock();
	}

	int serial_getc(mbed::SerialBase* s)
	{
		UNREFERENCED_PARAMETER(s);

		return STATE.api->serial_getc(STATE.api);
	}

	void serial_putc(mbed::SerialBase* s, int c)
	{
		UNREFERENCED_PARAMETER(s);
		STATE.api->serial_putc(STATE.api, c);
	}

	int serial_readable(mbed::SerialBase* s)
	{
		UNREFERENCED_PARAMETER(s);
		return STATE.api->serial_readable(STATE.api);
	}

	void serial_attach(mbed::SerialBase* s, int mode)
	{
		STATE.serial = s;
		STATE.api->serial_attach(STATE.api, mode);
	}

	void serial_detach(mbed::SerialBase* s)
	{
		UNREFERENCED_PARAMETER(s);
		STATE.api->serial_detach(STATE.api);
		STATE.serial = NULL;
	}

	void serial_send_break(mbed::SerialBase* s)
	{
		UNREFERENCED_PARAMETER(s);
		STATE.api->serial_send_break(STATE.api);
	}

	void textlcd_init(TextLCD_Base* text, int cols, int rows)
	{
		UNREFERENCED_PARAMETER(text);
		STATE.api->textlcd_init(STATE.api, cols, rows);
	}

	void textlcd_locate(TextLCD_Base* text, int col, int row)
	{
		UNREFERENCED_PARAMETER(text);
		STATE.api->textlcd_locate(STATE.api, col, row);
	}

	void textlcd_cls(TextLCD_Base* text)
	{
		UNREFERENCED_PARAMETER(text);
		STATE.api->textlcd_cls(STATE.api);
	}

	void textlcd_putc(TextLCD_Base* text, int c)
	{
		UNREFERENCED_PARAMETER(text);
		STATE.api->textlcd_putc(STATE.api, c);
	}

	void gpio_write(PinName pin, bool is_out, int value)
	{
		STATE.api->gpio_write(STATE.api, pin, is_out, value);
	}

	int gpio_read(PinName pin, bool is_out)
	{
		return STATE.api->gpio_read(STATE.api, pin, is_out);
	}

	void gpio_irq_connect(uint32_t id, PinName pin, gpio_irq_handler handler)
	{
		STATE.api->gpio_irq_connect(STATE.api, id, pin, (AESIM::gpio_irq_handler)handler);
	}

	void gpio_irq_disconnect(uint32_t id)
	{
		STATE.api->gpio_irq_disconnect(STATE.api, id);
	}

	void gpio_irq_set(uint32_t id, int evt, bool enable)
	{
		STATE.api->gpio_irq_set(STATE.api, id, evt, enable);
	}

	void gpio_irq_enable(uint32_t id, bool enable)
	{
		STATE.api->gpio_irq_enable(STATE.api, id, enable);
	}

	///////////////////////////////////////


}
