/*
 * Menu.cpp
 *
 *  Created on: 30-Oct-2017
 *      Author: Amit Chaudhary
 */

#include"Line.hpp"
#include"Menu.hpp"

namespace App
{



/*
Menu::Menu(Nokia5110LCD* pNokiaLCD):ActiveLine(0), m_pNokiaLCD(pNokiaLCD)
{
	for(int i = 0; i<Menu::NO_OF_LINES_IN_MENU * Menu::NO_OF_CHARS_IN_LINE; i++)
		MenuText[i] = ' ';

	for(int i = 0; i<Menu::NO_OF_LINES_IN_MENU * Menu::NO_OF_HANDLERS_IN_LINE; i++)
		EventHandlers[i] = nullptr;
}
*/
Menu::Menu():ActiveLine(0)
{
	for(int i = 0; i<Menu::NO_OF_LINES_IN_MENU * Menu::NO_OF_CHARS_IN_LINE; i++)
		MenuText[i] = ' ';

	for(int i = 0; i<Menu::NO_OF_LINES_IN_MENU * Menu::NO_OF_HANDLERS_IN_LINE; i++)
		EventHandlers[i] = nullptr;
}

bool Menu::AddLine(Line &rLine, unsigned char LineNumber)
{
	if(LineNumber <= NO_OF_LINES_IN_MENU )
	{
		std::memcpy(&MenuText[LineNumber * NO_OF_CHARS_IN_LINE + 1 ],&rLine.Text[0],NO_OF_CHARS_IN_LINE);
		std::memcpy(&EventHandlers[LineNumber*Menu::NO_OF_HANDLERS_IN_LINE],&rLine.aEventHandlers[0],NO_OF_HANDLERS_IN_LINE*sizeof(EventHandler_t));
		/*EventHandlers[LineNumber + 0] = rLine.aEventHandlers[0];
		EventHandlers[LineNumber + 1] = rLine.aEventHandlers[1];
		EventHandlers[LineNumber + 2] = rLine.aEventHandlers[2];*/

		if( LineNumber == 0)
		{
			MenuText[LineNumber * NO_OF_CHARS_IN_LINE  ] = m_Pointer;
		}
		return true;
	}
	return false;

}

void Menu::EventHandler(Event_t anEvent)
{
	if(anEvent == Touch)
	{
		//if(EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + Touch ])
			//EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + Touch ]();

		MovePointer(Down);
	}
	else if(anEvent == LongTouch)
	{
		if(EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + LongTouch ])
			EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + LongTouch ]();
	}
	else if(anEvent == LongLongTouch)
	{
		if(EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + LongLongTouch ])
			EventHandlers[ActiveLine*Menu::NO_OF_HANDLERS_IN_LINE + LongLongTouch ]();
	}
	else
	{
		// Nothing To Do
	}
}

void Menu::MovePointer(Direction_t Direction)
{
	//clear the pointer from current line
	SetLineChar(static_cast<unsigned char>(ActiveLine), 0, m_Space );

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
		// Roll over to the last line
		ActiveLine -= 1;

	}
	else
	{

	}
	SetLineChar(static_cast<unsigned char>(ActiveLine), 0, m_Pointer );
}


void Menu::SetLineText(unsigned char Line, unsigned char Col, const char* pName)
{
	if( (Col <= (NO_OF_CHARS_IN_LINE-1)) && (Col >= 0)
		&&(Line <= (NO_OF_LINES_IN_MENU-1)) && (Line >= 0) )
	{
		std::memcpy(&MenuText[Line * NO_OF_CHARS_IN_LINE + Col],pName,(NO_OF_CHARS_IN_LINE - Col));
	}

}

void Menu::SetLineChar(unsigned char Line, unsigned char Col, char Ch)
{
	if( (Col <= (NO_OF_CHARS_IN_LINE-1)) && (Col >= 0)
		&&(Line <= (NO_OF_LINES_IN_MENU-1)) && (Line >= 0) )
	{
		MenuText[Line * NO_OF_CHARS_IN_LINE + Col] = Ch;
	}

}




}
