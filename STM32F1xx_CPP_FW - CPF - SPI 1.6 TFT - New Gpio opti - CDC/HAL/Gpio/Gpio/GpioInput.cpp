/*
* GpioInput.cpp
*
*  Created on: 08-Oct-2017
*  - Added the CPF related changes - 31-March-2019
*      Author: Amit Chaudhary
*/


#include "GpioInput.h"
#include"DigitalInOut.h"

namespace HAL
{
    
    GpioInput::GpioInput(Port_t Port, PIN_t Pin, Mode_t Mode, IntEdge_t Edge, Callback_t Callback)
                        :m_Port(Port),m_PinMask(1<<Pin),m_Mode(Mode),m_Edge(Edge),m_Callback(Callback)
        {
            g_GpioCallbacks[Pin] = m_Callback;
        }
        
       
      void GpioInput::HwInit()
      {
        Gpio_ClockEnable(m_Port);

        SetInputMode();  
        
        if(m_Callback)
        {
          RegisterCallback(m_Callback);
        }      
      }     
    
      void GpioInput::SetEXTI_Intr()
      {      
        LL_EXTI_DisableEvent_0_31(m_PinMask);
        LL_EXTI_EnableIT_0_31(m_PinMask);
        
        switch(m_Edge)
        {
        case INTERRUPT_ON_RISING:
          LL_EXTI_DisableFallingTrig_0_31(m_PinMask);
          LL_EXTI_EnableRisingTrig_0_31(m_PinMask);
          break;
        case INTERRUPT_ON_FALLING:
          LL_EXTI_DisableRisingTrig_0_31(m_PinMask);
          LL_EXTI_EnableFallingTrig_0_31(m_PinMask);
          break;
        case INTERRUPT_ON_RISING_FALLING:
          LL_EXTI_EnableFallingTrig_0_31(m_PinMask);
          LL_EXTI_EnableRisingTrig_0_31(m_PinMask);
          break;
        default: break;  
        }  
      }  
    
      void GpioInput::SetInputMode()
      {      
         uint32_t LLpin = get_LL_pin(POSITION_VAL(m_PinMask));
         
        switch(m_Mode)
        {
        case INPUT_FLOATING:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_FLOATING);  
          break;
        case ANALOG:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_ANALOG);
          break;
        case INPUT_PULLUP:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_INPUT);
          LL_GPIO_SetPinPull((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_PULL_UP);
          break;
        case INPUT_PULLDOWN:
          LL_GPIO_SetPinMode((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_MODE_INPUT);
          LL_GPIO_SetPinPull((GPIO_TypeDef*)m_Port,LLpin,LL_GPIO_PULL_DOWN);
          break;
        default: break;  
        }  
      }
    
      void GpioInput::ISR()
      {
        // if we don't check EXTI->PR for 0,
        // in that case POSITION_VAL(EXTI->PR) will return 32, which will
        // try to access with 32 index _GpioCallbacks[ExternalInterrupt]
        // and will crash because array is only 16 elements
        if(EXTI->PR)
        {
          uint32_t ExternalInterrupt = (uint32_t)POSITION_VAL(EXTI->PR);               
          g_GpioCallbacks[ExternalInterrupt]->CallbackFunction();                
          LL_EXTI_ClearFlag_0_31( (uint32_t)((uint32_t)1U)<<ExternalInterrupt);
        }
      }
    
      void GpioInput::RegisterCallback(DigitalInCallback_t DigitalInCallback)
      {
        g_GpioCallbacks[POSITION_VAL(m_PinMask)] = DigitalInCallback;
        
        SetEXTI_Intr();
        
        InterruptManagerInstance.RegisterDeviceInterrupt(Gpio_get_pin_IRQn(POSITION_VAL(m_PinMask)),0,this);
      }        
}
