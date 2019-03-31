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

RotaryEncoderPoll::RotaryEncoderPoll(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,BtnHandler_t CW_Handler, BtnHandler_t CCW_Handler, BtnHandler_t SW_Low_Handler, BtnHandler_t SW_High_Handler ):
        _CW(CWPin),
        _CCW(CCWPin),
        _SW(SWPin,BtnPoll::BTN_PULL_DOWN,0,SW_Low_Handler,SW_High_Handler),
        _CW_Handler(CW_Handler),
        _CCW_Handler(CCW_Handler),
        _RotaryState(ROTATING_CLOCKWISE)
{
            
}

RotaryEncoderPoll::Status_t RotaryEncoderPoll::HwInit(void *pInitStruct)
{
    (void)pInitStruct;
    _CW.HwInit(); 
    _CCW.HwInit(); 
    _SW.HwInit(); 
    //_CW.RegisterHandler(,nullptr);
    return 1;
}

void RotaryEncoderPoll::RunStateMachine()
{      
  _CW.RunStateMachine();
  _CCW.RunStateMachine();
  _SW.RunStateMachine();
  if ((_CW.getState() == true) && (_CCW.getState() == false))
  {
      _RotaryState = ROTATING_CLOCKWISE;
      _CW_Handler();
  }
  else  if((_CCW.getState() == true) && (_CW.getState() == false))
  {
      _RotaryState = ROTATING_ANTI_CLOCKWISE;
      _CCW_Handler();
  }
} 

//void RotaryEncoderPoll::_CW_High_Handler()
//{
//    if(_CCW.ReadState())
//    {
//        if(_CW_Handler) _CW_Handler();
//    }
//    else
//    {
//        if(_CCW_Handler) _CCW_Handler();
//    }
//}
    
    
}