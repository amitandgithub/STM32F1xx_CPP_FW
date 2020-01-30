/******************
** FILE: BtnInt.cpp
**
** DESCRIPTION:
**  BtnInt class implementation using Interrupts
**
** CREATED: 22/2/2019, by Amit Chaudhary
******************/
#include "BtnInt.h"


namespace BSP
{
  
  
  BtnInt::BtnInt(Port_t Port, PIN_t Pin) :
    m_BtnPin(Port,Pin)
    {
      
      
    }
    BtnInt::Status_t BtnInt::HwInit(Mode_t Mode,IntEdge_t Edge, Callback_t OnRise, Callback_t OnFall)
    {
      m_OnRiseCallback = OnRise;
      m_OnFallCallback = OnFall;
      m_BtnPin.HwInit(Mode,Edge,this); 
      return 1;
    }
    
    void BtnInt::CallbackFunction()
    {
      if(Read() == true)
      {
        if(m_OnRiseCallback)
          m_OnRiseCallback->CallbackFunction();
      }
      
      else
      {
        if(m_OnFallCallback)
          m_OnFallCallback->CallbackFunction();  
      }             
    }
    
}
