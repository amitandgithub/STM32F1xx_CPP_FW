
#include"Test.h"

using namespace BSP;


#if 0
/******************************************************************************
** GpioOutput Test
******************************************************************************/
GpioOutput LED(Gpio::A0);

void Gpio_Output_Test()
{
    LED.HwInit();
    LED.High();
    LED.Low();
    LED.SetPull(GpioOutput::GPIO_OUTPUT_TYPE_PULL_DOWN);
      
    while(1)
    {
//        LED.High();
//        LED.Low();
        LED.Toggle();
        //LL_mDelay(1000);
        
        if(!LED.ReadOutput())
        {
           LL_mDelay(1000);
        }
        else
        {
            LL_mDelay(70); 
        }
        
    }
}
/******************************************************************************/
#endif // GpioOutput Test 


BSP::Led C13_Led(Gpio::C13);

#if 0
/******************************************************************************
** LED Test
******************************************************************************/

BSP::Led C13_Led(Gpio::C13);

void Led_Test()
{
      C13_Led.HwInit();
      
      if(C13_Led.IsOn())
      {
          C13_Led.Blink();
      }
      
      C13_Led.Blink(400,70);
      C13_Led.Blink(400,70, 5);
    while(1)
    {
        C13_Led.Blink(400,70);
    }
}

/******************************************************************************/
#endif // LED Test


#if 0
/******************************************************************************
** GpioInput Test 
******************************************************************************/
void Gpio_Intput_Test()
{
    //Callback myCallback();
    static GpioInput Button(Gpio::A8);
    Button.HwInit();
    C13_Led.HwInit();
      
    while(1)
    {
        if(Button.Read())
        {
           C13_Led.Blink(400,70); 
        }
        
    }
}
/******************************************************************************/
#endif // GpioInput Test


#if 0
/******************************************************************************
** GpioInput Test 
******************************************************************************/

class ButtonCallback : public Callback
{
    void CallbackFunction()
    {
        C13_Led.Blink(400,70); 
    }
};

ButtonCallback BtnCallback;

void Gpio_Intr_Intput_Test()
{
    static GpioInput Button(Gpio::A8, GpioInput::INPUT_PULL_DOWN, GpioInput::IT_ON_RISING, &BtnCallback );
    Button.HwInit();
    C13_Led.HwInit();
      
    while(1)
    {
        
    }
}
/******************************************************************************/
#endif // GpioInput Test

#if 1
void BtnInt_Test()
{
    static BtnInt BtnInt_A8(Gpio::A8, BSP::BtnInt::BTN_PULL_DOWN, Rising,Falling );
    BtnInt_A8.HwInit();
    C13_Led.HwInit();
      
    while(1)
    {
        
    }
}

void Rising()
{
    static uint32_t Rising;    
    Rising++;
}

void Falling()
{
    static uint32_t Falling;    
    Falling++;
}

#endif

//void Hw_ButtonIntr_Test()
//{
//    static BSP::HwButton HwButton_A8(Gpio::A8);
//    HwButton_A8.HwInit();
//    HwButton_A8.RegisterEventHandler(HwButton::LongPress,Button_ISR);
//    C13_Led.HwInit();
//    while(1)
//    {
//        HwButton_A8.RunStateMachine();
//        LL_mDelay(10);
//    }
//}


BtnPoll btn(Gpio::A8,BSP::BtnPoll::BTN_PULL_DOWN,0);
BtnPoll btn_A1 (Gpio::A1,BSP::BtnPoll::BTN_PULL_DOWN,0);

void Hw_BtnPoll_Test()
{
    btn.HwInit();
    btn_A1.HwInit();
    btn.RegisterHandler(Btn_Low_Handler, Btn_High_Handler);
    //btn_A1.RegisterHandler(btn_A1_Low_Handler, btn_A1_High_Handler);
    C13_Led.HwInit();
    while(1)
    {
        btn.RunStateMachine();  
        btn_A1.RunStateMachine();  
    }
}
uint32_t Highs,Lows;
void Btn_Low_Handler()
{
    C13_Led.On();
    Lows++;
}

void Btn_High_Handler()
{
    C13_Led.Off();
    Highs++;
}

