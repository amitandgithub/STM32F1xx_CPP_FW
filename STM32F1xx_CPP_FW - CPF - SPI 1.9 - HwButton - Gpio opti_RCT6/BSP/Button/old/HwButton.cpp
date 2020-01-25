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
    
HwButton::HwButton(Pin_t  Pin, GpioInput_Params_t Params, Intr_Event_Edge_t Edge, ISR_t aISR): _Button(Pin,Params,Edge,aISR)
{
	_Events                = 0UL;
	_CurrentState          = HwButton::IdleState;
	_HighEdgeDetectedTime  = 0UL;
	_PreviousInputPinState = false;
	_EdgeDetected          = false;

	PressedHandler          = nullptr;
	ReleasedHandler         = nullptr;
	DoubleClickHandler      = nullptr;
	ClickHandler            = nullptr;
	LongPressHandler        = nullptr;
	LongLongPressHandler    = nullptr;
}

HwButton::Status_t HwButton::HwInit(void *pInitStruct)
{
	_Button.HwInit();
	return true;
}

void HwButton::RunStateMachine()
{
	static uint32_t ElapsedTime = 0UL;


	switch(_CurrentState)
	{
	case HwButton::IdleState:
	{
		 if( ReadButton() == true )
		 {
			 _HighEdgeDetectedTime = GetTicks();
			 _CurrentState = HwButton::PressedState;
		 }
		 break;
	}
	case HwButton::PressedState:
	{
		 if( ReadButton() == false )
		 {
			ElapsedTime = GetTicksSince(_HighEdgeDetectedTime);
			_CurrentState = HwButton::ReleasedState;
		 }
		 break;
	}

	case HwButton::ReleasedState:
	{

		 if( (ElapsedTime >= ClickTimeLow) && (ElapsedTime <= ClickTimeHigh) )
		 {
			 _CurrentState = HwButton::ClickIdentifyState;
		 }
		 else if ( (ElapsedTime >= LongPressTimeLow) && (ElapsedTime <= LongPressTimeHigh) )
		 {
			 _CurrentState = HwButton::LongPressState;
		 }
		 else if ( (ElapsedTime >= LongLongPressTimeLow) && (ElapsedTime <= LongLongPressTimeHigh) )
		 {
			 _CurrentState = HwButton::LongLongPressState;
		 }
		 else if (ElapsedTime >= UnSupportedTime)
		 {
			 _CurrentState = HwButton::IdleState;
		 }
		 else
		 {
			 _CurrentState = HwButton::IdleState;
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
			LastClickStateEntered =  GetTicks();
			_CurrentState = HwButton::ClickState;
		}
		else
		{
			_CurrentState = HwButton::DoubleClickState;
		}

		break;
	}
	case HwButton::ClickState:
	{
		SetEvent(HwButton::Click);
		_CurrentState = HwButton::IdleState;
		break;
	}
	case HwButton::DoubleClickState:
	{
		SetEvent(HwButton::DoubleClick);
		_CurrentState = HwButton::IdleState;
		break;
	}
	case HwButton::LongPressState:
	{
		SetEvent(HwButton::LongPress);
		_CurrentState = HwButton::IdleState;
		break;
	}
	case HwButton::LongLongPressState:
	{
		SetEvent(HwButton::LongLongPress);
		_CurrentState = HwButton::IdleState;
		break;
	}
	default: _CurrentState = HwButton::IdleState;
	}

	EventHandler();
}

bool HwButton::ReadButton()
{
	bool CurrentInputPinState = false;

	CurrentInputPinState = _Button.Read();

	if(_PreviousInputPinState != CurrentInputPinState)
	{
		_PreviousInputPinState = SampleButtonUsingTimer();
	}
	return _PreviousInputPinState;

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
		_SysTickTimer->DelayTicks(2);
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

return _Button.Read();

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

return _Button.Read();

#endif
}
void HwButton::EventHandler()
{
	// If there is some event present, Handle it
	if(_Events)
	{
		if( (_Events & (1<<Pressed)) && (PressedHandler))
		{
			PressedHandler();
			ClearEvent(Pressed);
		}
		else if( (_Events & (1<<Released)) && (ReleasedHandler))
		{
			ReleasedHandler();
			ClearEvent(Released);
		}
		else if( (_Events & (1<<Click)) && (ClickHandler))
		{
			ClickHandler();
			ClearEvent(Click);
		}
		else if( (_Events & (1<<DoubleClick)) && (DoubleClickHandler))
		{
			DoubleClickHandler();
			ClearEvent(DoubleClick);
		}
		else if( (_Events & (1<<LongPress)) && (LongPressHandler))
		{
			LongPressHandler();
			ClearEvent(LongPress);
		}
		else if( (_Events & (1<<(LongLongPress)) && (LongLongPressHandler)) )
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
