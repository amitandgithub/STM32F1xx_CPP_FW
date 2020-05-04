/*
* DigitalIO.cpp
*
*  Created on: 08-Oct-2017
*  - Added the CPF related changes - 31-March-2019
*  - 16-Jan-2020 - Optimised by removing data members 
*    and inputing directly in HwInit() func to save RAM
* - 16-Mar-2020 - Added base class for common functions
*      Author: Amit Chaudhary
*/


#include "DigitalIO.h"

namespace HAL
{   
  Callback* HAL::DigitalIO::g_GpioCallbacks[16];  
  
  IRQn_Type DigitalIO::Gpio_get_pin_IRQn(uint8_t pin)
  {                          
    IRQn_Type LL_PINS[16] = 
    {
      EXTI0_IRQn ,
      EXTI1_IRQn,  
      EXTI2_IRQn,
      EXTI3_IRQn,
      EXTI4_IRQn,
      EXTI9_5_IRQn,
      EXTI9_5_IRQn,
      EXTI9_5_IRQn,
      EXTI9_5_IRQn,
      EXTI9_5_IRQn,
      EXTI15_10_IRQn ,
      EXTI15_10_IRQn ,
      EXTI15_10_IRQn ,
      EXTI15_10_IRQn ,
      EXTI15_10_IRQn ,
      EXTI15_10_IRQn ,       
    };
    
    return LL_PINS[pin];  
  }
  
  //    template<PIN_t _pin>
  //    IRQn_Type Gpio_get_pin_IRQn()
  //    {
  //        IRQn_Type LL_IRQns[16] = 
  //        {
  //            EXTI0_IRQn ,
  //            EXTI1_IRQn,  
  //            EXTI2_IRQn,
  //            EXTI3_IRQn,
  //            EXTI4_IRQn,
  //            EXTI9_5_IRQn,
  //            EXTI9_5_IRQn,
  //            EXTI9_5_IRQn,
  //            EXTI9_5_IRQn,
  //            EXTI9_5_IRQn,
  //            EXTI15_10_IRQn ,
  //            EXTI15_10_IRQn ,
  //            EXTI15_10_IRQn ,
  //            EXTI15_10_IRQn ,
  //            EXTI15_10_IRQn ,
  //            EXTI15_10_IRQn ,       
  //        };
  //        
  //        return LL_IRQns[_pin]; 
  //    }
  
  uint32_t DigitalIO::get_LL_pin(uint8_t pin)
  {
    uint32_t LL_PINS[16] = 
    {
      LL_GPIO_PIN_0,
      LL_GPIO_PIN_1,
      LL_GPIO_PIN_2,
      LL_GPIO_PIN_3,
      LL_GPIO_PIN_4,
      LL_GPIO_PIN_5,
      LL_GPIO_PIN_6,
      LL_GPIO_PIN_7,
      LL_GPIO_PIN_8,
      LL_GPIO_PIN_9,
      LL_GPIO_PIN_10,
      LL_GPIO_PIN_11,
      LL_GPIO_PIN_12,
      LL_GPIO_PIN_13,
      LL_GPIO_PIN_14,
      LL_GPIO_PIN_15        
    };
    
    return LL_PINS[pin];                                
  }
 
