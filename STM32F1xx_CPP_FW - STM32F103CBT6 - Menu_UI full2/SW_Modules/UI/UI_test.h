

#include"Menu.h"
#include"ST7735_defs.h"
#include"Utils.h"
#include"Window.h" 
#include"TextWindow.h" 
#include"SettingWindow.h"  
#include"NamedSettingWindow.h"  
#include"Screen.h" 
#include"UI.h" 
#include"AnalogIn.h"

extern GpioOutput LED;

void PowerMonitorUI();

HMI::InputEvents_t InputEvents = HMI::NONE;

static volatile uint32_t sg_CurrentsettingValue;

class SettingCallback : public HMI::SettingCallback
{
    virtual void CallbackFunction(uint32_t UpdatedSettingValue)
    {
      //printf("setting Done = %d \n\r",UpdatedSettingValue); 
      sg_CurrentsettingValue = UpdatedSettingValue;
    }
};

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

static SettingCallback aSettingCallback;
#define CURRENT_FONT st7735_Font_11x18
#define POWER_SCREEN_FONT st7735_Font_11x18
enum
{
  Line1 = 0,
  Line2,
  Line3,
  Line4,
  Line5,
  Line6,
  Line7,  
};
//st7735_Font_7x10  st7735_Font_11x18 st7735_Font_16x26 

/*************************************************** Setting Window *****************************************************************/
char Text1[20] = "Delay              ";
uint32_t Delay1;

const HMI::WindowContext_t  WindowContext1   = {0,127,18*Line1,18*Line2,&CURRENT_FONT,GREEN,BLACK,RED}; 
const HMI::SettingContext_t SettingContext1  = {&Delay1,Text1,&aSettingCallback,14,3,6};
                                             
