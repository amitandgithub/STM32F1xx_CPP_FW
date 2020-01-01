/*
* ST7735.h
*
*  Created on: 28-Oct-2017
*      Author: Amit Chaudhary
*/

#ifndef h
#define h

#include "CPP_HAL.h" 
#include "Spi.h"
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "ST7735_defs.h"


namespace BSP 
{

#define ST7735_TEMPLATE_PARAMS HAL::Spi*   pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin
#define ST7735_TEMPLATE_T pSpiDriverLCD,CSPort,CSPin,DCPort,DCPin,RstPort,RstPin,BklPort,BklPin
  template<ST7735_TEMPLATE_PARAMS>
    class ST7735 
    {
    public:
      void HwInit();
      void DisplayInit(void);
      void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
      void WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
      void FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, uint16_t color);
      void FillScreen(uint16_t color);
      void DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, const uint16_t* data);
      void InvertColors(bool invert);
      void Select(){m_CSPin.Low();}
      void Deselect(){m_CSPin.High();}
      void Reset(){m_RstPin.Low(); LL_mDelay(5); m_RstPin.High();}
      void WriteCommand(uint8_t cmd);
      void WriteData(uint8_t* buff, size_t buff_size) ;
      void ExecuteCommandList(const uint8_t *addr);
      void SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
      void WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
    private:
      HAL::DigitalOut<CSPort,CSPin> m_CSPin;
      HAL::DigitalOut<DCPort,DCPin> m_DCPin;
      HAL::DigitalOut<RstPort,RstPin> m_RstPin;
      HAL::DigitalOut<BklPort,BklPin> m_BklPin;     
    };
  
  template<ST7735_TEMPLATE_PARAMS>
    void ST7735<ST7735_TEMPLATE_T>::HwInit()
    {
      pSpiDriverLCD->HwInit();
      m_CSPin.HwInit();
      m_DCPin.HwInit();
      m_RstPin.HwInit();
      m_BklPin.HwInit();
      DisplayInit();      
    }
  
  template<ST7735_TEMPLATE_PARAMS>
    void ST7735<ST7735_TEMPLATE_T>::DisplayInit()
    {      
      Select();
      Reset();
      ExecuteCommandList(init_cmds1);
      ExecuteCommandList(init_cmds2);
      ExecuteCommandList(init_cmds3);
      Deselect();
    }

  template<ST7735_TEMPLATE_PARAMS>
   void ST7735<ST7735_TEMPLATE_T>::WriteCommand(uint8_t cmd) 
  {
    uint8_t command = cmd;
    m_DCPin.Low();
    pSpiDriverLCD->TxPoll(&command,1);
  }
  template<ST7735_TEMPLATE_PARAMS>
   void ST7735<ST7735_TEMPLATE_T>::WriteData(uint8_t* buff, size_t buff_size) 
  {
    m_DCPin.High();
    pSpiDriverLCD->TxPoll(buff, buff_size);
  }
  template<ST7735_TEMPLATE_PARAMS>
   void ST7735<ST7735_TEMPLATE_T>::ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;
    
    numCommands = *addr++;
    while(numCommands--) {
      uint8_t cmd = *addr++;
      WriteCommand(cmd);
      
      numArgs = *addr++;
      // If high bit set, delay follows args
      ms = numArgs & DELAY;
      numArgs &= ~DELAY;
      if(numArgs) {
        WriteData((uint8_t*)addr, numArgs);
        addr += numArgs;
      }
      
      if(ms) {
        ms = *addr++;
        if(ms == 255) ms = 500;
        LL_mDelay(ms);
      }
    }
  }
  template<ST7735_TEMPLATE_PARAMS>
   void ST7735<ST7735_TEMPLATE_T>::SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    WriteCommand(CASET);
    uint8_t data[] = { 0x00, x0 + XSTART, 0x00, x1 + XSTART };
    WriteData(data, sizeof(data));
    
    // row address set
    WriteCommand(RASET);
    data[1] = y0 + YSTART;
    data[3] = y1 + YSTART;
    WriteData(data, sizeof(data));
    
    // write to RAM
    WriteCommand(RAMWR);
  }

  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::DrawPixel(uint16_t x, uint16_t y, uint16_t color) 
  {
    if((x >= WIDTH) || (y >= HEIGHT))
      return;
    
    Select();
    
    SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    WriteData(data, sizeof(data));
    
    Deselect();
  }
  template<ST7735_TEMPLATE_PARAMS>
   void ST7735<ST7735_TEMPLATE_T>::WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) 
   {
    uint32_t i, b, j;
    
    SetAddressWindow(x, y, x+font.width-1, y+font.height-1);
    
    for(i = 0; i < font.height; i++) {
      b = font.data[(ch - 32) * font.height + i];
      for(j = 0; j < font.width; j++) {
        if((b << j) & 0x8000)  {
          uint8_t data[] = { color >> 8, color & 0xFF };
          WriteData(data, sizeof(data));
        } else {
          uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
          WriteData(data, sizeof(data));
        }
      }
    }
  }
  
  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
  {
    Select();
    
    while(*str) {
      if(x + font.width >= WIDTH) {
        x = 0;
        y += font.height;
        if(y + font.height >= HEIGHT) {
          break;
        }
        
        if(*str == ' ') {
          // skip spaces in the beginning of the new line
          str++;
          continue;
        }
      }
      
      WriteChar(x, y, *str, font, color, bgcolor);
      x += font.width;
      str++;
    }
    
    Deselect();
  }
  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, uint16_t color) 
  {
    // clipping
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH) w = WIDTH - x;
    if((y + h1 - 1) >= HEIGHT) h1 = HEIGHT - y;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h-1);
    
    uint8_t data[] = { color >> 8, color & 0xFF };

    m_DCPin.High();
    for(y = h1; y > 0; y--) {
      for(x = w; x > 0; x--) {

        pSpiDriverLCD->TxPoll(data, sizeof(data));
      }
    }    
    Deselect();
  }
  
  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::FillScreen(uint16_t color) 
  {
    FillRectangle(0, 0, WIDTH, HEIGHT, color);
  }
  
  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, const uint16_t* data) 
  {
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH) return;
    if((y + h1 - 1) >= HEIGHT) return;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h1-1);
    WriteData((uint8_t*)data, sizeof(uint16_t)*w*h1);    
    Deselect();
  }
  template<ST7735_TEMPLATE_PARAMS>
  void ST7735<ST7735_TEMPLATE_T>::InvertColors(bool invert) 
  {
    Select();
    WriteCommand(invert ? INVON : INVOFF);
    Deselect();
  }
   
}
  
#endif /* h */
