/*
* ST7735.h
*
*  Created on: 28-Oct-2017
*      Author: Amit Chaudhary
*/

#ifndef ST7735_h
#define ST7735_h

#include "CPP_HAL.h" 
#include "Spi.h"
#include "GpioOutput.h"
#include "ST7735_defs.h"


namespace BSP 
{

//#define ST7735_TEMPLATE_PARAMS HAL::Spi*   pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin
//#define ST7735_TEMPLATE_T pSpiDriverLCD,CSPort,CSPin,DCPort,DCPin,RstPort,RstPin,BklPort,BklPin
//  
    class ST7735 
    {
    public:
      ST7735(HAL::Spi* pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin);
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
      HAL::Spi*   m_Spi;
      HAL::GpioOutput m_CSPin;
      HAL::GpioOutput m_DCPin;
      HAL::GpioOutput m_RstPin;
      HAL::GpioOutput m_BklPin;     
    }; 
}
  
#endif /* ST7735_h */
