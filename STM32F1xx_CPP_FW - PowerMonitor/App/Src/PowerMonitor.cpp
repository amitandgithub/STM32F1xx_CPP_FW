


#include"PowerMonitor.h"

#include"Callback.h"
#include"CPP_HAL.h"
#include"ff.h"
#include"diskio.h"


using namespace HAL;
using namespace BSP;


extern INA219 INA219_Dev;
extern AnalogIn ChipTemperature;
extern Rtc rtc;

static AnalogIn adc2(LL_ADC_CHANNEL_2);
static HAL::PulseOut<(uint32_t)TIM3,TIM3_CH3_PIN_B0,10000> Dimmer; //TIM3_CH1_PIN_A6 
 
#if SD_CARD
/* Globals for SD_CARD task*/
#define SD_CARD_SAVE_FREQ 100

FATFS fs;
FIL fil;
FRESULT fr;
char FileName[30];
#endif


#define CHARS_IN_LINE 14
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

class DownCallback : public HAL::Callback
{
    void CallbackFunction()
    {
     InputEvents = HMI::DOWN;
    }
};
  
class PressCallback : public HAL::Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::PRESS;
    }
};

class LongPressCallback1 : public HAL::Callback
{
    void CallbackFunction()
    {
      InputEvents = HMI::LONGPRESS;
    }
};

static DownCallback DownCb;

static PressCallback PressCb;

static LongPressCallback1 LongPressCb;

static BSP::HwButton<HAL::DigitalIn<A8,INPUT_PULLDOWN>,reinterpret_cast <HAL::Callback *>(&DownCb),reinterpret_cast <HAL::Callback *>(&PressCb),reinterpret_cast <HAL::Callback *>(&LongPressCb)> B9_HwBtnInt; 

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
class TextUpCallback : public Callback
{
    void CallbackFunction()
    {
      HMI::UI::GoToScreen(2);
    }
};
class TextDownCallback : public Callback
{
    void CallbackFunction()
    {
      HMI::UI::GoToScreen(2);
    }
};
class TextPressCallback : public Callback
{
    void CallbackFunction()
    {
      HMI::UI::GoToPreviousScreen();
    }
};

class TextLPressCallback : public Callback
{
    void CallbackFunction()
    {
      SystemReset();
    }
};

