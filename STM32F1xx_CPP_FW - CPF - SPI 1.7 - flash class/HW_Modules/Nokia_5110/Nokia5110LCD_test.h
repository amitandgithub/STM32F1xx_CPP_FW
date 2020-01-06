/******************
** FILE: Nokia5110LCD_test.h
**
******************/

#include "Nokia5110LCD.h"
#include "NokiaLCD.h"

#ifndef Nokia5110LCD_test_h
#define Nokia5110LCD_test_h

extern BSP::NokiaLCD NokiaLCDObj;

#define LCDx NokiaLCDObj

void Nokia_Lcd_Test()
{
  char i = '1';
  static bool InitDone;
  
  if(InitDone == false)
  {
    LCDx.HwInit();
    InitDone = true;
  }
  
  while(1)
  {    
    LCDx.DisplayStr(0,0,"Avni Chaudhary"); // 127395 tmpl, 124884 - obj(2511 cycles less than template)
    LCDx.DisplayStr(1,0,"Avni Chaudhary");
    LCDx.DisplayStr(2,0,"Shiwani Sangwn");
    LCDx.DisplayChar(3,2,i++);
    LL_mDelay(100);    
  }
  
}

#endif