uint32_t Highs_A1,LowsA1;
void btn_A1_Low_Handler()
{
    C13_Led.On();
    LowsA1++;
}

void btn_A1_High_Handler()
{
    C13_Led.Off();
    Highs_A1++;
}

uint32_t CW,CCW,High;


void Rotary_Encoder_Test()
{
    static RotaryEncoderPoll::RotaryState_t  RotaryState;
    RotaryEncoderPoll RotaryEncoder(Gpio::A5,Gpio::A7,Gpio::A1,CW_Handler,CCW_Handler,Btn_Low_Handler,Btn_High_Handler);
    RotaryEncoder.HwInit();
    C13_Led.HwInit();

    while(1)
    {
        RotaryEncoder.RunStateMachine();
//        RotaryState = RotaryEncoder.getRotaryState();
//        
//        if( RotaryState == RotaryEncoderPoll::ROTATING_CLOCKWISE )
//        {
//            CW++;
//        }
//        else if(RotaryState == RotaryEncoderPoll::ROTATING_ANTI_CLOCKWISE )
//        {
//            CCW++;
//        }
//        
//        if(RotaryEncoder.getButtonState())
//        {
//            High++;
//        }
    }
}

void CW_Handler()
{
    CW++;
}

void CCW_Handler()
{
    CCW++;
}

#if 0
void Gpio_Output_Test()
{
    Peripherals::GpioOutput LED(GPIOC,GPIO_PIN_13);
    LED.HwInit();
    while(1)
    {

        LED.On();
        HAL_Delay(100);
        LED.ToggleOutput();
        HAL_Delay(100);
        LED.ToggleOutput();
        HAL_Delay(100);
        LED.Off();
        HAL_Delay(100);
    }
}
Peripherals::GpioOutput LED(GPIOC,GPIO_PIN_13);

void Gpio_Intput_Test()
{
    Peripherals::GpioOutput LED(GPIOC,GPIO_PIN_13);
    LED.HwInit();
    while(1)
    {
        LED.ToggleOutput();
        HAL_Delay(100);
    }
}

void LED_Toggle()
{
    LED.ToggleOutput();
}
void Hw_Button_Test()
{
    static Peripherals::HwButton HwButton_A8(GPIOA, GPIO_PIN_8);
    HwButton_A8.HwInit();
    HwButton_A8.RegisterEventHandler(HwButton::Click,LED_Toggle);
    LED.HwInit();
    while(1)
    {
        HwButton_A8.RunStateMachine();
        HAL_Delay(10);
    }
}



void I2C_Master_Test()
{
    static uint16_t I2C_array[120];
    static uint8_t array[] = "Amit";
    static uint8_t array1[10];
    Peripherals::I2C_Master I2C1_Master(I2C_Master::I2C1_SCL_B6_SDA_B7);
    I2C1_Master.HwInit();
    
    while(1)
    {
        I2C1_Master.Scan(I2C_array,120);
        
        while(I2C1_Master.Send(0x28,array,4) != HAL_OK);
        
        while(I2C1_Master.GetState() != HAL_I2C_STATE_READY);
        
        while(I2C1_Master.Read(0x28,array1,9) != HAL_OK);
        
        HAL_Delay(10);
    }
}

void I2C_Slave_Test()
{
    uint8_t array[] = "Amit Chaudhary";
    uint8_t array1[10];
    static I2C_Slave I2C1_Slave(I2C_Slave::I2C1_SCL_B6_SDA_B7);
    I2C1_Slave.HwInit();
    
    //I2C1_Slave.m_RxCallback = I2c_RxCallback;
    //I2C1_Slave.m_TxCallback = I2c_TxCallback;
    
    while(1)
    {
        
        //I2C1_Slave.StartListening();
        while(I2C1_Slave.Read(array1,4,I2C_FIRST_FRAME) != HAL_OK);
        //I2C1_Slave.Read(array1,1,I2C_FIRST_FRAME);
        while(I2C1_Slave.GetState() != HAL_I2C_STATE_READY);
        
        //HAL_Delay(100);
        
        while(I2C1_Slave.Send(&array[5],9,I2C_LAST_FRAME)!= HAL_OK);
        //I2C1_Slave.Send(&array[5],3,I2C_LAST_FRAME);
        
        
        while(I2C1_Slave.GetState() != HAL_I2C_STATE_READY);
        
        // HAL_Delay(100);  
        
    }
}

