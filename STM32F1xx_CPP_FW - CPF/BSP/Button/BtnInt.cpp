/******************
** FILE: BtnInt.cpp
**
** DESCRIPTION:
**  BtnInt class implementation using Interrupts
**
** CREATED: 22/2/2019, by Amit Chaudhary
******************/
#include "BtnInt.h"


namespace BSP
{
    
    
BtnInt::BtnInt(Pin_t Pin, Pull_t Pull, HALCallback_t L2HCallback, HALCallback_t H2LCallback) :
        _BtnPin(Pin,(Pull==BTN_PULL_UP?HAL::GpioInput::INPUT_PULL_UP:HAL::GpioInput::INPUT_PULL_DOWN),HAL::GpioInput::IT_ON_RISING_FALLING,this),
        _L2HCallback(L2HCallback),
        _H2LCallback(H2LCallback)
{
    
    
}
BtnInt::Status_t BtnInt::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    return _BtnPin.HwInit(); 
}

void BtnInt::CallbackFunction()
{
    if(Read())
        _L2HCallback();
    else
        _H2LCallback();        
}

}
