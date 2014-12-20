// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#include "stopwatch.hpp"
#include "ui.hpp"
#include "input.hpp"
#include "mbed.h"

Serial pc(USBTX, USBRX); // tx, rx
DigitalOut led1(LED1);
DigitalOut led3(LED2);
DigitalOut led2(p5);
DigitalIn button1(p7);
InterruptIn button2(p8);
DigitalOut crossout(p9);
InterruptIn crossin(p9);

static void button2_rise()
{
	pc.printf("Button2 Rise.\n");
	crossout = 1;
}

static void button2_fall()
{
	pc.printf("Button2 Fall.\n");
	crossout = 0;
}

static void crossin_rise()
{
	pc.printf("Cross Rise.\n");
	led3 = 1;
}

static void crossin_fall()
{
	pc.printf("Cross Fall.\n");
	led3 = 0;
}

int main (void) 
{
    // terminal should be set to MBED's defaults: 9600,8,n,1
    pc.printf("CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu).\r\n");

    UI::instance().signal_display_time();
    InputSerial serial(pc);
    
	int oldval = 0;
	button2.rise(button2_rise);
	button2.fall(button2_fall);
	crossin.rise(crossin_rise);
	crossin.fall(crossin_fall);

    for(;;)
    {
		int val = button1.read();
		if (val != oldval && val == 1)
			led1 = !led1;
		oldval = val;

		wait(0.25);
		led2 = !led2;
    }
     
}

