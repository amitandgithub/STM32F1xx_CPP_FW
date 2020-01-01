/*
* GpioInput.h
*
*  Created on: 29-Dec-2019
*      Author: Amit Chaudhary
*/

#ifndef GpioInput_h
#define GpioInput_h

#include "DigitalInOut.h"


namespace HAL
{  
  
    class GpioInput : public InterruptSource 
    {
    public:
      
    typedef HAL::Callback* DigitalInCallback_t;
    
    
      GpioInput(Port_t Port, PIN_t Pin, Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, Callback_t Callback = nullptr);
      
      void HwInit();
      
      void SetInputMode();
      
      void SetInterrupt();
      
      void SetEXTI_Intr();
      
      void RegisterCallback(DigitalInCallback_t DigitalInCallback);
        
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)m_Port)->IDR) & m_PinMask ); }
      
      virtual void ISR();
      
    private: // 16 bytes
      uint16_t  m_PinMask;
      Mode_t    m_Mode; 
      IntEdge_t m_Edge;
      Port_t    m_Port;
      Callback_t m_Callback;
    };
    
}// namespace Bsp

#endif /* GpioInput_h */
