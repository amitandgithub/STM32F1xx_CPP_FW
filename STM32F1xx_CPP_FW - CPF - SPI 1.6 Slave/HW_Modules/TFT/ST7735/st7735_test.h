/******************
** FILE: st7735_test.h
**
******************/

//#include"HAL_Objects.h"
#include "ST7735.h"

#ifndef st7735_test_h
#define st7735_test_h

void loop();

#define ST7735_DELAY 500



extern BSP::ST7735<&spi2,A11,A9,A10,A0> TFT_1_8;
extern BSP::ST7735<&spi2,B0,A9,A10,A0> TFT_Small;

//#define TFT  TFT_1_8
#define TFT  TFT_Small

void st7735_Test()
{
  char i = '1';
  static bool InitDone;
  
  if(InitDone == false)
  {
  TFT.HwInit();
  //TFT1.HwInit();
    InitDone = true;
  }
  
  
  //while(1)
  {
   loop();  
    
  }
  
}

void loop() 
{
    // Check border
    TFT.FillScreen(BLACK);

    for(int x = 0; x < WIDTH; x++) {
        TFT.DrawPixel(x, 0, RED);
        TFT.DrawPixel(x, HEIGHT-1, RED);
    }

    for(int y = 0; y < HEIGHT; y++) {
        TFT.DrawPixel(0, y, RED);
        TFT.DrawPixel(WIDTH-1, y, RED);
    }

    LL_mDelay(ST7735_DELAY);

    // Check fonts
    TFT.FillScreen(BLACK);
    TFT.WriteString(0, 0, "st7735_Font_7x10, red on black, lorem ipsum dolor sit amet", st7735_Font_7x10, RED, BLACK);
    TFT.WriteString(0, 3*10, "st7735_Font_11x18, green, lorem ipsum", st7735_Font_11x18, GREEN, BLACK);
    TFT.WriteString(0, 3*10+3*18, "st7735_Font_16x26", st7735_Font_16x26, BLUE, BLACK);
    LL_mDelay(200);

    // Check colors
    TFT.FillScreen(BLACK);
    TFT.WriteString(0, 0, "BLACK", st7735_Font_11x18, WHITE, BLACK);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(BLUE);
    TFT.WriteString(0, 0, "BLUE", st7735_Font_11x18, BLACK, BLUE);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(RED);
    TFT.WriteString(0, 0, "RED", st7735_Font_11x18, BLACK, RED);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(GREEN);
    TFT.WriteString(0, 0, "GREEN", st7735_Font_11x18, BLACK, GREEN);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(CYAN);
    TFT.WriteString(0, 0, "CYAN", st7735_Font_11x18, BLACK, CYAN);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(MAGENTA);
    TFT.WriteString(0, 0, "MAGENTA", st7735_Font_11x18, BLACK, MAGENTA);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(YELLOW);
    TFT.WriteString(0, 0, "YELLOW", st7735_Font_11x18, BLACK, YELLOW);
    LL_mDelay(ST7735_DELAY);

    TFT.FillScreen(WHITE);
    TFT.WriteString(0, 0, "WHITE", st7735_Font_11x18, BLACK, WHITE);
    LL_mDelay(ST7735_DELAY);

}
#endif

