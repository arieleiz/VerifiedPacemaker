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

// each thread (artificially) has just one signal, so they could all
// be 0x1 or something, but for nicety sake I do it this way:
#define SIGNAL_CSECONDS 0x0001
#define SIGNAL_SECONDS  0x0002
#define SIGNAL_MINUTES  0x0004
#define SIGNAL_DISPLAY  0x0008