TextUpCallback Upcb;
TextDownCallback Downcb;
TextPressCallback Presscb;
TextLPressCallback LPresscb;
const HMI::WindowContext_t  TextWindowContext1   = {0,127,18*Line5,18*Line6,&CURRENT_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t TextContext1  = {(char*)"Live Data     ",&Upcb,&Downcb,nullptr,nullptr};

char PowerText[6*14+1] = "T = 00:00:00  V =           I =            C =            Temp =         A2 =       ";

const HMI::WindowContext_t  PowerWindowContext   = {0,127,0,159,&POWER_SCREEN_FONT,GREEN,BLACK,RED}; 
const HMI::TextWindowContext_t PowerTextContext  = {(char*)PowerText,nullptr,nullptr,&Presscb,&LPresscb};


HMI::Screen HomeScreen;
HMI::Screen HomeScreen1;
HMI::Screen LivePowerScreen;

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
  

 static volatile uint32_t t1;
 
  void PowerMonitorInit()
  {
    Dimmer.HwInit();  
    Dimmer.SetDutyCycle(50); //1-24.6mv 2-52.5 10-276mv  50-1.39v 99 - 2.75v 100 2.77v
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
    
#if SD_CARD   
  // Mmm dd yyyy
  mem_cpy(&FileName[0], "Pawer", 5);
  FileName[5] = '_';
  FileName[6] = __DATE__[0]; // month[0]
  FileName[7] = __DATE__[1]; // month[1]
  FileName[8] = __DATE__[2]; // month[2]
  FileName[9] = '_';
  FileName[10] = __DATE__[4]; // Day[0]
  FileName[11] = __DATE__[5]; // Day[1]
  FileName[12] = '_';
  FileName[13] = __TIME__[0]; // Hour[0]
  FileName[14] = __TIME__[1]; // Hour[1]
  FileName[15] = '_';
  FileName[16] = __TIME__[3]; // Min[0]
  FileName[17] = __TIME__[4]; // Min[1]
  FileName[18] = '_';
  FileName[19] = __TIME__[6]; // Sec[0]
  FileName[20] = __TIME__[7]; // Sec[1]
  FileName[21] = '.';
  FileName[22] = 't';
  FileName[23] = 'x';
  FileName[24] = 't';
  FileName[25] = 0;
#endif
  
  }
  uint32_t SD_Time;
  void PowerMonitorRun()
  {  
    uint32_t Millis;
    
    INA219_Dev.HwInit();
    ChipTemperature.HwInit((Port_t)0,0,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    adc2.HwInit(A2,LL_ADC_SAMPLINGTIME_239CYCLES_5);
    B9_HwBtnInt.Run();  
    PowerMonitor_UI();
    B9_HwBtnInt.Run();  
    PowerOutputUART();
    if(InputEvents != HMI::NONE)
    {
      MyUI.EventHandler(InputEvents);
      InputEvents = HMI::NONE;
    }
    Millis = HAL::GetTick();
    MyUI.Display(HMI::Screen::WINDOW_HIGHLIGHT_COLOR);
    t1 = HAL::GetTick() - Millis;
    
    Millis = HAL::GetTick();
#if SD_CARD
    RUN_EVERY_MILLIS(1000,Save_To_SD_Card(FileName,&PowerText[0],84));
#endif
    SD_Time = HAL::GetTick() - Millis;
    
  }
  
BSP::INA219::Power_t Power;
void PowerMonitor_UI()
{ 
  INA219_Dev.Run(&Power);
  
  rtc.GetTime((char*)&PowerText[CHARS_IN_LINE*Line1 +4]);
  
  PowerText[CHARS_IN_LINE*Line1 + 12] = ' ';
  
  ftoa(Power.Voltage, (char*)&PowerText[CHARS_IN_LINE*Line2 + 4],2,' ');    
  
  ftoa(Power.Current, (char*)&PowerText[CHARS_IN_LINE*Line3 + 4], 1,' '); 
  
  ftoa(Power.mAH, (char*)&PowerText[CHARS_IN_LINE*Line4 + 5], 2,' ');  
  
  ftoa(__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300,1400,25,3300,ChipTemperature.Read(),LL_ADC_RESOLUTION_12B), (char*)&PowerText[CHARS_IN_LINE*Line5 + 9], 1,' ');
  
  intToStr((uint32_t)adc2.ReadVoltage(), (char*)&PowerText[CHARS_IN_LINE*Line6 + 8], 4); 
}


void PowerOutputUART()
{
  PowerText[CHARS_IN_LINE*Line2 + 10] = 0;
  printf("%s",&PowerText[CHARS_IN_LINE*Line2]);
  PowerText[CHARS_IN_LINE*Line2 + 10] = ' ';
  
  PowerText[CHARS_IN_LINE*Line3 + 8] = '\n';
  PowerText[CHARS_IN_LINE*Line3 + 9] = '\r';
  PowerText[CHARS_IN_LINE*Line3 + 10] = 0;
  printf("%s",&PowerText[CHARS_IN_LINE*Line3]);
  PowerText[CHARS_IN_LINE*Line3 + 8] = ' ';
  PowerText[CHARS_IN_LINE*Line3 + 9] = ' ';
  PowerText[CHARS_IN_LINE*Line3 + 10] = ' ';
}



#if SD_CARD

bool Save_To_SD_Card1(const char* FileName, char* pData, uint32_t len)
{   
  uint32_t Writtenlen;
  if(pData == nullptr)
    return false; 
  
  fr = f_mount(&fs, "", 1); // 1861648
  if (fr != FR_OK) 
  {
    return false;
  }
  fr = f_open(&fil, FileName, FA_WRITE | FA_OPEN_ALWAYS);
  if (fr == FR_OK) 
  {
     // Mark the end of the string to be saved on SD card
    pData[len] = '\0';
  
    /* Seek to end of the file to append data */
    fr = f_lseek(&fil, f_size(&fil)); 
    
    if (fr != FR_OK) f_close(&fil);
    
    //f_printf(&fil,pData);
    f_write(&fil,pData,84,&Writtenlen);
    
    f_close(&fil); // 1143384
    
    pData[len] = ' ';
    
    fr = f_mount(0, "", 0); // 117
    if (fr != FR_OK) 
    {
      return false;
    }
    return true;
  }
  return false;  
  
}

bool Save_To_SD_Card(const char* FileName, char* pData, uint32_t len)
{   
  uint32_t Writtenlen;
  
  if(pData == nullptr)
    return false;  
  
  if (f_mount(&fs, "", 1) == FR_OK) // 1861648
  {
    if (f_open(&fil, FileName, FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) 
    {      
      // Mark the end of the string to be saved on SD card
      //pData[len] = '\0';
  
      /* Seek to end of the file to append data */    
      if (f_lseek(&fil, f_size(&fil)) != FR_OK) f_close(&fil);
      
      //f_printf(&fil,pData);
      f_write(&fil,pData,84,&Writtenlen);
      
      f_close(&fil); // 1143384
      
       //pData[len] = ' ';
       
      if (f_mount(0, "", 0) == FR_OK)  // 117
      {
        return true;
      }
    }
  }
  return false;  
}

#endif


