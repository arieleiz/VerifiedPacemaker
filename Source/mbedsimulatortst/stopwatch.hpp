// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#pragma once

#include "mbed.h"
#include "rtos.h"

class StopWatch
{
public:
    static StopWatch& instance()
    {
        static StopWatch imp;
        return imp;
    }
    
    ~StopWatch();
    
    void start();
    void stop();
    void reset();

private:
    static void timer_func(void const* args);
    static void thread_func_minutes(void const* args);
    static void thread_func_seconds(void const* args);
    static void thread_func_cseconds(void const* args);
    
private:
    StopWatch();

    // disable copy ctor && asign
    StopWatch(const StopWatch&);
    const StopWatch& operator=(const StopWatch&);

private:
    bool _running;
    Thread _minutes, _seconds, _cseconds;    
    RtosTimer _timer;
};    
    
