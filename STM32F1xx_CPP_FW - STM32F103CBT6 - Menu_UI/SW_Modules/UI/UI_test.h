

#include"Menu.h"
#include"ST7735_defs.h"

#include"Window.h" 
#include"TextWindow.h" 
#include"SettingWindow.h" 
#include"Screen.h" 

extern GpioOutput LED;

HMI::InputEvents_t InputEvents = HMI::NONE;

class DownCallback : public Callback
{
    void CallbackFunction()
    {
     InputEvents = HMI::DOWN;
    }
};
  
class PressCallback : public Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::PRESS;
    }
};

class LongPressCallback1 : public Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::LONGPRESS;
    }
};



static DownCallback DownCb;

static PressCallback PressCb;

static LongPressCallback1 LongPressCb;

static BSP::HwButton<DigitalIn<B9,INPUT_PULLDOWN>,
  reinterpret_cast <Callback *>(&DownCb),
  reinterpret_cast <Callback *>(&PressCb),
  reinterpret_cast <Callback *>(&LongPressCb)> B9_HwBtnInt; 

//st7735_Font_7x10  st7735_Font_11x18 st7735_Font_16x26 
char Text1[20] = "Delay              ";
uint32_t Delay1;
const HMI::WindowContext_t  WindowContext1   = {0+1,127-1,0+1,18+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext1  = {&Delay1,Text1,14,3,6};

char Text2[20] = "Brightnes          ";
uint32_t Brightness;
const HMI::WindowContext_t  WindowContext2   = {0+1,127-1,18+1,18*2+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext2  = {&Brightness,Text2,14,3,10};

char Text3[20] = "Amit               ";
uint32_t Delay3;
const HMI::WindowContext_t  WindowContext3   = {0+1,127-1,18*2+1,18*3+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext3  = {&Delay3,Text3,14,3,5};

char Text4[20] = "Sumit             ";
uint32_t Delay4;
const HMI::WindowContext_t  WindowContext4   = {0+1,127-1,18*3+1,18*4+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext4  = {&Delay4,Text4,14,3,6};

char Text5[20] = "Avni              ";
uint32_t Delay5;
const HMI::WindowContext_t  WindowContext5   = {0+1,127-1,18*4+1,18*5+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext5  = {&Delay5,Text5,14,3,5};

char Text6[20] = "Vani              ";
uint32_t Delay6;
const HMI::WindowContext_t  WindowContext6   = {0+1,127-1,18*5+1,18*6+1,&st7735_Font_11x18,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext6  = {&Delay6,Text6,14,3,5};

HMI::Screen HomeScreen;

void Window_test()
{
  static HMI::SettingWindow TextWindow1(&WindowContext1,&SettingContext1);
  static HMI::SettingWindow TextWindow2(&WindowContext2,&SettingContext2);
  static HMI::SettingWindow TextWindow3(&WindowContext3,&SettingContext3);
  static HMI::SettingWindow TextWindow4(&WindowContext4,&SettingContext4);
  static HMI::SettingWindow TextWindow5(&WindowContext5,&SettingContext5);
  static HMI::SettingWindow TextWindow6(&WindowContext6,&SettingContext6);
  
  TFT_1_8.HwInit();
  TFT_1_8.Rotate(BSP::ST7735::ROTATE_270_DEG);
  TFT_1_8.FillScreen(BLACK);
  B9_HwBtnInt.HwInit();
  HomeScreen.Register(&TextWindow1);
  HomeScreen.Register(&TextWindow2); 
  HomeScreen.Register(&TextWindow3); 
  HomeScreen.Register(&TextWindow4); 
  HomeScreen.Register(&TextWindow5);
  HomeScreen.Register(&TextWindow6);
  
  TFT_1_8.DrawRctangle(0,0,159,127,CYAN);

  while(1)
  {
    B9_HwBtnInt.Run();

    if(InputEvents != HMI::NONE)
    {
      HomeScreen.EventHandler(InputEvents);
      InputEvents = HMI::NONE;
    }
    HomeScreen.Display(HMI::Screen::WINDOW_HIGHLIGHT_COLOR);
    
  }
}






#if 0
class DownCallback : public Callback
{
    void CallbackFunction()
    {
     InputEvents_t = HMI::DOWN;
    }
};
  
class PressCallback : public Callback
{
    void CallbackFunction()
    {
      InputEvents_t = HMI::PRESS;
    }
};

class LongPressCallback1 : public Callback
{
    void CallbackFunction()
    {
      InputEvents_t = HMI::LONGPRESS;
    }
};

//st7735_Font_7x10  st7735_Font_16x26 st7735_Font_11x18
 static HMI::Menu<160,128,&st7735_Font_16x26,16,26,GREEN,BLACK,RED> HomeScr;
 
class MenuPressCallback : public Callback
{
    void CallbackFunction()
    {
      char TimeString[10];
      rtc.GetTime(&TimeString[0]); 
      HomeScr.UpdateLineText(0,0,TimeString,8);
    }
};

class MenuLongPressCallback : public Callback
{
    void CallbackFunction()
    {
      HAL::SystemReset();
    }
};

MenuPressCallback MenuPressCb; 
MenuLongPressCallback MenuLongPressCb;

void Menu_test();

static DownCallback DownCb;

static PressCallback PressCb;

static LongPressCallback1 LongPressCb;


static BSP::HwButton<DigitalIn<B9,INPUT_PULLDOWN>,
  reinterpret_cast <Callback *>(&DownCb),
  reinterpret_cast <Callback *>(&PressCb),
  reinterpret_cast <Callback *>(&LongPressCb)> B9_HwBtnInt; 

void Menu_test()
{   
  HomeScr.LineTextInit(0,"Amit");
  HomeScr.LineTextInit(1,"Line 1");
  HomeScr.LineTextInit(2,"Line 2");
  HomeScr.LineTextInit(3,"Line 3");
  HomeScr.LineTextInit(4,"Line 4");
  HomeScr.LineTextInit(5,"Line 5"); 
  
  HomeScr.RegisterLineEventHandler(0,nullptr,nullptr,nullptr,nullptr);
  HomeScr.RegisterLineEventHandler(1,nullptr,nullptr,nullptr,nullptr);
  HomeScr.RegisterLineEventHandler(2,nullptr,nullptr,nullptr,nullptr);
  HomeScr.RegisterLineEventHandler(3,nullptr,nullptr,nullptr,nullptr);
    
  TFT_1_8.HwInit();
  TFT_1_8.Rotate(BSP::ST7735::ROTATE_270_DEG);
  TFT_1_8.FillScreen(BLACK);
  HomeScr.FlushDisplay();
  B9_HwBtnInt.HwInit(); 
  rtc.RegisterCallback(Rtc::RTC_SEC,&PressCb);
  while(1)
  {    
    B9_HwBtnInt.Run();
    HomeScr.Run(InputEvents_t); 
    InputEvents_t = HMI::NONE;
    //LL_mDelay(100);
  }
  
}

#endif