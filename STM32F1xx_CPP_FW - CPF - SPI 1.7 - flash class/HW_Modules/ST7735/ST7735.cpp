/******************
** FILE: ST7735.cpp
**
** DESCRIPTION:
**  ST7735 implementation
**
** CREATED: 28/12/2019, by Amit Chaudhary
******************/

#include "ST7735.h"

namespace BSP
{

  ST7735::ST7735(HAL::Spi* pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin): m_Spi(pSpiDriverLCD),m_CSPin(CSPort,CSPin),m_DCPin(DCPort,DCPin),m_RstPin(RstPort,RstPin),m_BklPin(BklPort,BklPin)
  {
    
  }
    void ST7735::HwInit()
    {
      m_Spi->HwInit();
      m_CSPin.HwInit();
      m_DCPin.HwInit();
      m_RstPin.HwInit();
      m_BklPin.HwInit();
      DisplayInit();      
    }
  
  
    void ST7735::DisplayInit()
    {      
      Select();
      Reset();
      ExecuteCommandList(init_cmds1);
      ExecuteCommandList(init_cmds2);
      ExecuteCommandList(init_cmds3);
      Deselect();
    }

  
   void ST7735::WriteCommand(uint8_t cmd) 
  {
    uint8_t command = cmd;
    m_DCPin.Low();
    m_Spi->TxPoll(&command,1);
  }
  
   void ST7735::WriteData(uint8_t* buff, size_t buff_size) 
  {
    m_DCPin.High();
    m_Spi->TxPoll(buff, buff_size);
  }
  
   void ST7735::ExecuteCommandList(const uint8_t *addr) 
   {
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
  
   void ST7735::SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    WriteCommand(CASET);
    uint8_t data[] = { 0x00, static_cast<uint8_t>(x0 + XSTART), 0x00, static_cast<uint8_t>(x1 + XSTART) };
    WriteData(data, sizeof(data));
    
    // row address set
    WriteCommand(RASET);
    data[1] = y0 + YSTART;
    data[3] = y1 + YSTART;
    WriteData(data, sizeof(data));
    
    // write to RAM
    WriteCommand(RAMWR);
  }

  
  void ST7735::DrawPixel(uint16_t x, uint16_t y, uint16_t color) 
  {
    if((x >= WIDTH) || (y >= HEIGHT))
      return;
    
    Select();
    
    SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color & 0xFF) };
    WriteData(data, sizeof(data));
    
    Deselect();
  }
  
   void ST7735::WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) 
   {
    uint32_t i, b, j;
    
    SetAddressWindow(x, y, x+font.width-1, y+font.height-1);
    
    for(i = 0; i < font.height; i++) {
      b = font.data[(ch - 32) * font.height + i];
      for(j = 0; j < font.width; j++) {
        if((b << j) & 0x8000)  {
          uint8_t data[] = { static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color & 0xFF) };
          WriteData(data, sizeof(data));
        } else {
          uint8_t data[] = { static_cast<uint8_t>(bgcolor >> 8), static_cast<uint8_t>(bgcolor & 0xFF) };
          WriteData(data, sizeof(data));
        }
      }
    }
  }
  
  
  void ST7735::WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
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
  
  void ST7735::FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, uint16_t color) 
  {
    // clipping
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH) w = WIDTH - x;
    if((y + h1 - 1) >= HEIGHT) h1 = HEIGHT - y;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h1-1);
    
    uint8_t data[] = { static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color & 0xFF) };

    m_DCPin.High();
    for(y = h1; y > 0; y--) {
      for(x = w; x > 0; x--) {

        m_Spi->TxPoll(data, sizeof(data));
      }
    }    
    Deselect();
  }
  
  
  void ST7735::FillScreen(uint16_t color) 
  {
    FillRectangle(0, 0, WIDTH, HEIGHT, color);
  }
  
  
  void ST7735::DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h1, const uint16_t* data) 
  {
    if((x >= WIDTH) || (y >= HEIGHT)) return;
    if((x + w - 1) >= WIDTH) return;
    if((y + h1 - 1) >= HEIGHT) return;
    
    Select();
    SetAddressWindow(x, y, x+w-1, y+h1-1);
    WriteData((uint8_t*)data, sizeof(uint16_t)*w*h1);    
    Deselect();
  }
  
  void ST7735::InvertColors(bool invert) 
  {
    Select();
    WriteCommand(invert ? INVON : INVOFF);
    Deselect();
  }
   
  
}