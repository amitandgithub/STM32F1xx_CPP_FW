

/******************
** FILE: GpioOutput.h
**
** DESCRIPTION:
**  Gpio related functionality
**
** CREATED: 28/12/2019, by Amit Chaudhary
******************/
#ifndef GpioOutput_h
#define GpioOutput_h

#include"DigitalInOut.h"

namespace HAL
{
  
    class GpioOutput
    {
      
    public:
      
      GpioOutput(Port_t Port, PIN_t Pin, Mode_t Mode = OUTPUT_OPEN_DRAIN_PUSH_PULL);
      
      void HwInit(); 
      
      uint16_t GetPinmask(){return m_PinMask;} 
      
      Port_t GetPort(){return m_Port;} 
      
      void SetSpeed(uint32_t speed){LL_GPIO_SetPinSpeed((GPIO_TypeDef*)m_Port,get_LL_pin(m_Pin),speed);} 
      
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)m_Port)->ODR) & m_PinMask ); }
      
      void High(){((GPIO_TypeDef*)m_Port)->BSRR |= m_PinMask;}
      
      void Low(){((GPIO_TypeDef*)m_Port)->BRR |= m_PinMask;}
      
      void Toggle(){((GPIO_TypeDef*)m_Port)->ODR ^= m_PinMask;}
      
      GpioOutput& operator= (uint32_t value) { value ? High() : Low() ; return *this; };
      
      GpioOutput& operator= (GpioOutput& rhs) { rhs.Read() ? High() : Low() ; return *this;} 
      
      operator bool() { return Read(); }
      
    private:// 8 bytes      
      Port_t m_Port;
      uint16_t m_PinMask;
      uint8_t m_Pin;
      Mode_t m_Mode;     
    };
  
}


#endif // DigitalOut