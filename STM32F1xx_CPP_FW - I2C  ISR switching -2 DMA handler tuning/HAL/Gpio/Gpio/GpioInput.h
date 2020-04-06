/*
* GpioInput.h
*
*  Created on: 29-Dec-2019
*      Author: Amit Chaudhary
*  - 16-Jan-2020 - Optimised by removing data members 
*    and inputing directly in HwInit() func to save RAM
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
    
    
      GpioInput(Port_t Port, PIN_t Pin);
      
      void HwInit( Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, Callback_t Callback = nullptr);
      
      void SetInputMode(Mode_t Mode);
      
      void SetInterrupt();
      
      void SetEXTI_Intr(IntEdge_t Edge);
      
      void RegisterCallback(DigitalInCallback_t DigitalInCallback);
        
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)m_Port)->IDR) & m_PinMask ); }
      
      virtual void ISR();
      
    private:
      uint16_t  m_PinMask;
      Port_t    m_Port;
      uint8_t   m_Pin;
//      PIN_t     m_Pin;
//      Mode_t    m_Mode; 
//      IntEdge_t m_Edge;
      //Callback_t m_Callback;
    };
    
}// namespace Bsp

#endif /* GpioInput_h */
