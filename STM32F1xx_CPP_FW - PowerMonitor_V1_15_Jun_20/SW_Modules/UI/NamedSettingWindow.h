/*
* NamedSettingWindow.h
*
*  Created on: 8-May-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef NamedSettingWindow_h
#define NamedSettingWindow_h

//#include"stdint.h"
#include<cstring>
//#include"Callback.h"
#include "Window.h"

#define CPU_LOAD 1
namespace HMI
{
  

  
  class NamedSettingWindow : public Window
  {
    
  public:   
    
     NamedSettingWindow(const WindowContext_t* WindowContext, const NamedSettingContext_t* NamedSettingContext) :
     Window(WindowContext),
     m_SettingContext(NamedSettingContext)
    {
      //m_SettingText = NamedSettingContext->SettingText;        
      m_CurrentNameIndex = 0;
      CopyName(0);
      NamedSettingContext->SettingText[NamedSettingContext->Settinglen] = 0; //Add null termination character 
    }
    
    virtual bool Display(Color_t BackgroundColor)
    {
#if CPU_LOAD
      uint32_t Millis = HAL::GetTick();
#endif
        
        if(m_ConfigMode == 1)
        {
          WriteString(m_pWindowContext->x1, m_pWindowContext->y1, m_SettingContext->SettingText, m_pWindowContext->font, m_pWindowContext->highlightColor, BackgroundColor);
        }
        else
        {
           WriteString(m_pWindowContext->x1 , m_pWindowContext->y1, m_SettingContext->SettingText, m_pWindowContext->font, m_pWindowContext->textColor, BackgroundColor); 
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
        if( (m_ConfigMode == 1) )
        {          
          if(m_CurrentNameIndex == 0) m_CurrentNameIndex = m_SettingContext->NamedSettingTableLen - 1;
          m_CurrentNameIndex--;          
          CopyName(m_CurrentNameIndex);
        }
      }
      else if(InputEvents == HMI::DOWN)
      {
        if((m_ConfigMode == 1))
        {                    
          m_CurrentNameIndex++;
          if(m_CurrentNameIndex >= m_SettingContext->NamedSettingTableLen) m_CurrentNameIndex = 0;
          
          CopyName(m_CurrentNameIndex);         
        }
      }
      else if(InputEvents == HMI::PRESS)
      {
        if(m_ConfigMode == 0)
        {      
          m_ConfigMode = 1;
          m_CurrentNameIndex = 0; // Reset this index          
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
      *m_SettingContext->pSettingVariable = m_SettingContext->pNamedSettingTable[m_CurrentNameIndex].SettingValue;
       if(m_SettingContext->pCallback)  m_SettingContext->pCallback->CallbackFunction(m_SettingContext->pNamedSettingTable[m_CurrentNameIndex].SettingValue);
    }

    void CopyName(uint8_t NameIndex)
    {
      uint8_t idx = 0;
      
      while( (idx < m_SettingContext->Settinglen - m_SettingContext->SettingVariablePosition) && (m_SettingContext->pNamedSettingTable[NameIndex].SettingText[idx] != 0) ) // (m_SettingContext->pNamedSettingTable[NameIndex].SettingText[idx] != 0) && 
      {
        m_SettingContext->SettingText[m_SettingContext->SettingVariablePosition + idx] = m_SettingContext->pNamedSettingTable[NameIndex].SettingText[idx];
        idx++;
      }
      memset(&m_SettingContext->SettingText[m_SettingContext->SettingVariablePosition + idx],' ',m_SettingContext->Settinglen - m_SettingContext->SettingVariablePosition - idx);
    }
    
    virtual void Init()
    {
      for(uint32_t i = 0; i < m_SettingContext->NamedSettingTableLen; i++)
      {
        if(*m_SettingContext->pSettingVariable == m_SettingContext->pNamedSettingTable[i].SettingValue )
        {
          CopyName(i);
        }
      }      
    }
    
  private:
    const NamedSettingContext_t*        m_SettingContext;    
    //char*                               m_SettingText;  
    uint8_t                             m_CurrentNameIndex; // to keep track of current digit under setting by user    
    uint8_t                             m_ConfigMode;
#if CPU_LOAD
    uint32_t                            m_CurrentMillis;
#endif    

  };
}

#endif // NamedSettingWindow_h
