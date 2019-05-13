//LedTest.h
#include"Led.h"

//This tests the GpioOutput as well

void Led_Test()
{
    BSP::Led C13_Led(Gpio::C13);
    C13_Led.HwInit();
    
    if(C13_Led.IsOn())
    {
        C13_Led.Blink();
    }
    
    C13_Led.Blink(400,70);
    C13_Led.Blink(400,70, 5);
    
    while(1)
    {
        C13_Led.Blink(400,70);
    }
}