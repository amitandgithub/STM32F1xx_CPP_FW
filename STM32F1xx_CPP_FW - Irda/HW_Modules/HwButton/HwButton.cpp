/*
* HwButton.cpp
*
*  Created on: 17-Oct-2017
*      Author: Amit Chaudhary
*
*  27-Sept-2018 : Modified to newer version by using STM32_HAL layer
*  29-Jan-2019 : Modified to newer version by using STM32_LL layer
*/


#include "HwButton.h"

namespace BSP
{
  
  HwButton::HwButton(Port_t Port, PIN_t Pin) : m_Button(Port,Pin)
  {
    m_Events                = 0UL;
    m_CurrentState          = HwButton::IdleState;
    m_HighEdgeDetectedTime  = 0UL;
    m_PreviousInputPinState = false;
    m_EdgeDetected          = false;
    
    PressedHandler          = nullptr;
    ReleasedHandler         = nullptr;
    DoubleClickHandler      = nullptr;
   
  }
  
  HwButton::Status_t HwButton::HwInit(Mode_t Mode,IntEdge_t Edge, Callback_t clickHandler, Callback_t longPressHandler, Callback_t longlongPressHandler)
  {
    m_Button.HwInit(Mode,Edge);
    ClickHandler            = clickHandler;
    LongPressHandler        = longPressHandler;
    LongLongPressHandler    = longlongPressHandler;
    
    return true;
  }
  
  void HwButton::Run()
  {
    static uint32_t ElapsedTime = 0UL;
    
    
    switch(m_CurrentState)
    {
    case HwButton::IdleState:
      {
        if( ReadButton() == true )
        {
          m_HighEdgeDetectedTime = GetTick();
          m_CurrentState = HwButton::PressedState;
        }
        break;
      }
    case HwButton::PressedState:
      {
        if( ReadButton() == false )
        {
          ElapsedTime = GetTicksSince(m_HighEdgeDetectedTime);
          m_CurrentState = HwButton::ReleasedState;
        }
        break;
      }
      
    case HwButton::ReleasedState:
      {
        
        if( (ElapsedTime >= ClickTimeLow) && (ElapsedTime <= ClickTimeHigh) )
        {
          m_CurrentState = HwButton::ClickIdentifyState;
        }
        else if ( (ElapsedTime >= LongPressTimeLow) && (ElapsedTime <= LongPressTimeHigh) )
        {
          m_CurrentState = HwButton::LongPressState;
        }
        else if ( (ElapsedTime >= LongLongPressTimeLow) && (ElapsedTime <= LongLongPressTimeHigh) )
        {
          m_CurrentState = HwButton::LongLongPressState;
        }
        else if (ElapsedTime >= UnSupportedTime)
        {
          m_CurrentState = HwButton::IdleState;
        }
        else
        {
          m_CurrentState = HwButton::IdleState;
        }
        break;
      }
    case HwButton::ClickIdentifyState:
      {
        static uint32_t LastClickStateEntered = 0;
        static uint32_t temp  = 0;
        
        
        temp  = GetTicksSince(LastClickStateEntered);
        
        if ( (temp) > 300 )
        {
          LastClickStateEntered =  GetTick();
          m_CurrentState = HwButton::ClickState;
        }
        else
        {
          m_CurrentState = HwButton::DoubleClickState;
        }
        
        break;
      }
    case HwButton::ClickState:
      {
        SetEvent(HwButton::Click);
        m_CurrentState = HwButton::IdleState;
        break;
      }
    case HwButton::DoubleClickState:
      {
        SetEvent(HwButton::DoubleClick);
        m_CurrentState = HwButton::IdleState;
        break;
      }
    case HwButton::LongPressState:
      {
        SetEvent(HwButton::LongPress);
        m_CurrentState = HwButton::IdleState;
        break;
      }
    case HwButton::LongLongPressState:
      {
        SetEvent(HwButton::LongLongPress);
        m_CurrentState = HwButton::IdleState;
        break;
      }
    default: m_CurrentState = HwButton::IdleState;
    }
    
    EventHandler();
  }
  
  bool HwButton::ReadButton()
  {
    bool CurrentInputPinState = false;
    
    CurrentInputPinState = m_Button.Read();
    
    if(m_PreviousInputPinState != CurrentInputPinState)
    {
      m_PreviousInputPinState = SampleButtonUsingTimer();
    }
    return m_PreviousInputPinState;
    
  }
  bool HwButton::SampleButtonUsingTimer()
  {
#if DEBOUNCE_REQUIRED
    uint32_t High = 0;
    uint32_t Low = 0;
    uint32_t TotalSamples = 10UL;
    
    for(uint32_t i = 0; i<TotalSamples; i++)
    {
      
      
      if(GpioInput::ReadInput() == true)
      {
        High++;
      }
      else
      {
        Low++;
      }
      m_SysTickTimer->DelayTicks(2);
    }
    
    if( High > ( TotalSamples - (TotalSamples>>1)) )
    {
      return true;
    }
    else
    {
      return false;
    }
#else
    
    return m_Button.Read();
    
#endif
  }
  bool HwButton::SampleButton()
  {
#if SOFTWARE_DEBOUNCE_REQUIRED
    uint32_t High = 0;
    uint32_t Low = 0;
    uint32_t TotalSamples = 10000UL;
    
    for(uint32_t i = 0; i<TotalSamples; i++)
    {
      if(GpioInput::ReadInput() == true)
      {
        High++;
      }
      else
      {
        Low++;
      }
    }
    
    if( High > ( TotalSamples - (TotalSamples/20)) )
    {
      return true;
    }
    else
    {
      return false;
    }
#else
    
    return m_Button.Read();
    
#endif
  }
  void HwButton::EventHandler()
  {
    // If there is some event present, Handle it
    if(m_Events)
    {
      if( (m_Events & (1<<Pressed)) && (PressedHandler))
      {
        PressedHandler->CallbackFunction();
        ClearEvent(Pressed);
      }
      else if( (m_Events & (1<<Released)) && (ReleasedHandler))
      {
        ReleasedHandler->CallbackFunction();
        ClearEvent(Released);
      }
      else if( (m_Events & (1<<Click)) && (ClickHandler))
      {
        ClickHandler->CallbackFunction();
        ClearEvent(Click);
      }
      else if( (m_Events & (1<<DoubleClick)) && (DoubleClickHandler))
      {
        DoubleClickHandler->CallbackFunction();
        ClearEvent(DoubleClick);
      }
      else if( (m_Events & (1<<LongPress)) && (LongPressHandler))
      {
        LongPressHandler->CallbackFunction();
        ClearEvent(LongPress);
      }
      else if( (m_Events & (1<<(LongLongPress)) && (LongLongPressHandler)) )
      {
        LongLongPressHandler->CallbackFunction();
        ClearEvent(LongLongPress);
      }
      
      else
      {
        // Nothing to do
      }
    }
    
  }
  bool HwButton::RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler)
  {
    switch(eEvent)
    {
    case HwButton::Click         : ClickHandler = aBtnHandler;break;
    case HwButton::DoubleClick   : DoubleClickHandler = aBtnHandler;break;
    case HwButton::LongPress     : LongPressHandler = aBtnHandler;break;
    case HwButton::LongLongPress : LongLongPressHandler = aBtnHandler;break;
    default:break;
    }
    return true;
  }
  
  
}
