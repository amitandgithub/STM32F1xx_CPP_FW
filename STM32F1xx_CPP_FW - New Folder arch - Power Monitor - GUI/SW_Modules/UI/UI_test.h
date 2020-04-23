

#include"Menu.h"
#include"ST7735_defs.h"


HMI::InputDeviceEvents_t InputDeviceEvents = HMI::NONE;

class DownCallback : public Callback
{
    void CallbackFunction()
    {
     InputDeviceEvents = HMI::DOWN;
    }
};
  
class PressCallback : public Callback
{
    void CallbackFunction()
    {
      InputDeviceEvents = HMI::PRESS;
    }
};

class LongPressCallback1 : public Callback
{
    void CallbackFunction()
    {
      InputDeviceEvents = HMI::LONGPRESS;
    }
};

//st7735_Font_7x10  st7735_Font_16x26 st7735_Font_11x18
 static HMI::Menu<128,160,&st7735_Font_11x18,11,18,GREEN,BLACK,RED> HomeScr;
 
class MenuPressCallback : public Callback
{
    void CallbackFunction()
    {
      static char testchar = '0';
      HomeScr.UpdateLineText(0,6,&testchar,1);
      testchar++;
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


static BSP::HwButton<DigitalIn<A2,INPUT_PULLDOWN>,
  reinterpret_cast <Callback *>(&DownCb),
  reinterpret_cast <Callback *>(&PressCb),
  reinterpret_cast <Callback *>(&LongPressCb)> B9_HwBtnInt; 






void Menu_test()
{   
  HomeScr.LineInit(0,"Amit",   &MenuPressCb,&MenuLongPressCb );
  HomeScr.LineInit(1,"Line 1", &MenuPressCb,&MenuLongPressCb );
  HomeScr.LineInit(2,"Line 2", &MenuPressCb,&MenuLongPressCb );
  HomeScr.LineInit(3,"Line 3", &MenuPressCb,&MenuLongPressCb );
  HomeScr.LineInit(4,"Line 4", &MenuPressCb,&MenuLongPressCb );
  HomeScr.LineInit(5,"Line 5", &MenuPressCb,&MenuLongPressCb ); 
  
  TFT_1_8.HwInit();
  TFT_1_8.Rotate(BSP::ST7735::ROTATE_180_DEG);
  TFT_1_8.FillScreen(BLACK);
  HomeScr.FlushDisplay();
  B9_HwBtnInt.HwInit(); 
  while(1)
  {    
    B9_HwBtnInt.Run();
    HomeScr.Run(InputDeviceEvents); 
    InputDeviceEvents = HMI::NONE;
    //LL_mDelay(100);
  }
  
}






