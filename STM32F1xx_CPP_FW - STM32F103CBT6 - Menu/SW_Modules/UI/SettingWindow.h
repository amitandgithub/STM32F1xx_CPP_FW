/*
* SettingWindow.h
*
*  Created on: 28-Apr-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef SettingWindow_h
#define SettingWindow_h

//#include"stdint.h"
//#include<cstring>
//#include"Callback.h"
#include "Window.h"

namespace HMI
{ 
  static const uint32_t Power_of_10[11] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
  
  typedef struct
    {
      uint32_t* pSettingVariable;
      char*     SettingText;
      uint8_t   Settinglen;
      uint8_t   SettingVariableTotalDigits;
      uint8_t   SettingVariablePosition;
    }SettingContext_t;
  
  class SettingWindow : public Window
  {
    
  public:   
    
     SettingWindow(const WindowContext_t* WindowContext, const SettingContext_t* SettingContext) :
     Window(WindowContext),
     m_SettingContext(SettingContext)
    {
      m_CurrentMillis = 0;
      m_SettingText = SettingContext->SettingText;        
      ResetCounter();
      intToStr( *SettingContext->pSettingVariable,&SettingContext->SettingText[ SettingContext->SettingVariablePosition],m_TotalDigits,' ');
      SettingContext->SettingText[SettingContext->Settinglen-1] = 0;
    }
    
    void UpdateMemory()
    {
      uint32_t temp = 0;
      
      for(uint8_t i=0;i<m_TotalDigits;i++)
      {
        temp  += (m_SettingText[m_SettingContext->SettingVariablePosition + i] - '0') * Power_of_10[m_TotalDigits -1 -i];
      }      
      *m_SettingContext->pSettingVariable = temp;
    }
    
    void ResetCounter()
    {
      m_TotalDigits = m_SettingContext->SettingVariableTotalDigits;
    }
    
    virtual bool Run(InputEvents_t InputEvents)
    {      
      EventHandler(InputEvents);
      
      Display();
      
      return m_ConfigMode;
    }
    
    void Display()
    {      
      for(uint8_t i = 0; i< (m_SettingContext->Settinglen) && m_SettingText[i]; i++ ) // && m_SettingText[i]
      {
        if( (m_ConfigMode == 1) && (i == (m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex)) )
        {
          WriteChar(m_pWindowContext->x1 + (m_pWindowContext->font->width) * i,
                            m_pWindowContext->y1, m_SettingText[i], m_pWindowContext->font, m_pWindowContext->highlightColor, m_pWindowContext->backgroundColor);
        }
        else
        {
          WriteChar(m_pWindowContext->x1 + (m_pWindowContext->font->width) * i,
                            m_pWindowContext->y1, m_SettingText[i], m_pWindowContext->font, m_pWindowContext->textColor , m_pWindowContext->backgroundColor);
        }
      }
    }
    
    void EventHandler(InputEvents_t InputEvents)
    {

      if(InputEvents == HMI::UP)
      {
        if( (m_ConfigMode == 1) && (m_TotalDigits < m_SettingContext->Settinglen) )
        {          
          m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex]++;  // Increment the value on user input
          
          if(m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex] > '9') m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex] = '0';
        }
      }
      else if(InputEvents == HMI::DOWN)
      {
       if( (m_ConfigMode == 1) && (m_TotalDigits < m_SettingContext->Settinglen) )
        {           
          m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex]--; // decrement the value on user input
          
          if(m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex] < '0') m_SettingText[m_SettingContext->SettingVariablePosition - 1 + m_CurrentDigitIndex] = '9';
        }
      }
      else if(InputEvents == HMI::PRESS)
      {
        if(m_ConfigMode == 0)
        {      
          m_ConfigMode = 1;
          m_CurrentMillis = 0;
          ResetCounter(); 
          m_CurrentDigitIndex = 0;
        }
        
        if( (m_ConfigMode == 1) && (m_TotalDigits) )
        {
          m_CurrentDigitIndex++;          
          m_TotalDigits--; // Move to next character for setting its value from user
        }
        else
        {        
          ResetCounter();
          m_ConfigMode = 0; // Setting of all the digits are done, exit config mode  
          UpdateMemory(); // convert use entered chars to intiger value 
        }
      }
      else if(InputEvents == HMI::LONGPRESS)
      {
        ResetCounter();
        m_ConfigMode = 0; // Setting of all the digits are done, exit config mode 
        UpdateMemory();  // convert use entered chars to intiger value
      }
    }
    
  private:
    //const WindowContext_t*  m_pWindowContext;
    const SettingContext_t* m_SettingContext;    
    char*         m_SettingText;   
    uint8_t       m_CurrentMillis;
    uint8_t       m_CurrentDigitIndex; // to keep track of current digit under setting by user    
    uint8_t       m_TotalDigits;
    uint8_t       m_ConfigMode;
    uint16_t      m_CurrentBackgroundColor;
    
    
    
    
  };
}

#endif // SettingWindow_h
