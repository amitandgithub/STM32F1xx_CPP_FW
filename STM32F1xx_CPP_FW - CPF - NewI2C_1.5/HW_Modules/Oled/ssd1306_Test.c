


#include "ssd1306_Test.h"

void ssd1306_test()
{
  

while(1)
{
    SSD1306_GotoXY (0,0);
    SSD1306_Puts ((char*)"Amit Avni", &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY (0, 30);
    SSD1306_Puts ((char*)"Siwani Golu)", &Font_11x18, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen(); //display

    HAL_Delay (2000);

    SSD1306_ScrollRight(0,7);  // scroll entire screen
    HAL_Delay(2000);  // 2 sec

    SSD1306_ScrollLeft(0,7);  // scroll entire screen
    HAL_Delay(2000);  // 2 sec
  
    SSD1306_DMA_Display();
    HAL_Delay(2000);  // 2 sec
    
    SSD1306_DMA_Dim(true);
    HAL_Delay(3000);  // 3 sec
    SSD1306_DMA_Dim(false);
}
    SSD1306_Stopscroll();
    SSD1306_Clear();

    SSD1306_DrawBitmap(0,0,logo, 128, 64, 1);
    SSD1306_UpdateScreen();

    HAL_Delay(2000);

    SSD1306_ScrollRight(0x00, 0x0f);    // scroll entire screen right

    HAL_Delay (2000);

    SSD1306_ScrollLeft(0x00, 0x0f);  // scroll entire screen left

    HAL_Delay (2000);

    SSD1306_Scrolldiagright(0x00, 0x0f);  // scroll entire screen diagonal right

    HAL_Delay (2000);

    SSD1306_Scrolldiagleft(0x00, 0x0f);  // scroll entire screen diagonal left

    HAL_Delay (2000);

    SSD1306_Stopscroll();   // stop scrolling. If not done, screen will keep on scrolling


    SSD1306_InvertDisplay(1);   // invert the display

    HAL_Delay(2000);

    SSD1306_InvertDisplay(0);  // normalize the display

    HAL_Delay(2000);

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