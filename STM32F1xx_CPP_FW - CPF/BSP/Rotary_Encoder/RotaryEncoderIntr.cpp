/******************
** FILE: RotaryEncoderIntr.cpp
**
** DESCRIPTION:
**  Rotary Encoder Implementation using polling
**
** CREATED: 2/3/2019, by Amit Chaudhary
******************/
#include "RotaryEncoderIntr.h"

namespace BSP
{

RotaryEncoderIntr::RotaryEncoderIntr(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                                     HALCallback_t CWHandler, 
                                     HALCallback_t CCWHandler,
                                     HALCallback_t SWL2HCallback,
                                     HALCallback_t SWH2LCallback ):
        _CWPin(CWPin,HAL::GpioInput::INPUT_PULL_DOWN,HAL::GpioInput::IT_ON_FALLING,&_CWCallback),
        _CCWPin(CCWPin,HAL::GpioInput::INPUT_PULL_DOWN),                            
        _SWPin(SWPin,HAL::GpioInput::INPUT_PULL_DOWN,HAL::GpioInput::IT_ON_RISING_FALLING,&_SWCallback), 
        _CWCurrState(0),
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler),   
        _RotaryCurrState(NOT_ROTATING)
{
            
}

RotaryEncoderIntr::Status_t RotaryEncoderIntr::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CWPin.HwInit(); 
    _CCWPin.HwInit(); 
    _SWPin.HwInit(); 
    return 1;
}

void RotaryEncoderIntr::CW_L2H_Handler()
{
//    if(_CCW.getState() == false)
//    {
//        if(_CWHandler) _CWHandler();
//    }
//    else
//    {
//        if(_CCWHandler) _CCWHandler();
//    }
}
    
 void RotaryEncoderIntr::CW_H2L_Handler()
{
    
}

}