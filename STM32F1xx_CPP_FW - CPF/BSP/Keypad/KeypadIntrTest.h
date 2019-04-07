

#include "KeypadIntr.h"


void KeypadIntr_Test()
{
    BtnInt Key0(Gpio::A5, BSP::BtnInt::BTN_PULL_DOWN, &Key0RiseCB,&Key0FallCB );
    BtnInt Key1(Gpio::A7, BSP::BtnInt::BTN_PULL_DOWN, &Key1RiseCB,&Key1FallCB );
    BtnInt Key2(Gpio::A1, BSP::BtnInt::BTN_PULL_DOWN, &Key2RiseCB,&Key2FallCB );
    BtnInt Key3(Gpio::A8, BSP::BtnInt::BTN_PULL_DOWN, &Key3RiseCB,&Key3FallCB );
    
    KeypadIntr<4> MyKeypad;
    MyKeypad.AddKey(0,&Key0);
    MyKeypad.AddKey(1,&Key1);
    MyKeypad.AddKey(2,&Key2);
    MyKeypad.AddKey(3,&Key3);

    while(1)
    {
        //MyKeypad.Run();
    }
}