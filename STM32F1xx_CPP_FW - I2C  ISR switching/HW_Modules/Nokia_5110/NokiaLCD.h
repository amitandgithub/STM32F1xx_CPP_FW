/*
* NokiaLCD.hpp
*
*  Created on: 28-Oct-2017
*  - 30 Dec 2019
*      Author: Amit Chaudhary
*/

#ifndef APP_INC_NokiaLCD_HPP_
#define APP_INC_NokiaLCD_HPP_

#include "Spi.h"
#include "GpioOutput.h"
//#include "DisplayBuffer.h"

namespace BSP 
{
  
  //#define DUAL_BUFFER
  
  class NokiaLCD 
  {
  public:
    typedef uint32_t Status_t;
    typedef HAL::Spi SPIDrv_t;
    
    typedef enum
    {
      COMMAND = 0,
      DATA
    }DC_t;
    
    static const uint8_t NORMAL  = 0x0C;
    static const uint8_t INVERSE = 0x0D;
    
    static const uint8_t LCD_X  = 84;
    static const uint8_t LCD_Y  = 48;
    
    static const uint8_t SIZE_OF_1_CHAR = 6;
    static const uint8_t NO_OF_CHAR_IN_LINE = 14;
    static const uint16_t DISPLAY_BUF_SIZE = (LCD_X*LCD_Y)/8;
    static const uint8_t TOTAL_ROWS = 6;
    
    NokiaLCD(SPIDrv_t* pSpi,Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin);

    bool HwInit();
    bool DisplayInit();
    
    void DisplayChar(unsigned char Row, unsigned char Col, const char aChar);
    void DisplayStr(unsigned char Row, unsigned char Col, const char* Str);
    void DisplayBuf(char* pBuffer);
    void DisplayBitmap(char* pBuffer);
    
    
#ifdef WITH_DISPLAY_BUFFER
    void DrawCharBuf(unsigned char Row, unsigned char Col, const char aChar){m_DisplayBuffer.DrawChar(Row,Col,aChar);}
    void DrawStrBuf(unsigned char Row, unsigned char Col, const char* Str){m_DisplayBuffer.DrawStr(Row,Col,Str);}
    void DrawBuffer(char* pBuffer){m_DisplayBuffer.DrawBuffer(pBuffer);}
    void DrawBitmap(char* pBuffer){m_DisplayBuffer.DrawBitmap(pBuffer);}
    char* GetDisplayBuf(){ return m_DisplayBuffer.GetBuffer();}
#endif
    
    void SetBrigntness(uint8_t Brightness = 0x13){m_Brightness = Brightness;DisplayInit();}
    void PowerDown();
    void BackLightON(){m_Backlight.High();}
    void BackLightOFF(){m_Backlight.Low();}
    void BackLightToggle(){m_Backlight.Toggle();}
    bool GetBackLightState(){ return (m_Backlight.Read() ? true : false); }
    Status_t Refresh();
    void Run(){Refresh();}
    
    
  private:
    void SetMode(uint8_t LCDMode);
    void Write(DC_t DC, unsigned char data);
    void GoToXY(uint8_t x, uint8_t y);
    void ClearDisplay();
    void DrawString(const char *characters);	
    void LCDCharacter(const char character);
    
  private:
    SPIDrv_t*           m_pSpi;
    HAL::GpioOutput     m_DC;
    HAL::GpioOutput     m_Reset;
    HAL::GpioOutput     m_Backlight;
    HAL::GpioOutput     m_CS;
    uint8_t             m_Brightness;
   // Peripherals::SPI_Base::Transaction_t m_CurrentTransaction;
#ifdef WITH_DISPLAY_BUFFER
    DisplayBuffer m_DisplayBuffer;
#endif
    
  };
  
  
  
}
#endif /* APP_INC_NokiaLCD_HPP_ */
