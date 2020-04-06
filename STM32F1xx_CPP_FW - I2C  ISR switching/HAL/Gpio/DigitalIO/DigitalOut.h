

/******************
** FILE: DigitalOut.h
**
** DESCRIPTION:
**  Gpio related functionality
**
** CREATED: 28/7/2019, by Amit Chaudhary
******************/
#ifndef DigitalOut_h
#define DigitalOut_h

#include"DigitalIO.h"

namespace HAL
{
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode = OUTPUT_OPEN_DRAIN_PUSH_PULL, uint32_t Speed = LL_GPIO_SPEED_FREQ_HIGH>
    class DigitalOut : public DigitalIO
    {
      
    public:
      
      void HwInit(){SetOutputMode(Port,Pin,Mode,Speed);}
      
      uint16_t GetPinmask(){return 1<<Pin;} 
      
      Port_t GetPort(){return Port;}          
      
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)Port)->ODR) & (1<<Pin) ); }
      
      void High(){((GPIO_TypeDef*)Port)->BSRR |= (1<<Pin);}
      
      void Low(){((GPIO_TypeDef*)Port)->BRR |= (1<<Pin);}
      
      void Toggle(){((GPIO_TypeDef*)Port)->ODR ^= (1<<Pin);}
      
      DigitalOut& operator= (uint32_t value) { value ? High() : Low() ; return *this; };
      
      DigitalOut& operator= (DigitalOut& rhs) { rhs.Read() ? High() : Low() ; return *this;} 
      
      operator bool() { return Read(); }
      
    };
  
//  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint32_t Speed>
//    void DigitalOut<Port,Pin,Mode,Speed>::HwInit()
//    {     
//      SetOutputMode(Port,Pin,Mode,Speed);     
//    }
  
//DigitalOut<C13> Led;
//  Led.HwInit();
// Led = ! Led;

//  DigitalOut<C13> Led;
//
// //DigitalIn<A8,INPUT_PULLUP,INTERRUPT_ON_FALLING,> Btn;
// DigitalIn<A11,INPUT_PULLDOWN> Btn;
// 
//   Btn.HwInit();
//   Led.HwInit();
//   
//         if(Btn.Read())
//       {
//         Led = 1;
//       }
//       else
//       {
//         Led = 0;
//       }
  
}


#endif // DigitalOut