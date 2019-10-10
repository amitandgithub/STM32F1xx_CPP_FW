// RotaryEncoderPollTest.h



#include "RotaryEncoderPoll.h"



class CWCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t CW;
        CW++;
    }
};

class CCWCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t CCW;
        CCW++;
    }
};
class SWRiseCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t SWRise;
        SWRise++;
    }
};

class SWFallCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t SWFall;
        SWFall++;
    }
};

class CWCallback        CW_Callback;
class CCWCallback       CCW_Callback;
class SWRiseCallback    SW_Rise;
class SWFallCallback    SW_Fall;



void Rotary_Encoder_Test()
{
    //int i = sizeof(RotaryEncoderPoll);// 144
    RotaryEncoderPoll RotaryEncoder(Gpio::A5,Gpio::A7,Gpio::A1,&CW_Callback,&CCW_Callback,&SW_Rise,&SW_Fall);
    RotaryEncoder.HwInit();

    while(1)
    {
        RotaryEncoder.Run();
    }
}