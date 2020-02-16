/******************
** FILE: GpioOutput.cpp
**
** DESCRIPTION:
**  GpioOutput class Implementation
**
** CREATED: 1/17/2019, by Amit Chaudhary
******************/

#include "GpioOutput.h"

namespace HAL
{ 
  GpioOutput::GpioOutput(Port_t Port, PIN_t Pin): m_Port(Port),m_PinMask(1<<Pin),m_Pin(Pin)
  {
    
  }
  
  void GpioOutput::HwInit(Mode_t Mode,uint32_t speed)
  {
//     uint32_t LLpin = get_LL_pin(POSITION_VAL(m_PinMask));
     uint32_t LLpin = get_LL_pin(m_Pin);
     
    Gpio_ClockEnable(m_Port);
    
    switch(Mode)
    {
    case OUTPUT_OPEN_DRAIN:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case OUTPUT_OPEN_DRAIN_PUSH_PULL:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_OPEN_DRAIN_FAST:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_OUTPUT);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_AF_PUSH_PULL:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_PUSHPULL);
      break;
    case OUTPUT_AF_OPEN_DRAIN:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_ALTERNATE);
      LL_GPIO_SetPinOutputType((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_OUTPUT_OPENDRAIN);
      break;
    case ANALOG:
      LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_ANALOG);
    default: break;  
    }
    
    LL_GPIO_SetPinSpeed((GPIO_TypeDef*)m_Port,LLpin,speed);
  }
}