// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#pragma once

#include "mbed.h"
#include "TextLCD.h"
#include "rtos.h"

class UI
{
public:
    static UI& instance()
    {
        static UI imp;
        return imp;
    }
    
    ~UI();

    void signal_display_time();
    
private:
    void display_time();
    static void thread_func(void const* args);
    
private:
    UI();
    
    // disable copy ctor && asign
    UI(const UI&);
    const UI& operator=(const UI&);
    
private:
    void output_time_element(unsigned num, char postfix);
    
private:
    TextLCD _lcd;
    Thread _thread;
};
