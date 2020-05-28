/*
* HwButton.hpp
*
*  Created on: 17-Oct-2017
*      Author: Amit Chaudhary
*
*  27-Sept-2018 : Modified to newer version by using STM32_HAL layer
*/

#ifndef HW_BUTTON_H
#define HW_BUTTON_H

#include "GpioInput.h" 
#include "DigitalIn.h"

using namespace HAL;

#define HW_BUTTON_DEBUG 0
namespace BSP
{
  
  template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler = nullptr >
    class HwButton
    {
    public:
      using Status_t = uint32_t;
      
      static const bool SOFTWARE_DEBOUNCE_REQUIRED = false;
      
      //typedef HAL::Callback* BtnHandler;
      
      typedef enum 
      {
        Idle = 0,
        Pressed,
        Released,
        Click,
        DoubleClick,
        LongPress,
        LongLongPress
      }BtnEvent_t;
      
      enum
      {
        ClickTimeLow          = 50,
        ClickTimeHigh         = 150,
//        DoubleClickTimeLow    = 400,
//        DoubleClickTimeHigh   = 1000,
        LongPressTimeLow      = 200,
        LongPressTimeHigh     = 700,// about 1-2 sec
        LongLongPressTimeLow  = 700, // more than 5 sec
        LongLongPressTimeHigh = 2000,
        UnSupportedTime = 6100,
      };
      
      typedef enum
      {
        IdleState = 0,
        PressedState,
        ReleasedState,
        LongPressState,     // about 1-2 sec
        LongLongPressState, // more than 5 sec
        ClickState,
        ClickIdentifyState,
        DoubleClickState
      }BtnStates;
      
      HwButton();
      
      ~HwButton(){};
      
      Status_t HwInit();
      void Run();
      bool ReadButton();
     // bool RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler);
      
    private:
      void SetEvent(BtnEvent_t aBtnEvent){ m_Events |= 1UL << aBtnEvent;}
      void ClearEvent(BtnEvent_t aBtnEvent){ m_Events &= ~(1UL << aBtnEvent);}
      void EventHandler();
      bool SampleButton();
      bool SampleButtonUsingTimer();
      
    private:
      
      T               m_Button;
      uint32_t        m_Events;
      uint32_t        m_HighEdgeDetectedTime;
      bool            m_PreviousInputPinState;
      BtnStates       m_CurrentState;
      bool            m_EdgeDetected;
      
      //    BtnHandler      PressedHandler;
      //    BtnHandler      ReleasedHandler;
      //    BtnHandler      ClickHandler;
      //    BtnHandler      DoubleClickHandler;
      //    BtnHandler      LongPressHandler;
      //    BtnHandler      LongLongPressHandler;
      
      
    };
  
  
  template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
    HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::HwButton() : m_Events(0),m_CurrentState(HwButton::IdleState),m_HighEdgeDetectedTime(0), m_PreviousInputPinState(false), m_EdgeDetected(false)
    {
      //    m_Events                = 0UL;
      //    m_CurrentState          = HwButton::IdleState;
      //    m_HighEdgeDetectedTime  = 0UL;
      //    m_PreviousInputPinState = false;
      //    m_EdgeDetected          = false;   
    }
    
    template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
      HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::Status_t
        HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
        HwInit()
        {
          m_Button.HwInit();
          return true;
        }
        
        template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
          void
            HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
            Run()
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
#if HW_BUTTON_DEBUG
                  printf("ElapsedTime = %d \n\r",ElapsedTime);
#endif
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
            
            template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
              bool
                HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
                ReadButton()
                {
                  bool CurrentInputPinState = false;
                  
                  CurrentInputPinState = m_Button.Read();
                  
                  if(m_PreviousInputPinState != CurrentInputPinState)
                  {
                    m_PreviousInputPinState = SampleButtonUsingTimer();
                  }
                  return m_PreviousInputPinState;
                  
                }
                template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
                  bool
                    HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
                    SampleButtonUsingTimer()
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
                    
                    template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
                      bool
                        HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
                        SampleButton()
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
                        
                        template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
                          void
                            HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
                            EventHandler()
                            {
                              // If there is some event present, Handle it
                              if(m_Events)
                              {
//                                if( (m_Events & (1<<Pressed)) && (PressedHandler))
//                                {
//                                  PressedHandler->CallbackFunction();
//                                  ClearEvent(Pressed);
//                                }
//                                else if( (m_Events & (1<<Released)) && (ReleasedHandler))
//                                {
//                                  ReleasedHandler->CallbackFunction();
//                                  ClearEvent(Released);
//                                }
                                if( (m_Events & (1<<Click)) && (clickHandler))
                                {
                                  clickHandler->CallbackFunction();
                                  ClearEvent(Click);
                                }
//                                else if( (m_Events & (1<<DoubleClick)) && (DoubleClickHandler))
//                                {
//                                  DoubleClickHandler->CallbackFunction();
//                                  ClearEvent(DoubleClick);
//                                }
                                else if( (m_Events & (1<<LongPress)) && (longPressHandler))
                                {
                                  longPressHandler->CallbackFunction();
                                  ClearEvent(LongPress);
                                }
                                else if( (m_Events & (1<<(LongLongPress)) && (longlongPressHandler)) )
                                {
                                  longlongPressHandler->CallbackFunction();
                                  ClearEvent(LongLongPress);
                                }
                                
                                else
                                {
                                  // Nothing to do
                                }
                              }
                              
                            }
                            
                            //  template<typename T, HAL::Callback* clickHandler, HAL::Callback* longPressHandler, HAL::Callback* longlongPressHandler >
                            //    bool
                            //      HwButton<T,clickHandler,longPressHandler,longlongPressHandler>::
                            //   RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler)
                            //  {
                            //    switch(eEvent)
                            //    {
                            //    case HwButton::Click         : ClickHandler = aBtnHandler;break;
                            //    case HwButton::DoubleClick   : DoubleClickHandler = aBtnHandler;break;
                            //    case HwButton::LongPress     : LongPressHandler = aBtnHandler;break;
                            //    case HwButton::LongLongPress : LongLongPressHandler = aBtnHandler;break;
                            //    default:break;
                            //    }
                            //    return true;
                            //  }
                            //  
                            
}// namespace BSP
#endif /* HW_BUTTON_H */
