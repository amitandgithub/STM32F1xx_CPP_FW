/*
 * ControlScreen.cpp
 *
 *  Created on: 25-Dec-2017
 *      Author: Amit Chaudhary
 *  23-Oct-2018: Modifed to add display buffer support.
 */

#include "ControlScreen.h"

namespace Peripherals 
{

ControlScreen::ControlScreen(): Screen(),ActiveLine(0)
{

  for(int i = 0; i<NO_OF_LINES_IN_SCREEN; i++)
  {
		m_LineEventHandlers.Array[i].LongTouchHandler = nullptr;
        m_LineEventHandlers.Array[i].LLongTouchHandler = nullptr;
  }
  InvertRow(ActiveLine);
  DrawStr(LAST_LINE,0,"Next          ");
  AddHandler(LAST_LINE,Peripherals::UI::GoToNextScreen,nullptr);
}

ControlScreen::ControlScreen(char* pText): Screen(pText),ActiveLine(0)
{

  for(int i = 0; i<NO_OF_LINES_IN_SCREEN; i++)
  {
      AddHandler(i,nullptr,nullptr);
  }
  InvertRow(ActiveLine);
  DrawStr(LAST_LINE,0,"Next          ");
  AddHandler(LAST_LINE,Peripherals::UI::GoToNextScreen,nullptr);
}

ControlScreen::~ControlScreen()
{
  for(int i = 0; i<NO_OF_LINES_IN_SCREEN; i++)
  {
      AddHandler(i,nullptr,nullptr);
  }
}

void ControlScreen::AddHandler(unsigned char LineNo, EventHandler_t LongTouchHandler, EventHandler_t LLongTouchHandler)
{
		m_LineEventHandlers.Array[LineNo].LongTouchHandler = LongTouchHandler;
        m_LineEventHandlers.Array[LineNo].LLongTouchHandler = LLongTouchHandler;
}

void ControlScreen::EventHandler(Event_t anEvent)
{
	if(anEvent == Touch)
	{

		MovePointer(Down);
	}
	else if(anEvent == LongTouch)
	{
		if(m_LineEventHandlers.Array[ActiveLine].LongTouchHandler)
			m_LineEventHandlers.Array[ActiveLine].LongTouchHandler();
	}
	else if(anEvent == LongLongTouch)
	{
		if(m_LineEventHandlers.Array[ActiveLine].LLongTouchHandler)
			m_LineEventHandlers.Array[ActiveLine].LLongTouchHandler();
	}
	else
	{
		// Nothing To Do
	}
}

void ControlScreen::MovePointer(Direction_t Direction)
{
	//clear the pointer from current line
        InvertRow(ActiveLine);

	if( (ActiveLine == FIRST_LINE) && (Direction == Up) )
	{
		// Roll over to the last line
		ActiveLine = LAST_LINE;
	}
	else if( (ActiveLine == LAST_LINE) && (Direction == Down) )
	{
		// Roll over to the last line
		ActiveLine = FIRST_LINE;
	}
	else if( Direction == Down )
	{
		// Roll over to the last line
		ActiveLine += 1;
	}
	else if( Direction == Up )
	{
		// Roll over to the First line
		ActiveLine -= 1;

	}
	else
	{

	}

     InvertRow(ActiveLine);
}

} /* namespace Peripherals */