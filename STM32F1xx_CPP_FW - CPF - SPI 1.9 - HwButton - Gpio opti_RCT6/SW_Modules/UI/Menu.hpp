/*
 * Menu.hpp
 *
 *  Created on: 30-Oct-2017
 *      Author: Amit Chaudhary
 */

#ifndef APP_INC_MENU_HPP_
#define APP_INC_MENU_HPP_

#include"Line.hpp"
#include"Nokia5110LCD.h"
#include <cstring>

namespace Peripherals
{

class Menu
{
public:
	typedef void(*EventHandler_t)();
	typedef enum
	{
		Up = 0,
		Down

	}Direction_t;
	typedef enum
	{
		Touch = 0,
		LongTouch,
		LongLongTouch,
		MaxEvents
	}Event_t;

	const static unsigned char NO_OF_CHARS_IN_LINE = 12U;
	const static unsigned char NO_OF_HANDLERS_IN_LINE = 3U;
	const static unsigned char NO_OF_LINES_IN_MENU = 6U;
	const static char m_Pointer = '>';
	const static char m_Space = ' ';
	constexpr static unsigned char LAST_LINE = (NO_OF_LINES_IN_MENU-1U);
	const static unsigned char FIRST_LINE = 0U;
	constexpr static unsigned char SIZE_OF_MENU = ( (NO_OF_LINES_IN_MENU * NO_OF_CHARS_IN_LINE) + (NO_OF_LINES_IN_MENU * NO_OF_HANDLERS_IN_LINE * sizeof(EventHandler_t)) ) ;

	//Menu(Nokia5110LCD* m_pNokiaLCD);

	Menu();

	~Menu(){};

	bool AddLine(Line &rLine, unsigned char LineNumber);

	void DeleteLine(unsigned char LineNumber){};

	void EventHandler(Event_t anEvent);

	inline unsigned char GetActiveLine() const {return ActiveLine;}

	void SetLineText(unsigned char Line, unsigned char Col, const char* pName);

	void SetLineChar(unsigned char Line, unsigned char Col, char Ch);
  
        inline char * GetMenuAddress(){return MenuText;}


	//EventHandler_t  __attribute__ ((aligned (2))) EventHandlers[NO_OF_LINES_IN_MENU * NO_OF_HANDLERS_IN_LINE];

private:
  	char MenuText[NO_OF_LINES_IN_MENU * NO_OF_CHARS_IN_LINE];

	EventHandler_t EventHandlers[NO_OF_LINES_IN_MENU * NO_OF_HANDLERS_IN_LINE];
	void MovePointer(Direction_t Direction);
	unsigned char ActiveLine;
	//Nokia5110LCD* m_pNokiaLCD;

};






















}




































#endif /* APP_INC_MENU_HPP_ */
