


#include "ssd1306_Test.h"
#include "Utils.h"
#include "ssd1306.h"
extern BSP::INA219 INA219_Dev;
static BSP::INA219::Power_t Power;

#define SAMPLES_PER_SEC 20
float mAh,current_samples;
void PowerMonitor();

uint32_t num = 0;
uint32_t num1 = 0;
void Display_I_and_V(uint8_t font);
void RunAnimation();
void Waveform();

void ssd1306_test()
{
   static bool InitDone;
  
  if(InitDone == false)
  {
  SSD1306_Init();
  INA219_Dev.HwInit();
  INA219_Dev.SetCalibration_32V_2A();
  Mlx90615_Init();
  InitDone = true;
  }

  while(1)
  {    
    PowerMonitor();
    //RunCatAnimation();
    //RunArrow();
    //SSD1306_UpdateScreen();    
    //RunAnimation();
   //Pixels(num = !num);
    //Waveform();
    
    //printf("V = %f      I = %f \n",Power.Voltage,Power.Current);
    return;
  }  
  
}

extern volatile uint32_t i2c_dma_delay;
extern __IO uint32_t uwTick;
void Display_I_and_V(uint8_t font)
{
  static uint8_t I2C_Voltage[] = "V           ";
  static uint8_t I2C_Current[] = "I           ";
  static uint8_t I2C_mAH[]     = "C           "; 
  
  ftoa(Power.Voltage, (char*)&I2C_Voltage[2], 2);  
  SSD1306_GotoXY (0,0);
  SSD1306_Puts ( (char*)I2C_Voltage, &Font_11x18, SSD1306_COLOR_WHITE);//Font_11x18     
  
  ftoa(Power.Current, (char*)&I2C_Current[2], 3);  
  SSD1306_GotoXY (0,font*1);
  SSD1306_Puts ((char*)I2C_Current, &Font_11x18, SSD1306_COLOR_WHITE);
  
  ftoa(MLX90615_Get_Temperature(), (char*)&I2C_mAH[2], 2);  
  //ftoa(mAh, (char*)&I2C_mAH[2], 2); 
  SSD1306_GotoXY (0,font*2);
  SSD1306_Puts ((char*)I2C_mAH, &Font_11x18, SSD1306_COLOR_WHITE);

  SSD1306_UpdateScreen();
  //HAL_Delay (300);
}

#if 0

color = !color;
SSD1306_SetBrightness(color?255:0); // 1.9mA
HAL_Delay (8000);
SSD1306_Off(); // 6.3 uA
HAL_Delay (8000);
SSD1306_On(); // 5.8 mA

#endif
uint8_t direction = 1 , height;
void Waveform()
{
  uint16_t x,y;
  for(x=0;x<128;x++)
  {
    for(y=0;y<height;y++)
    {      
        SSD1306_DrawPixel(x,y,(SSD1306_COLOR_t)1);
    }
    SSD1306_UpdateScreen(); 

    if(direction == 1)
    {
       if(++height == 64) direction = 0;
    }
    else
    {
      if(--height == 0) direction = 1;
    }
  }
  
  for(x=0;x<128;x++)
  {
    for(y=0;y<64;y++)
    {      
        SSD1306_DrawPixel(x,y,(SSD1306_COLOR_t)0);
    }
    SSD1306_UpdateScreen(); 
  }

}

void RunAnimation()
{
  
  while (1)
  {
    //// HORSE ANIMATION START //////
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse1,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse2,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse3,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse4,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse5,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse6,128,64,1);
    SSD1306_UpdateScreen();
    
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse7,128,64,1);
    SSD1306_UpdateScreen();
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse8,128,64,1);
    SSD1306_UpdateScreen();
    
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse9,128,64,1);
    SSD1306_UpdateScreen();
    
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,horse10,128,64,1);
    SSD1306_UpdateScreen();
    
    
    //// HORSE ANIMATION ENDS //////
  }
}


void RunCatAnimation()
{
  
  while (1)
  {
    //// HORSE ANIMATION START //////
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,cat1,128,64,1);
    SSD1306_UpdateScreen();
    HAL_Delay (200);
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,cat2,128,64,1);
    SSD1306_UpdateScreen();
    HAL_Delay (200);
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,cat3,128,64,1);
    SSD1306_UpdateScreen();
    HAL_Delay (200);
    
    SSD1306_Clear();
    SSD1306_DrawBitmap(0,0,cat4,128,64,1);
    SSD1306_UpdateScreen();
    HAL_Delay (200);
    
  }
}


void RunArrow()
{  
  while (1)
  {
    //// HORSE ANIMATION START //////
    for(uint8_t i = 0; i<128; i += 2)
    {
      SSD1306_Clear();
        SSD1306_DrawBitmap(i,0,man,128,64,1);
        SSD1306_UpdateScreen();
        HAL_Delay (60);
    }    
    
    for(uint8_t i = 128; i>0; i -= 2)
    {
      SSD1306_Clear();
        SSD1306_DrawBitmap(i,0,man,128,64,1);
        SSD1306_UpdateScreen();
        HAL_Delay (60);
    } 
  }
}


uint32_t sample_count,Sec_count,Disp_count;
uint32_t PowerInterval = 1000, previousPowerMillis = 0;
uint32_t DisplayInterval = 10, previousDisplayMillis = 0; 
void PowerMonitor()
{
  uint32_t currentMillis = HAL_GetTick();
     
  if (currentMillis- previousPowerMillis >= PowerInterval)
  {
    previousPowerMillis = currentMillis;
    INA219_Dev.Run(&Power);
    mAh += Power.Current /3600.0;   
    Sec_count++;
  }
  
 if (currentMillis - previousDisplayMillis >= DisplayInterval)
  {
     previousDisplayMillis = currentMillis;
     Display_I_and_V(22);
     Disp_count++;
  }
 
}




float MLX90615_Get_Temperature()
{
  static float Temperature;
  Mlx90615_ReadTempTarget(&Temperature,1);
  return Temperature;
}
//
//    Mlx90615_ReadTempAmbient(&Temperature);
//    printf("Ambient = %f",Temperature);
//    Mlx90615_ReadTempTarget(&Temperature,1);
//    printf("\t Object = %f\n",Temperature);






