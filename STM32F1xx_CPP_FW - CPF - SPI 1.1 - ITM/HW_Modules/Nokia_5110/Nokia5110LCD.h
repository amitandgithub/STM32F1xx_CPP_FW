/*
* Nokia5110LCD.h
*
*  Created on: 28-Oct-2017
*      Author: Amit Chaudhary
*/

#ifndef Nokia5110LCD_h
#define Nokia5110LCD_h

#include "Spi.h"
#include "DigitalIn.h"
#include "DigitalOut.h"
//#include "DisplayBuffer.h"
#define  SPI_TX pSpiDriverLCD->TxIntr
namespace BSP 
{
  
  extern const int8_t ASCII[96][6];
  //#define DUAL_BUFFER
#define Nokia5110LCD_TEMPLATE_PARAMS HAL::Spi*   pSpiDriverLCD, Port_t CSPort, PIN_t CSPin,Port_t DCPort, PIN_t DCPin, Port_t RstPort, PIN_t RstPin, Port_t BklPort, PIN_t BklPin
#define Nokia5110LCD_TEMPLATE_T pSpiDriverLCD,CSPort,CSPin,DCPort,DCPin,RstPort,RstPin,BklPort,BklPin
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    class Nokia5110LCD 
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
      
      Nokia5110LCD(){};    
      ~Nokia5110LCD(){};
      