  void DigitalIO::SetInputMode(Port_t port, PIN_t pin, Mode_t mode, HAL::InterruptSource* This, IntEdge_t Edge, Callback_t Callback)
  {
    Gpio_ClockEnable(port);
    
    uint32_t LLpin = get_LL_pin(pin);
    
    switch(mode)
    {
    case INPUT_FLOATING:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_FLOATING);
      break;
    case ANALOG:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_ANALOG);
      break;
    case INPUT_PULLUP:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_INPUT);
      LL_GPIO_SetPinPull((GPIO_TypeDef*)port,LLpin,LL_GPIO_PULL_UP);
      break;
    case INPUT_PULLDOWN:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_INPUT);
      LL_GPIO_SetPinPull((GPIO_TypeDef*)port,LLpin,LL_GPIO_PULL_DOWN);
      break;
    default: break;  
    }  
    
    if(Callback)
    {
      InterruptManagerInstance.RegisterDeviceInterrupt(Gpio_get_pin_IRQn(pin),0,This);
      RegisterCallback(pin,Edge,Callback);
    } 
    
  }
  
  void DigitalIO::SetOutputMode(Port_t port, PIN_t pin,Mode_t mode, uint32_t speed)
  {      
    uint32_t LLpin = get_LL_pin(pin);
    
    Gpio_ClockEnable(port);
    
    switch(mode)
    {
    case OUTPUT_OPEN_DRAIN:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case OUTPUT_OPEN_DRAIN_PUSH_PULL:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_OPEN_DRAIN_FAST:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_AF_PUSH_PULL:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_AF_OPEN_DRAIN:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetPinSpeed((GPIO_TypeDef*)port,LLpin,speed);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case ANALOG:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)port,LLpin,LL_GPIO_MODE_ANALOG);
    default: break;  
    }  
  }
  
  void DigitalIO::Gpio_ClockEnable(Port_t _GPIOx)
  {   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif
  }
  void DigitalIO::Gpio_ClockDisable(Port_t _GPIOx)
  {   
  if((GPIO_TypeDef*)_GPIOx == GPIOA )
  {
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOA);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOB )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOB);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOC )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOC);
  }
  else  if((GPIO_TypeDef*)_GPIOx == GPIOD )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOD);
  }
#if defined(GPIOE)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOE )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOE);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOE);
  }
#endif
#if defined(GPIOF)
  else  if((GPIO_TypeDef*)_GPIOx == GPIOF )
  {
    //LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOG);
    HAL::ClockManager::Disable(HAL::ClockManager::CLOCK_GPIOG);
  }
#endif 
    
  }
  
  
      void DigitalIO::SetEXTI_Intr(PIN_t pin,IntEdge_t edge)
      {      
        LL_EXTI_DisableEvent_0_31((1<<pin));
        LL_EXTI_EnableIT_0_31((1<<pin));
        
        switch(edge)
        {
        case INTERRUPT_ON_RISING:
          LL_EXTI_DisableFallingTrig_0_31((1<<pin));
          LL_EXTI_EnableRisingTrig_0_31((1<<pin));
          break;
        case INTERRUPT_ON_FALLING:
          LL_EXTI_DisableRisingTrig_0_31((1<<pin));
          LL_EXTI_EnableFallingTrig_0_31((1<<pin));
          break;
        case INTERRUPT_ON_RISING_FALLING:
          LL_EXTI_EnableFallingTrig_0_31((1<<pin));
          LL_EXTI_EnableRisingTrig_0_31((1<<pin));
          break;
        default: break;  
        }  
      }

    void DigitalIO::RegisterCallback(PIN_t pin,IntEdge_t edge,HAL::Callback* DigitalInCallback)
    {
      g_GpioCallbacks[pin] = DigitalInCallback;
      
      SetEXTI_Intr(pin,edge);      
    }
    
  void DigitalIO::DispatchCallback()
  {
          // if we don't check EXTI->PR for 0,
        // in that case POSITION_VAL(EXTI->PR) will return 32, which will
        // try to access with 32 index g_GpioCallbacks[ExternalInterrupt]
        // and will crash because array is only 16 elements
        if(EXTI->PR)
        {
          uint32_t ExternalInterrupt = (uint32_t)POSITION_VAL(EXTI->PR);               
          if(g_GpioCallbacks[ExternalInterrupt]) g_GpioCallbacks[ExternalInterrupt]->CallbackFunction();                
          LL_EXTI_ClearFlag_0_31( (uint32_t)((uint32_t)1U)<<ExternalInterrupt);
        }
  }
  
  
}