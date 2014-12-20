// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#include "input.hpp"
#include "stopwatch.hpp"

InputSerial::InputSerial(Serial& serial) :
    _pc(serial),
    _thread(thread_func, this, osPriorityNormal)

{
}

InputSerial::~InputSerial()
{
    
}

void InputSerial::thread_func(void const* args)
{
    InputSerial* serial = (InputSerial*)args;
    serial->_pc.printf("Use keyboard for (S)tart, Sto(p) or (R)eset:\r\n");

    for(;;)
    {
        int c = serial->_pc.getc();
        switch(c)
        {
        case 's':
            StopWatch::instance().start();
            break;
            
        case 'p':
            StopWatch::instance().stop();
            break;
            
        case 'r':
            // reset prevention logic during running timer
            // is implemented in StopWatch
            StopWatch::instance().reset();
            break;
        default:
            serial->_pc.printf("Unknown key: '%c'\r\n", c);
            break;
        }
    }
}