void INA219_Test()
{
    static INA219::Power_t Power;
    static Peripherals::I2C_Master I2C1_Master(I2C_Master::I2C1_SCL_B6_SDA_B7);
    static INA219 INA219_Obj(&I2C1_Master,0x80);
    I2C1_Master.HwInit();
    INA219_Obj.SetCalibration_32V_2A();
    while(1)
    {
        INA219_Obj.Run(&Power);
        printf("V = %f      I = %f \n",Power.Voltage,Power.Current);
        HAL_Delay(100);
    }    
}

void SPI_Poll_Test()
{
    static uint8_t array[4] = {0xA5,0xA5,0xA5,0xA5};
    Peripherals::GpioOutput LED(GPIOB,GPIO_PIN_0);
    Peripherals::GpioOutput CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_Poll SPI_Obj(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7, &CS, 100000);
    SPI_Obj.HwInit();
    LED.HwInit();
    while(1)
    {
        SPI_Obj.Tx(array,4);
        HAL_Delay(1);
    }    
}

void SPI_Int_Test()
{
    static uint8_t array[4] = {0xA5,0xA5,0xA5,0xA5};
    Peripherals::GpioOutput LED(GPIOB,GPIO_PIN_0);
    Peripherals::GpioOutput CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_Poll SPI_Obj(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7, &CS, 100000);
    SPI_Obj.HwInit();
    LED.HwInit();
    while(1)
    {
        SPI_Obj.Tx(array,4);
        HAL_Delay(1);
    }    
}

void SPI1_DMA_Test()
{
    
    static Peripherals::SPI_Base::Transaction_t aTransaction;
    static uint8_t TxBuf[4*1024] = {0xA5,0xA5,0xA5,0xA5};
    static uint8_t RxBuf[4]; 
    Peripherals::GpioOutput CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_DMA SPI_Obj(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7, &CS, 100000);
    SPI_Obj.HwInit();
    
    aTransaction.TxBuf = TxBuf; 
    aTransaction.TxLen = sizeof(TxBuf);
    aTransaction.RxBuf = RxBuf; 
    aTransaction.RxLen = 0;//sizeof(RxBuf);
    aTransaction.pCS = &CS;
    while(1)
    {
        while(SPI_Obj.Post(&aTransaction) != 1);
        //while( (SPI_Obj.GetStatus() & Peripherals::SPI_Base::SPI_BUSY) == Peripherals::SPI_Base::SPI_BUSY );
        while(1);
        //SPI_Obj.Run();
        //HAL_Delay(100);
    }    
}


void SPI2_DMA_Test()
{
    static Peripherals::SPI_Base::Transaction_t aTransaction;
    static uint8_t TxBuf[4*1024] = {0xA5,0xA5,0xA5,0xA5};
    static uint8_t RxBuf[4]; 
    Peripherals::GpioOutput CS(GPIOB,GPIO_PIN_12);
    static Peripherals::SPI_DMA SPI_Obj(Peripherals::SPI_Poll::SPI2_B12_B13_B14_B15, &CS, 100000);
    SPI_Obj.HwInit();
    
    aTransaction.TxBuf = TxBuf; 
    aTransaction.TxLen = sizeof(TxBuf);
    aTransaction.RxBuf = RxBuf; 
    aTransaction.RxLen = 0;//sizeof(RxBuf);
    aTransaction.pCS = &CS;
    while(1)
    {
        while(SPI_Obj.Post(&aTransaction) != 1);
        while(1);
        //SPI_Obj.Run();
        //HAL_Delay(100); 
    }
}

#define Nokia5110LCD_SPI1_Dual_Display_TEST 0

