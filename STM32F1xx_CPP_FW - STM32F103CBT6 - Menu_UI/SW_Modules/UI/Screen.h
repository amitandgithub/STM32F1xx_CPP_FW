/*
* Screen.hpp
*
*  Created on: 25-Dec-2017
*      Author: Amit Chaudhary
*  21-Oct-2018: Modifed to add display buffer support.
*/

#include <cstring>
#include<stdio.h>
//#include"UI.h"

#ifndef SCREEN_HPP_
#define SCREEN_HPP_


#include "Window.h"

namespace HMI
{
  
  class Screen : public Window
  {
  public:
    static const uint8_t MAX_WINDOWS = 6;
    static const uint16_t WINDOW_HIGHLIGHT_COLOR = MAGENTA;//YELLOW;
    static const uint8_t ACTIVE_WINDOW_NONE = 255;
    
    Screen():Window(nullptr),m_CurrentActiveWindow(0),m_RegisteredWindowsIndex(0),m_Captured(0),m_CurrentWindowIndex(0)
    {
      HAL::MemSet32((uint32_t*)m_Windows,0,MAX_WINDOWS);
    }
    
    uint8_t Register(Window* pWindow)
    {
      if(pWindow != nullptr)
      {
        m_Windows[m_RegisteredWindowsIndex] = pWindow;
        return m_RegisteredWindowsIndex++;
      }
      return 0xFF;
    }    
    
    virtual bool Display(Color_t BackgroundColor)
    {            
      if(m_CurrentWindowIndex == m_CurrentActiveWindow)
      {
        m_Windows[m_CurrentWindowIndex]->Display(BackgroundColor); 
      }
      else
      {
        m_Windows[m_CurrentWindowIndex]->Display(BLACK);
      }
      
      if(++m_CurrentWindowIndex >= MAX_WINDOWS) m_CurrentWindowIndex = 0;  
      
      return 0;
    }   
    
    uint8_t EventHandler(InputEvents_t InputEvents)
    {
      if(InputEvents == HMI::UP)
      {
        if(m_Captured)
        {
          m_Windows[m_CurrentActiveWindow]->EventHandler(HMI::UP);
        }
        else
        {
          m_CurrentActiveWindow--;
          if(m_CurrentActiveWindow == 0)  m_CurrentActiveWindow = MAX_WINDOWS-1;
        }            
      }
      else if(InputEvents == HMI::DOWN)
      {
        if(m_Captured)
        {
          m_Windows[m_CurrentActiveWindow]->EventHandler(HMI::DOWN);
        }
        else
        {
          m_CurrentActiveWindow++;          
          if(m_CurrentActiveWindow >= MAX_WINDOWS)  m_CurrentActiveWindow = 0; 
        }             
      }
      else if(InputEvents == HMI::PRESS)
      {
        if(m_Captured)
        {
          m_Captured = m_Windows[m_CurrentActiveWindow]->EventHandler(HMI::PRESS);
        }
        else
        {
          m_Captured = 1;
        }       
      }
      else if(InputEvents == HMI::LONGPRESS)
      {
        if(m_Captured)
        {
          m_Captured = m_Windows[m_CurrentActiveWindow]->EventHandler(HMI::LONGPRESS);
        }
        else
        {
          m_Captured = 0;
        }      
      }
      return 0;
    }
    
  private:
    uint8_t     m_CurrentWindowIndex;
    uint8_t     m_Captured;
    uint8_t     m_CurrentActiveWindow;
    uint8_t     m_RegisteredWindowsIndex;
    Window*     m_Windows[MAX_WINDOWS];
  
  };
  
}

#endif /* SCREEN_HPP_ */