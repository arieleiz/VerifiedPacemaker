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


static void show_rate(ModelImpl::STATE_ENTRY* /*state*/)
{
	int rate = (int)Model::SystemVariables.int_PM_DisplayAndAlarm_aa_rate;
	lcd.locate(0, 0);
	lcd.printf("BPM: %03i", rate);
}

static void show_alarm(ModelImpl::SYNCHRONIZATION_CHANNEL* chan)
{
	lcd.locate(0, 1);
	lcd.printf("%s", chan->context);
}

static void pacemaker_input_handler(const void* arg)
{
	ModelImpl::Dispatcher* disp = (ModelImpl::Dispatcher*)arg;

	for (;;)
	{
		ModelImpl::DelayedThread::signal_wait(1);
		while (pc.readable())
		{
			switch (pc.getc() & 0xDF)
			{
			case 'N': trace_mode("Normal mode");  disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_normal); break;
			case 'S': trace_mode("Sleep mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_sleep); break;
			case 'E': trace_mode("Sports mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_sports); break;
			case 'M': trace_mode("Manual mode"); disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_manual); break;

				// these two will have no effect if we are not in manual mode, the signal will just be cleared next tick
			case 'A': disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_gen_1); break;
			case 'V': disp->fire_channel(&Model::GLOBAL_CHANNEL_VAR_testPM_gen_0); break; // XXXX gen2
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

void init_as_pacemaker(ModelImpl::Dispatcher& disp)
{
	trace_mode("*** STARTING AS PACEMAKER ***");

	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Apace, print_event, (void*)"APACE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Vpace, print_event, (void*)"VPACE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Asense, print_event, (void*)"ASENSE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Vsense, print_event, (void*)"VSENSE");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Asignal, print_event, (void*)"ASIGNAL");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_Vsignal, print_event, (void*)"VSIGNAL");

	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_AlarmL, show_alarm, (void*)"TOO LOW! ");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_AlarmH, show_alarm, (void*)"TOO HIGH!");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_NoAlarm, show_alarm, (void*)"NORMAL   ");

	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_testPM_normal, show_mode, (void*)"Normal mode ");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_testPM_sleep, show_mode, (void*)"Sleep mode");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_testPM_sports, show_mode, (void*)"Sports mode");
	disp.set_channel_fire_cb(&Model::GLOBAL_CHANNEL_VAR_testPM_manual, show_mode, (void*)"Manual mode");

	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Apace, LED1, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vpace, LED2, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Asense, LED3, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vsense, LED4, ModelImpl::SendChannelModePulseUp, 250000);

	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Apace, PM_PIN_PACE_A, ModelImpl::SendChannelModePulseUp, 40000);
	disp.set_channel_action(&Model::GLOBAL_CHANNEL_VAR_Vpace, PM_PIN_PACE_V, ModelImpl::SendChannelModePulseUp, 40000);

	disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_Asignal, PM_PIN_SIGNAL_A, PullDown, ModelImpl::ReceiveChannelInterruptRise);
	disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_Vsignal, PM_PIN_SIGNAL_V, PullDown, ModelImpl::ReceiveChannelInterruptRise);

	disp.set_state_enter_cb(&Model::PM_DisplayAndAlarm_STATE_ShowRate, show_rate, NULL);

	disp.add_process(&Model::PM_ModeSwitch_PROCESS_STATE);
	disp.add_process(&Model::PM_Sensor_A_PROCESS_STATE);
	disp.add_process(&Model::PM_Sensor_V_PROCESS_STATE);
	disp.add_process(&Model::PM_Pacer_AA_PROCESS_STATE);
	disp.add_process(&Model::PM_Leds_Apace_PROCESS_STATE);
	disp.add_process(&Model::PM_Leds_Asense_PROCESS_STATE);
	disp.add_process(&Model::PM_Leds_Vpace_PROCESS_STATE);
	disp.add_process(&Model::PM_Leds_Vsense_PROCESS_STATE);
	disp.add_process(&Model::PM_DisplayAndAlarm_PROCESS_STATE);

	input_thread.init(pacemaker_input_handler, &disp);
}
