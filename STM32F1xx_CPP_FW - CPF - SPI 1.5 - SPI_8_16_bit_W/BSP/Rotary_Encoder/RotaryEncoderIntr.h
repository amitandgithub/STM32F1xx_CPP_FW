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
        typedef Callback* Callback_t;
        using Status_t = uint32_t;
        using Pin_t = HAL::Gpio::Pin_t;
        
        class CWCallback : public Callback
        {
            public:
            CWCallback(RotaryEncoderIntr* This):_This(This){};
            virtual void CallbackFunction();
            private:
            RotaryEncoderIntr* _This;
        };
        
        class SWCallback : public Callback
        {
        public:
            SWCallback(RotaryEncoderIntr* This):_This(This){};
            virtual void CallbackFunction();
        private:
            RotaryEncoderIntr* _This;
        };
        
        RotaryEncoderIntr(Pin_t CWPin,Pin_t CCWPin,Pin_t SWPin,
                          Callback_t CWHandler = nullptr, 
                          Callback_t CCWHandler = nullptr,
                          Callback_t SWL2HCallback = nullptr, 
                          Callback_t SWH2LCallback = nullptr);
        
        ~RotaryEncoderIntr(){};
        
        Status_t            HwInit(void *pInitStruct = nullptr);
        
        void                RegisterHandler(Callback_t CW_Handler, Callback_t CCW_Handler, Callback_t SWL2HCallback , Callback_t SWH2LCallback);
        
    private:
        GpioInput           _CWPin;
        GpioInput           _CCWPin;
        GpioInput           _SWPin;  
        Callback_t          _CWHandler;
        Callback_t          _CCWHandler;
        Callback_t          _SW_L2H_Handler;
        Callback_t          _SW_H2L_Handler;
        CWCallback          _CWCallback;
        SWCallback          _SWCallback;      
    };

#endif //RotaryEncoderIntr_h
}
