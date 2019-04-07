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
                                     HALCallback_t CWHandler, 
                                     HALCallback_t CCWHandler,
                                     HALCallback_t SWL2HCallback,
                                     HALCallback_t SWH2LCallback ):
        _CW(CWPin,BSP::BtnPoll::BTN_PULL_DOWN,1,CW_L2H_Handler,CW_H2L_Handler), // for CW Debounce time for 1 msec works fine + 100nf and 1K RC debounce
        _CCW(CCWPin,BSP::BtnPoll::BTN_PULL_DOWN,10),                            // for CCW Debounce time for 8 ~ 10 msec works fine + 100nf and 1K RC debounce
        _SW(SWPin,BSP::BtnPoll::BTN_PULL_DOWN,0,SWH2LCallback,SWL2HCallback), 
        _CWCurrState(0),
        _CWHandler(CWHandler),
        _CCWHandler(CCWHandler),   
        _RotaryCurrState(NOT_ROTATING)
{
            
}

RotaryEncoderPoll::Status_t RotaryEncoderPoll::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CW.HwInit(); 
    _CCW.HwInit(); 
    _SW.HwInit(); 
    _CWCurrState = _CW.getState();
    return 1;
}

void RotaryEncoderPoll::RunStateMachine()
{  
    bool CW_New_State; 
    bool CCW_New_State; 

    
    _CW.RunStateMachine();
    
    _CCW.RunStateMachine();
    
    _SW.RunStateMachine();
    
    CW_New_State  =  _CW.getState(); 
     
    
    if(_CWCurrState == CW_New_State)
        return;
    
    CCW_New_State =  _CCW.Read();
    
    if ((CW_New_State == true))
    {     
        if((CCW_New_State == false))
        {
            _RotaryCurrState = ROTATING_CLOCKWISE;
            if(_CWHandler) _CWHandler();
            //LL_mDelay(10);
            
        }
        else
        {
            _RotaryCurrState = ROTATING_ANTI_CLOCKWISE;  
            if(_CCWHandler) _CCWHandler();
           // LL_mDelay(10);
        }
    }
    
    _CWCurrState = CW_New_State;
    
}

void RotaryEncoderPoll::CW_L2H_Handler()
{
    
}
    
 void RotaryEncoderPoll::CW_H2L_Handler()
{
    
}

}