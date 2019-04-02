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
        _CWPin(CWPin,HAL::GpioInput::INPUT_PULL_UP,HAL::GpioInput::IT_ON_FALLING,this),
        _CCWPin(CCWPin,HAL::GpioInput::INPUT_PULL_UP),                            
        _SWPin(SWPin,HAL::GpioInput::INPUT_PULL_UP,HAL::GpioInput::IT_ON_RISING_FALLING,this), 
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler), 
        _SW_L2H_Handler(SWL2HCallback),
        _SW_H2L_Handler(SWH2LCallback),
        _RotaryCurrState(NOT_ROTATING)
{
            
}

RotaryEncoderIntr::Status_t RotaryEncoderIntr::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CWPin.HwInit(); 
    _CCWPin.HwInit(); 
    _SWPin.HwInit(); 
    _CWPinState = _CWPin.Read();
    _SWPinState = _SWPin.Read();
    return 1;
}

void RotaryEncoderIntr::CallbackFunction()
{
    bool CWCurrentState     =   _CWPin.Read();
    bool CCWCurrentState    =   _CCWPin.Read();
    bool SWCurrentState     =   _SWPin.Read();
    
    if(_CWPinState != CWCurrentState)
    {
        _CWPinState = CWCurrentState;
        
        if((CWCurrentState == false) && (CCWCurrentState == true) )
        {
            _RotaryCurrState = ROTATING_CLOCKWISE;
            
            if(_CWHandler) 
                _CWHandler->CallbackFunction();
        }
        else if ((CWCurrentState == true) && (CCWCurrentState == false) )
        {
            _RotaryCurrState = ROTATING_ANTI_CLOCKWISE;
            
            if(_CCWHandler) 
                _CCWHandler->CallbackFunction();
        }        
    }
    
    if(_SWPinState != SWCurrentState)
    {
        _SWPinState = SWCurrentState;
        if(SWCurrentState)
        {
            if(_SW_L2H_Handler) 
                _SW_L2H_Handler->CallbackFunction();
        }
        else
        {
            if(_SW_H2L_Handler) 
                _SW_H2L_Handler->CallbackFunction();
        }
            
    }
}

void RotaryEncoderIntr::CWCallback::CallbackFunction()
{
   // RotaryEncoderIntr::_this->_CCWPin.Read();
    
}

void RotaryEncoderIntr::SWCallback::CallbackFunction()
{
    
}


}