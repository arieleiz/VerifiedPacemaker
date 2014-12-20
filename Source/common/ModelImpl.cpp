/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include "ModelImpl.h"

#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#define DISPATCHER (DispatcherPrivate::INSTANCE)

#ifdef DISPATCH_DEBUG
#define TRACE(str)	do { if(DISPATCHER._dbgout != NULL) {DISPATCHER._dbglock->lock(); DISPATCHER._dbgout->printf str ; DISPATCHER._dbglock->unlock(); } } while(0)
#else
#define TRACE(str)	do {} while(0)
#endif

#define SIGNAL_RUN_TASKS	0x00000001
#define SIGNAL_CHANGED		0x00000001

#ifdef _WIN64
#error This code cannot be compiled in x64 mode since pointers are used as 32-bit callback IDs (fixable, but no reason)
#endif

namespace ModelImpl
{
	DispatcherPrivate DispatcherPrivate::INSTANCE;
	extern volatile PROCESS_ENTRY_STATE** PROCESS_LIST;
	extern int MAX_PROCESS_LIST;
	extern SYNCHRONIZATION_CHANNEL** ALL_CHANNELS;

	DispatcherPrivate::DispatcherPrivate()
	{
		_cnt_proc = 0;
		_tick = 0;
	}

	void DispatcherPrivate::set_clock_multiplier(int multiplier)
	{
		_clock_multiplier = multiplier;
	}

#ifdef DISPATCH_DEBUG
	void DispatcherPrivate::init_debug(Serial* serial, Mutex* dbgmutex)
	{
		_dbgout = serial;
		_dbglock = dbgmutex;
		TRACE(("Dispatcher initializing.\r\n"));
	}
#endif


	void DispatcherPrivate::add_process(PROCESS_ENTRY_STATE* ps)
	{
		if (_cnt_proc >= MAX_PROCESS_LIST)
		{
			error("Too many processes!\r\n");
			return;
		}
		PROCESS_LIST[_cnt_proc] = ps;
		++_cnt_proc;
			
	}

	void DispatcherPrivate::ticker_handler()
	{
		++ DISPATCHER._tick;
	}

	void DispatcherPrivate::channel_irq_handler(uint32_t id, gpio_irq_event event)
	{
		SYNCHRONIZATION_CHANNEL* channel = (SYNCHRONIZATION_CHANNEL*)id;
		switch (channel->data->state->recv.mode)
		{
		case ReceiveChannelModeNone:
			break;
			
		case ReceiveChannelInterruptRise:
			if (event == IRQ_RISE)
				INSTANCE.fire_channel(channel);
			break;
		case ReceiveChannelInterruptFall:
			if (event == IRQ_FALL)
				INSTANCE.fire_channel(channel);
			break;
		}
	}

	void DispatcherPrivate::fire_channel(SYNCHRONIZATION_CHANNEL* channel)
	{
		channel->data->state->fired = true;
		for (int i = 0; i < MAX_SEND_DATA; ++i)
		{
			SendData* sd = &channel->data->state->send[i];
			if (sd->pin == NC)
				continue;
			switch (sd->mode)
			{
			case SendChannelModeNone:
				break;
			case SendChannelModeSet:
				gpio_write(&sd->gpio, 1);
				break;
			case SendChannelModeToggle:
				sd->last_write = 1 - sd->last_write;
				gpio_write(&sd->gpio, sd->last_write);
				break;
			case SendChannelModeReset:
				gpio_write(&sd->gpio, 0);
				break;
			case SendChannelModePulseUp:
				gpio_write(&sd->gpio, 1);
				sd->insert(us_ticker_read() + sd->pulse_length_us);
				break;
			case SendChannelModePulseDown:
				gpio_write(&sd->gpio, 0);
				sd->insert(us_ticker_read() + sd->pulse_length_us);
				break;
			}
		}
		if (channel->fired != NULL)
			channel->fired(channel);
		osSignalSet(_dispid, SIGNAL_CHANGED);
	}

	void DispatcherPrivate::set_channel_fire_cb(SYNCHRONIZATION_CHANNEL* channel, void(*func)(SYNCHRONIZATION_CHANNEL* me), void* context)
	{
		channel->fired = func;
		channel->context = context;
	}

	void DispatcherPrivate::set_channel_action(SYNCHRONIZATION_CHANNEL* channel, PinName pin, SendChannelMode mode, int pulse_length_us)
	{
		for (int i = 0; i < MAX_SEND_DATA; ++i)
		{
			SendData* sd = &channel->data->state->send[i];
			if (sd->pin != NC && sd->pin != pin)
				continue;

			gpio_init_out(&sd->gpio, pin);
			sd->mode = mode;
			sd->pulse_length_us = pulse_length_us;
			sd->pin = pin;
		}
	}

