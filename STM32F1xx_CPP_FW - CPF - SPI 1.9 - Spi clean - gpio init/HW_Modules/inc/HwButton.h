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

#include "stm32f1xx_hal.h"
#include "GpioInput.hpp"

namespace Peripherals
{

class HwButton : public GpioInput
{
public:
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

	HwButton(PORT_t Port, PIN_t Pin);
	HwButton(PORT_t Port, PIN_t Pin, ISR_t aISR,Intr_Event_Edge_t eIntOnWhichEdge, PULL_t Pull = PULL_UP);
	virtual ~HwButton(){};

	virtual Status_t HwInit();
	void RunStateMachine();
	bool ReadButton();
	bool RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler);

private:
	void SetEvent(BtnEvent_t aBtnEvent){ m_Events |= 1UL << aBtnEvent;}
	void ClearEvent(BtnEvent_t aBtnEvent){ m_Events &= ~(1UL << aBtnEvent);}
	void EventHandler();
	bool SampleButton();
	bool SampleButtonUsingTimer();
    uint32_t GetTicks(){return HAL_GetTick();};
    uint32_t GetTicksSince(uint32_t LastTick);
	const bool DEBOUNCE_REQUIRED = false;


	uint32_t        m_Events;
	uint32_t        m_HighEdgeDetectedTime;
	bool       m_PreviousInputPinState;
	BtnStates  m_CurrentState;
	bool       m_EdgeDetected;
	//Peripherals::SysTickTimer* m_SysTickTimer;


	BtnHandler PressedHandler;
	BtnHandler ReleasedHandler;
	BtnHandler ClickHandler;
	BtnHandler DoubleClickHandler;
	BtnHandler LongPressHandler;
	BtnHandler LongLongPressHandler;


};

}// namespace Peripherals
#endif /* HW_BUTTON_H */
