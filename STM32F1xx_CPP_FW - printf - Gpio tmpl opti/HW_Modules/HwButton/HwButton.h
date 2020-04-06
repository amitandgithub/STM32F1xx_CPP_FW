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

using namespace HAL;

namespace BSP
{
  
  class HwButton
  {
  public:
    using Status_t = uint32_t;
    
    static const bool SOFTWARE_DEBOUNCE_REQUIRED = false;
    
    typedef Callback_t BtnHandler;
    
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
      ClickTimeLow          = 70,
      ClickTimeHigh         = 400,
      DoubleClickTimeLow    = 400,
      DoubleClickTimeHigh   = 1000,
      LongPressTimeLow      = 700,
      LongPressTimeHigh     = 1500,// about 1-2 sec
      LongLongPressTimeLow  = 2000, // more than 5 sec
      LongLongPressTimeHigh = 6000,
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
    
    HwButton(Port_t Port, PIN_t Pin);
    
    ~HwButton(){};
    
    Status_t HwInit(Mode_t Mode,IntEdge_t Edge, Callback_t clickHandler, Callback_t longPressHandler, Callback_t longlongPressHandler = nullptr);
    void Run();
    bool ReadButton();
    bool RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler);
    
  private:
    void SetEvent(BtnEvent_t aBtnEvent){ m_Events |= 1UL << aBtnEvent;}
    void ClearEvent(BtnEvent_t aBtnEvent){ m_Events &= ~(1UL << aBtnEvent);}
    void EventHandler();
    bool SampleButton();
    bool SampleButtonUsingTimer();
    
  private:
   // HAL::GpioInput  m_Button;
    HAL::GpioInput  m_Button;
    uint32_t        m_Events;
    uint32_t        m_HighEdgeDetectedTime;
    bool            m_PreviousInputPinState;
    BtnStates       m_CurrentState;
    bool            m_EdgeDetected;
    
    BtnHandler      PressedHandler;
    BtnHandler      ReleasedHandler;
    BtnHandler      ClickHandler;
    BtnHandler      DoubleClickHandler;
    BtnHandler      LongPressHandler;
    BtnHandler      LongLongPressHandler;
    
    
  };
  
}// namespace BSP
#endif /* HW_BUTTON_H */
