/******************
** FILE: BtnPoll.cpp
**
** DESCRIPTION:
**  BtnPoll class implementation
**
** CREATED: 1/31/2019, by Amit Chaudhary
** Modified for new Gpio class - 15 Jan 2020
******************/
#include "BtnPoll.h"


namespace BSP
{
  
  
  BtnPoll::BtnPoll(Port_t Port, PIN_t Pin, uint8_t Debounce) :
    m_BtnPin(Port,Pin),
    m_debounce(Debounce),
    m_previousState(BTN_IDLE),
    m_currentState(BTN_IDLE),
    m_startMillis(0U)
    {
      
      
    }
    BtnPoll::Status_t BtnPoll::HwInit(Mode_t Mode,IntEdge_t Edge, Callback_t OnRise, Callback_t OnFall)
    {
      m_previousState = Read();
      m_BtnPin.HwInit(Mode,Edge); 
      m_OnRiseCallback = OnRise;
      m_OnFallCallback = OnFall;
      return true;
    }
    
    void BtnPoll::Run()
    {      
      switch(m_currentState)
      {        
      case BTN_IDLE:        
        if(Read() == m_previousState)
          return;
        
        if(m_debounce != 0)
        {
          m_currentState = BTN_DEBOUNCE; 
          m_startMillis  = GetTick();
        }
        else
        {
          m_currentState = Read();            
        }
        break;
        
      case BTN_DEBOUNCE:        
        if(GetTick() - (uint32_t)m_startMillis >= m_debounce)
        {
          if (Read() != m_previousState)
          {
            m_currentState = Read();  
          }
          else
          {
            m_currentState = BTN_IDLE;    
          }            
        }
        break;
        
      case BTN_LOW: 
        m_currentState = BTN_IDLE;
        m_previousState = BTN_LOW;
        if(m_OnFallCallback) 
          m_OnFallCallback->CallbackFunction();
        break;
        
      case BTN_HIGH:
        m_currentState = BTN_IDLE;
        m_previousState = BTN_HIGH;
        if(m_OnRiseCallback) 
          m_OnRiseCallback->CallbackFunction();        
        break;         
        
      default: break;
      }
      
    }
    
}
