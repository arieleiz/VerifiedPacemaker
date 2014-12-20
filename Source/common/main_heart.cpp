/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include "main.h"

static void heart_input_handler(const void* arg)
{
	ModelImpl::Dispatcher* disp = (ModelImpl::Dispatcher*)arg;

	for (;;)
	{
		ModelImpl::DelayedThread::signal_wait(1);
		while (pc.readable())
		{
			switch (pc.getc() & 0xDF)
			{
			case 'R': trace_mode("Random mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_random); break;
			case 'T': trace_mode("Test mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_test); break;
			case 'D': trace_mode("dead mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_test); break;
			case 'M': trace_mode("Manual mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_manual); break;

				// these two will have no effect if we are not in manual mode, the signal will just be cleared next tick
			case 'A': disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_gen_1); break;
			case 'V': disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testHM_gen_0); break; // XXXX gen2
			case 'O':
				int val = 0;
				for (;;)
				{
					char c = (char)pc.getc();
					if (c >= '0' && c <= '9')
						val = val * 10 + c - '0';
					else
					{
						if (c == '\r' || c == '\n')
						{
							Model::SystemVariables.int_GLOBAL_cur_msti = val;
							pclock.lock();
							pc.printf("Observation period set to: %imsec\r\n", val);
							pclock.unlock();
						}
						break;
					}
				}
				break;
			}
		}
	}
}

void init_as_heart(ModelImpl::Dispatcher& disp)
{
	trace_mode("*** STARTING AS HEART ***");

	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Apace, print_event, (void*)"APACE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Vpace, print_event, (void*)"VPACE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Asignal, print_event, (void*)"ASIGNAL");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Vsignal, print_event, (void*)"VSIGNAL");

	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Apace, LED1, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vpace, LED2, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Asignal, LED3, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vsignal, LED4, ModelImpl::SendChannelModePulseUp, 250000);

	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Asignal, HM_PIN_SIGNAL_A, ModelImpl::SendChannelModePulseUp, 40000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vsignal, HM_PIN_SIGNAL_V, ModelImpl::SendChannelModePulseUp, 40000);

	disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_Apace, HM_PIN_PACE_A, PullDown, ModelImpl::ReceiveChannelInterruptRise);
	disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_Vpace, HM_PIN_PACE_V, PullDown, ModelImpl::ReceiveChannelInterruptRise);

	disp.add_process(&Model::HM_ModeSwitch_PROCESS_STATE);
	disp.add_process(&Model::HM_Asignal_PROCESS_STATE);
	disp.add_process(&Model::HM_Apace_PROCESS_STATE);
	disp.add_process(&Model::HM_Vsignal_PROCESS_STATE);
	disp.add_process(&Model::HM_Vpace_PROCESS_STATE);
	disp.add_process(&Model::HM_Leds_Apace_PROCESS_STATE);
	disp.add_process(&Model::HM_Leds_Asignal_PROCESS_STATE);
	disp.add_process(&Model::HM_Leds_Vpace_PROCESS_STATE);
	disp.add_process(&Model::HM_Leds_Vsignal_PROCESS_STATE);

	input_thread.init(heart_input_handler, &disp);

}
