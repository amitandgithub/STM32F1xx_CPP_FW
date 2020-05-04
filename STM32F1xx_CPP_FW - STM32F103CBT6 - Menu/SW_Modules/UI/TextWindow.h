/*
* TextWindow.h
*
*  Created on: 28-Apr-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef TextWindow_h
#define TextWindow_h

#include"stdint.h"
#include<cstring>
#include"Callback.h"
#include "ST7735.h"
#include "Window.h"

namespace HMI
{ 
  
  class TextWindow : public Window
  {
    
  public:   
    
     TextWindow(const WindowContext_t* WindowContext) :
      Window(nullptr),
     m_pWindowContext(WindowContext)
    {

    }
    
    virtual bool Run(InputEvents_t InputEvents)
    {      
      EventHandler(InputEvents);
      
      Display();
      
      return 0;
    }
    
    void Display()
    {      
      TFT_1_8.WriteString(m_pWindowContext->x1, m_pWindowContext->y1, m_Text, m_pWindowContext->font, m_pWindowContext->highlightColor, m_pWindowContext->backgroundColor); 
    }
    
    void EventHandler(InputEvents_t InputEvents)
    {
      if(InputEvents == HMI::UP)
      {

      }
      else if(InputEvents == HMI::DOWN)
      {

      }
      else if(InputEvents == HMI::PRESS)
      {

      }
      else if(InputEvents == HMI::LONGPRESS)
      {

      }
    }
    
  private:
    const WindowContext_t*  m_pWindowContext;  
    char*         m_Text;   
  };
}

#endif // TextWindow_h
