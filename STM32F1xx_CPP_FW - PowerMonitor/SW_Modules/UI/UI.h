/*
* UI.h
*
*  Created on: 5-May-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef UI_h
#define UI_h

#include"stdint.h"
#include<cstring>
#include"Callback.h"
#include "ST7735.h"
#include "Window.h"

namespace HMI
{       
  class UI : public Window
  {
    static const uint8_t MAX_SCREENS = 3;
  public:   
    
    UI():Window(nullptr)
    {
      
    }    
    
    void Run(InputEvents_t InputEvents);
    
    
    uint8_t Register(Window* pScren);   
    
    virtual bool Display(Color_t BackgroundColor);
    
    uint8_t EventHandler(InputEvents_t InputEvents);
    
    static void GoToPreviousScreen();
    
    static void GoToScreen(uint8_t Screen);
    
  private:
    static uint8_t      m_PreviousActiveScreen;
    static uint8_t      m_CurrentActiveScreen;
    static uint8_t      m_RegisteredScreenIndex;
    static Color_t      m_BackgroundColor;
    static uint8_t      m_Captured;      
    static Window*      m_Screens[MAX_SCREENS];
    
  };
}

#endif // UI_h
