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
    
    
BtnInt::BtnInt(Pin_t Pin, Pull_t Pull, Callback_t L2HCallback, Callback_t H2LCallback) :
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
    if(Read() == true)
    {
        if(_L2HCallback)
            _L2HCallback->CallbackFunction();
    }
        
    else
    {
        if(_H2LCallback)
            _H2LCallback->CallbackFunction();  
    }             
}

}
