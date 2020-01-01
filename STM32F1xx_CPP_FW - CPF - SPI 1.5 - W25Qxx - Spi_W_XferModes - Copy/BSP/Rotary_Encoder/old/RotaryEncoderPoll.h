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
    
    class RotaryEncoderPoll : public Callback
    {
    public:
        using Status_t = uint32_t;
        using Pin_t = HAL::Gpio::Pin_t;
        typedef Callback* Callback_t;   
        
        typedef enum
        {
            NOT_ROTATING,
            ROTATING_CLOCKWISE,
            ROTATING_ANTI_CLOCKWISE
        }RotaryState_t;
        
        RotaryEncoderPoll(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                          Callback_t CWHandler = nullptr, 
                          Callback_t CCWHandler = nullptr,
                          Callback_t SWL2HCallback = nullptr, 
                          Callback_t SWH2LCallback = nullptr);
        
        ~RotaryEncoderPoll(){};
        
        Status_t            HwInit(void *pInitStruct = nullptr);
        
        void                Run();
        
        RotaryState_t       getRotaryState();
        
        bool                getButtonState();
        
        void                RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWH2LCallback, Callback_t SWL2HCallback);
        
        virtual void CallbackFunction();
        
    private:
        BtnPoll             _CW;
        BtnPoll             _CCW;
        BtnPoll             _SW;  
        Callback_t       _CWHandler;
        Callback_t       _CCWHandler;
       // bool                _CWCurrState;
       // RotaryState_t       _RotaryCurrState;   
        
    };
    // This function implements Clear-On-Read on Rotary Encoder State
//    inline RotaryEncoderPoll::RotaryState_t RotaryEncoderPoll::getRotaryState()
//    {
//        //RotaryState_t returnState = _RotaryCurrState;
//        _RotaryCurrState = NOT_ROTATING;
//        return returnState;
//    }
    
    inline bool RotaryEncoderPoll::getButtonState()
    {
        return _SW.getState();
    }
    inline void RotaryEncoderPoll::RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWH2LCallback, Callback_t SWL2HCallback)
    {
        _CWHandler     = CW_Handler;
        _CCWHandler    = CCW_Handler;
        _SW.RegisterHandler(SWH2LCallback,SWL2HCallback);
    }
}
#endif //RotaryEncoderPoll_h

