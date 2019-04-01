/******************
** CLASS: RotaryEncoderIntr
**
** DESCRIPTION:
**  Implements the Rotary Encoder class using polling
**
** CREATED: 2/3/2019, by Amit Chaudhary
**
** FILE: RotaryEncoderIntr.h
**
******************/
#ifndef RotaryEncoderIntr_h
#define RotaryEncoderIntr_h

#include "GpioInput.h"
#include "Callback.h"

using namespace HAL;

namespace BSP
{   
      
    
    class RotaryEncoderIntr
    {
    public:
        using Status_t = uint32_t;
        using Pin_t = HAL::Gpio::Pin_t;
        typedef void(*BtnHandler_t)();
        class CWCallback : public Callback
        {
            virtual void CallbackFunction();
        };
        
        class SWCallback : public Callback
        {
            virtual void CallbackFunction();
        };
        
        typedef enum
        {
            NOT_ROTATING,
            ROTATING_CLOCKWISE,
            ROTATING_ANTI_CLOCKWISE
        }RotaryState_t;
        
        RotaryEncoderIntr(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                          HALCallback_t CWHandler = nullptr, 
                          HALCallback_t CCWHandler = nullptr,
                          HALCallback_t SWL2HCallback = nullptr, 
                          HALCallback_t SWH2LCallback = nullptr);
        
        ~RotaryEncoderIntr(){};
        
        Status_t            HwInit(void *pInitStruct = nullptr);
        
        RotaryState_t       getRotaryState();
        
        bool                getButtonState();
        
        void                RegisterHandler(HALCallback_t CW_Handler, HALCallback_t CCW_Handler, HALCallback_t SWH2LCallback, HALCallback_t SWL2HCallback);
        
        void         CW_L2H_Handler();
        
        void         CW_H2L_Handler();
    private:
        GpioInput             _CWPin;
        GpioInput             _CCWPin;
        GpioInput             _SWPin;  
        HALCallback_t       _CWHandler;
        HALCallback_t       _CCWHandler;
        bool                _CWCurrState;
        RotaryState_t       _RotaryCurrState; 
        CWCallback          _CWCallback;
        SWCallback          _SWCallback;
        
    };
    // This function implements Clear-On-Read on Rotary Encoder State
    inline RotaryEncoderIntr::RotaryState_t RotaryEncoderIntr::getRotaryState()
    {
        RotaryState_t returnState = _RotaryCurrState;
        _RotaryCurrState = NOT_ROTATING;
        return returnState;
    }
    
    inline bool RotaryEncoderIntr::getButtonState()
    {
        return true;
    }
    inline void RotaryEncoderIntr::RegisterHandler(HALCallback_t CW_Handler, HALCallback_t CCW_Handler, HALCallback_t SWH2LCallback, HALCallback_t SWL2HCallback)
    {
        _CWHandler     = CW_Handler;
        _CCWHandler    = CCW_Handler;
        //_SW.RegisterHandler(SWH2LCallback,SWL2HCallback);
    }
#endif //RotaryEncoderIntr_h
}
