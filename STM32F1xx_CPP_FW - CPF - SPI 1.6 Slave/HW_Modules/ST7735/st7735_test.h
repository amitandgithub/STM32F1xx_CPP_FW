/******************
** FILE: st7735_test.h
**
******************/

//#include"HAL_Objects.h"
#include "st7735.h"

#ifndef st7735_test_h
#define st7735_test_h

void loop();
#define ST7735_DELAY 500

void st7735_Test()
{
  char i = '1';
  static bool InitDone;
  
  if(InitDone == false)
  {
   ST7735_Init();
   //spi2.SetBaudrate(Spi::SPI_BAUDRATE_DIV128);
    InitDone = true;
  }
  
  
  //while(1)
  {
   loop();  
    
  }
  
}

#if 0
void loop() 
{
    // Check border
    ST7735_FillScreen(ST7735_BLACK);

    for(int x = 0; x < ST7735_WIDTH; x++) {
        ST7735_DrawPixel(x, 0, ST7735_RED);
        ST7735_DrawPixel(x, ST7735_HEIGHT-1, ST7735_RED);
    }

    for(int y = 0; y < ST7735_HEIGHT; y++) {
        ST7735_DrawPixel(0, y, ST7735_RED);
        ST7735_DrawPixel(ST7735_WIDTH-1, y, ST7735_RED);
    }

    LL_mDelay(ST7735_DELAY);

    // Check fonts
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0, 0, "st7735_Font_7x10, red on black, lorem ipsum dolor sit amet", st7735_Font_7x10, ST7735_RED, ST7735_BLACK);
    ST7735_WriteString(0, 3*10, "st7735_Font_11x18, green, lorem ipsum", st7735_Font_11x18, ST7735_GREEN, ST7735_BLACK);
    ST7735_WriteString(0, 3*10+3*18, "st7735_Font_16x26", st7735_Font_16x26, ST7735_BLUE, ST7735_BLACK);
    LL_mDelay(200);

    // Check colors
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0, 0, "BLACK", st7735_Font_11x18, ST7735_WHITE, ST7735_BLACK);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_BLUE);
    ST7735_WriteString(0, 0, "BLUE", st7735_Font_11x18, ST7735_BLACK, ST7735_BLUE);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_RED);
    ST7735_WriteString(0, 0, "RED", st7735_Font_11x18, ST7735_BLACK, ST7735_RED);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_GREEN);
    ST7735_WriteString(0, 0, "GREEN", st7735_Font_11x18, ST7735_BLACK, ST7735_GREEN);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_CYAN);
    ST7735_WriteString(0, 0, "CYAN", st7735_Font_11x18, ST7735_BLACK, ST7735_CYAN);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_MAGENTA);
    ST7735_WriteString(0, 0, "MAGENTA", st7735_Font_11x18, ST7735_BLACK, ST7735_MAGENTA);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_YELLOW);
    ST7735_WriteString(0, 0, "YELLOW", st7735_Font_11x18, ST7735_BLACK, ST7735_YELLOW);
    LL_mDelay(ST7735_DELAY);

    ST7735_FillScreen(ST7735_WHITE);
    ST7735_WriteString(0, 0, "WHITE", st7735_Font_11x18, ST7735_BLACK, ST7735_WHITE);
    LL_mDelay(ST7735_DELAY);

}

#endif

#endif

