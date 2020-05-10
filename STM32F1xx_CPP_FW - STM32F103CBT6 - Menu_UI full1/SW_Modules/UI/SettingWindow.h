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
#include"Callback.h"
#include "Window.h"

#define CPU_LOAD 1
namespace HMI
{ 

  
  class SettingWindow : public Window
  {
    
  public:   
    
     SettingWindow(const WindowContext_t* WindowContext, const SettingContext_t* SettingContext) :
     Window(WindowContext),
     m_SettingContext(SettingContext)
    {
      m_SettingText = SettingContext->SettingText;        
      m_CurrentDigitIndex = m_SettingContext->SettingVariablePosition - 1;
      intToStr( *SettingContext->pSettingVariable,&SettingContext->SettingText[ SettingContext->SettingVariablePosition],SettingContext->SettingVariableTotalDigits,' ');
      SettingContext->SettingText[SettingContext->Settinglen] = 0;  // Null termination for Display driver to detect the end-of-string
    }
    
    virtual bool Display(Color_t BackgroundColor)
    {
#if CPU_LOAD
      uint32_t Millis = HAL::GetTick();
#endif
      
      for(uint8_t i = 0; i < m_SettingContext->Settinglen; i++ )
      {
        if( (m_ConfigMode == 1) && (i == m_CurrentDigitIndex) )
        {
          WriteChar(m_pWindowContext->x1 + (m_pWindowContext->font->width) * i,
                            m_pWindowContext->y1, m_SettingText[i], m_pWindowContext->font, m_pWindowContext->highlightColor, BackgroundColor);
        }
        else
        {
          WriteChar(m_pWindowContext->x1 + (m_pWindowContext->font->width) * i,
                            m_pWindowContext->y1, m_SettingText[i], m_pWindowContext->font, m_pWindowContext->textColor , BackgroundColor);
        }
      }
#if CPU_LOAD
      m_CurrentMillis = HAL::GetTick() - Millis;
#endif
      return 0;
    }
    
    virtual uint8_t EventHandler(InputEvents_t InputEvents)
    {
      if(InputEvents == HMI::UP)
      {
        if( (m_ConfigMode == 1) && (m_CurrentDigitIndex < m_SettingContext->SettingVariablePosition + m_SettingContext->SettingVariableTotalDigits) )
        {          
          m_SettingText[m_CurrentDigitIndex]++;  // Increment the value on UP user event
          
          if(m_SettingText[m_CurrentDigitIndex] > '9') m_SettingText[m_CurrentDigitIndex] = '0';
        }
      }
      else if(InputEvents == HMI::DOWN)
      {
        if( (m_ConfigMode == 1) && (m_CurrentDigitIndex < m_SettingContext->SettingVariablePosition + m_SettingContext->SettingVariableTotalDigits) )
        {           
          m_SettingText[m_CurrentDigitIndex]--; // decrement the value on DOWN user event
          
          if(m_SettingText[m_CurrentDigitIndex] < '0') m_SettingText[m_CurrentDigitIndex] = '9';
        }
      }
      else if(InputEvents == HMI::PRESS)
      {
        if(m_ConfigMode == 0)
        {      
          m_ConfigMode = 1;
          m_CurrentDigitIndex = m_SettingContext->SettingVariablePosition - 1; // Reset this index          
        }        
        
        if( (m_ConfigMode == 1) && (m_CurrentDigitIndex < m_SettingContext->SettingVariablePosition + m_SettingContext->SettingVariableTotalDigits - 1) )
        {
          m_CurrentDigitIndex++; // Move to next character for setting its value from user  
        }
        else
        {        
          m_ConfigMode = 0; // Setting of all the digits are done, exit config mode  
          UpdateMemory(); // convert use entered chars to intiger value 
        }
      }
      else if(InputEvents == HMI::LONGPRESS)
      {
        m_ConfigMode = 0; // Setting of all the digits are done, exit config mode 
        UpdateMemory();  // convert use entered chars to intiger value
      }
      return m_ConfigMode;
    }
    
    //Convert user entered string into intiger value and update that in original memeory location
    void UpdateMemory()
    {
      uint32_t temp = 0;
      
      for(uint8_t i = m_SettingContext->SettingVariablePosition; i < m_SettingContext->SettingVariablePosition + m_SettingContext->SettingVariableTotalDigits; i++)
      {
        temp  += (m_SettingText[i] - '0') * Power_of_10[m_SettingContext->SettingVariablePosition + m_SettingContext->SettingVariableTotalDigits - 1 - i];
      }      
      *m_SettingContext->pSettingVariable = temp;
      if(m_SettingContext->pCallback) m_SettingContext->pCallback->CallbackFunction(temp);
    }
    
  private:
    const SettingContext_t*     m_SettingContext;    
    char*                       m_SettingText;  
    uint8_t                     m_CurrentDigitIndex; // to keep track of current digit under setting by user    
    uint8_t                     m_ConfigMode;
#if CPU_LOAD
    uint32_t                    m_CurrentMillis;
#endif    

  };
}

#endif // SettingWindow_h
