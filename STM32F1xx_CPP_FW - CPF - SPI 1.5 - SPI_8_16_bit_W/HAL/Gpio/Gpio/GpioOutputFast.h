/******************
** CLASS: GpioOutputFast
**
** DESCRIPTION:
**  GpioOutput implementation using templates
**
** CREATED: 4/11/2019, by Amit Chaudhary
**
** FILE: GpioOutputFast.h
**
******************/
#ifndef GpioOutputFast_h
#define GpioOutputFast_h

#include "Gpio.h"

namespace HAL
{
    using Pin_t = Gpio::Pin_t;
    using Pull_t = GpioOutput::GpioOutput_Params_t;
    using Port_t = Gpio::Port_t;
    
    template <Pin_t  Pin, Pull_t Pull = GpioOutput::PUSH_PULL>
    class GpioOutputFast : public Gpio
    {
    public:
        const uint16_t PinMask = (uint16_t)(1U<<getPin(Pin));
        const Port_t Port = getPort(Pin);
        
        GpioOutputFast():Gpio(Pin)
        {            
        }
        
        ~GpioOutputFast()
        {
        }
        
    };
    
}

#endif //GpioOutputFast_h
