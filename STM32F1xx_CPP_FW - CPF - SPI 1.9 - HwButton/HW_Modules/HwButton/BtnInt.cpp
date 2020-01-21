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
    
    
BtnInt::BtnInt(Port_t Port, PIN_t Pin,Mode_t Mode,IntEdge_t Edge, Callback_t OnRise, Callback_t OnFall) :
  m_BtnPin(Port,Pin,Mode,Edge,this),
  m_OnRiseCallback(OnRise),
  m_OnFallCallback(OnFall)
{
    
    
}
BtnInt::Status_t BtnInt::HwInit()
{
     m_BtnPin.HwInit(); 
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
