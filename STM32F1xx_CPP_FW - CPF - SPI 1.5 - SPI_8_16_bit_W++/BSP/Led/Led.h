/******************
** CLASS: Led
**
** DESCRIPTION:
**  LED class functionality
**
** CREATED: 31/10/2019, by Amit Chaudhary
**
** FILE: Led.h
**
******************/
#ifndef Led_h
#define Led_h

#include"DigitalOut.h"

namespace BSP
{
  
  using namespace HAL; 
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode = OUTPUT_OPEN_DRAIN_PUSH_PULL>
    class Led 
    {      
    public:      
      void HwInit(){ m_LedPin.HwInit();}
      
      void HwDeinit();
      
      void On(){ m_LedPin.High();}
      
      void Off(){ m_LedPin.Low();}
      
      void Toggle(){ m_LedPin.Toggle();}
      
      bool IsOn(){ return m_LedPin.ReadOutput() ? true : false;}
      
      void Blink(uint32_t on = 500U, uint32_t off= 500U, uint32_t ntimes = 1);
      
      void millisec_delay(uint32_t millis){LL_mDelay(millis); }
      
    private:      
      DigitalOut<Port,Pin,Mode> m_LedPin;      
    };
  
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode>
    void Led<Port,Pin,Mode>::Blink(uint32_t on, uint32_t off, uint32_t ntimes )
    {
      for(uint32_t i = 0U; i<ntimes; i++ )
      {
        On();
        millisec_delay(on);
        Off();
        millisec_delay(off);
      }
    }
  
  
}
#endif //Led.h
