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
#include <list>
#include <mutex>

static LONGLONG gFirst = 0;

uint64_t us_ticker_read64(void)
{
	static ULONGLONG cur = 0;
	static bool first_set = false;

	FILETIME fs;
	GetSystemTimeAsFileTime(&fs);
	LARGE_INTEGER li;
	li.HighPart = fs.dwHighDateTime;
	li.LowPart = fs.dwLowDateTime;

	cur = li.QuadPart;
	if (!first_set)
	{
		gFirst = cur;
		first_set = true;
	}

	return (cur - gFirst) / 10;
}

uint32_t us_ticker_read(void)
{
	return (uint32_t)us_ticker_read64(); // ok if wraps around
}

static ticker_event_handler GTickerHandler = NULL;

void us_ticker_set_handler(ticker_event_handler handler)
{
	GTickerHandler = handler;
}

void us_ticker_init(void)
{
	us_ticker_read64();
}

/*void us_ticker_set_interrupt(timestamp_t timestamp)
{
}*/

class WaitableTimerWrapper
{
public:
	WaitableTimerWrapper()
	{
		handle = CreateWaitableTimer(NULL, FALSE, NULL);
		destroyed = false;
	}

	~WaitableTimerWrapper()
	{
		destroyed = true;
		mutex.lock();
		events.clear();
		mutex.unlock();
		CancelWaitableTimer(handle);
		CloseHandle(handle);
	}

	void run_next(bool schedule_only)
	{
		timestamp_t next;
		CancelWaitableTimer(handle);

		for (;;)
		{
			next = 0x7fffffffffffffffUI64;
			timestamp_t now = us_ticker_read64();
			mutex.lock();
			ticker_event_t* to_run = NULL;
			for (auto it = events.begin(); it != events.end(); ++ it)
			{
				if ((*it)->timestamp < next)
				{
					next = (*it)->timestamp;

					if (!schedule_only && (next < now))
					{
						to_run = *it;
						events.erase(it);
						break;
					}
				}
			}
			mutex.unlock();

			if (to_run == NULL)
			{
				LARGE_INTEGER when;
				if (next > now)
				{
					GetSystemTimeAsFileTime((FILETIME*)&when);

					when.QuadPart = -((((LONGLONG)next * 10) + gFirst) - when.QuadPart);
				}
				else
					when.QuadPart = -50;
				SetWaitableTimer(handle, &when, 0, timer_routine, this, TRUE);
				return;
			}
			
			if (GTickerHandler != NULL)
				GTickerHandler(to_run->id);
		}
	}

	void add_timeout(ticker_event_t* obj)
	{
		if (destroyed)
			return;
		CancelWaitableTimer(handle);
		mutex.lock();
		events.push_back(obj);
		mutex.unlock();
		run_next(true);
	}

	void remove_timeout(ticker_event_t* obj)
	{
		if (destroyed)
			return;
		CancelWaitableTimer(handle);
		mutex.lock();
		events.remove(obj);
		mutex.unlock();
		run_next(true);
	}

	static VOID APIENTRY timer_routine(
		_In_opt_ LPVOID lpArgToCompletionRoutine,
		_In_     DWORD dwTimerLowValue,
		_In_     DWORD dwTimerHighValue
		)
	{
		WaitableTimerWrapper* me = (WaitableTimerWrapper*)lpArgToCompletionRoutine;
		UNREFERENCED_PARAMETER(dwTimerLowValue);
		UNREFERENCED_PARAMETER(dwTimerHighValue);
		if (me->destroyed)
			return;
		me->run_next(false);
	}

	bool destroyed;
	std::mutex mutex;
	std::list<ticker_event_t*> events;

	HANDLE handle;
};

static WaitableTimerWrapper& getWaitableTimer()
{
	static WaitableTimerWrapper timer;
	return timer;
}

void us_ticker_insert_event(ticker_event_t *obj, timestamp_t timestamp, uint32_t id)
{
	obj->timestamp = timestamp;
	obj->id = id;
	getWaitableTimer().add_timeout(obj);
}

void us_ticker_remove_event(ticker_event_t *obj)
{
	getWaitableTimer().remove_timeout(obj);
}

