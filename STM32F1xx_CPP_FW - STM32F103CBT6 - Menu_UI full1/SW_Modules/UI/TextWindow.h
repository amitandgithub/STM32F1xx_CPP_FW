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
    
     TextWindow(const WindowContext_t* WindowContext,const TextWindowContext_t* TextWindowContext) :
      Window(WindowContext),
     m_pTextWindowContext(TextWindowContext)
    {
      m_Text = m_pTextWindowContext->SettingText;
      //m_pTextWindowContext->SettingText[m_pTextWindowContext->Settinglen] = 0;  // Null termination for Display driver to detect the end-of-string
    }    
    
    virtual bool Display(Color_t BackgroundColor)
    {      
      TFT_1_8.WriteString(m_pWindowContext->x1, m_pWindowContext->y1, m_Text, m_pWindowContext->font, m_pWindowContext->textColor,BackgroundColor); 
      return 0;
    }
    
    uint8_t EventHandler(InputEvents_t InputEvents)
    {
//      if(InputEvents == HMI::UP)
//      {
//        if(m_pTextWindowContext->pUPCb) m_pTextWindowContext->pUPCb->CallbackFunction();
//      }
//      else if(InputEvents == HMI::DOWN)
//      {
//        if(m_pTextWindowContext->pDownCb) m_pTextWindowContext->pDownCb->CallbackFunction();
//      }
      if(InputEvents == HMI::PRESS)
      {
        if(m_pTextWindowContext->pPressCb) m_pTextWindowContext->pPressCb->CallbackFunction();
      }
      else if(InputEvents == HMI::LONGPRESS)
      {
        if(m_pTextWindowContext->pLPressCb) m_pTextWindowContext->pLPressCb->CallbackFunction();
      }
      return 0;
    }
    
  private:
    const TextWindowContext_t*  m_pTextWindowContext;
    char*                       m_Text;      
  };
}

#endif // TextWindow_h
