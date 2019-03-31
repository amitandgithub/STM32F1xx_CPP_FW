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
            NOT_ROTATING,
            ROTATING_CLOCKWISE,
            ROTATING_ANTI_CLOCKWISE
        }RotaryState_t;
        
        RotaryEncoderPoll(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                          HALCallback_t CWHandler = nullptr, 
                          HALCallback_t CCWHandler = nullptr,
                          HALCallback_t SWL2HCallback = nullptr, 
                          HALCallback_t SWH2LCallback = nullptr);
        
        ~RotaryEncoderPoll(){};
        
        Status_t            HwInit(void *pInitStruct = nullptr);
        
        void                RunStateMachine();
        
        RotaryState_t       getRotaryState();
        
        bool                getButtonState();
        
        void                RegisterHandler(HALCallback_t CW_Handler, HALCallback_t CCW_Handler, HALCallback_t SWH2LCallback, HALCallback_t SWL2HCallback);
        
        static void         CW_L2H_Handler();
        
        static void         CW_H2L_Handler();
    private:
        BtnPoll             _CW;
        BtnPoll             _CCW;
        BtnPoll             _SW;  
        HALCallback_t       _CWHandler;
        HALCallback_t       _CCWHandler;
        bool                _CWCurrState;
        RotaryState_t       _RotaryCurrState;   
        
    };
    // This function implements Clear-On-Read on Rotary Encoder State
    inline RotaryEncoderPoll::RotaryState_t RotaryEncoderPoll::getRotaryState()
    {
        RotaryState_t returnState = _RotaryCurrState;
        _RotaryCurrState = NOT_ROTATING;
        return returnState;
    }
    
    inline bool RotaryEncoderPoll::getButtonState()
    {
        return _SW.getState();
    }
    inline void RotaryEncoderPoll::RegisterHandler(HALCallback_t CW_Handler, HALCallback_t CCW_Handler, HALCallback_t SWH2LCallback, HALCallback_t SWL2HCallback)
    {
        _CWHandler     = CW_Handler;
        _CCWHandler    = CCW_Handler;
        _SW.RegisterHandler(SWH2LCallback,SWL2HCallback);
    }
#endif //RotaryEncoderPoll_h
}
