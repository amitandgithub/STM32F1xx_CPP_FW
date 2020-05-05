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
    static const uint8_t MAX_SCREENS = 2;
  public:   
    
     UI():Window(nullptr),m_CurrentActiveScreen(0),m_RegisteredScreenIndex(0),m_Captured(0),m_CurrentScreenIndex(0)
    {

    }
    
    
    
    void Run(InputEvents_t InputEvents)
    {
      if(InputEvents != HMI::NONE)
      {
        EventHandler(InputEvents);
        InputEvents = HMI::NONE;
      }
      Display(BLACK);
    }
    
    
    uint8_t Register(Window* pScren)
    {
      if(pScren != nullptr)
      {
        m_Screens[m_RegisteredScreenIndex] = pScren;
        return m_RegisteredScreenIndex++;
      }
      return 0xFF;
    }    
    
    virtual bool Display(Color_t BackgroundColor)
    {      
      m_Screens[m_CurrentActiveScreen]->Display(BLACK);
      
      return 0;
    }
    
    uint8_t EventHandler(InputEvents_t InputEvents)
    {
      if(InputEvents == HMI::UP)
      {
        if(m_Captured == 0)
        {
          m_CurrentActiveScreen--;
          if(m_CurrentActiveScreen == 0)  m_CurrentActiveScreen = MAX_SCREENS-1;
        }
        else
        {
          m_Screens[m_CurrentActiveScreen]->EventHandler(HMI::UP);
        }
      }
      else if(InputEvents == HMI::DOWN)
      {
        if(m_Captured == 0)
        {
          m_CurrentActiveScreen++;          
          if(m_CurrentActiveScreen >= MAX_SCREENS)  m_CurrentActiveScreen = 0;
        }
        else
        {
          m_Screens[m_CurrentActiveScreen]->EventHandler(HMI::DOWN);
        }
      }
      else if(InputEvents == HMI::PRESS)
      {
        if(m_Captured == 0)
        {
          m_Captured = 1;
        } 
        else
        {
          m_Screens[m_CurrentActiveScreen]->EventHandler(HMI::PRESS);
        }
      }
      else if(InputEvents == HMI::LONGPRESS)
      {        
        if(m_Captured == 1)
        {
          m_Captured = m_Screens[m_CurrentActiveScreen]->EventHandler(HMI::LONGPRESS);
        } 
      }
      return 0;
    }
    
  private:
      uint8_t   m_CurrentScreenIndex;
      uint8_t   m_Captured;
      uint8_t   m_CurrentActiveScreen;
      uint8_t   m_RegisteredScreenIndex;
      Window*   m_Screens[MAX_SCREENS];
      
  };
}

#endif // UI_h
