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


static BSP::INA219::Power_t Power;
static char TimeString[]     = "T =            ";
static uint8_t I2C_Voltage[] = "V =            ";
static uint8_t I2C_Current[] = "I =            ";
static uint8_t I2C_mAH[]     = "C =  1234.00mAh"; 

void Nokia_Lcd_Test()
{
  static bool InitDone;  
  if(InitDone == false)
  {
    LCDx.HwInit();
    INA219_Dev.HwInit();
    INA219_Dev.SetCalibration_32V_2A(); //SetCalibration_32V_2A//SetCalibration_16V_400mA
    InitDone = true;
  }
  
  while(1)
  {    
    rtc.GetTime(&TimeString[4]); 
    INA219_Dev.Run(&Power);
    ftoa(Power.Voltage, (char*)&I2C_Voltage[4], 4);
    ftoa(Power.Current, (char*)&I2C_Current[4], 4); 
    ftoa(Power.mAH, (char*)&I2C_mAH[4], 2); 
    
    LCDx.DisplayStr(0,0,TimeString); // 127395 tmpl, 124884 - obj(2511 cycles less than template)
    LCDx.DisplayStr(1,0,(char const*)I2C_Voltage);
    LCDx.DisplayStr(2,0,(char const*)I2C_Current);
    LCDx.DisplayStr(3,0,(char const*)I2C_mAH);
    //LL_mDelay(100);    
  }
  
}

#endif