#if Nokia5110LCD_SPI1_Dual_Display_TEST
uint32_t i,j;
#define DEL 80
void Nokia5110LCD_SPI1_Dual_Display_Test()
{
    uint8_t Txcomplete=0,Txcomplete1=0;
    static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_Poll        SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 100000);
    static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput      Backlight(GPIOB,GPIO_PIN_10);
    static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
    static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
    
    static Peripherals::GpioOutput      CS1(GPIOB,GPIO_PIN_14);
    static Peripherals::GpioOutput      D_C1(GPIOB,GPIO_PIN_13);
    static Peripherals::GpioOutput      Reset1(GPIOB,GPIO_PIN_12);
    static Peripherals::GpioOutput      Backlight1(GPIOB,GPIO_PIN_11);    
    static Peripherals::Nokia5110LCD    LCD1(&SPI_Obj,&CS1,&D_C1,&Reset1,&Backlight1);
    
    LCD.HwInit();
    LCD1.HwInit();
    LCD.SetBrigntness(0x15);
    LCD1.SetBrigntness(0x13);
   
    while(1)
    {
#if 1
        if(Txcomplete == 0)
        {
            LCD.InvertRowBuf(i%6);
            LCD.DrawStrBuf(0,0,"Avni Chaudhary");
            LCD.DrawStrBuf(1,0,"IS");
            LCD.DrawCharBuf(2,2,i++);
            LCD.DrawStrBuf(3,3,"Good");
            LCD.DrawStrBuf(4,7,"Girl");            
            LCD.InvertRowBuf(i%6);
        }

//#elif 0     
        if(Txcomplete1 == 0)
        {
            LCD1.InvertRowBuf(j%6);
            LCD1.DrawStrBuf(0,0,"Amit Chaudhary");
            LCD1.DrawStrBuf(1,0,"IS");
            LCD1.DrawCharBuf(2,2,j++);
            LCD1.DrawStrBuf(3,3,"Good");
            LCD1.DrawStrBuf(4,7,"Boy");
            LCD1.InvertRowBuf(j%6);
        }
        
        Txcomplete1 = LCD1.Refresh();
        HAL_Delay(DEL);
        Txcomplete = LCD.Refresh();
        HAL_Delay(DEL);

#endif
        
        //HAL_Delay(3*DEL);
        
       // HAL_Delay(DEL);
       // LCD.ClearBuffer();
       // LCD.Refresh();
        
        
    }    
}
#endif

#define Nokia5110LCD_SPI1_TEST 0

#if Nokia5110LCD_SPI1_TEST
uint32_t i,j;
#define DEL 80
void Nokia5110LCD_SPI1_Test()
{
    uint8_t Txcomplete=0;
    static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_DMA         SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 100000);
    static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput      Backlight(GPIOB,GPIO_PIN_10);
    static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
    static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
    
    LCD.HwInit();
    LCD.SetBrigntness(0x13);
   
    while(1)
    {
        if(Txcomplete == 0)
        {
            LCD.InvertRowBuf(i%6);
            LCD.DrawStrBuf(0,0,"Avni Chaudhary");
            LCD.DrawStrBuf(1,0,"IS");
            LCD.DrawCharBuf(2,2,i++);
            LCD.DrawStrBuf(3,3,"Good");
            LCD.DrawStrBuf(4,7,"Girl");            
            LCD.InvertRowBuf(i%6);
        }
        
        Txcomplete = LCD.Refresh();
        HAL_Delay(DEL);

        
        
    }    
}
#endif

#define Nokia5110LCD_SPI2_Test 0

#if Nokia5110LCD_SPI2_Test
void Nokia5110LCD_SPI2_Test()
{
    #define DEL 200
    Peripherals::GpioOutput CS(GPIOB,GPIO_PIN_12);
    static Peripherals::SPI_DMA SPI_Obj(Peripherals::SPI_Poll::SPI2_B12_B13_B14_B15,&CS, 100000); // Nokia LCD with SPI 2
    static Peripherals::GpioOutput D_C(GPIOA,GPIO_PIN_9);
    static Peripherals::GpioOutput Reset(GPIOA,GPIO_PIN_10);
    static Peripherals::GpioOutput Backlight(GPIOB,GPIO_PIN_10);
    static Peripherals::Nokia5110LCD LCD(&SPI_Obj,&D_C,&Reset,&Backlight);
    LCD.HwInit();
    SPI_Obj.SPI2_TxDoneCallback = SPI2_DMA_Tx_Complete_Callback;
    while(1)
    {
        LCD.DrawLine(0,0,"Avni Chaudhary");
        HAL_Delay(DEL);
        LCD.DrawLine(1,0,"IS");
        HAL_Delay(DEL);
        LCD.DrawLine(2,2,"A");
        HAL_Delay(DEL);
        LCD.DrawLine(3,3,"Good");
        HAL_Delay(DEL);
        LCD.DrawLine(4,7,"Girl");
        HAL_Delay(DEL);
        LCD.Clear();
    }    
}
#endif