      void HwInit();
      void DisplayInit();
      
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
      void BackLightON(){m_BklPin.High();}
      void BackLightOFF(){m_BklPin.Low();}
      void BackLightToggle(){m_BklPin.ToggleOutput();}
      bool GetBackLightState(){ return (m_BklPin.ReadOutputValue() ? true : false); }
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
      HAL::DigitalOut<CSPort,CSPin> m_CSPin;
      HAL::DigitalOut<DCPort,DCPin> m_DCPin;
      HAL::DigitalOut<RstPort,RstPin> m_RstPin;
      HAL::DigitalOut<BklPort,BklPin> m_BklPin;
      uint8_t     m_Brightness;
      //Peripherals::SPI_Base::Transaction_t m_CurrentTransaction;
      char* m_pCurrentBuffer;
#ifdef WITH_DISPLAY_BUFFER
      DisplayBuffer m_DisplayBuffer;
#endif
      
    };
  
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::HwInit()
    {
      m_Brightness = 0x14;
      pSpiDriverLCD->HwInit();
      m_CSPin.HwInit();
      m_DCPin.HwInit();
      m_RstPin.HwInit();
      m_BklPin.HwInit();
      DisplayInit();
#ifdef WITH_DISPLAY_BUFFER
      m_DisplayBuffer.Clear();
#endif
      
    }
  
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DisplayInit()
    {
      //Reset the LCD to a known state
      m_CSPin.High();   
      m_RstPin.Low();
      LL_mDelay(10);
      m_RstPin.High();
      
      Write(COMMAND, 0x21); //Tell LCD that extended commands follow
      Write(COMMAND, 0xBF); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark, Amit: 0xBF works fine with 3.3v
      Write(COMMAND, 0x04); //Set Temp coefficent
      Write(COMMAND, m_Brightness); //LCD bias mode 1:48: Try 0x13 or 0x14 ,Amit: 0x13 works fine with 3.3v, for blue display 0x15 works fine
      
      Write(COMMAND, 0x20); //We must send 0x20 before modifying the display control mode
      Write(COMMAND, 0x0C); //Set display control, normal mode. 0x0D for inverse
      
      ClearDisplay();
    }
  
  
  //There are two memory banks in the LCD, data/RAM and commands. This
  //function sets the DC pin high or low depending, and then sends
  //the data byte
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::Write(DC_t DC, unsigned char data)
    {    
      if(DC == DATA)
      {
        m_DCPin.High();
      }
      else
      {
        m_DCPin.Low();       
      }   
      
      m_CSPin.Low();
      SPI_TX(&data,1);
      while(pSpiDriverLCD->GetState() != HAL::Spi::SPI_READY);
      m_CSPin.High();    
    }
  
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::SetMode(uint8_t LCDMode)
    {
      if(LCDMode == INVERSE)
      {
        Write(COMMAND,INVERSE);
      }
      else if(LCDMode == NORMAL)
      {
        Write(COMMAND,NORMAL);
      }
      else
      {
        
      }
    }
  
  //Clears the LCD by writing zeros to the entire screen
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::ClearDisplay()
    {
      int32_t index;
      //m_CSPin.Low();
      for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
        Write(DATA, 0x00);
      
      //m_CSPin.High();
      
      GoToXY(0, 0); //After we clear the display, return to the home position
      
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::GoToXY(int x, int y)
    {
      Write(COMMAND, 0x80 | x);  // Column.
      Write(COMMAND, 0x40 | y);  // Row.  ?
    }
  
  //This takes a large array of bits and sends them to the LCD
  //void Nokia5110LCD::DrawBitmap(const char my_array[])
  //{
  //	int index;
  //	for (index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
  //		Write(DATA, my_array[index]);
  //}
  //This function takes in a character, looks it up in the font table/array
  //And writes it to the screen
  //Each character is 8 bits tall and 5 bits wide. We pad one blank column of
  //pixels on each side of the character for readability.
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::LCDCharacter(const char character)
    {
      int32_t index;
      //Write(DATA, 0x00); //Blank vertical line padding
      //m_CSPin.Low();
      for (index = 0 ; index < 5 ; index++)
        Write(DATA, ASCII[character - 0x20][index]);
      //0x20 is the ASCII character for Space (' '). The font table starts with this character
      
      Write(DATA, 0x00); //Blank vertical line padding
      //m_CSPin.High();
      
    }
  
  //Given a string of characters, one by one is passed to the LCD
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DrawString(const char *characters)
    {
      while (*characters)
        LCDCharacter(*characters++);
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DisplayChar(unsigned char Row, unsigned char Col, const char aChar)
    {
      GoToXY(Col*SIZE_OF_1_CHAR,Row);
      LCDCharacter(aChar);
      
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DisplayStr(unsigned char Row, unsigned char Col, const char* Str)
    {
      uint8_t i=0;
      GoToXY(Col*SIZE_OF_1_CHAR,Row);
      DrawString("              ");
      
      GoToXY(Col*SIZE_OF_1_CHAR,Row);
      
      while (*Str && i<NO_OF_CHAR_IN_LINE)
      {
        LCDCharacter(*Str++);
        i++;
      }
      
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DisplayBuf(char* pBuffer)
    {
      
      GoToXY(0*SIZE_OF_1_CHAR,0);
      
      for(int i = 0; i < (LCD_X * LCD_Y / 8) ; i++)
        Write(DATA, pBuffer[i]);
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::DisplayBitmap(char* pBuffer)
    {   
      m_pCurrentBuffer = pBuffer;
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    void Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::PowerDown()
    {
      uint8_t cmd = 0x08 | 0x04;
      
      ClearDisplay();
      
      //m_CSPin.Low();
      pSpiDriverLCD->TxPoll(&cmd,1);
      //m_CSPin.High();
      
    }
  template<Nokia5110LCD_TEMPLATE_PARAMS>
    Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::
  Status_t Nokia5110LCD<Nokia5110LCD_TEMPLATE_T>::Refresh( )
  {
    //    if ( (m_CurrentTransaction.TxnStatus.Event & SPI_Base::SPI_TX_COMPLETE) == SPI_Base::SPI_TX_COMPLETE)
    //    {    
    //        GoToXY(0*SIZE_OF_1_CHAR,0);
    //        
    //        m_DCPin.High();
    //        
    //        m_CurrentTransaction.TxBuf = (uint8_t*)m_pCurrentBuffer; 
    //
    //        m_CurrentTransaction.TxLen = DISPLAY_BUF_SIZE;
    //        m_CurrentTransaction.pCS = m_pCS;
    //        m_CurrentTransaction.TxnStatus.TimeValue = HAL_GetTick() & 0x0000FFFF;
    //        m_CurrentTransaction.TxnStatus.Event &=  ~(SPI_Base::SPI_TX_COMPLETE);  
    //        //while((m_CurrentTransaction.TxnStatus.Event & SPI_Base::SPI_TX_COMPLETE) == SPI_Base::SPI_TX_COMPLETE);
    //        
    //        return pSpiDriverLCD->Post(&m_CurrentTransaction);
    //        
    //    }
    return 1;
    
    
  }
  
  //This table contains the hex values that represent pixels
  //for a font that is 6 pixels wide and 8 pixels high
  static const int8_t ASCII[96][6] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} // 20
    ,{0x00, 0x00, 0x5f, 0x00, 0x00, 0x00} // 21 !
    ,{0x00, 0x07, 0x00, 0x07, 0x00, 0x00} // 22 "
    ,{0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00} // 23 #
    ,{0x24, 0x2a, 0x7f, 0x2a, 0x12, 0x00} // 24 $
    ,{0x23, 0x13, 0x08, 0x64, 0x62, 0x00} // 25 %
    ,{0x36, 0x49, 0x55, 0x22, 0x50, 0x00} // 26 &
    ,{0x00, 0x05, 0x03, 0x00, 0x00, 0x00} // 27 '
    ,{0x00, 0x1c, 0x22, 0x41, 0x00, 0x00} // 28 (
    ,{0x00, 0x41, 0x22, 0x1c, 0x00, 0x00} // 29 )
    ,{0x14, 0x08, 0x3e, 0x08, 0x14, 0x00} // 2a *
    ,{0x08, 0x08, 0x3e, 0x08, 0x08, 0x00} // 2b +
    ,{0x00, 0x50, 0x30, 0x00, 0x00, 0x00} // 2c ,
    ,{0x08, 0x08, 0x08, 0x08, 0x08, 0x00} // 2d -
    ,{0x00, 0x60, 0x60, 0x00, 0x00, 0x00} // 2e .
    ,{0x20, 0x10, 0x08, 0x04, 0x02, 0x00} // 2f /
    ,{0x3e, 0x51, 0x49, 0x45, 0x3e, 0x00} // 30 0
    ,{0x00, 0x42, 0x7f, 0x40, 0x00, 0x00} // 31 1
    ,{0x42, 0x61, 0x51, 0x49, 0x46, 0x00} // 32 2
    ,{0x21, 0x41, 0x45, 0x4b, 0x31, 0x00} // 33 3
    ,{0x18, 0x14, 0x12, 0x7f, 0x10, 0x00} // 34 4
    ,{0x27, 0x45, 0x45, 0x45, 0x39, 0x00} // 35 5
    ,{0x3c, 0x4a, 0x49, 0x49, 0x30, 0x00} // 36 6
    ,{0x01, 0x71, 0x09, 0x05, 0x03, 0x00} // 37 7
    ,{0x36, 0x49, 0x49, 0x49, 0x36, 0x00} // 38 8
    ,{0x06, 0x49, 0x49, 0x29, 0x1e, 0x00} // 39 9
    ,{0x00, 0x36, 0x36, 0x00, 0x00, 0x00} // 3a :
    ,{0x00, 0x56, 0x36, 0x00, 0x00, 0x00} // 3b ;
    ,{0x08, 0x14, 0x22, 0x41, 0x00, 0x00} // 3c <
    ,{0x14, 0x14, 0x14, 0x14, 0x14, 0x00} // 3d =
    ,{0x00, 0x41, 0x22, 0x14, 0x08, 0x00} // 3e >
    ,{0x02, 0x01, 0x51, 0x09, 0x06, 0x00} // 3f ?
    ,{0x32, 0x49, 0x79, 0x41, 0x3e, 0x00} // 40 @
    ,{0x7e, 0x11, 0x11, 0x11, 0x7e, 0x00} // 41 A
    ,{0x7f, 0x49, 0x49, 0x49, 0x36, 0x00} // 42 B
    ,{0x3e, 0x41, 0x41, 0x41, 0x22, 0x00} // 43 C
    ,{0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00} // 44 D
    ,{0x7f, 0x49, 0x49, 0x49, 0x41, 0x00} // 45 E
    ,{0x7f, 0x09, 0x09, 0x09, 0x01, 0x00} // 46 F
    ,{0x3e, 0x41, 0x49, 0x49, 0x7a, 0x00} // 47 G
    ,{0x7f, 0x08, 0x08, 0x08, 0x7f, 0x00} // 48 H
    ,{0x00, 0x41, 0x7f, 0x41, 0x00, 0x00} // 49 I
    ,{0x20, 0x40, 0x41, 0x3f, 0x01, 0x00} // 4a J
    ,{0x7f, 0x08, 0x14, 0x22, 0x41, 0x00} // 4b K
    ,{0x7f, 0x40, 0x40, 0x40, 0x40, 0x00} // 4c L
    ,{0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00} // 4d M
    ,{0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00} // 4e N
    ,{0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00} // 4f O
    ,{0x7f, 0x09, 0x09, 0x09, 0x06, 0x00} // 50 P
    ,{0x3e, 0x41, 0x51, 0x21, 0x5e, 0x00} // 51 Q
    ,{0x7f, 0x09, 0x19, 0x29, 0x46, 0x00} // 52 R
    ,{0x46, 0x49, 0x49, 0x49, 0x31, 0x00} // 53 S
    ,{0x01, 0x01, 0x7f, 0x01, 0x01, 0x00} // 54 T
    ,{0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00} // 55 U
    ,{0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00} // 56 V
    ,{0x3f, 0x40, 0x38, 0x40, 0x3f, 0x00} // 57 W
    ,{0x63, 0x14, 0x08, 0x14, 0x63, 0x00} // 58 X
    ,{0x07, 0x08, 0x70, 0x08, 0x07, 0x00} // 59 Y
    ,{0x61, 0x51, 0x49, 0x45, 0x43, 0x00} // 5a Z
    ,{0x00, 0x7f, 0x41, 0x41, 0x00, 0x00} // 5b [
    ,{0x02, 0x04, 0x08, 0x10, 0x20, 0x00} // 5c /
    ,{0x00, 0x41, 0x41, 0x7f, 0x00, 0x00} // 5d ]
    ,{0x04, 0x02, 0x01, 0x02, 0x04, 0x00} // 5e ^
    ,{0x40, 0x40, 0x40, 0x40, 0x40, 0x00} // 5f _
    ,{0x00, 0x01, 0x02, 0x04, 0x00, 0x00} // 60 `
    /*,{0x00, 0x00, 0x00, 0x00, 0x, 0x0000} // 20 // Padded to fix the bug, when try to print a, it prints b*/
    ,{0x20, 0x54, 0x54, 0x54, 0x78, 0x00} // 61 a
    ,{0x7f, 0x48, 0x44, 0x44, 0x38, 0x00} // 62 b
    ,{0x38, 0x44, 0x44, 0x44, 0x20, 0x00} // 63 c
    ,{0x38, 0x44, 0x44, 0x48, 0x7f, 0x00} // 64 d
    ,{0x38, 0x54, 0x54, 0x54, 0x18, 0x00} // 65 e
    ,{0x08, 0x7e, 0x09, 0x01, 0x02, 0x00} // 66 f
    ,{0x0c, 0x52, 0x52, 0x52, 0x3e, 0x00} // 67 g
    ,{0x7f, 0x08, 0x04, 0x04, 0x78, 0x00} // 68 h
    ,{0x00, 0x44, 0x7d, 0x40, 0x00, 0x00} // 69 i
    ,{0x20, 0x40, 0x44, 0x3d, 0x00, 0x00} // 6a j
    ,{0x7f, 0x10, 0x28, 0x44, 0x00, 0x00} // 6b k
    ,{0x00, 0x41, 0x7f, 0x40, 0x00, 0x00} // 6c l
    ,{0x7c, 0x04, 0x18, 0x04, 0x78, 0x00} // 6d m
    ,{0x7c, 0x08, 0x04, 0x04, 0x78, 0x00} // 6e n
    ,{0x38, 0x44, 0x44, 0x44, 0x38, 0x00} // 6f o
    ,{0x7c, 0x14, 0x14, 0x14, 0x08, 0x00} // 70 p
    ,{0x08, 0x14, 0x14, 0x18, 0x7c, 0x00} // 71 q
    ,{0x7c, 0x08, 0x04, 0x04, 0x08, 0x00} // 72 r
    ,{0x48, 0x54, 0x54, 0x54, 0x20, 0x00} // 73 s
    ,{0x04, 0x3f, 0x44, 0x40, 0x20, 0x00} // 74 t
    ,{0x3c, 0x40, 0x40, 0x20, 0x7c, 0x00} // 75 u
    ,{0x1c, 0x20, 0x40, 0x20, 0x1c, 0x00} // 76 v
    ,{0x3c, 0x40, 0x30, 0x40, 0x3c, 0x00} // 77 w
    ,{0x44, 0x28, 0x10, 0x28, 0x44, 0x00} // 78 x
    ,{0x0c, 0x50, 0x50, 0x50, 0x3c, 0x00} // 79 y
    ,{0x44, 0x64, 0x54, 0x4c, 0x44, 0x00} // 7a z
    ,{0x00, 0x08, 0x36, 0x41, 0x00, 0x00} // 7b {
    ,{0x00, 0x00, 0x7f, 0x00, 0x00, 0x00} // 7c |
    ,{0x00, 0x41, 0x36, 0x08, 0x00, 0x00} // 7d }
    ,{0x10, 0x08, 0x08, 0x10, 0x08, 0x00} // 7e ~
    ,{0x78, 0x46, 0x41, 0x46, 0x78, 0x00} // 7f DEL
  };
  
  
  
}
#endif /* Nokia5110LCD_h */
