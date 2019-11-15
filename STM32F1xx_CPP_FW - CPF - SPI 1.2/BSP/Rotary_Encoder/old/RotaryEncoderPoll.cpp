/******************
** FILE: RotaryEncoderPoll.cpp
**
** DESCRIPTION:
**  Rotary Encoder Implementation using polling
**
** CREATED: 2/3/2019, by Amit Chaudhary
******************/
#include "RotaryEncoderPoll.h"

namespace BSP
{

RotaryEncoderPoll::RotaryEncoderPoll(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                                     Callback_t CWHandler, 
                                     Callback_t CCWHandler,
                                     Callback_t SWL2HCallback,
                                     Callback_t SWH2LCallback ):
        _CW(CWPin,BSP::BtnPoll::BTN_PULL_DOWN,1,this,nullptr), // for CW Debounce time for 1 msec works fine + 100nf and 1K RC debounce
        _CCW(CCWPin,BSP::BtnPoll::BTN_PULL_DOWN,0),                            // for CCW Debounce time for 0 msec works fine + 100nf and 1K RC debounce
        _SW(SWPin,BSP::BtnPoll::BTN_PULL_DOWN,0,SWH2LCallback,SWL2HCallback), 
       // _CWCurrState(0),
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler)   
       // _RotaryCurrState(NOT_ROTATING)
{
            
}

RotaryEncoderPoll::Status_t RotaryEncoderPoll::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CW.HwInit(); 
    _CCW.HwInit(); 
    _SW.HwInit(); 
    //_CWCurrState = _CW.getState();
    return 1;
}

void RotaryEncoderPoll::Run()
{  
    _CW.Run();
    
    _CCW.Run();
    
    _SW.Run();
}

void RotaryEncoderPoll::CallbackFunction()
{
    if(_CCW.getState() == false)
    {
       // _RotaryCurrState = ROTATING_CLOCKWISE;
        if(_CWHandler) 
            _CWHandler->CallbackFunction();        
    }
    else
    {
      //  _RotaryCurrState = ROTATING_ANTI_CLOCKWISE;
        if(_CCWHandler) 
            _CCWHandler->CallbackFunction();        
    }
}
}