#define Nokia5110LCD_Dual_Test 0

#if Nokia5110LCD_Dual_Test
void Nokia5110LCD_Dual_Test()
{
    #define DEL_DUAL 100
    static SPI_HandleTypeDef hspi1;
    static Peripherals::GpioOutput CS1(GPIOA,GPIO_PIN_4);
    static DMA_HandleTypeDef hdma_spi1_rx;
    static DMA_HandleTypeDef hdma_spi1_tx;
    
    /* LCD on SPI 1*/
    static Peripherals::SPI_DMA SPI_Obj1(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7,&CS1, 100000,&hspi1, &hdma_spi1_rx, &hdma_spi1_tx);
    static Peripherals::GpioOutput D_C1(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput Reset1(GPIOB,GPIO_PIN_0);
    static Peripherals::GpioOutput Backlight1(GPIOB,GPIO_PIN_10);
    static Peripherals::Nokia5110LCD LCD1(&SPI_Obj1,&D_C1,&Reset1,&Backlight1);
    LCD1.HwInit();
    LCD1.SetContrast(0x15);
    
    /* LCD on SPI 2*/
    static SPI_HandleTypeDef hspi2;
    static DMA_HandleTypeDef hdma_spi2_rx;
    static DMA_HandleTypeDef hdma_spi2_tx;
    static Peripherals::GpioOutput CS2(GPIOA,GPIO_PIN_15);
    
    static Peripherals::SPI_DMA SPI_Obj2(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7, &CS2, 100000,&hspi2, &hdma_spi2_rx, &hdma_spi2_tx); // Nokia LCD with SPI 2
    static Peripherals::GpioOutput D_C2(GPIOA,GPIO_PIN_9);
    static Peripherals::GpioOutput Reset2(GPIOA,GPIO_PIN_10);
    static Peripherals::GpioOutput Backlight2(GPIOB,GPIO_PIN_10);
    static Peripherals::Nokia5110LCD LCD2(&SPI_Obj2,&D_C2,&Reset2,&Backlight2);
    LCD2.HwInit();
    
    
    
    while(1)
    {
        LCD1.DrawLine(0,0,"Amit Chaudhary");
        HAL_Delay(DEL_DUAL);
        LCD1.DrawLine(1,0,"IS");
        HAL_Delay(DEL_DUAL);
        LCD1.DrawLine(2,2,"A");
        HAL_Delay(DEL_DUAL);
        LCD1.DrawLine(3,3,"Good");
        HAL_Delay(DEL_DUAL);
        LCD1.DrawLine(4,7,"Boy");
        HAL_Delay(DEL_DUAL);
        
        
        LCD2.DrawLine(0,0,"Avni Chaudhary");
        HAL_Delay(DEL_DUAL);
        LCD2.DrawLine(1,0,"IS");
        HAL_Delay(DEL_DUAL);
        LCD2.DrawLine(2,2,"A");
        HAL_Delay(DEL_DUAL);
        LCD2.DrawLine(3,3,"Good");
        HAL_Delay(DEL_DUAL);
        LCD2.DrawLine(4,7,"Girl");
        HAL_Delay(DEL_DUAL);
        
        LCD1.Clear();
        LCD2.Clear();
    }    
}
#endif

#define Queue_TEST 0

#if Queue_TEST

void Queue_Test()
{
    using Queue = Queue<uint32_t,10> ;
    static Queue Uint_Q;
    uint32_t Data;
    Queue::QSTATUS status = Queue::QEMPTY;
    
    while(1) 
    {
        while ( status != Queue::QFULL)
        {
           status = Uint_Q.Write(0xDeadBabe);
          // printf("Write -> Status(%d)\n",status);
        }
        
        while ( status != Queue::QEMPTY)
        {
           status = Uint_Q.Read(Data); 
           //printf("Read -> Status(%d)  Data = 0x%x\n",status, Data);           
        } 
    }   
}
#endif


#define RTC_TEST 0

#if RTC_TEST

void RTC_Test()
{
    static Peripherals::RTC_Module rtc;
    static uint32_t TimeCounter;
    static char TimeCounterStr[10];

    static char TimeStr[10];
    uint8_t Txcomplete=0;
    static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_DMA         SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 100000);
    static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput      Backlight(GPIOA,GPIO_PIN_1);
    static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
    static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
    
    LCD.HwInit();
    LCD.SetBrigntness(0x13);
    LCD.BackLightON();
    rtc.HwInit();
    
    while(1) 
    {
        //rtc.GetTime(&RTC_Time);
        
        rtc.GetTime(TimeStr);
        TimeCounter = rtc.GetTimeCounter();
        intToStr(TimeCounter, TimeCounterStr, 10);
        if(Txcomplete == 0)
        {
            LCD.DrawStrBuf(0,0,TimeStr);
            LCD.DrawStrBuf(1,0,TimeCounterStr);
            LCD.DrawStrBuf(2,0,"Avni Chaudhary");
        }
        
        Txcomplete = LCD.Refresh();        
        HAL_Delay(300);
        LCD.ClearBuffer();
    }   
}
#endif

