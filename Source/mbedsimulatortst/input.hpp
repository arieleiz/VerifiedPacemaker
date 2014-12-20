// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#pragma once

#include "mbed.h"
#include "rtos.h"

class InputSerial 
{
public:
    InputSerial(Serial& serial);
    ~InputSerial();
    
private:
    static void thread_func(void const* args);

private:
	InputSerial(const InputSerial& serial);
	InputSerial& operator=(const InputSerial& serial);

private:
    Serial& _pc;
    Thread _thread;
};

