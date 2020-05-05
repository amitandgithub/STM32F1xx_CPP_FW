/*
* Window.h
*
*  Created on: 28-Apr-2020
*      Author: Amit Chaudhary
*  
*/

#ifndef Window_h
#define Window_h

#include "CPP_HAL.h" 
#include"stdint.h"
#include<cstring>
#include"Callback.h"
#include "ST7735.h"

extern BSP::ST7735 TFT_1_8; 

namespace HMI
{
  typedef enum
  {     
    UP,
    DOWN,
    PRESS,
    LONGPRESS,   
    NONE,
  }InputEvents_t;  
  
  typedef struct
  {    
    uint16_t    x1;
    uint16_t    x2;
    uint16_t    y1;    
    uint16_t    y2;
    FontDef*    font;
    uint16_t    textColor;    
    uint16_t    backgroundColor;
    uint16_t    highlightColor;
  }WindowContext_t;
  
  typedef uint16_t Color_t;
  class Window
  {
    
  public:
    
     
    Window(const WindowContext_t* pWindowContext): m_pWindowContext(pWindowContext)
    {
      
    }
    
    virtual bool Display(Color_t BackgroundColor) = 0;
    
    virtual uint8_t EventHandler(InputEvents_t InputEvents) = 0;
    
    void Select(uint16_t bgcolor)
    {
      //TFT_1_8.WriteString(x,y,str,font,color,m_pWindowContext->backgroundColor);
      //m_bgcolor = bgcolor;
    }
    
    void Deselect(uint16_t bgcolor)
    {
      //m_bgcolor = bgcolor;
    }
        
    void WriteChar(uint16_t x, uint16_t y, char ch, FontDef* font, uint16_t color, uint16_t bgcolor)
    {
      TFT_1_8.WriteChar(x,y,ch,font,color,bgcolor);
    }
    
    void WriteString(uint16_t x, uint16_t y, const char* str, FontDef* font, uint16_t color, uint16_t bgcolor)
    {
      TFT_1_8.WriteString(x,y,str,font,color,bgcolor);
    }
    
     void DrawRctangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
     {
        TFT_1_8.DrawRctangle(x,y,w,h,color);
     }
  protected:
    //uint16_t m_bgcolor;
    const WindowContext_t* m_pWindowContext;
    
  }; 
  
}



#endif // Window_h