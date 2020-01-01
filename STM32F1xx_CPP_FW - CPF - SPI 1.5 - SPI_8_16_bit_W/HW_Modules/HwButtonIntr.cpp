/*
 * HwButtonIntr.cpp
 *
 *  Created on: 21-Oct-2017
 *      Author: Amit Chaudhary
 *
 *  27-Sept-2018 : Modified to newer version by using STM32_HAL layer
 */

#include "HwButtonIntr.h"

namespace Peripherals
{
HwButtonIntr::PIN_t HwButtonIntr::m_Pin = 0;
HwButtonIntr::BtnStates  HwButtonIntr::m_CurrentState = HwButtonIntr::IdleState;

HwButtonIntr::HwButtonIntr(PORT_t Port, PIN_t Pin, Intr_Event_Edge_t eIntOnWhichEdge, PULL_t Pull) 
                                        : GpioInput(Port, Pin, ISR, eIntOnWhichEdge, Pull )
{
	m_Pin = Pin;
	m_Events                = 0UL;
	m_HighEdgeDetectedTime  = 0UL;
	m_PreviousInputPinState = false;

	PressedHandler          = nullptr;
	ReleasedHandler         = nullptr;
	DoubleClickHandler      = nullptr;
	ClickHandler            = nullptr;
	LongPressHandler        = nullptr;
	LongLongPressHandler    = nullptr;
}

Status_t HwButtonIntr::HwInit()
{
	GpioInput::HwInit();
	return true;
}

void HwButtonIntr::RunStateMachine()
{
	static uint32_t ElapsedTime = 0UL;


	switch(m_CurrentState)
	{
		case HwButtonIntr::IdleState:
		{
			 break;
		}
		case HwButtonIntr::PressedState:
		{
			m_HighEdgeDetectedTime = GetTicks();
			m_CurrentState = HwButtonIntr::IdleState;
			break;
		}
		case HwButtonIntr::ReleasedState:
		{
			 ElapsedTime = GetTicksSince(m_HighEdgeDetectedTime);

			 if( (ElapsedTime >= ClickTimeLow) && (ElapsedTime <= ClickTimeHigh) )
			 {
				 m_CurrentState = HwButtonIntr::ClickIdentifyState;
			 }
			 else if ( (ElapsedTime >= LongPressTimeLow) && (ElapsedTime <= LongPressTimeHigh) )
			 {
				 m_CurrentState = HwButtonIntr::LongPressState;
			 }
			 else if ( (ElapsedTime >= LongLongPressTimeLow) && (ElapsedTime <= LongLongPressTimeHigh) )
			 {
				 m_CurrentState = HwButtonIntr::LongLongPressState;
			 }
			 else if (ElapsedTime >= UnSupportedTime)
			 {
				 m_CurrentState = HwButtonIntr::IdleState;
			 }
			 else
			 {
				 //m_CurrentState = HwButtonIntr::IdleState;
			 }
			 break;
		}
		case HwButtonIntr::ClickIdentifyState:
		{
			static uint32_t LastClickStateEntered = 0;//m_SysTickTimer->GetTicks();
			static uint32_t temp  = 0;//m_SysTickTimer->GetTicksSince(LastClickStateEntered);


			temp  = GetTicksSince(LastClickStateEntered);

			if ( (temp) > 300 )
			{
				LastClickStateEntered =  GetTicks();
				m_CurrentState = HwButtonIntr::ClickState;
			}
			else
			{
				m_CurrentState = HwButtonIntr::DoubleClickState;
			}

			break;
		}
		case HwButtonIntr::ClickState:
		{
			SetEvent(HwButtonIntr::Click);
			m_CurrentState = HwButtonIntr::IdleState;
			break;
		}
		case HwButtonIntr::DoubleClickState:
		{
			SetEvent(HwButtonIntr::DoubleClick);
			m_CurrentState = HwButtonIntr::IdleState;
			break;
		}
		case HwButtonIntr::LongPressState:
		{
			SetEvent(HwButtonIntr::LongPress);
			m_CurrentState = HwButtonIntr::IdleState;
			break;
		}
		case HwButtonIntr::LongLongPressState:
		{
			SetEvent(HwButtonIntr::LongLongPress);
			m_CurrentState = HwButtonIntr::IdleState;
			break;
		}
		default: m_CurrentState = HwButtonIntr::IdleState;
	}

	EventHandler();
}

void HwButtonIntr::EventHandler()
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
bool HwButtonIntr::RegisterEventHandler(BtnEvent_t eEvent,BtnHandler aBtnHandler)
{
	switch(eEvent)
	{
	case HwButtonIntr::Click         : ClickHandler = aBtnHandler;break;
	case HwButtonIntr::DoubleClick   : DoubleClickHandler = aBtnHandler;break;
	case HwButtonIntr::LongPress     : LongPressHandler = aBtnHandler;break;
	case HwButtonIntr::LongLongPress : LongLongPressHandler = aBtnHandler;break;
	default:break;
	}
	return true;
}

uint32_t HwButtonIntr::GetTicksSince(uint32_t LastTick)
{
    uint32_t CurrentTicks = GetTicks();

    return ( (CurrentTicks >= LastTick)? (CurrentTicks - LastTick ) :   (CurrentTicks - LastTick + 0xFFFFFFFFUL) );
}

void HwButtonIntr::ISR()
{

	/* Masks the further interrupt requests until this is serviced */
	 EXTI->IMR &= ~m_Pin;    
    
     /* Interrupt triggered because of Rising edge */
	 if( (EXTI->RTSR) & m_Pin ) 
	 {
		 m_CurrentState = HwButtonIntr::PressedState;
         
		 /* Configure Interrupt for the next falling edge */
		 EXTI->FTSR |= m_Pin;
         /* Clear Rising edge interrupt configuration */
		 EXTI->RTSR &= ~m_Pin; 

	 }
     /* Interrupt triggered because of Falling edge */
	 else					 
	 {         
		 m_CurrentState = HwButtonIntr::ReleasedState;
         
		 /* Configure Interrupt for the next Rising edge */
		 EXTI->RTSR |= m_Pin;
         
         /* Interrupt triggered because of Falling edge */
		 EXTI->FTSR &= ~m_Pin;
	 }

	/* Enable the further interrupt requests */
	 EXTI->IMR |= m_Pin;
}

}



