// CIS541 HW5, Ariel Eizenberg (arieleiz@seas.upenn.edu)
#include "ui.hpp"
#include "wall.hpp"
#include "signals.hpp"

UI::UI() : 
    _lcd(p15, p16, p17, p18, p19, p20, TextLCD::LCD16x4),
    _thread(thread_func, this, osPriorityNormal)
{
    _lcd.cls();
    _lcd.setMode(TextLCD::DispOn);
    _lcd.setCursor(TextLCD::CurOff_BlkOff);
    _lcd.setBacklight(TextLCD::LightOn); 
}

UI::~UI()
{
}

void UI::signal_display_time()
{
    _thread.signal_set(SIGNAL_DISPLAY);
}

void UI::thread_func(void const* args)
{
    UI* ui = (UI*)args;
    for(;;)
    {
        Thread::signal_wait(SIGNAL_DISPLAY);
        ui->display_time();
    }
}
    
void UI::display_time()
{
    unsigned int min, sec, csec;
    
    Wall::instance().get(min, sec, csec);
    
    _lcd.locate(0, 0);
    
    output_time_element(min, ':');
    output_time_element(sec, ':');
    output_time_element(csec, 0);
}
    
void UI::output_time_element(unsigned num, char postfix)
{
    _lcd.putc('0' + (num / 10));
    _lcd.putc('0' + (num % 10));
    if(postfix != 0)
        _lcd.putc(postfix);
}