	void DispatcherPrivate::disable_channel_actions(SYNCHRONIZATION_CHANNEL* channel)
	{
		for(int i = 0; i < MAX_SEND_DATA; ++ i)
			channel->data->state->send[i].mode = SendChannelModeNone;
	}

	void DispatcherPrivate::set_receive_input(SYNCHRONIZATION_CHANNEL* channel, PinName pin, PinMode pull, ReceiveChannelMode rcmode)
	{
		if (channel->data->state->recv.mode != ReceiveChannelModeNone)
			gpio_irq_free(&channel->data->state->recv.gpio_irq);

		channel->data->state->recv.mode = rcmode;
		gpio_init_in(&channel->data->state->recv.gpio, pin);
		gpio_irq_init(&channel->data->state->recv.gpio_irq, pin, (&DispatcherPrivate::channel_irq_handler), (uint32_t)channel);
		gpio_mode(&channel->data->state->recv.gpio, pull);
		gpio_irq_event evt = IRQ_NONE;
		switch (rcmode)
		{
		case ReceiveChannelInterruptRise:
			evt = IRQ_RISE;
			break;

		case ReceiveChannelInterruptFall:
			evt = IRQ_FALL;
			break;

		case ReceiveChannelModeNone:
			break;
		}

		if (evt != IRQ_NONE)
		{
			gpio_irq_set(&channel->data->state->recv.gpio_irq, evt, 1);
			gpio_irq_enable(&channel->data->state->recv.gpio_irq);
		}

	}

