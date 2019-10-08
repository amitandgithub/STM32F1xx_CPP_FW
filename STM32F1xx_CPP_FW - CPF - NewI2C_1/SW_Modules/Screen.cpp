/*
 * Screen.cpp
 *
 *  Created on: 25-Dec-2017
 *      Author: Amit Chaudhary
 *  21-Oct-2018: Modifed to add display buffer support.
 */

#include "Screen.h"

namespace Peripherals {

Screen::Screen():DisplayBuffer()
{
    
}

Screen::Screen(char* pText): DisplayBuffer(pText)
{
    m_TouchHandler      = nullptr;
    m_LongTouchHandler  = nullptr;
    m_LLongTouchHandler = nullptr;
}

Screen::~Screen()
{
    m_TouchHandler      = nullptr;
    m_LongTouchHandler  = nullptr;
    m_LLongTouchHandler = nullptr;
}

void Screen::SetScreenHandler( EventHandler_t TouchHandler, EventHandler_t LongTouchHandler, EventHandler_t LLongTouchHandler)
{
    m_TouchHandler      = TouchHandler;
    m_LongTouchHandler  = LongTouchHandler;
    m_LLongTouchHandler = LLongTouchHandler;
}
    
void Screen::EventHandler(Event_t anEvent)
{
    if(anEvent == Touch)
	{
        if(m_TouchHandler != nullptr)
        {
             m_TouchHandler();
        }
        else
        {
            Peripherals::UI::GoToNextScreen();
        }       
	}
	else if(anEvent == LongTouch)
	{
        if(m_LongTouchHandler != nullptr)
        {
             m_LongTouchHandler();
        }
        else
        {
            Peripherals::UI::GoToPreviousScreen();
        }       
	}
	else if(anEvent == LongLongTouch)
	{
        if(m_LLongTouchHandler != nullptr)
        {
             m_LLongTouchHandler();
        }
        else
        {
            Peripherals::UI::GoToPreviousScreen();
        }       
	}
	else
	{
		// Nothing To Do
	}

}


} /* namespace Peripherals */