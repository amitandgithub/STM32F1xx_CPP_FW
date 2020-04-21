

/******************
** FILE: DigitalIO.h
**
** DESCRIPTION:
**  Gpio related functionality
**
** CREATED: 16/3/2020, by Amit Chaudhary
******************/
#ifndef DigitalIO_h
#define DigitalIO_h


#include"chip_peripheral.h"
#include"InterruptManager.h"
#include"CPP_HAL.h"

#include"DigitalInOut.h"

extern HAL::InterruptManager InterruptManagerInstance;

namespace HAL
{ 
  
  class DigitalIO
  {  
    
  public:
    
    static void SetSpeed(Port_t port,PIN_t pin,uint32_t speed){LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,get_LL_pin(pin),speed);}  
    
    static IRQn_Type Gpio_get_pin_IRQn(uint8_t pin);
    
    static uint32_t get_LL_pin(uint8_t pin);
    
    static void Gpio_ClockEnable(Port_t GPIOx);
    
    static void Gpio_ClockDisable(Port_t GPIOx);
    
    static void SetInputMode(Port_t port, PIN_t pin, Mode_t mode,HAL::InterruptSource* This = nullptr,IntEdge_t Edge = NO_INTERRUPT, Callback_t Callback = nullptr);
    
    static void SetOutputMode(Port_t port, PIN_t pin,Mode_t mode, uint32_t speed);
    
    static void SetAlternateFunction(Port_t port, PIN_t pin,uint32_t AltFun);
      
    static void RegisterCallback(PIN_t pin,IntEdge_t edge,HAL::Callback* DigitalInCallback);
    
    static void SetEXTI_Intr(PIN_t pin,IntEdge_t edge);
    
    static void DispatchCallback();
    
    static HAL::Callback* g_GpioCallbacks[16];    
  };
}

#endif // DigitalInOut_h