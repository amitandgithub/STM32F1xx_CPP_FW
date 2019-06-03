/*
 * Nokia5110LCD.hpp
 *
 *  Created on: 28-Oct-2017
 *      Author: Amit Chaudhary
 */

#ifndef APP_INC_NOKIA5110LCD_HPP_
#define APP_INC_NOKIA5110LCD_HPP_


#include "SPI_Poll.h"
#include "SPI_IT.h"
#include "SPI_DMA.h"
#include "GpioInput.hpp"
#include "GpioOutput.hpp"
#include "DisplayBuffer.h"

namespace Peripherals 
{

//#define DUAL_BUFFER

class Nokia5110LCD 
{
public:
    //typedef Peripherals::SPI_Poll SPIDrv_t;
    //typedef Peripherals::SPI_IT SPIDrv_t;
    typedef Peripherals::SPI_DMA SPIDrv_t;
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

	Nokia5110LCD(SPIDrv_t*   pSpiDriverLCD ,
                 GpioOutput* pCS,
				 GpioOutput* pDataCommandSelectGpio,
				 GpioOutput* pResetPinGpio,
                 GpioOutput* pBackLightGpio);
    
    Nokia5110LCD(SPIDrv_t::SPIx_t Spix,
				 GpioOutput::PORT_t ResetPort, GpioOutput::PIN_t ResetPin,
				 GpioOutput::PORT_t DCPort, GpioOutput::PIN_t DCPin,
				 GpioOutput::PORT_t BKLPort, GpioOutput::PIN_t BKLPin);
    
	~Nokia5110LCD(){};
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
    void BackLightON(){m_pBackLightGpio->On();}
    void BackLightOFF(){m_pBackLightGpio->Off();}
    void BackLightToggle(){m_pBackLightGpio->ToggleOutput();}
    bool GetBackLightState(){ return (m_pBackLightGpio->ReadOutputValue() ? true : false); }
    Status_t Refresh();
    void Run(){Refresh();}
    
    
private:
    void SetMode(uint8_t LCDMode);
	void Write(DC_t DC, unsigned char data);
	void GoToXY(int x, int y);
	void ClearDisplay();
	void DrawString(const char *characters);	
	void LCDCharacter(const char character);
    
private:
    SPIDrv_t*   m_pSpiDriverLCD;
	GpioOutput* m_pDataCommandSelectGpio;
	GpioOutput* m_pResetPinGpio;
	GpioOutput* m_pBackLightGpio;
    GpioOutput* m_pCS;
    uint8_t     m_Brightness;
    Peripherals::SPI_Base::Transaction_t m_CurrentTransaction;
    char* m_pCurrentBuffer;
#ifdef WITH_DISPLAY_BUFFER
    DisplayBuffer m_DisplayBuffer;
#endif
    
};



}
#endif /* APP_INC_NOKIA5110LCD_HPP_ */
