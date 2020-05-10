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
  static const uint32_t Power_of_10[11] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
  
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
  
  class SettingCallback
  {
  public:
    virtual void CallbackFunction(uint32_t UpdatedSettingValue) = 0;
  };
  
  typedef struct
  {
    uint32_t* pSettingVariable;
    char*     SettingText;
    SettingCallback* pCallback;
    uint8_t   Settinglen;
    uint8_t   SettingVariableTotalDigits;
    uint8_t   SettingVariablePosition;
  }SettingContext_t;
  
  typedef struct
  {
    uint32_t  SettingValue;
    char*     SettingText;
  }NamedSettingTable_t;
  
  typedef struct
  {
    uint32_t* pSettingVariable;
    char*     SettingText;
    SettingCallback* pCallback;
    uint8_t   Settinglen;
    uint8_t   SettingVariablePosition;
    uint8_t   NamedSettingTableLen;
    const NamedSettingTable_t* pNamedSettingTable;
  }NamedSettingContext_t;
  
  typedef struct
  {
    char*     SettingText;
//    uint8_t   Settinglen;
    HAL::Callback* pUPCb;
    HAL::Callback* pDownCb;
    HAL::Callback* pPressCb;
    HAL::Callback* pLPressCb;    
  }TextWindowContext_t;
    
  typedef uint16_t Color_t;
  class Window
  {
    
  public:   
     
    Window(const WindowContext_t* pWindowContext): m_pWindowContext(pWindowContext)
    {
      
    }
    
    virtual bool Display(Color_t BackgroundColor) = 0;
    
    virtual uint8_t EventHandler(InputEvents_t InputEvents) = 0;
        
    void ClearScreen(uint16_t color)
    {
      TFT_1_8.FillScreen(color);
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
    const WindowContext_t* m_pWindowContext;    
  }; 
  
}



#endif // Window_h