/*
 * Screen.hpp
 *
 *  Created on: 25-Dec-2017
 *      Author: Amit Chaudhary
 *  21-Oct-2018: Modifed to add display buffer support.
 */

#include <cstring>
#include<stdio.h>
#include"UI.h"

#ifndef SCREEN_HPP_
#define SCREEN_HPP_


#include "DisplayBuffer.h"

namespace Peripherals
{

class Screen : public DisplayBuffer
{
public:
  	typedef void(*EventHandler_t)();
    
//    static const        uint8_t NO_OF_LINES_IN_SCREEN          = 6U;
    
	typedef enum
	{
		Touch = 0,
		LongTouch,
		LongLongTouch,
		MaxEvents
	}Event_t;

	Screen();
    
    Screen(char* pText);

	virtual ~Screen();
    
    virtual void EventHandler(Event_t anEvent);
    
    void SetScreenHandler( EventHandler_t TouchHandler, EventHandler_t LongTouchHandler, EventHandler_t LLongTouchHandler);
    
private:
    
    EventHandler_t m_TouchHandler;
    EventHandler_t m_LongTouchHandler;
    EventHandler_t m_LLongTouchHandler;
};

} /* namespace Peripherals */

#endif /* SCREEN_HPP_ */