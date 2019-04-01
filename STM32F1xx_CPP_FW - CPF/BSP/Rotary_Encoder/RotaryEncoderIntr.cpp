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
        _CWPin(CWPin,HAL::GpioInput::INPUT_PULL_DOWN,CW_L2H_Handler,CW_H2L_Handler),
        _CCW(CCWPin,BSP::BtnPoll::BTN_PULL_DOWN,0),                            
        _SW(SWPin,BSP::BtnInt::BTN_PULL_DOWN,SWH2LCallback,SWL2HCallback), 
        _CWCurrState(0),
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler),   
        _RotaryCurrState(NOT_ROTATING)
{
            
}

RotaryEncoderIntr::Status_t RotaryEncoderIntr::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CW.HwInit(); 
    _CCW.HwInit(); 
    _SW.HwInit(); 
    _CWCurrState = _CW.getState();
    return 1;
}

void RotaryEncoderIntr::CW_L2H_Handler()
{
    if(_CCW.getState() == false)
    {
        if(_CWHandler) _CWHandler();
    }
    else
    {
        if(_CCWHandler) _CCWHandler();
    }
}
    
 void RotaryEncoderIntr::CW_H2L_Handler()
{
    
}

}