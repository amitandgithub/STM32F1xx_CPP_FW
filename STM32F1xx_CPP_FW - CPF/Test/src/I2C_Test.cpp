

#include"I2C_Test.h"



static I2c I2CDevIntr(Gpio::B6, Gpio::B7,100000U);
I2c::I2CStatus_t Status;
uint8_t RepeatedStart = 0;
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
    SlaveAddress = 0x68<<1;
    I2c_test_id = I2C_POLL_TX_1_RX_1;
    TxBuf[0] = 0x00;
    TxBuf[1] = 0x00;
    I2CDevIntr.XferPoll(SlaveAddress,TxBuf,2);       

    TxBuf[0] = 8;
    while(1)
    {
        switch(I2c_test_id)
        {            
        case I2C_POLL_TX_1_RX_1:
            TxLen = RxLen = 1;
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
            Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
            I2c_test_id = I2C_POLL_TX_1_RX_2;
            break;    
        case I2C_POLL_TX_1_RX_2:
            TxLen = 1;
            RxLen = 2;
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
            Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
            I2c_test_id = I2C_POLL_TX_1_RX_3;
            break; 
        case I2C_POLL_TX_1_RX_3:
            TxLen = 1;
            RxLen = 3;
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
            Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
            I2c_test_id = I2C_POLL_TX_2_RX_2;
            break; 
        case I2C_POLL_TX_2_RX_2:  
            TxLen = RxLen = 2;
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
            Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
            I2c_test_id = I2C_POLL_TX_3_RX_3;
            break;  
            
        case I2C_POLL_TX_3_RX_3:  
            TxLen = RxLen = 3;
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);   
            Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
            I2c_test_id = I2C_POLL_DS1307_RTC_RAM;
            break;
            
        case I2C_POLL_DS1307_RTC_RAM: 
            TxLen = RxLen = 56;           
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);            
            Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_DS1307_RTC_RAM = Pass"), STR("I2C_POLL_DS1307_RTC_RAM = Fail"));
            I2c_test_id = I2C_POLL_DS1307_RTC_RAM_REPEATED_START;
            break;
            
        case I2C_POLL_DS1307_RTC_RAM_REPEATED_START: 
            TxLen = RxLen = 56;           
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
            I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,1);           
            Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_DS1307_RTC_RAM_REPEATED_START = Pass"), STR("I2C_POLL_DS1307_RTC_RAM_REPEATED_START = Fail"));
            I2c_test_id = I2C_POLL_TX_1_RX_1;
            RepeatedStart = !RepeatedStart;
            break;
            
        default: break;
        }
        
        //MemSet(TxBuf,0,100);
        for(uint8_t i = 1; i<70; i++) TxBuf[i] = i;
        MemSet(RxBuf,0,100);
        LL_mDelay(50);
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


#endif //I2C_POLL