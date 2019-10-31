/******************
** CLASS: BtnPolling
**
** DESCRIPTION:
**  Implements the basic button functionality with Interrupt method
**
** CREATED: 31/10/2019, by Amit Chaudhary
**
** FILE: BtnPolling.h
**
******************/
#ifndef BtnPolling_h
#define BtnPolling_h

#include"DigitalIn.h"

#define BTN_DEBOUNCE_REQUIRED

namespace BSP
{
  
  using namespace HAL;   
#if 0  
  template<Port_t Port, PIN_t Pin,Mode_t Mode = INPUT_PULLUP, uint8_t Debounce = 0, Callback_t OnRise = nullptr, Callback_t OnFall = nullptr>
    class BtnPolling 
    {
    public:
      typedef HAL::Callback* DigitalInCallback_t;
       
      typedef enum
      {
        BTN_LOW,
        BTN_HIGH,
        BTN_IDLE,
        BTN_DEBOUNCE       
      }BtnState_t;
      
      BtnPolling() : m_previousState(BTN_IDLE),m_currentState(BTN_IDLE),m_startMillis(0)
      {
      }
      
      void HwInit();
      
      void Run();
      
      BtnState_t Read(){return  m_BtnPin.Read() ? BTN_HIGH : BTN_LOW;}
      
      void RegisterCallback(DigitalInCallback_t _OnRise, DigitalInCallback_t _OnFall) { m_OnRise = _OnRise; m_OnFall = _OnFall; };
      
      BtnState_t getState(){return m_previousState;}
      
      uint32_t getMillis(){ return HAL_GetTick();}
      
    private:
      HAL::DigitalIn<Port, Pin, Mode>     m_BtnPin;
      BtnState_t             m_previousState;
      BtnState_t             m_currentState;
      uint16_t               m_startMillis;
      DigitalInCallback_t       m_OnRise;
      DigitalInCallback_t       m_OnFall;
      
    };    
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint8_t Debounce, Callback_t OnRise, Callback_t OnFall>
    void BtnPolling<Port,Pin,Mode,Debounce,OnRise,OnFall>::HwInit()
    {
      m_BtnPin.HwInit();
      m_OnRise = OnRise;
      m_OnFall = OnFall;
    }    
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint8_t Debounce, Callback_t OnRise, Callback_t OnFall>
    void BtnPolling<Port,Pin,Mode,Debounce,OnRise,OnFall>::Run()
    {
      switch(m_currentState)
      {
        
      case BTN_IDLE:        
        if(Read() == m_previousState) return;
        
        if(Debounce != 0)
        {
          m_currentState = BTN_DEBOUNCE; 
          m_startMillis  = getMillis();
        }
        else
        {
          m_currentState = Read();            
        }
        break;
        
      case BTN_DEBOUNCE:        
        if(getMillis() - (uint32_t)m_startMillis >= Debounce)
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
        
        if(m_OnFall) m_OnFall->CallbackFunction();
        break;
        
      case BTN_HIGH:
        m_currentState = BTN_IDLE;
        m_previousState = BTN_HIGH;
        
        if(m_OnRise) m_OnRise->CallbackFunction();        
        break;         
        
      default: break;
      }
    }  

#else
  
 template<Port_t Port, PIN_t Pin,Mode_t Mode = INPUT_PULLUP, uint8_t Debounce = 0>
    class BtnPolling 
    {
    public:
      typedef HAL::Callback* DigitalInCallback_t;
       
      typedef enum
      {
        BTN_LOW,
        BTN_HIGH,
        BTN_IDLE,
        BTN_DEBOUNCE       
      }BtnState_t;
      
      BtnPolling() : m_previousState(BTN_IDLE),m_currentState(BTN_IDLE),m_startMillis(0)
      {
      }
      
      void HwInit();
      
      void Run();
      
      BtnState_t Read(){return  m_BtnPin.Read() ? BTN_HIGH : BTN_LOW;}
      
      BtnState_t getState(){return m_previousState;}
      
      uint32_t getMillis(){ return HAL_GetTick();}
      
    private:
      HAL::DigitalIn<Port, Pin, Mode>     m_BtnPin;
      BtnState_t             m_previousState;
      BtnState_t             m_currentState;
      uint16_t               m_startMillis;      
    };    
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint8_t Debounce>
    void BtnPolling<Port,Pin,Mode,Debounce>::HwInit()
    {
      m_BtnPin.HwInit();
    }    
  
  template<Port_t Port, PIN_t Pin,Mode_t Mode, uint8_t Debounce>
    void BtnPolling<Port,Pin,Mode,Debounce>::Run()
    {
      switch(m_currentState)
      {
        
      case BTN_IDLE:        
        if(Read() == m_previousState) return;
        
        if(Debounce != 0)
        {
          m_currentState = BTN_DEBOUNCE; 
          m_startMillis  = getMillis();
        }
        else
        {
          m_currentState = Read();            
        }
        break;
        
      case BTN_DEBOUNCE:        
        if(getMillis() - (uint32_t)m_startMillis >= Debounce)
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
        
       // if(OnFall) OnFall->CallbackFunction();
        break;
        
      case BTN_HIGH:
        m_currentState = BTN_IDLE;
        m_previousState = BTN_HIGH;
        
       // if(OnRise) OnRise->CallbackFunction();        
        break;         
        
      default: break;
      }
      return m_previousState;
    }  
  
#endif // 0
  
}

#endif //BtnPolling_h
