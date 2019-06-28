

#include"BtnInt.h"

using namespace BSP;

class RisingCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t rising;
        rising++;
    }
};

class FallingCallback : public Callback
{
    void CallbackFunction()
    {
        static uint32_t falling;
        falling++;
    }
};

class RisingCallback Rising;
class FallingCallback Falling;

void BtnInt_Test()
{
    uint32_t size;
    size = sizeof(BtnInt);/*40 bytes*/ size++;
    
    BtnInt BtnInt_A8(Gpio::A8, BSP::BtnInt::BTN_PULL_DOWN, &Rising,&Falling );
    BtnInt_A8.HwInit();      
    
    while(1)
    {
        
    }
}
