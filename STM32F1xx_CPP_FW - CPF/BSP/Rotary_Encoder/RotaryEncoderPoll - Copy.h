/******************
** CLASS: RotaryEncoderPoll
**
** DESCRIPTION:
**  Implements the Rotary Encoder class using polling
**
** CREATED: 2/3/2019, by Amit Chaudhary
**
** FILE: RotaryEncoderPoll.h
**
******************/
#ifndef RotaryEncoderPoll_h
#define RotaryEncoderPoll_h

#include "BtnPoll.h"

namespace BSP
{
    
using namespace HAL;  

class RotaryEncoderPoll
{
public:
    using Status_t = uint32_t;
    using Pin_t = HAL::Gpio::Pin_t;
    typedef void(*BtnHandler_t)();
    
    typedef enum
    {
        ROTATING_CLOCKWISE,
        ROTATING_ANTI_CLOCKWISE
    }RotaryState_t;
    
                        RotaryEncoderPoll(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,BtnHandler_t CW_Handler = nullptr, 
                                          BtnHandler_t CCW_Handler = nullptr, BtnHandler_t SW_Low_Handler = nullptr,
                                          BtnHandler_t SW_High_Handler = nullptr);
                        
                        ~RotaryEncoderPoll(){};
                        
    Status_t            HwInit(void *pInitStruct = nullptr);
    
    void                RegisterHandler(BtnHandler_t CW_Handler, BtnHandler_t CCW_Handler, BtnHandler_t SW_Low_Handler, BtnHandler_t SW_High_Handler);
    
    void                RunStateMachine();
    
    RotaryState_t       getRotaryState();
    
    bool                getButtonState();
    
private:
    //void(RotaryEncoderPoll::*Fptr_)() = &RotaryEncoderPoll::_CW_High_Handler;
    BtnPoll             _CW;
    BtnPoll             _CCW;
    BtnPoll             _SW;    
    BtnHandler_t        _CW_Handler;
    BtnHandler_t        _CCW_Handler;
    RotaryState_t       _RotaryState;   
    
};

inline RotaryEncoderPoll::RotaryState_t RotaryEncoderPoll::getRotaryState()
{
    return _RotaryState;
}

inline bool RotaryEncoderPoll::getButtonState()
{
    return _SW.getState();
}

inline void RotaryEncoderPoll::RegisterHandler(BtnHandler_t CW_Handler, BtnHandler_t CCW_Handler, BtnHandler_t SW_Low_Handler, BtnHandler_t SW_High_Handler)
{
    _CW_Handler     = CW_Handler;
    _CCW_Handler    = CCW_Handler;
    _SW.RegisterHandler(SW_Low_Handler,SW_High_Handler);
}
#endif //RotaryEncoderPoll_h
}
