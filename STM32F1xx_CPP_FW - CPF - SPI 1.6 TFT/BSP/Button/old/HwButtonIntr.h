/*
 * HwButtonIntr.hpp
 *
 *  Created on: 21-Oct-2017
 *      Author: Amit Chaudhary
 *
 *  27-Sept-2018 : Modified to newer version by using STM32_HAL layer
 */

#ifndef HW_BUTTON_INTR_H
#define HW_BUTTON_INTR_H


#include "GpioInput.hpp"
#include "stm32f1xx_hal.h"

namespace Peripherals
{

class HwButtonIntr : public GpioInput
{
public:
	static const uint32_t QSIZE = 20;
    const bool DEBOUNCE_REQUIRED = false;

	typedef void(*BtnHandler)(void);
    typedef void(*HwButtonISR)(void);
	typedef enum : uint8_t
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
		LongPressTimeLow      = 300,
		LongPressTimeHigh     = 400,// about 1-2 sec
		LongLongPressTimeLow  = 2000, // more than 5 sec
		LongLongPressTimeHigh = 6000,
		UnSupportedTime = 6100,
	};

	typedef enum : uint8_t
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

	HwButtonIntr(PORT_t Port, PIN_t Pin, Intr_Event_Edge_t eIntOnWhichEdge = HwButtonIntr::IT_ON_RISING, PULL_t Pull = PULL_UP);
	virtual ~HwButtonIntr(){};

	virtual Status_t HwInit();
	void RunStateMachine();
	bool RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler);

private:
    uint32_t GetTicks(){return HAL_GetTick();};
    uint32_t GetTicksSince(uint32_t LastTick);
	void SetEvent(BtnEvent_t aBtnEvent){ m_Events |= 1UL << aBtnEvent;}
	void ClearEvent(BtnEvent_t aBtnEvent){ m_Events &= ~(1UL << aBtnEvent);}
	void EventHandler();
	inline static void ISR();
    
private:	
	uint8_t             m_Events;
	uint32_t            m_HighEdgeDetectedTime;
	bool                m_PreviousInputPinState;
    BtnHandler          PressedHandler;
	BtnHandler          ReleasedHandler;
	BtnHandler          ClickHandler;
	BtnHandler          DoubleClickHandler;
	BtnHandler          LongPressHandler;
	BtnHandler          LongLongPressHandler;
	HwButtonISR         m_HwButtonISR;
    static BtnStates    m_CurrentState;
	static PIN_t        m_Pin;

};

}// namespace Bsp



#endif /* HW_BUTTON_INTR_H */
