// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#include "stopwatch.hpp"
#include "ui.hpp"
#include "wall.hpp"
#include "signals.hpp"

StopWatch::StopWatch() :
    _running(false),
    _minutes(thread_func_minutes, this, osPriorityHigh),
    _seconds(thread_func_seconds, this, osPriorityHigh),
    _cseconds(thread_func_cseconds, this, osPriorityRealtime),
    _timer(timer_func, osTimerPeriodic, this)
{
    UI::instance().signal_display_time();
}

StopWatch::~StopWatch()
{
}

void StopWatch::start()
{   
    if(!_running)
    {
        _running = true;
        _timer.start(10);
    }
}

void StopWatch::stop()
{   
    if(_running)
    {
        _timer.stop();
        _running = false;
    }
}

void StopWatch::reset()
{
    if(!_running)
    {
        Wall::instance().reset();
        UI::instance().signal_display_time();
    }
}

void StopWatch::timer_func(void const* args)
{
    StopWatch* sw = (StopWatch*)args;
    sw->_cseconds.signal_set(SIGNAL_CSECONDS);
}
 
  
void StopWatch::thread_func_cseconds(void const* args)
{
    StopWatch* sw = (StopWatch*)args;
    
    for(;;)
    {
        Thread::signal_wait(SIGNAL_CSECONDS);
        bool overflowed = false;
        Wall::instance().inc_cseconds(overflowed);
        UI::instance().signal_display_time();
        if(overflowed)
            sw->_seconds.signal_set(SIGNAL_SECONDS);
    }
}

void StopWatch::thread_func_seconds(void const* args)
{
    StopWatch* sw = (StopWatch*)args;
    for(;;)
    {
        Thread::signal_wait(SIGNAL_SECONDS);
        bool overflowed = false;
        Wall::instance().inc_seconds(overflowed);
        UI::instance().signal_display_time();
        if(overflowed)
            sw->_minutes.signal_set(SIGNAL_MINUTES);
    }
}

void StopWatch::thread_func_minutes(void const* /*args*/)
{
    //StopWatch* sw = (StopWatch*)args;
    for(;;)
    {
        Thread::signal_wait(SIGNAL_MINUTES);
        Wall::instance().inc_minutes();
        UI::instance().signal_display_time();
    }    
}
