/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

#include "Model.h"
#include "Model.cpp"
#include "../common/main.cpp"
#include "../common/ModelImpl.cpp"

#include "main.h"
using namespace Model;


static void show_rate(ModelImpl::STATE_ENTRY* /*state*/)
{
	int rate = (int)SystemVariables.int_PM_DisplayAndAlarm_aa_rate;
	lcd.locate(0, 0);
	lcd.printf("BPM: %03i", rate);
}

static void show_alarm(ModelImpl::SYNCHRONIZATION_CHANNEL* chan)
{
	lcd.locate(0, 1);
	lcd.printf("%s", chan->context);
}

static void reached_bad_state(ModelImpl::STATE_ENTRY* state)
{
	pc.printf("@@@BAD! : [%s]r\n", (const char*)state->context);
	assert(false);
}

#define RT_ASSERT(cond) rt_assert((cond), #cond)

static void rt_assert(bool cond, const char* condstr)
{
	if (!cond)
	{
		pc.printf("@@@FAILED! : [%s]r\n", condstr);
		assert(false);
	}
}

static void rtcheck_av_s5(ModelImpl::STATE_ENTRY*)
{
	RT_ASSERT(SystemVariables.clk_Observer_AV_t >= SystemVariables.int_GLOBAL_cur_pvab);
}

static void rtcheck_vrp_s3(ModelImpl::STATE_ENTRY*)
{
	RT_ASSERT(SystemVariables.clk_Observer_VRP_t >= SystemVariables.int_GLOBAL_cur_vrp);
}

static void rtcheck_uri_s3(ModelImpl::STATE_ENTRY*)
{
	RT_ASSERT(SystemVariables.clk_Observer_URI_t >= (SystemVariables.int_GLOBAL_cur_avi + SystemVariables.int_GLOBAL_cur_avi_ext + SystemVariables.int_GLOBAL_cur_pvarp));
	RT_ASSERT(SystemVariables.clk_Observer_URI_t >= (SystemVariables.int_GLOBAL_cur_uri));
}

static void rtcheck_lri_s3(ModelImpl::STATE_ENTRY*)
{
	RT_ASSERT(SystemVariables.clk_Observer_LRI_t <= (SystemVariables.int_GLOBAL_cur_lri));
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
			case 'N': trace_mode("Normal mode");  disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_normal); break;
			case 'S': trace_mode("Sleep mode"); disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_sleep); break;
			case 'E': trace_mode("Sports mode"); disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_sports); break;
			case 'M': trace_mode("Manual mode"); disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_manual); break;

				// these two will have no effect if we are not in manual mode, the signal will just be cleared next tick
			case 'A': disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_gen_1); break;
			case 'V': disp->fire_channel(&GLOBAL_CHANNEL_VAR_testPM_gen_0); break; // XXXX gen2
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
							SystemVariables.int_GLOBAL_cur_msti = val;
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

	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Apace, print_event, (void*)"APACE");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Vpace, print_event, (void*)"VPACE");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Asense, print_event, (void*)"ASENSE");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Vsense, print_event, (void*)"VSENSE");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Asignal, print_event, (void*)"ASIGNAL");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_Vsignal, print_event, (void*)"VSIGNAL");

	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_AlarmL, show_alarm, (void*)"TOO LOW! ");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_AlarmH, show_alarm, (void*)"TOO HIGH!");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_NoAlarm, show_alarm, (void*)"NORMAL   ");

	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_testPM_normal, show_mode, (void*)"Normal mode ");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_testPM_sleep, show_mode, (void*)"Sleep mode");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_testPM_sports, show_mode, (void*)"Sports mode");
	disp.set_channel_fire_cb(&GLOBAL_CHANNEL_VAR_testPM_manual, show_mode, (void*)"Manual mode");

	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Apace, LED1, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Vpace, LED2, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Asense, LED3, ModelImpl::SendChannelModePulseUp, 250000);
	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Vsense, LED4, ModelImpl::SendChannelModePulseUp, 250000);

	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Apace, PM_PIN_PACE_A, ModelImpl::SendChannelModePulseUp, 40000);
	disp.set_channel_action(&GLOBAL_CHANNEL_VAR_Vpace, PM_PIN_PACE_V, ModelImpl::SendChannelModePulseUp, 40000);

	disp.set_receive_input(&GLOBAL_CHANNEL_VAR_Asignal, PM_PIN_SIGNAL_A, PullDown, ModelImpl::ReceiveChannelInterruptRise);
	disp.set_receive_input(&GLOBAL_CHANNEL_VAR_Vsignal, PM_PIN_SIGNAL_V, PullDown, ModelImpl::ReceiveChannelInterruptRise);

	disp.set_state_enter_cb(&PM_DisplayAndAlarm_STATE_ShowRate, show_rate, NULL);

	// observer
	disp.set_state_enter_cb(&Observer_AV_STATE_BAD1, reached_bad_state, "BAD1");
	disp.set_state_enter_cb(&Observer_AV_STATE_BAD2, reached_bad_state, "BAD12");
	disp.set_state_enter_cb(&Observer_AV_STATE_S5, rtcheck_av_s5, NULL);
	disp.set_state_enter_cb(&Observer_VRP_STATE_S3, rtcheck_vrp_s3, NULL);
	disp.set_state_enter_cb(&Observer_URI_STATE_S3, rtcheck_uri_s3, NULL);
	disp.set_state_enter_cb(&Observer_LRI_STATE_S3, rtcheck_lri_s3, NULL);

	// channels

	disp.add_process(&PM_ModeSwitch_PROCESS_STATE);
	disp.add_process(&PM_Sensor_A_PROCESS_STATE);
	disp.add_process(&PM_Sensor_V_PROCESS_STATE);
	disp.add_process(&PM_Pacer_AA_PROCESS_STATE);
	disp.add_process(&PM_Leds_Apace_PROCESS_STATE);
	disp.add_process(&PM_Leds_Asense_PROCESS_STATE);
	disp.add_process(&PM_Leds_Vpace_PROCESS_STATE);
	disp.add_process(&PM_Leds_Vsense_PROCESS_STATE);
	disp.add_process(&PM_DisplayAndAlarm_PROCESS_STATE);
	disp.add_process(&Observer_LRI_PROCESS_STATE);
	disp.add_process(&Observer_URI_PROCESS_STATE);
	disp.add_process(&Observer_VRP_PROCESS_STATE);
	disp.add_process(&Observer_AV_PROCESS_STATE);

	input_thread.init(pacemaker_input_handler, &disp);
}

void init_as_heart(ModelImpl::Dispatcher& disp)
{
	trace_mode("*** CANNOT INIT PACEMAKER ON HEART HARDWARE ***");
	for(;;) { }
}

