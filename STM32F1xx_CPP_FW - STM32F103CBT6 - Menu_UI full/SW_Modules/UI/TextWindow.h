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
    
    
    virtual bool Display(Color_t BackgroundColor)
    {      
      TFT_1_8.WriteString(m_pWindowContext->x1, m_pWindowContext->y1, m_Text, m_pWindowContext->font, m_pWindowContext->highlightColor,BackgroundColor); 
      return 0;
    }
    
    uint8_t EventHandler(InputEvents_t InputEvents)
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
      return 0;
    }
    
  private:
    const WindowContext_t*  m_pWindowContext;  
    char*         m_Text;   
  };
}

#endif // TextWindow_h
