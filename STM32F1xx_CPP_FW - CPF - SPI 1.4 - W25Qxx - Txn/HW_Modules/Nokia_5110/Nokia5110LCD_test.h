/******************
** FILE: Nokia5110LCD_test.h
**
******************/

//#include"HAL_Objects.h"
#include "Nokia5110LCD.h"

#ifndef Nokia5110LCD_test_h
#define Nokia5110LCD_test_h


extern HAL::Spi spi1;
extern BSP::Nokia5110LCD<&spi1,B1,B0,B10,A0> LCD;



void Nokia_Lcd_Test()
{
  char i = '1';
  static bool InitDone;
  
  if(InitDone == false)
  {
    LCD.HwInit();
    InitDone = true;
  }
  
  
  while(1)
  {
    
    LCD.DisplayStr(0,0,"Avni Chaudhary");
    LCD.DisplayChar(3,2,i++);
    LL_mDelay(100);    
    
  }
  
}






































#endif // Nokia5110LCD_test_h