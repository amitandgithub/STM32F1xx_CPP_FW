/******************
** CLASS: Led
**
** DESCRIPTION:
**  LED class functionality
**
** CREATED: 1/26/2019, by Amit Chaudhary
**
** FILE: Led.h
**
******************/
#ifndef Led_h
#define Led_h

#include "Gpio.h"
#include "GpioOutput.h"

namespace BSP
{
    
using namespace HAL; 

class Led 
{
    
public:
    using Pin_t = HAL::Gpio::Pin_t;
    using GpioOutput_Params_t =  HAL::GpioOutput::GpioOutput_Params_t;
    using Status_t = uint32_t;
    
    constexpr Led(Gpio::Pin_t Pin) ;    
    
    ~Led(){};
    
    Status_t HwInit(void *pInitStruct = nullptr);
    
    Status_t HwDeinit();
    
    void On();
    
    void Off();
    
    void Toggle();
    
    bool IsOn();
    
   void Blink(uint32_t on = 500U, uint32_t off= 500U, uint32_t ntimes = 1);
    
    
   
private:
    void _delay(uint32_t millis);
    GpioOutput _Led;
  
};


constexpr  Led::Led(Gpio::Pin_t Pin) : _Led(Pin)
{
    
}

inline Led::Status_t Led::HwInit(void *pInitStruct)
{
    return _Led.HwInit();
}

inline void Led::On()
{
    _Led.High();
}

inline void Led::Off()
{
    _Led.Low();
}

inline void Led::Toggle()
{
    _Led.Toggle();
}

inline bool Led::IsOn()
{
    return _Led.ReadOutput() ? true : false;
}

inline void Led::Blink(uint32_t on, uint32_t off, uint32_t ntimes )
{
    for(auto i = 0U; i<ntimes; i++ )
    {
        On();
        _delay(on);
        Off();
        _delay(off);
    }
}

inline void Led::_delay(uint32_t millis)
{
    LL_mDelay(millis); 
}

}
#endif //Led.h
