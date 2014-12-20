// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#include "wall.hpp"

Wall::Wall()
{
    reset();
}
        
Wall::~Wall()
{
}

void Wall::inc_minutes()
{
    _mutex.lock();
    _minutes += 1;
    _mutex.unlock();
}
    
void Wall::inc_seconds(bool& overflowed)
{
    _mutex.lock();
    _seconds += 1;
    if(_seconds >= 60)
    {
        _seconds = 0;
        overflowed = true;
    }
    else 
        overflowed = false;
    _mutex.unlock();
}


void Wall::inc_cseconds(bool& overflowed)
{
    _mutex.lock();
    _cseconds += 1;
    if(_cseconds >= 100)
    {
        _cseconds = 0;
        overflowed = true;
    }
    else 
        overflowed = false;
    _mutex.unlock();
}
        
void Wall::get(unsigned int &minutes, unsigned int &seconds, unsigned int &cseconds)
{
    _mutex.lock();
    minutes = _minutes;
    seconds = _seconds;
    cseconds = _cseconds;
    _mutex.unlock();
}

void Wall::reset()
{
    _mutex.lock();
    _minutes = 0;
    _seconds = 0;
    _cseconds = 0;
    _mutex.unlock();
}
