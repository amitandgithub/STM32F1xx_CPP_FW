

#include"I2C_Test.h"



static I2c I2CDevIntr(Gpio::B6, Gpio::B7,100000U);
I2c::I2CStatus_t Status;
uint16_t SlaveAddress = 0x0010;
uint8_t TxBuf[100];
uint8_t RxBuf[100];
uint16_t TxLen,RxLen;
char text[100][30];

// RTC DS1307 
#if I2C_POLL

void I2c_Poll_Tests()
{
    uint32_t I2c_test_id = 0;
    
    //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
    
    I2CDevIntr.HwInit();
    
    //I2CDevIntr.ScanBus(RxBuf,20);
    
    I2c_test_id = I2C_POLL_TX_1_RX_1;
    TxBuf[0] = 0x00;
    TxBuf[1] = 0x00;
    DS1307_Write(TxBuf,2);
    SlaveAddress = 0x68<<1;
    
    while(1)
    {
        switch(I2c_test_id)
        {            
        case I2C_POLL_TX_1_RX_1:
            DS1307_Write(TxBuf,TxLen);
			DS1307_Read(0x08,RxBuf,RxLen);			
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);            
            TxLen = RxLen = 1;
            TxBuf[0] = 'M';
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            LL_mDelay(5);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 1 )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
            I2c_test_id = I2C_POLL_TX_2_RX_2;
            break;             
        case I2C_POLL_TX_2_RX_2:  
            TxLen = RxLen = 2;
            TxBuf[0] = 'L'-1;
            TxBuf[1] = 'M'-1;
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            TxBuf[1] += 1;
            LL_mDelay(25);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 2 )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
            I2c_test_id = I2C_POLL_TX_3_RX_3;
            break;  
            
        case I2C_POLL_TX_3_RX_3:  
            TxLen = RxLen = 3;
            TxBuf[0] = 'L'-1;
            TxBuf[1] = 'M'-1;
            TxBuf[2] = 'N'-1;
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            TxBuf[1] += 1;
            TxBuf[2] += 1;
            LL_mDelay(25);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 2 )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
            I2c_test_id = I2C_POLL_DS1307_RTC_RAM;
            break;            

        case I2C_POLL_DS1307_RTC_RAM: 
            TxLen = RxLen = 56;
            TxBuf[0] = 0x08;
            MemSet(&TxBuf[1],0xAB,TxLen+1);
            DS1307_Write(TxBuf,TxLen+1);
            DS1307_Read(0x08,RxBuf,RxLen);
            Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_DS1307_RTC_RAM = Pass"), STR("I2C_POLL_DS1307_RTC_RAM = Fail"));
            break;
            
        default: break;
        }
        
        MemSet(TxBuf,0,100);
        MemSet(RxBuf,0,100);
        LL_mDelay(1500);
    }
}

#endif //I2C_POLL

void I2c_Poll_Tests1()
{
    uint32_t I2c_test_id = 0;
    
    //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
    
    I2CDevIntr.HwInit();
    
    //I2CDevIntr.ScanBus(RxBuf,20);
    
    I2c_test_id = I2C_POLL_TX_1_RX_1;
    TxBuf[0] = 0x00;
    TxBuf[1] = 0x00;
    DS1307_Write(TxBuf,2);
    
    while(1)
    {
        switch(I2c_test_id)
        {            
        case I2C_POLL_TX_1_RX_1:  
            TxLen = RxLen = 1;
            TxBuf[0] = 'M';
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            LL_mDelay(5);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 1 )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
            I2c_test_id = I2C_POLL_TX_2_RX_2;
            break;             
        case I2C_POLL_TX_2_RX_2:  
            TxLen = RxLen = 2;
            TxBuf[0] = 'L'-1;
            TxBuf[1] = 'M'-1;
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            TxBuf[1] += 1;
            LL_mDelay(25);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 2 )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
            I2c_test_id = I2C_POLL_TX_3_RX_3;
            break;  
            
        case I2C_POLL_TX_3_RX_3:  
            TxLen = RxLen = 3;
            TxBuf[0] = 'L'-1;
            TxBuf[1] = 'M'-1;
            TxBuf[2] = 'N'-1;
            I2CDevIntr.TxPoll(SlaveAddress,TxBuf,TxLen);
            TxBuf[0] += 1;
            TxBuf[1] += 1;
            TxBuf[2] += 1;
            LL_mDelay(25);
            I2CDevIntr.RxPoll(SlaveAddress,RxBuf,RxLen);
            LL_mDelay(5);
            Test_Condition( !(std::memcmp( (const void*) TxBuf,(const void*) RxBuf, 2 )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
            I2c_test_id = I2C_POLL_DS1307_RTC_RAM;
            break;            

        case I2C_POLL_DS1307_RTC_RAM: 
            TxLen = RxLen = 56;
            TxBuf[0] = 0x08;
            MemSet(&TxBuf[1],0xAB,TxLen+1);
            DS1307_Write(TxBuf,TxLen+1);
            DS1307_Read(0x08,RxBuf,RxLen);
            Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_DS1307_RTC_RAM = Pass"), STR("I2C_POLL_DS1307_RTC_RAM = Fail"));
            break;
            
        default: break;
        }
        
        MemSet(TxBuf,0,100);
        MemSet(RxBuf,0,100);
        LL_mDelay(1500);
    }
}


bool Test_Condition(bool condition, char* PassStr, char* FailStr)
{
    if(condition)
    {
        printf(PassStr);
        printf("\n");
        return true;
    }
    else
    {
        printf(FailStr);
        printf("\n");
        return true;
    }   
    
}

void MemSet(uint8_t* mem, uint8_t data, uint32_t size)
{
    while(size--) *mem++ = data;
}


void DS1307_Write(uint8_t* txBuf, uint8_t txLen )
{
    I2CDevIntr.TxPoll(0x68<<1,txBuf,txLen);  
}

void DS1307_Read(uint8_t MemAddress, uint8_t* rxBuf, uint8_t rxLen )
{
    I2CDevIntr.TxPoll(0x68<<1,&MemAddress,1);

    I2CDevIntr.RxPoll(0x68<<1,rxBuf,rxLen);     
}