#define PRINTF_TEST 0

#if PRINTF_TEST

static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
static Peripherals::SPI_DMA         SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 100000);
static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
static Peripherals::GpioOutput      Backlight(GPIOA,GPIO_PIN_1);
static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
void putc ( void* p, char c);

void Printf_Test()
{
    uint8_t Txcomplete=0;

    
    LCD.HwInit();
    LCD.SetBrigntness(0x13);
    LCD.BackLightON();
    
    init_printf(nullptr,putc);
    Printf((char*)"Hello World");
    
    while(1) 
    {
        if(Txcomplete == 0)
        {
            Printf((char*)"%c",'A');
            LCD.DrawStrBuf(0,0,"Avni Chaudhary");
        }
        
        Txcomplete = LCD.Refresh();        
        HAL_Delay(300);
        LCD.ClearBuffer();
    }   
}


void putc ( void* p, char c)
{
    LCD.DrawCharBuf(1,0,c);
}
#endif



#define UI_TEST 1

#if UI_TEST

 static Screen::Event_t gEvent = Screen::MaxEvents;

void UI_Test()
{
    static Peripherals::HwButton HwButton_A8(GPIOA, GPIO_PIN_8);

    
    static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_DMA         SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 9000000);
    static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput      Backlight(GPIOA,GPIO_PIN_1);
    static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
    static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
    
    static UI MyUI(&LCD);
    static Screen HomeScreen( (char *)
                   " <Live Power> "
                   "              "
                   "              "
                   "              "
                   "              "
                   "              "
                   );
    static ControlScreen Menu( (char *)
                   "Line0         "
                   "Line1         "
                   "Line2         "
                   "Line3         "
                   "Line4         "
                   "              "
                   );
   
    
    HwButton_A8.HwInit();
    HwButton_A8.RegisterEventHandler(HwButton::Click,ClickEvent);
    HwButton_A8.RegisterEventHandler(HwButton::LongPress,LongPressEvent);
    HwButton_A8.RegisterEventHandler(HwButton::LongLongPress,LongLongPressEvent);
    MyUI.Init();
    MyUI.AddScreen(&HomeScreen);
    MyUI.AddScreen(&Menu);
    LCD.BackLightON();
    
    //HomeScreen.SetScreenHandler(ScreenTouchHandler,ScreenLongTouchHandler,nullptr);
    //HomeScreen.ScroolText(0,DisplayBuffer::SPEED_NORMAL,DisplayBuffer::LEFT);
    Menu.SetScreenHandler(ScreenTouchHandler,nullptr,nullptr);
   // Menu.AddHandler(5,ScreenTouchHandler,nullptr);
    while(1) 
    {
        //PowerMonitorScreen.DrawStr(0,0,"Amit Chaudhary");
        HwButton_A8.RunStateMachine();
        MyUI.EventHamdler(gEvent);
        MyUI.Run();
        //AL_Delay(100);
    }   
}