	void DispatcherPrivate::set_state_enter_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context)
	{
		state->state_enter = func;
		state->context = context;
	}

	void DispatcherPrivate::set_state_leave_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context)
	{
		state->state_leave= func;
		state->context = context;
	}

	void DispatcherPrivate::set_state_pre_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context)
	{
		transition->transition_pre_execute = func;
		transition->context = context;
	}

	void DispatcherPrivate::set_state_post_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context)
	{
		transition->transition_post_execute = func;
		transition->context = context;
	}

	void DispatcherPrivate::set_process_start_cb(PROCESS_ENTRY* entry, void(*func)(PROCESS_ENTRY* me), void* context)
	{
		entry->process_start = func;
		entry->context = context;
	}

	void DispatcherPrivate::run()
	{
		// we need the stack space, this is the only way to get it
		_dispid = osThreadGetId();
		_ticker.attach_us(ticker_handler, _clock_multiplier);
		_wall.start();
		TRACE(("Dispatcher starting all threads.\r\n"));
		for (int i = 0; i < _cnt_proc; ++i)
			PROCESS_LIST[i]->thread->thread.start();

		TRACE(("Dispatcher starting.\r\n"));
		for (;;)
		{
			run_steps();
		}
	}

	void DispatcherPrivate::run_steps()
	{
		int now = _wall.read_us();
		TRACE(("run_steps(): starting, now=%i.\r\n", now));

		clear_signals();
		clear_normal_step();
		run_tasks(StateCommited);
#ifdef DISPATCH_DEBUG
		int tick_before = get_tick();
#endif

		// no commited nodes left, see if we have enough time for a normal loop
		for(;;)
		{
			int tdiff = wall_diff(now);
			if(tdiff >= _clock_multiplier)
				break;

			run_tasks(StateNormal | StateUrgent);
	
			DelayedThread::signal_wait(SIGNAL_CHANGED, (_clock_multiplier - tdiff) / 1000);
		}

		run_tasks(StateUrgent);

		TRACE(("Dispatcher wait end (diff=%i, tdiff=%i).\r\n", wall_diff(now), get_tick() - tick_before));
	}

	void DispatcherPrivate::clear_signals()
	{
		for (int i = 0; ALL_CHANNELS[i] != NULL; ++i)
		{
			// XXX am I sure?
			ALL_CHANNELS[i]->data->state->prev_fired = ALL_CHANNELS[i]->data->state->fired;
			ALL_CHANNELS[i]->data->state->fired = false;
		}
	}

	void DispatcherPrivate::clear_normal_step()
	{
		for (int i = 0; i < _cnt_proc; ++i)
		{
			PROCESS_LIST[i]->thread->mutex.lock();
			PROCESS_LIST[i]->normal_stepped = false;
			PROCESS_LIST[i]->thread->mutex.unlock();
		}
	}

	void DispatcherPrivate::run_tasks(int types)
	{
		int prevmode = types;
		for (;;)
		{
			//TRACE(("DispatcherPrivate::run_tasks(): running types=%i loop.\r\n", types));
			bool anyfound = false;
			bool restartloop = false;
			for (int i = 0; i < _cnt_proc; ++i)
			{
				STATE_ENTRY* se = PROCESS_LIST[i]->current;
				if (se == NULL)
					continue;

				if ((types != StateCommited) && se->data->mode == StateCommited)
				{
					// found commited step in middle of urgent/normal loop
					types = StateCommited;
					restartloop = true;
					break;
				}

				if (!PROCESS_LIST[i]->thread->mutex.trylock())
				{
					//TRACE(("DispatcherPrivate::run_tasks(): could not lock mutex of to %s\r\n", PROCESS_LIST[i]->data->name));
					anyfound = true;
					continue;
				}

				if ((PROCESS_LIST[i]->current->data->mode & types) != 0)
				{
					if (PROCESS_LIST[i]->current->data->mode != StateNormal
						|| !PROCESS_LIST[i]->normal_stepped)
					{
						// we grabbed the lock, means its not running
						anyfound = true;
						//TRACE(("DispatcherPrivate::run_tasks(): sending signal to %s\r\n", PROCESS_LIST[i]->data->name));
						PROCESS_LIST[i]->thread->thread.signal_set(SIGNAL_RUN_TASKS);
					}
				}
				PROCESS_LIST[i]->thread->mutex.unlock();
			}
			if (restartloop)
			{
				TRACE(("DispatcherPrivate::run_tasks(): found commited during normal loop, restarting in commited mode.\r\n"));
				continue;
			}

			if (!anyfound)
			{
				if (prevmode != types)
				{
					TRACE(("DispatcherPrivate::run_tasks(): done processing new commited nodes, resuming with normal/urgent\r\n"));
					types = prevmode;
					continue;
				}

				//TRACE(("DispatcherPrivate::run_tasks(): did not find any process with types=%i.\r\n", types));
				break;
			}

			//TRACE(("DispatcherPrivate::run_tasks(): waiting\r\n"));
			DelayedThread::signal_wait(SIGNAL_CHANGED);

			bool changed = false;
			for (int i = 0; i < _cnt_proc; ++i)
			{
				if (PROCESS_LIST[i]->thread->mutex.trylock())
				{
					if (PROCESS_LIST[i]->current != NULL
						&& PROCESS_LIST[i]->current->data->mode == StateCommited
						&& PROCESS_LIST[i]->stepped)
					{
						// we grabbed the lock, means it stopped running
						changed = true;
					}
					PROCESS_LIST[i]->thread->mutex.unlock();
				}
				else
				{
					//TRACE(("DispatcherPrivate::run_tasks(): could not lock mutex of %s\n", PROCESS_LIST[i]->data->name));
					changed = true; // still running
				}
			}
			if (!changed)
			{
				if (types == StateCommited)
				{
					TRACE(("DispatcherPrivate::run_steps(): nothing changed in commited loop!.\r\n"));
				}
				else
				{
					//TRACE(("DispatcherPrivate::run_steps(): nothing changed, done with step!.\r\n"));
					return;
				}
			}
		}
	}

	void DispatcherPrivate::task_main(const void* arg)
	{
		INSTANCE.task_main_imp((volatile PROCESS_ENTRY_STATE*)arg);
	}

	void DispatcherPrivate::task_main_imp(volatile PROCESS_ENTRY_STATE* process)
	{
		const PROCESS_ENTRY_DATA* pdata = process->data;
		PROCESS_ENTRY* pcb = process->process;

		TRACE(("[%s] task_main_imp(): starting\r\n", process->data->name));
		process->thread->mutex.lock();
		if (pcb->process_start != NULL)
			pcb->process_start(pcb);

		STATE_ENTRY* state = pdata->initstate;
		enter_state(process, state);
		process->state = ProcessRunning;
		process->thread->mutex.unlock();
		osSignalSet(_dispid, SIGNAL_CHANGED);

		for (;;)
		{
			//TRACE(("[%s] task_main_imp(): waiting for signal.\r\n", process->data->name));
			process->state = ProcessWaiting;
			DelayedThread::signal_wait(SIGNAL_RUN_TASKS);
			process->state = ProcessRunning;
			//TRACE(("[%s] task_main_imp(): woken up.\r\n", process->data->name));

			process->thread->mutex.lock();
			//TRACE(("DispatcherPrivate::run_tasks(): locked\r\n"));

			do
			{
				//TRACE(("[%s:%s] task_main_imp(): current state type is %i.\r\n", process->data->name, process->current->data->name, process->current->data->mode));
				TRANSITION_ENTRY* transition = find_valid_transition(process);
				if (transition != NULL)
				{
					take_transition(process, transition);
				}
				else
				{
					process->stepped = false;
					//TRACE(("[%s:%s] task_main_imp(): no transitions found.\r\n", process->data->name, process->current->data->name));
					break;
				}

			} while (process->current->data->mode == StateCommited);

			//TRACE(("DispatcherPrivate::run_tasks(): unlocking\r\n"));
			process->thread->mutex.unlock();
			osSignalSet(_dispid, SIGNAL_CHANGED);
		}

	}

	TRANSITION_ENTRY* DispatcherPrivate::find_valid_transition(volatile PROCESS_ENTRY_STATE* process)
	{
		STATE_ENTRY* state = process->current;
		TRANSITION_ENTRY* const* transitions = state->data->transitions;

		if (transitions == NULL)
			return NULL;

		for (int i = 0; transitions[i] != NULL; ++i)
		{
			if (can_take_transition(process, transitions[i]))
			{
				return transitions[i];
			}
		}

		return NULL;
	}

	bool DispatcherPrivate::can_take_transition(volatile PROCESS_ENTRY_STATE* /*process*/, TRANSITION_ENTRY* transition)
	{
		if (transition->data->receive != NULL)
		{
			SYNCHRONIZATION_CHANNEL* channel = transition->data->receive;
			if (!(channel->data->state->fired || channel->data->state->prev_fired))
 				return false;
		}
		if (transition->data->guard != NULL)
		{
			_lock.lock();
			bool res = transition->data->guard();
			_lock.unlock();
			if (!res)
				return false;
		}

		return true;
	}

	void DispatcherPrivate::take_transition(volatile PROCESS_ENTRY_STATE* process, TRANSITION_ENTRY* transition)
	{
		if (process->current->data->mode == StateNormal)
			process->normal_stepped = true;
		process->stepped = true;

		TRACE(("[%s:%s!>%s] take_transition(): before\r\n", process->data->name, process->current->data->name, transition->data->name));
		leave_state(process, process->current);
		
		if (transition->transition_pre_execute != NULL)
			transition->transition_pre_execute(transition);
		
		if (transition->data->update != NULL)
		{
			_lock.lock();
			transition->data->update();
			_lock.unlock();
		}

		if (transition->data->receive != NULL)
		{
			SYNCHRONIZATION_CHANNEL* channel = transition->data->receive;
			if (!channel->data->broadcast)
				channel->data->state->fired = false;
		}

		if (transition->data->send != NULL)
			fire_channel(transition->data->send);

		if (transition->transition_post_execute != NULL)
			transition->transition_post_execute(transition);
		
		enter_state(process, transition->data->target);

		TRACE(("[%s:%s!<%s] take_transition(): after\r\n", process->data->name, process->current->data->name, transition->data->name));
	}

	void DispatcherPrivate::enter_state(volatile PROCESS_ENTRY_STATE* process, STATE_ENTRY* state)
	{
		process->current = state;
		TRACE(("[%s.>%s] entering_state(): entering\r\n", process->data->name, state->data->name));
		if (state->state_enter != NULL)
			state->state_enter(state);
	}

	void DispatcherPrivate::leave_state(volatile PROCESS_ENTRY_STATE* process, STATE_ENTRY* state)
	{
		process->current = NULL;
		TRACE(("[%s.<%s] leaving_state(): leaving\r\n", process->data->name, state->data->name));
		if (state->state_leave != NULL)
			state->state_leave(state);
	}

	int DispatcherPrivate::add_time(int timecur, int timespan)
	{
		int tdiff = 0x7fffffff - timecur;
		if (tdiff >= timespan)
			return timecur + timespan;

		return timespan - tdiff;
	}

	int DispatcherPrivate::sub_time(int timecur, int timeprev)
	{
		if (timecur >= timeprev)
			return timecur - timeprev;
		else
			return (0x7fffffff - timecur) + timeprev;
	}

	int DispatcherPrivate::wall_add(int timespan)
	{
		return add_time(_wall.read_us(), timespan);
	}

	int DispatcherPrivate::wall_diff(int timeprev)
	{
		return sub_time(_wall.read_us(), timeprev);
	}

	void SendData::handler()
	{
		switch (mode)
		{
		case SendChannelModePulseUp:
			gpio_write(&gpio, 0);
			break;
		case SendChannelModePulseDown:
			gpio_write(&gpio, 1);
			break;

		default:
			break;
		}
	}

}