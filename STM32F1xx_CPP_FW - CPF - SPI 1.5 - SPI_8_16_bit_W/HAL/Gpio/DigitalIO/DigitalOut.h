

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

#include"DigitalInOut.h"

namespace HAL
{
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode = OUTPUT_OPEN_DRAIN_PUSH_PULL, uint32_t Speed = LL_GPIO_SPEED_FREQ_LOW>
    class DigitalOut
    {
      
    public:
      
      void HwInit();
      
      uint16_t GetPinmask(){return Gpio_get_pin_pos<Pin>();} 
      
      Port_t GetPort(){return Port;} 
      
      void SetSpeed(uint32_t speed){LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,speed);} 
      
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)Port)->ODR) & Gpio_get_pin_pos<Pin>() ); }
      
      void High(){((GPIO_TypeDef*)Port)->BSRR |= Gpio_get_pin_pos<Pin>();}
      
      void Low(){((GPIO_TypeDef*)Port)->BRR |= Gpio_get_pin_pos<Pin>();}
      
      void Toggle(){((GPIO_TypeDef*)Port)->ODR ^= Gpio_get_pin_pos<Pin>();}
      
      DigitalOut& operator= (uint32_t value) { value ? High() : Low() ; return *this; };
      
      DigitalOut& operator= (DigitalOut& rhs) { rhs.Read() ? High() : Low() ; return *this;} 
      
      operator bool() { return Read(); }
      
    };
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint32_t Speed>
    void DigitalOut<Port,Pin,Mode,Speed>::HwInit()
    {
      Gpio_ClockEnable<Port>();
      
      switch(Mode)
      {
      case OUTPUT_OPEN_DRAIN:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_OPENDRAIN);
        break;
      case OUTPUT_OPEN_DRAIN_PUSH_PULL:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_PUSHPULL);
        break;
      case OUTPUT_OPEN_DRAIN_FAST:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_OPENDRAIN);
        break;
      case OUTPUT_OPEN_DRAIN_PUSH_PULL_FAST:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_PUSHPULL);
         break;
    case OUTPUT_AF_PUSH_PULL:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_PUSHPULL);
         break;
    case OUTPUT_AF_OPEN_DRAIN:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinSpeed((GPIO_TypeDef*)Port,Pin,Speed);
        LL_GPIO_SetPinOutputType((GPIO_TypeDef*)Port,Pin,LL_GPIO_OUTPUT_OPENDRAIN);
         break;
    case ANALOG:
        LL_GPIO_SetPinMode((GPIO_TypeDef*)Port,Pin,LL_GPIO_MODE_ANALOG);
      default: break;  
      }  
    }
  
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