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
#include "../common/main_heart.cpp"
#include "../common/ModelImpl.cpp"

void init_as_pacemaker(ModelImpl::Dispatcher& disp)
{
	trace_mode("*** CANNOT INIT HEART ON PACEMAKER HARDWARE ***");
	for(;;) { }
}

