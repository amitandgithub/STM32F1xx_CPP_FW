/*
 * HwButton.cpp
 *
 *  Created on: 17-Oct-2017
 *      Author: Amit Chaudhary
 *
 *  27-Sept-2018 : Modified to newer version by using STM32_HAL layer
 */


#include "HwButton.h"

namespace Peripherals
{


HwButton::HwButton(PORT_t Port, PIN_t Pin) : GpioInput(Port, Pin)
{
	m_Events                = 0UL;
	m_CurrentState          = HwButton::IdleState;
	m_HighEdgeDetectedTime  = 0UL;
	m_PreviousInputPinState = false;
	m_EdgeDetected          = false;

	PressedHandler          = nullptr;
	ReleasedHandler         = nullptr;
	DoubleClickHandler      = nullptr;
	ClickHandler            = nullptr;
	LongPressHandler        = nullptr;
	LongLongPressHandler    = nullptr;

}

HwButton::HwButton(PORT_t Port, PIN_t Pin, ISR_t aISR, Intr_Event_Edge_t eIntOnWhichEdge, PULL_t Pull) 
                                        : GpioInput(Port, Pin, aISR, eIntOnWhichEdge, Pull )
{
	m_Events                = 0UL;
	m_CurrentState          = HwButton::IdleState;
	m_HighEdgeDetectedTime  = 0UL;
	m_PreviousInputPinState = false;
	m_EdgeDetected          = false;

	PressedHandler          = nullptr;
	ReleasedHandler         = nullptr;
	DoubleClickHandler      = nullptr;
	ClickHandler            = nullptr;
	LongPressHandler        = nullptr;
	LongLongPressHandler    = nullptr;
}

Status_t HwButton::HwInit()
{
	GpioInput::HwInit();
	//m_SysTickTimer = Bsp::SysTickTimer::GetInstance();
	return true;
}

void HwButton::RunStateMachine()
{
	static uint32_t ElapsedTime = 0UL;


	switch(m_CurrentState)
	{
	case HwButton::IdleState:
	{
		 if( ReadButton() == true )
		 {
			 m_HighEdgeDetectedTime = GetTicks();
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
		static uint32_t LastClickStateEntered = 0;//m_SysTickTimer->GetTicks();
		static uint32_t temp  = 0;//m_SysTickTimer->GetTicksSince(LastClickStateEntered);


		temp  = GetTicksSince(LastClickStateEntered);

		if ( (temp) > 300 )
		{
			LastClickStateEntered =  GetTicks();
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

	CurrentInputPinState = GpioInput::ReadInput();

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

return GpioInput::ReadInput();

#endif
}
bool HwButton::SampleButton()
{
#if DEBOUNCE_REQUIRED
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

return GpioInput::ReadInput();

#endif
}
void HwButton::EventHandler()
{
	// If there is some event present, Handle it
	if(m_Events)
	{
		if( (m_Events & (1<<Pressed)) && (PressedHandler))
		{
			PressedHandler();
			ClearEvent(Pressed);
		}
		else if( (m_Events & (1<<Released)) && (ReleasedHandler))
		{
			ReleasedHandler();
			ClearEvent(Released);
		}
		else if( (m_Events & (1<<Click)) && (ClickHandler))
		{
			ClickHandler();
			ClearEvent(Click);
		}
		else if( (m_Events & (1<<DoubleClick)) && (DoubleClickHandler))
		{
			DoubleClickHandler();
			ClearEvent(DoubleClick);
		}
		else if( (m_Events & (1<<LongPress)) && (LongPressHandler))
		{
			LongPressHandler();
			ClearEvent(LongPress);
		}
		else if( (m_Events & (1<<(LongLongPress)) && (LongLongPressHandler)) )
		{
			LongLongPressHandler();
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

uint32_t HwButton::GetTicksSince(uint32_t LastTick)
{
    uint32_t CurrentTicks = GetTicks();

    return ( (CurrentTicks >= LastTick)? (CurrentTicks - LastTick ) :   (CurrentTicks - LastTick + 0xFFFFFFFFUL) );
}


}