char Text2[20] = "Brightnes          ";      
uint32_t Brightness;                       
const HMI::WindowContext_t  WindowContext2   = {0,127,18*Line2,18*Line3,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContext2  = {&Brightness,Text2,&aSettingCallback,14,3,10};
                                             
char Text3[20] = "Amit               ";      
uint32_t Delay3;                             
const HMI::WindowContext_t  WindowContext3   = {0,127,18*Line3,18*Line4,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContext3  = {&Delay3,Text3,&aSettingCallback,14,3,5};
                                             
char Text4[20] = "Sumit             ";       
uint32_t Delay4;                             
const HMI::WindowContext_t  WindowContext4   = {0,127,18*Line1,18*Line2,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContext4  = {&Delay4,Text4,&aSettingCallback,14,3,6};
                                             
char Text5[20] = "Avni              ";       
uint32_t Delay5;                             
const HMI::WindowContext_t  WindowContext5   = {0,127,18*Line2,18*Line3,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContext5  = {&Delay5,Text5,&aSettingCallback,14,3,5};

char Text6[20] = "Vani              ";
uint32_t Delay6;
const HMI::WindowContext_t  WindowContext6   = {0,127,18*Line3,18*Line4,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::SettingContext_t SettingContext6  = {&Delay6,Text6,&aSettingCallback,14,3,5};

/*************************************************** NamedSetting Window *****************************************************************/
char namesetting1[20] = "Hello              ";       
uint32_t Frequency;                             
const HMI::WindowContext_t  NamedWindowContext1   = {0,127,18*Line4,18*Line5,&CURRENT_FONT,GREEN,BLACK,RED};
const HMI::NamedSettingTable_t NamedSettingTable1[] = { {0,(char*)"Amit"}, {1,(char*)"Shivani"},{2,(char*)"Avni"} ,{3,(char*)"Kaka"},{4,(char*)"Ritika"},{5,(char*)"Vani"},{6,(char*)"Dadu"},{7,(char*)"Dadi"}};
const HMI::NamedSettingContext_t NamedSettingContext1  = {&Frequency,namesetting1,&aSettingCallback,14,6,8,NamedSettingTable1};


/*************************************************** Text Window *****************************************************************/
class TextPressCallback : public Callback
{
    void CallbackFunction()
    {
      sg_CurrentsettingValue++;
    }
};

class TextLPressCallback : public Callback
{
    void CallbackFunction()
    {
      SystemReset();
    }
};

TextPressCallback Presscb;
TextLPressCallback LPresscb;
const HMI::WindowContext_t  TextWindowContext1   = {0,127,18*Line5,18*Line6,&CURRENT_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t TextContext1  = {(char*)"Reset Data    ",&Presscb,&Presscb,&Presscb,&LPresscb};

char PowerText[6*14+1] = "T = 00:00:00  V =           I =            C =            Temp =         A2 =       ";

const HMI::WindowContext_t  PowerWindowContext   = {0,127,0,159,&POWER_SCREEN_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t PowerTextContext  = {(char*)PowerText,&Presscb,&Presscb,&Presscb,&LPresscb};


HMI::Screen HomeScreen;
HMI::Screen HomeScreen1;
HMI::Screen LivePowerScreen;

void Window_test()
{
  static HMI::UI MyUI;
  
  static HMI::SettingWindow SettingWindow1(&WindowContext1,&SettingContext1);
  static HMI::SettingWindow SettingWindow2(&WindowContext2,&SettingContext2);
  static HMI::SettingWindow SettingWindow3(&WindowContext3,&SettingContext3); 
  static HMI::NamedSettingWindow NamedSettingWindow4(&NamedWindowContext1,&NamedSettingContext1);
  static HMI::TextWindow TextWindow1(&TextWindowContext1,&TextContext1);
  
  static HMI::SettingWindow TextWindow4(&WindowContext4,&SettingContext4);
  static HMI::SettingWindow TextWindow5(&WindowContext5,&SettingContext5);
  static HMI::SettingWindow TextWindow6(&WindowContext6,&SettingContext6);
  
  static HMI::TextWindow PowerWindow(&PowerWindowContext,&PowerTextContext);
  
  TFT_1_8.HwInit();
  TFT_1_8.Rotate(BSP::ST7735::ROTATE_270_DEG);
  TFT_1_8.FillScreen(BLACK);
  B9_HwBtnInt.HwInit();
  
  HomeScreen.Register(&SettingWindow1);
  HomeScreen.Register(&SettingWindow2); 
  HomeScreen.Register(&SettingWindow3); 
  HomeScreen.Register(&NamedSettingWindow4);
  HomeScreen.Register(&TextWindow1);
  
  HomeScreen1.Register(&TextWindow4); 
  HomeScreen1.Register(&TextWindow5);
  HomeScreen1.Register(&TextWindow6);
  
  LivePowerScreen.Register(&PowerWindow);
  
  MyUI.Register(&HomeScreen);
  MyUI.Register(&HomeScreen1);
  MyUI.Register(&LivePowerScreen);
  
  //TFT_1_8.DrawRctangle(0,0,159,CURRENT_FONT.width+1,CYAN);

  while(1)
  {
    B9_HwBtnInt.Run();  
    PowerMonitorUI();
    B9_HwBtnInt.Run();  
    if(InputEvents != HMI::NONE)
    {
      MyUI.EventHandler(InputEvents);
      InputEvents = HMI::NONE;
    }

    MyUI.Display(HMI::Screen::WINDOW_HIGHLIGHT_COLOR);
  }
}


#define CHARS_IN_LINE 14
void PowerMonitorUI()
{
  
  static bool InitDone; 
  BSP::INA219::Power_t Power = {0,};
  
  if(InitDone == false)  
  {    
    INA219_Dev.HwInit();
    adc2.HwInit(A2,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    InitDone = true;
  }  
  
  //INA219_Dev.Run(&Power);
  rtc.GetTime((char*)&PowerText[CHARS_IN_LINE*Line1 +4]);
  PowerText[CHARS_IN_LINE*Line1 + 12] = ' ';
  
  ftoa(Power.Voltage, (char*)&PowerText[CHARS_IN_LINE*Line2 + 4],2,' ');    
  
  ftoa(Power.Current, (char*)&PowerText[CHARS_IN_LINE*Line3 + 5], 1,' '); 
  
  ftoa(Power.mAH, (char*)&PowerText[CHARS_IN_LINE*Line4 + 5], 2,' ');  
  
  ftoa(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1400,25,3300,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B), (char*)&PowerText[CHARS_IN_LINE*Line5 + 7], 1,' ');  
  
  intToStr((uint32_t)adc2.ReadVoltage(), (char*)&PowerText[CHARS_IN_LINE*Line6 + 8], 4);  
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