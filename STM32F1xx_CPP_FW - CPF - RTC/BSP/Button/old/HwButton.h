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
     using Pin_t = HAL::Gpio::Pin_t;
     using ISR_t = void(*)();
     using Intr_Event_Edge_t = HAL::GpioInput::Intr_Event_Edge_t;
     using GpioInput_Params_t = HAL::GpioInput::GpioInput_Params_t;
     
	typedef void(*BtnHandler)(void);
    
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

	HwButton(Pin_t  Pin, GpioInput_Params_t Params = HAL::GpioInput::INPUT_PULL_UP , Intr_Event_Edge_t Edge = HAL::GpioInput::IT_ON_FALLING, ISR_t aISR = nullptr);
	~HwButton(){};

	Status_t HwInit(void *pInitStruct = nullptr);
	void RunStateMachine();
	bool ReadButton();
	bool RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler);

private:
	void SetEvent(BtnEvent_t aBtnEvent){ _Events |= 1UL << aBtnEvent;}
	void ClearEvent(BtnEvent_t aBtnEvent){ _Events &= ~(1UL << aBtnEvent);}
	void EventHandler();
	bool SampleButton();
	bool SampleButtonUsingTimer();
    uint32_t GetTicks(){return HAL_GetTick();};
    uint32_t GetTicksSince(uint32_t LastTick);
	
private:
    HAL::GpioInput  _Button;
    const bool      SOFTWARE_DEBOUNCE_REQUIRED = false;
	uint32_t        _Events;
	uint32_t        _HighEdgeDetectedTime;
	bool            _PreviousInputPinState;
	BtnStates       _CurrentState;
	bool            _EdgeDetected;

	BtnHandler      PressedHandler;
	BtnHandler      ReleasedHandler;
	BtnHandler      ClickHandler;
	BtnHandler      DoubleClickHandler;
	BtnHandler      LongPressHandler;
	BtnHandler      LongLongPressHandler;


};

}// namespace BSP
#endif /* HW_BUTTON_H */
