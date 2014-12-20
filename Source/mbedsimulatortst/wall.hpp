// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#pragma once

#include "mbed.h"
#include "rtos.h"

// I made this way-way-way-overengineered,
// since the HW assignment is artificial anyway
// and I tried to implement the "spirit" of it ...
class Wall
{
public:
    static Wall& instance()
    {
        static Wall imp;
        return imp;
    }

public:
    void inc_minutes();
    void inc_seconds(bool& overflowed);
    void inc_cseconds(bool& overflowed);
    void get(unsigned int &minutes, unsigned int &seconds, unsigned int &cseconds);
    void reset();

    ~Wall();
        
private:
    Wall();
    
    // disable copy & assign
    Wall(const Wall& other);
    const Wall& operator=(const Wall& other);
    
private:
    Mutex _mutex;
    unsigned int _minutes;
    unsigned int _seconds;
    unsigned int _cseconds;
};