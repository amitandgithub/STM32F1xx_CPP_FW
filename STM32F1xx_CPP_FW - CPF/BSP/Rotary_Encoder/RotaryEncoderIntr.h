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
    class RotaryEncoderIntr : public Callback
    {
    public:
        typedef Callback* Callback_t;
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
                          Callback_t CWHandler = nullptr, 
                          Callback_t CCWHandler = nullptr,
                          Callback_t SWL2HCallback = nullptr, 
                          Callback_t SWH2LCallback = nullptr);
        
        ~RotaryEncoderIntr(){};
        
        Status_t            HwInit(void *pInitStruct = nullptr);
        
        RotaryState_t       getRotaryState();
        
        bool                getButtonState();
        
        void                RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWH2LCallback, Callback_t SWL2HCallback);
        
        virtual void CallbackFunction();
    private:
        GpioInput           _CWPin;
        GpioInput           _CCWPin;
        GpioInput           _SWPin;  
        Callback_t          _CWHandler;
        Callback_t          _CCWHandler;
        Callback_t          _SW_L2H_Handler;
        Callback_t          _SW_H2L_Handler;
        RotaryState_t       _RotaryCurrState; 
        bool                _CWPinState;
        bool                _SWPinState;
        
    };

    inline RotaryEncoderIntr::RotaryState_t RotaryEncoderIntr::getRotaryState()
    {
        return _RotaryCurrState;
    }
    
    inline bool RotaryEncoderIntr::getButtonState()
    {
        return _SWPinState;
    }
    inline void RotaryEncoderIntr::RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWH2LCallback, Callback_t SWL2HCallback)
    {
        _CWHandler     = CW_Handler;
        _CCWHandler    = CCW_Handler;
        //_SW.RegisterHandler(SWH2LCallback,SWL2HCallback);
    }
#endif //RotaryEncoderIntr_h
}
