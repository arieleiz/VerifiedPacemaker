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
#include "Model.h"

Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);
DigitalOut led2(LED2);

void fun_off(ModelImpl::STATE_ENTRY* )
{
	led1 = 0;
	led2 = 0;
}

void fun_low(ModelImpl::STATE_ENTRY*)
{
	led1 = 1;
}

void fun_high(ModelImpl::STATE_ENTRY*)
{
	led2 = 1;
}


int main (void) 
{
    // terminal should be set to MBED's defaults: 9600,8,n,1
    pc.printf("CIS541 Lightswitch example, Ariel Eizenberg (arieleiz@seas.upenn.edu).\r\n");

	ModelImpl::Dispatcher disp;
	//disp.set_clock_multiplier(1000 /*us*/);
	//disp.set_clock_multiplier(250000 /*us*/);
	disp.set_clock_multiplier(1000000 /*us*/);
	//disp.set_clock_multiplier(5000000 /*us*/);
	//disp.set_clock_multiplier(5000000 /*us*/);
	#ifdef DISPATCH_DEBUG
	disp.init_debug(&pc);
	#endif

	//disp.set_channel_action(&Model::Template_CHANNEL_VAR_led, LED1, ModelImpl::SendChannelModePulseUp, 1000000);
	//disp.set_channel_action(&Model::Template_CHANNEL_VAR_manual, LED2, ModelImpl::SendChannelModeToggle);
	//disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_press, p7, ModelImpl::ReceiveChannelInterruptRise);
	disp.set_receive_input(&Model::GLOBAL_CHANNEL_VAR_press, p7, PullUp, ModelImpl::ReceiveChannelInterruptRise);
	
	disp.set_state_enter_cb(&Model::Light_STATE_off, fun_off, NULL);
	disp.set_state_enter_cb(&Model::Light_STATE_low, fun_low, NULL);
	disp.set_state_enter_cb(&Model::Light_STATE_bright, fun_high, NULL);

	disp.add_process(&Model::Light_PROCESS_STATE);
	//disp.add_process(&Model::Template_PROCESS_STATE);
	disp.run();

}

