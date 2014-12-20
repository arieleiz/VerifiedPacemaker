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
#include "ModelImpl.h"

Serial pc(USBTX, USBRX); // tx, rx
Mutex pclock;
TextLCD lcd(p15, p16, p17, p18, p19, p20, TextLCD::LCD16x2);
DigitalIn pmhmselector(PMHM_SELECTOR_PIN);
ModelImpl::DelayedThread input_thread;

void trace_mode(const char* txt)
{
	pclock.lock();
	pc.printf("%s\r\n", txt);
	pclock.unlock();
}

void show_mode(ModelImpl::SYNCHRONIZATION_CHANNEL* chan)
{
	pclock.lock();
	pc.printf("%s\r\n", (const char*)chan->context);
	pclock.unlock();
}

void print_event(ModelImpl::SYNCHRONIZATION_CHANNEL* chan)
{
	pclock.lock();
	pc.printf("@@@CHAN!: [%s]\r\n", (const char*)chan->context);
	pclock.unlock();
}

void input_handler_irq()
{
	input_thread.signal_set(1);
}

int main (void) 
{
    // terminal should be set to MBED's defaults: 9600,8,n,1
    pc.printf("CIS541 Final Project, Ariel Eizenberg (arieleiz@seas.upenn.edu).\r\n");

	ModelImpl::Dispatcher disp;
	//disp.set_clock_multiplier(1000 /*us*/);
	disp.set_clock_multiplier(1000 /*us*/);
	//disp.set_clock_multiplier(5000000 /*us*/);
	//disp.set_clock_multiplier(5000000 /*us*/);
	#ifdef DISPATCH_DEBUG
	disp.init_debug(&pc, &pclock);
	#endif
	
	if (pmhmselector.read() == 0)
		init_as_pacemaker(disp);
	else
		init_as_heart(disp);

	input_thread.start();
	pc.attach(input_handler_irq);

	disp.run();
}

