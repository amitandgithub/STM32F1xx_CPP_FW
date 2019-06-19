/*
 * UI.hpp
 *
 *  Created on: 01-Nov-2017
 *      Author: Amit Chaudhary
 *  13-Oct-2018 : Modified for the New version based upon STM32_HAL
 */

#ifndef APP_INC_UI_HPP_
#define APP_INC_UI_HPP_

#include"Nokia5110LCD.h"
#include <cstring>
#include "Screen.h"

namespace Peripherals
{


class UI
{
public:
    constexpr static unsigned char NO_OF_SCREENS_IN_UI = 10U;
    typedef uint8_t ScreenHandle_t;

	UI(Nokia5110LCD* pNokiaLCD);
    
	~UI(){};
    
	bool Init();        
    
    ScreenHandle_t AddScreen(Screen* pScreen );
    
    static ScreenHandle_t GetActiveScreen()                 {return ActiveScreen; }
    
    static void SetActiveScreen(ScreenHandle_t ScreenNo)    { PreviousActiveScreen = ActiveScreen; ActiveScreen = ScreenNo; }
    
    static void GoToScreen(ScreenHandle_t ScreenNo){ PreviousActiveScreen = ActiveScreen; ActiveScreen = ScreenNo; };
    
    static void GoToScreen(Screen* pScreen );
    
    static void GoToPreviousScreen();
    
    static void GoToNextScreen();
    
    void DisplayScreen();
    
    void Run(){DisplayScreen();}
    
    void EventHamdler(Screen::Event_t& rEvent);

private:
        static Screen* m_Screens[NO_OF_SCREENS_IN_UI];
        static ScreenHandle_t ActiveScreen;
        static unsigned char PreviousActiveScreen;
        static unsigned char TotalRegisteredScreens;
        static ScreenHandle_t TotalAddeedScreens;
        Nokia5110LCD* LCD;
};

























}

























#endif /* APP_INC_UI_HPP_ */
