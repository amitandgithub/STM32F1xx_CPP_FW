


#include "ssd1306_Test.h"
#include "Utils.h"
uint32_t num = 0;
uint32_t num1 = 0;
void Display_I_and_V(uint8_t font);
void ssd1306_test()
{
  while(1)
  {
    Display_I_and_V(37);    
    SSD1306_UpdateScreen();    
  }  
}


void Display_I_and_V(uint8_t font)
{
    static uint8_t I2C_Voltage[] = "V          ";
    static uint8_t I2C_Current[] = "I           ";

    
    intToStr(num%60, (char*)&I2C_Voltage[2], 5);

    SSD1306_GotoXY (0,0);
    SSD1306_Puts ( (char*)I2C_Voltage, &Font_16x26, SSD1306_COLOR_WHITE);    
    //SSD1306_UpdateScreen();
    
    if((num%60) == 0)
      num1++;
    
    intToStr(num1, (char*)&I2C_Current[2], 5);

    SSD1306_GotoXY (0,font*1);
    SSD1306_Puts ((char*)I2C_Current, &Font_16x26, SSD1306_COLOR_WHITE);
    HAL_Delay (2000);
    num++;
}

#if 0

    color = !color;
    SSD1306_SetBrightness(color?255:0); // 1.9mA
     HAL_Delay (8000);
    SSD1306_Off(); // 6.3 uA
      HAL_Delay (8000);
     SSD1306_On(); // 5.8 mA
     
#endif
     
     