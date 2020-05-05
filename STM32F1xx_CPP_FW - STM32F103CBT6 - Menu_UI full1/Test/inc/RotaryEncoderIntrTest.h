



#include "RotaryEncoderIntr.h"


void Rotary_EncoderIntr_Test()
{
   // int i = sizeof(RotaryEncoderIntr); // 116
    RotaryEncoderIntr RotaryEncoder(Gpio::A5,Gpio::A7,Gpio::A1,&CW_Callback,&CCW_Callback,&SW_Rise,&SW_Fall);
    RotaryEncoder.HwInit();

    while(1)
    {
    }
}