/*
 * UI.cpp
 *
 *  Created on: 01-Nov-2017
 *      Author: Amit Chaudhary
 *  23-Oct-2018: Modifed to add display buffer support.
 */

#include"UI.h"
#include"Nokia5110LCD.h"

using namespace std;

namespace Peripherals
{

unsigned char UI::ActiveScreen = 0; 
unsigned char UI::TotalRegisteredScreens = 0;
unsigned char UI::PreviousActiveScreen = 0;
UI::ScreenHandle_t UI::TotalAddeedScreens = 0;
Screen* UI::m_Screens[NO_OF_SCREENS_IN_UI];

UI::UI(Nokia5110LCD* pNokiaLCD): LCD(pNokiaLCD)
{

}
bool UI::Init()
{
	LCD->HwInit();
	LCD->DisplayStr(0, 0, "Initializing");
        TotalAddeedScreens = 0;
        for(uint8_t i = 0; i < NO_OF_SCREENS_IN_UI; i++)
        {
          m_Screens[i] = nullptr;
        }
        
	return true;
}

UI::ScreenHandle_t UI::AddScreen(Screen* pScreen )
{ 
    if((TotalAddeedScreens < NO_OF_SCREENS_IN_UI) && (pScreen != nullptr))
    {
        m_Screens[TotalAddeedScreens++] = pScreen;
        return (TotalAddeedScreens - 1); 
    }
    
    return 0xFF;
    
}

void UI::GoToNextScreen()
{
//    PreviousActiveScreen = ActiveScreen;
//    ++ActiveScreen %= TotalRegisteredScreens ;
//    ScreenHandle_t LActiveScreen = GetActiveScreen();
//    
//    while(m_Screens[(++LActiveScreen)%NO_OF_SCREENS_IN_UI] == nullptr);
    
//    ScreenHandle_t i;
//    
//    for( i = GetActiveScreen()+1; (i < NO_OF_SCREENS_IN_UI) && (m_Screens[i] != nullptr); i++ )
//    {
//        if(i == NO_OF_SCREENS_IN_UI)
//            i = 0;                      
//    }
    
    
    PreviousActiveScreen = ActiveScreen;
    
    ActiveScreen++;
    
    if(ActiveScreen == TotalAddeedScreens )
        ActiveScreen = 0;    
}

void UI::GoToPreviousScreen()
{    
    PreviousActiveScreen = ActiveScreen;
    
    ActiveScreen--;
    
    if(ActiveScreen == (uint8_t)-1 )
        ActiveScreen = TotalAddeedScreens-1;
    
}

void UI::EventHamdler(Screen::Event_t& rEvent)
{
	if( rEvent != Screen::MaxEvents )
	{
        m_Screens[ActiveScreen % NO_OF_SCREENS_IN_UI]->EventHandler(rEvent);
        rEvent = Screen::MaxEvents;
	}
}

void UI::DisplayScreen()
{ 
  if(m_Screens[ActiveScreen % TotalAddeedScreens]) 
  {
      LCD->DisplayBitmap( m_Screens[ActiveScreen % TotalAddeedScreens]->GetBuffer() );
  }
  else
  {
    if(m_Screens[0])
      LCD->DisplayBitmap( m_Screens[0]->GetBuffer() );
  }
  LCD->Refresh();

}

void UI::GoToScreen(Screen* pScreen )
{ 
    for(uint8_t i=0; i<= TotalAddeedScreens ; i++)
    {
        if(m_Screens[i] == pScreen)
        {
            GoToScreen(i);
        }
    }
}













}


