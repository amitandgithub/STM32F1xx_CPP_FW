/*
* Menu.h
*
*  Created on: 22-Apr-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef Menu_h
#define Menu_h

#include"stdint.h"
#include<cstring>
#include"Callback.h"
#include "ST7735.h"

#include"Window.h"
namespace HMI
{
//  typedef enum
//  {     
//    UP,
//    DOWN,
//    PRESS,
//    LONGPRESS,   
//    NONE,
//  }InputEvents_t;  
  
  template<uint16_t ScreenWidth ,uint16_t ScreenHeight, FontDef* font,uint16_t CharWidth,uint16_t CharHeight, uint16_t TextColor, uint16_t BackgroundColor, uint16_t HighlighterColor>
  class Menu
  {
  public:
    
    static constexpr uint8_t LINES_PER_MENU = ScreenHeight/CharHeight;
    static constexpr uint8_t CHARS_PER_LINE = ScreenWidth/CharWidth;
    static constexpr uint8_t HANDLERS_PER_LINE = 4;    
    
    typedef Callback LineHandler_t;
    
    
    typedef struct 
    {
      char Text[CHARS_PER_LINE+1]; // +1 for Null character
      LineHandler_t* Handlers[HANDLERS_PER_LINE];
//      LineHandler_t* PressHandler;
//      LineHandler_t* LongPressHandler;      
    }Line_t;
    
    typedef struct 
    {
      LineHandler_t* PressHandler;
      LineHandler_t* LongPressHandler;      
    }LineHandlers_t;
        
    Menu() : m_ActiveLine(0),m_PreviousLine(0)
    {
      for(uint8_t i = 0; i < LINES_PER_MENU; i++)
      {
        memset(&m_Lines[i].Text,' ',CHARS_PER_LINE);
        m_Lines[i].Text[CHARS_PER_LINE] = 0;        
        memset(m_Lines[i].Handlers,0,HANDLERS_PER_LINE);

      }      
    }
    
    uint8_t GetTotalCharsInLines(){return CHARS_PER_LINE;}
    
    uint8_t GetTotalLinesInMenu(){return LINES_PER_MENU;}
    
    void DisplayLine(uint8_t line, const char* Str, uint16_t textColor, uint16_t backgroundColor)
    {
        TFT_1_8.WriteString(0, font->height*line, Str, font, textColor, backgroundColor); 
    }
    
    void UpdateLineText(uint8_t line, uint8_t Pos, const char* Str, uint8_t len)
    {
      if(len >= CHARS_PER_LINE) 
      {
        len = CHARS_PER_LINE;      
      }
      
      if( (line < LINES_PER_MENU) && (Pos < CHARS_PER_LINE ) && (Str) )
      {
        memcpy(&m_Lines[line].Text[Pos],Str,len); 
      }
    }
    
    void MoveUp()
    {
      m_PreviousLine = m_ActiveLine;
      if(m_ActiveLine == 0)
      {
        m_ActiveLine = LINES_PER_MENU-1;
      }
      else
      {
        m_ActiveLine--;
      }    
    }
    
    void MoveDown()
    {
      m_PreviousLine = m_ActiveLine;  
      m_ActiveLine++;
      if(m_ActiveLine >= LINES_PER_MENU)
      {
        m_ActiveLine = 0;
      }    
    }
    
    void RegisterLineEventHandler(uint8_t line, LineHandler_t* UpHandler, LineHandler_t* DownHandler, LineHandler_t* PressHandler = nullptr, LineHandler_t* LongPressHandler = nullptr)
    {
      m_Lines[line].Handlers[HMI::UP] = UpHandler;
      m_Lines[line].Handlers[HMI::DOWN] = DownHandler;
      m_Lines[line].Handlers[HMI::PRESS] = PressHandler;
      m_Lines[line].Handlers[HMI::LONGPRESS] = LongPressHandler;
    }
      
    void LineTextInit(uint8_t line, const char* text)
    {
      uint8_t i;
      
      if( (line < LINES_PER_MENU) && (text) )
      {
        for(i = 0; i < CHARS_PER_LINE && text[i] ; i++)
        {
          m_Lines[line].Text[i] = text[i];
        }
        memset(&m_Lines[i].Text[i],' ',CHARS_PER_LINE-i);
      }
    }
    
    void FlushDisplay()
    {
      for(uint8_t i = 0; i < LINES_PER_MENU; i++)
      {
        DisplayLine(i,m_Lines[i].Text,TextColor,BackgroundColor);
      }
      Display();
    }

    void Display()
    {      
      if(m_PreviousLine < LINES_PER_MENU)
      {
        DisplayLine(m_PreviousLine,m_Lines[m_PreviousLine].Text,TextColor,BackgroundColor);
      }
      
      if(m_ActiveLine < LINES_PER_MENU)
      {
        DisplayLine(m_ActiveLine,m_Lines[m_ActiveLine].Text,TextColor,HighlighterColor);
      }     
    }
    
    void Run(HMI::InputEvents_t InputDeviceEvents)
    {
      if(InputDeviceEvents != HMI::NONE)
      {        
//        if(InputDeviceEvents == HMI::DOWN)
//        {        
//          m_PreviousLine = m_ActiveLine;  
//          m_ActiveLine++;
//          if(m_ActiveLine >= LINES_PER_MENU)
//          {
//            m_ActiveLine = 0;
//          }
//        }
//        else if(InputDeviceEvents == HMI::UP)
//        {     
//          m_PreviousLine = m_ActiveLine;
//          if(m_ActiveLine == 0)
//          {
//            m_ActiveLine = LINES_PER_MENU-1;
//          }
//          else
//          {
//             m_ActiveLine--;
//          }   
//        }
//        else if(InputDeviceEvents == HMI::PRESS)
//        {         
//          if(m_Lines[m_ActiveLine].PressHandler)
//          {
//            m_Lines[m_ActiveLine].PressHandler->CallbackFunction();
//          }
//        }
//        else if(InputDeviceEvents == HMI::LONGPRESS)
//        {         
//          if(m_Lines[m_ActiveLine].LongPressHandler)
//          {
//            m_Lines[m_ActiveLine].LongPressHandler->CallbackFunction();
//          }
//        }
        
        if(m_Lines[m_ActiveLine].Handlers[InputDeviceEvents])
        {
          m_Lines[m_ActiveLine].Handlers[InputDeviceEvents]->CallbackFunction();
        }
          
        Display();
      }            
    }
    
    Line_t m_Lines[LINES_PER_MENU];
    uint8_t m_ActiveLine;
    uint8_t m_PreviousLine;
  }; 
}


#endif //Menu_h