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
#include "TextLCD/TextLCD.h"
#include <Model.h>

extern Serial pc;
extern Mutex pclock;
extern TextLCD lcd;
extern DigitalIn pmhmselector;
extern ModelImpl::DelayedThread input_thread;

#define HM_PIN_SIGNAL_A p11
#define HM_PIN_SIGNAL_V p12
#define HM_PIN_PACE_A p13
#define HM_PIN_PACE_V p14

#define PM_PIN_SIGNAL_A p30
#define PM_PIN_SIGNAL_V p29
#define PM_PIN_PACE_A p28
#define PM_PIN_PACE_V p27

#define PM_PIN_ALARM p25
#define PMHM_SELECTOR_PIN p26


void init_as_pacemaker(ModelImpl::Dispatcher& disp);
void init_as_heart(ModelImpl::Dispatcher& disp);

void trace_mode(const char* txt);
void show_mode(ModelImpl::SYNCHRONIZATION_CHANNEL* chan);
void print_event(ModelImpl::SYNCHRONIZATION_CHANNEL* chan);