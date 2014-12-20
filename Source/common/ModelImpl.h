/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/
#pragma once

#include <mbed.h>
#include <rtos.h>

// public interface in this file

namespace ModelImpl
{
	enum SendChannelMode
	{
		SendChannelModeNone, SendChannelModeSet, SendChannelModeToggle, SendChannelModeReset, SendChannelModePulseUp, SendChannelModePulseDown
	};

	enum ReceiveChannelMode
	{
		ReceiveChannelModeNone, ReceiveChannelInterruptRise, ReceiveChannelInterruptFall
	};
}

#include "ModelPriv.h"

namespace ModelImpl
{
	class Dispatcher
	{
	public:
		Dispatcher()
		{ }

#ifdef DISPATCH_DEBUG
		inline void init_debug(Serial* dbgserial, Mutex* dbgmutex)
		{
			DispatcherPrivate::INSTANCE.init_debug(dbgserial, dbgmutex);
		}
#endif
		inline void set_clock_multiplier(int multiplier)
		{
			DispatcherPrivate::INSTANCE.set_clock_multiplier(multiplier);
		}

		inline void add_process(PROCESS_ENTRY_STATE* ps)
		{
			DispatcherPrivate::INSTANCE.add_process(ps);
		}

		inline void run()
		{
			DispatcherPrivate::INSTANCE.run();
		}

	public: // time
		inline int get_tick()
		{
			return DispatcherPrivate::INSTANCE.get_tick();
		}

	public: // channels
		// trigger receive channel manually
		inline void fire_channel(SYNCHRONIZATION_CHANNEL* channel)
		{
			DispatcherPrivate::INSTANCE.fire_channel(channel);
		}

		inline void set_channel_fire_cb(SYNCHRONIZATION_CHANNEL* channel, void(*func)(SYNCHRONIZATION_CHANNEL* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_channel_fire_cb(channel, func, context);
		}

		// set channel "send" action
		inline void set_channel_action(SYNCHRONIZATION_CHANNEL* channel, PinName pin, SendChannelMode mode, int pulse_length_us = 0)
		{
			DispatcherPrivate::INSTANCE.set_channel_action(channel, pin, mode, pulse_length_us);
		}

		inline void disable_channel_actions(SYNCHRONIZATION_CHANNEL* channel)
		{
			DispatcherPrivate::INSTANCE.disable_channel_actions(channel);
		}

		// trigger receive channel when interrupt occurs
		inline void set_receive_input(SYNCHRONIZATION_CHANNEL* channel, PinName pin, PinMode mode, ReceiveChannelMode rcmode)
		{
			DispatcherPrivate::INSTANCE.set_receive_input(channel, pin, mode, rcmode);
		}
		// 

	public: // states
		inline void set_state_enter_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_state_enter_cb(state, func, context);
		}

		inline void set_state_leave_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_state_leave_cb(state, func, context);
		}

	public: // transitions
		inline void set_state_pre_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_state_pre_execute_cb(transition, func, context);
		}

		inline void set_state_post_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_state_post_execute_cb(transition, func, context);
		}

	public: // processes
		inline void set_process_start_cb(PROCESS_ENTRY* entry, void(*func)(PROCESS_ENTRY* me), void* context)
		{
			DispatcherPrivate::INSTANCE.set_process_start_cb(entry, func, context);
		}
	};

}

