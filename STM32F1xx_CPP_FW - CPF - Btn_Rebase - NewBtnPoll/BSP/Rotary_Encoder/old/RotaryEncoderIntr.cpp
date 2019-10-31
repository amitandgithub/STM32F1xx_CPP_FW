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
                                     Callback_t CWHandler, 
                                     Callback_t CCWHandler,
                                     Callback_t SWL2HCallback,
                                     Callback_t SWH2LCallback ):
        _CWCallback(this),
        _SWCallback(this),
        _CWPin(CWPin,HAL::GpioInput::INPUT_PULL_UP,HAL::GpioInput::IT_ON_FALLING,&_CWCallback),
        _CCWPin(CCWPin,HAL::GpioInput::INPUT_PULL_UP),                            
        _SWPin(SWPin,HAL::GpioInput::INPUT_PULL_UP,HAL::GpioInput::IT_ON_RISING_FALLING,&_SWCallback), 
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler), 
        _SW_L2H_Handler(SWL2HCallback),
        _SW_H2L_Handler(SWH2LCallback)
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

void RotaryEncoderIntr::RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWL2HCallback, Callback_t SWH2LCallback)
{
    _CWHandler     = CW_Handler;
    _CCWHandler    = CCW_Handler;
    _SW_L2H_Handler = SWL2HCallback;
    _SW_H2L_Handler = SWH2LCallback;
}

void RotaryEncoderIntr::CWCallback::CallbackFunction()
{
    if(_This->_CCWPin.Read() == true)
    {
        if(_This->_CWHandler) 
            _This->_CWHandler->CallbackFunction();
    }
    else
    {
        if(_This->_CCWHandler) 
            _This->_CCWHandler->CallbackFunction();
    }
    
}

void RotaryEncoderIntr::SWCallback::CallbackFunction()
{
     if(_This->_SWPin.Read() == true)
    {
        if(_This->_SW_L2H_Handler) 
            _This->_SW_L2H_Handler->CallbackFunction();
    }
    else
    {
        if(_This->_SW_H2L_Handler) 
            _This->_SW_H2L_Handler->CallbackFunction();
    }
}


}