void ClickEvent(void)
{
	gEvent = Screen::Touch;
}
void LongPressEvent(void)
{
	gEvent = Screen::LongTouch;
}
void LongLongPressEvent(void)
{
	gEvent = Screen::LongLongTouch;
}

void ScreenTouchHandler(void)
{
	Peripherals::UI::GoToNextScreen();
}

void ScreenLongTouchHandler(void)
{
	Peripherals::UI::GoToPreviousScreen();
}

#endif

#define DISPLAY_BUFFER_TEST 1

#if DISPLAY_BUFFER_TEST


void Display_Buffer_Test()
{
    uint8_t Txcomplete=0;
    static Peripherals::GpioOutput      CS(GPIOA,GPIO_PIN_4);
    static Peripherals::SPI_DMA         SPI_Obj(Peripherals::SPI_Base::SPI1_A4_A5_A6_A7,&CS, 100000);
    static Peripherals::GpioOutput      D_C(GPIOB,GPIO_PIN_1);
    static Peripherals::GpioOutput      Backlight(GPIOA,GPIO_PIN_1);
    static Peripherals::GpioOutput      Reset(GPIOB,GPIO_PIN_0);
    static Peripherals::Nokia5110LCD    LCD(&SPI_Obj,&CS,&D_C,&Reset,&Backlight);
    static Screen MyScreen((char *)
                       "Amit          "
                       "Amit          "
                       "Amit          "
                       "Amit          "
                       "Amit          "
                       "Amit          "
                       );
    LCD.HwInit();
    LCD.SetBrigntness(0x13);
    LCD.BackLightON();
    while(1) 
    {
        LCD.DisplayBuf(MyScreen.GetBuffer());
        
        if(Txcomplete == 0)
        {
            Txcomplete = LCD.Refresh();
        }
               
        HAL_Delay(100);
       // LCD.ClearBuffer();
    }   
}

#endif

#define SD_TEST 1

#if SD_TEST


void SD_Test()
{
//    static Peripherals::GpioOutput CS1(GPIOA,GPIO_PIN_4,Peripherals::GpioOutput::AF_PP, Peripherals::GpioOutput::HIGH, Peripherals::GpioOutput::PULL_UP);
//    static Peripherals::SPI_Poll SPI1_Obj(Peripherals::SPI_Poll::SPI1_A4_A5_A6_A7, &CS1, 25000000);
    
    static Peripherals::GpioOutput CS2(GPIOB,GPIO_PIN_12,Peripherals::GpioOutput::AF_PP, Peripherals::GpioOutput::HIGH, Peripherals::GpioOutput::PULL_UP);
    static Peripherals::SPI_Poll SPI2_Obj(Peripherals::SPI_Poll::SPI2_B12_B13_B14_B15, &CS2, 25000000);
    
    SD SD_Card(&SPI2_Obj,&CS2);
    
    static volatile uint32_t Puts_Delay,Printf_Delay;
    static char name[1*1024] = {69,};
    FATFS fs;
    FIL fil;
    FRESULT fr;
    memset((void*)name,'X',sizeof(name));
    name[sizeof(name) -1] = '\n';
	fr = SD_Card.mount(&fs, "", 1); // 1861648
	if (fr != FR_OK) 
	{
		while(1);
	}
	/* Opens an existing file. If not exist, creates a new file. */
	fr = SD_Card.open(&fil, "Test_1_Nov18_2.txt", FA_WRITE | FA_OPEN_ALWAYS); // 736438
    
	if (fr == FR_OK) 
	{
		/* Seek to end of the file to append data */
		fr = f_lseek(&fil, f_size(&fil)); // 220
        
		if (fr != FR_OK) SD_Card.close(&fil);
                    
        Printf_Delay = HAL_GetTick();
		SD_Card.print(&fil,name); // 2843, 10k @9404506
        Printf_Delay = HAL_GetTick() - Printf_Delay; // 10k @ 256 prescalar for 132 ms 
        
		SD_Card.close(&fil); // 1143384

		
		fr = SD_Card.mount(0, "", 0); // 117
		if (fr != FR_OK) 
		{
			while(1);
		}
	}
	else
	{
		//while(1);
	} 
}

#endif

#endif