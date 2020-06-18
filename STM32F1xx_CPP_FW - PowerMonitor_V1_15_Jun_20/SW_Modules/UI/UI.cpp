/*
* UI.h
*
*  Created on: 5-May-2020
*      Author: Amit Chaudhary
*  
*/


#include "UI.h"

namespace HMI
{       
  
    // static objects initialisation
  UI::Window*   UI::m_Screens[MAX_SCREENS];
  Color_t       UI::m_BackgroundColor;
  uint8_t       UI::m_Captured;
  uint8_t       UI::m_CurrentActiveScreen;
  uint8_t       UI::m_PreviousActiveScreen;
  uint8_t       UI::m_RegisteredScreenIndex;
    
    
    
  void UI::Run(InputEvents_t InputEvents)
    {
      if(InputEvents != HMI::NONE)
      {
        EventHandler(InputEvents);
        InputEvents = HMI::NONE;
      }
      Display(BLACK);
    }
    
    
    uint8_t UI::Register(Window* pScren)
    {
      if((pScren != nullptr) && (m_RegisteredScreenIndex < MAX_SCREENS) )
      {
        m_Screens[m_RegisteredScreenIndex] = pScren;
        return m_RegisteredScreenIndex++;
      }
      return 0xFF;
    }    
    
    bool UI::Display(Color_t BackgroundColor)
    {      
      m_Screens[m_CurrentActiveScreen]->Display(m_BackgroundColor);
      
      return 0;
    }
    
    uint8_t UI::EventHandler(InputEvents_t InputEvents)
    {
      if(InputEvents == HMI::UP)
      {
        if(m_Captured == 0)
        {
          m_CurrentActiveScreen--;
          if(m_CurrentActiveScreen == 0)  m_CurrentActiveScreen = m_RegisteredScreenIndex-1;
          ClearScreen(BLACK);
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
          if(m_CurrentActiveScreen >= m_RegisteredScreenIndex)  m_CurrentActiveScreen = 0;
          ClearScreen(BLACK);
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
          m_BackgroundColor = MAGENTA;
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
          m_BackgroundColor = BLACK;         
          if(m_CurrentActiveScreen >= m_RegisteredScreenIndex)  m_CurrentActiveScreen = 0;          
        } 
      }
      return 0;
    }
    
    void UI::GoToPreviousScreen()
    {
      m_CurrentActiveScreen = m_PreviousActiveScreen;
      m_Screens[m_PreviousActiveScreen]->ClearScreen(BLACK);
    }
    
    void UI::GoToScreen(uint8_t Screen)
    {
      if(Screen < m_RegisteredScreenIndex)
      {
        m_PreviousActiveScreen = m_CurrentActiveScreen;
        m_CurrentActiveScreen = Screen;
        m_Captured = 0;
        m_BackgroundColor = BLACK;
        m_Screens[Screen]->ClearScreen(BLACK);        
      }
    }

}

