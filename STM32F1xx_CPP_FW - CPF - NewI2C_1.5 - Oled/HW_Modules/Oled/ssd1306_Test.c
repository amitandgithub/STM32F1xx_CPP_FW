


#include "ssd1306_Test.h"
#include "Utils.h"
uint32_t num = 0;
uint32_t num1 = 1000;
uint8_t Name[] = "Amit    ";
uint8_t Name1[] = "Avni    ";
void ssd1306_test()
{
  uint8_t x = 0,y = 0,color=0;
  //SSD1306_ScrollLeft(0,1);
  //SSD1306_ScrollRight(1,2);
  //SSD1306_ScrollLeft(0,7);
  SSD1306_GotoXY (0,0);
  SSD1306_Puts ((char*)Name, &Font_16x26, SSD1306_COLOR_WHITE); // Font_7x10
  
//  SSD1306_GotoXY (0, 16);
//  SSD1306_Puts ((char*)"Siwani Sangwan", &Font_11x18, SSD1306_COLOR_WHITE);
//  
//  SSD1306_GotoXY (0, 16*2);
//  SSD1306_Puts ((char*)"Avni", &Font_11x18, SSD1306_COLOR_WHITE);
//  
//  SSD1306_GotoXY (0, 16*3);
//  SSD1306_Puts ((char*)"Golu)", &Font_11x18, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();
  //SSD1306_Clear();
  while(1)
  {
    intToStr(num++, (char*)&Name[5], 6);
      
    //Name[0] += 1;
    SSD1306_GotoXY (0,26);
    SSD1306_Puts ( (char*)&Name[5], &Font_16x26, SSD1306_COLOR_WHITE);
    

    intToStr(num1, (char*)&Name1[5], 6);
    SSD1306_GotoXY (0,26*2);
    SSD1306_Puts ( (char*)&Name1[5], &Font_16x26, SSD1306_COLOR_WHITE);
    
    SSD1306_UpdateScreen();
    HAL_Delay (100);
    //SSD1306_SetBrightness(color?255:0);
    color = !color;
  }  
}