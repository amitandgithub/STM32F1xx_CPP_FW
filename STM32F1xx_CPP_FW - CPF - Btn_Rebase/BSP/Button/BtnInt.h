/******************
** CLASS: BtnInt
**
** DESCRIPTION:
**  Implements the basic button functionality with Interrupt method
**
** CREATED: 22/2/2019, by Amit Chaudhary
**
** FILE: BtnInt.h
**
******************/
#ifndef BtnInt_h
#define BtnInt_h

#include"DigitalIn.h"

#define BTN_DEBOUNCE_REQUIRED

namespace BSP
{
  
  using namespace HAL;   
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise = nullptr, Callback_t OnFall = nullptr>
    class BtnInt : public Callback
    {
    public:
      
      typedef enum
      {
        BTN_LOW,
        BTN_HIGH,
        BTN_IDLE,
        BTN_DEBOUNCE       
      }BtnState_t;
      
      void HwInit();
      
      bool Read(){return m_BtnPin.Read();}
      
      uint32_t getMillis(){ return HAL_GetTick();}
      
      virtual void CallbackFunction();
      
    private:
      HAL::DigitalIn<Port, Pin, Mode ,Edge>     m_BtnPin;
      
    };    
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise, Callback_t OnFall>
      void BtnInt<Port,Pin,Mode,Edge,OnRise,OnFall>::HwInit()
      {
        m_BtnPin.HwInit();
        m_BtnPin.RegisterCallback(this);
      }    
    
    template<Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise, Callback_t OnFall>
      void BtnInt<Port,Pin,Mode,Edge,OnRise,OnFall>::CallbackFunction()
      {
        if(Read() == true)
        {
          if(OnRise)
            OnRise->CallbackFunction();
        }
        
        else
        {
          if(OnFall)
            OnFall->CallbackFunction();  
        }             
      }
    
    
    
    
    
    
    
    
}



#endif //BtnInt_h
