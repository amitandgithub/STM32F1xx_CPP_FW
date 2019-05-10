/*
 * ControlScreen.hpp
 *
 *  Created on: 25-Dec-2017
 *      Author: Amit Chaudhary
 *  23-Oct-2018: Modifed to add display buffer support.
 */

#ifndef CONTROLSCREEN_HPP_
#define CONTROLSCREEN_HPP_

#include "Screen.h"

namespace Peripherals {

class ControlScreen : public Screen
{
public:
    typedef enum
	{
		Up = 0,
		Down
	}Direction_t;
    
    typedef struct
    {
        EventHandler_t LongTouchHandler;
        EventHandler_t LLongTouchHandler;        
    }LineEventHandler_t;
    
    typedef struct
    {
        LineEventHandler_t Array[NO_OF_LINES_IN_SCREEN];       
    }ControlScreenHandler_t;
    constexpr static unsigned char LAST_LINE = (NO_OF_LINES_IN_SCREEN-1U);
	const static unsigned char FIRST_LINE = 0U;
    
    ControlScreen();
    
    ControlScreen(char* pText);
    
    virtual ~ControlScreen();

    void AddHandler(unsigned char LineNo, EventHandler_t LongTouchHandler, EventHandler_t LLongTouchHandler);

	virtual void EventHandler(Event_t anEvent);

	unsigned char GetActiveLine() const {return ActiveLine;}
    
private:
    void MovePointer(Direction_t Direction);
private:
    ControlScreenHandler_t m_LineEventHandlers;
	unsigned char ActiveLine;

};

} /* namespace Peripherals */

#endif /* CONTROLSCREEN_HPP_ */