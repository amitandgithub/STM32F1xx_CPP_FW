/******************
** CLASS: DigitalIn
**
** DESCRIPTION:
**  Gpio input functionality
**
** CREATED: 8/9/2019, by Amit Chaudhary
**
** FILE: DigitalIn.h
**
******************/
#ifndef DigitalIn_h
#define DigitalIn_h


#include"DigitalIO.h"


namespace HAL
{    
  template<Port_t Port, PIN_t Pin, Mode_t Mode = INPUT_PULLUP, IntEdge_t Edge = NO_INTERRUPT, Callback_t Callback = nullptr>
    class DigitalIn : public DigitalIO, public InterruptSource
    {
    public:
    
      void HwInit(){SetInputMode(Port,Pin,Mode,this,Edge,Callback);}
        
      bool Read(){ return (bool)(  (((GPIO_TypeDef*)Port)->IDR) & 1<<Pin ); }
      
      virtual void ISR(){DispatchCallback();}
    };  
    
} 
#endif //DigitalIn_h