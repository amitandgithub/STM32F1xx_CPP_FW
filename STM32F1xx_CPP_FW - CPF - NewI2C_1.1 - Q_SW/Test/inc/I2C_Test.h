
#include"INA219.h"
#include"I2c.h"
#include<stdio.h> 
#include<cstring>
using namespace HAL;

#define I2C_PRINT printf

void I2c_Tests();
bool Test_Condition(bool condition, char* PassStr, char* FailStr);
void MemSet(uint8_t* mem, uint8_t data, uint32_t size);

#define STR(x) ((char*)(x))


typedef enum
{
	/* Tests with Polling*/
    I2C_POLL_TX_1_RX_1,
    I2C_POLL_TX_1_RX_2,
    I2C_POLL_TX_1_RX_3,
    I2C_POLL_TX_2_RX_2,
    I2C_POLL_TX_3_RX_3,
    I2C_INT_POLL_40_RX_40,

	/* Tests with Interrupts*/
    I2C_INT_TX_1_RX_1,
    I2C_INT_TX_1_RX_2,
    I2C_INT_TX_1_RX_3,
    I2C_INT_TX_2_RX_2,
    I2C_INT_TX_3_RX_3,
    I2C_INT_TX_40_RX_40,

	/* Tests with Transaction*/
    I2C_INT_TX_1_RX_1_TXN,
    I2C_INT_TX_1_RX_2_TXN,
    I2C_INT_TX_1_RX_3_TXN,
    I2C_INT_TX_2_RX_2_TXN,
    I2C_INT_TX_3_RX_3_TXN,
    I2C_INT_TX_40_RX_40_TXN,
    
    /* Tests with Queue*/
    I2C_INT_TX_QUEUE,
	
    I2C_INT_DS1307_RTC_TIME,
    I2C_INT_DS1307_RTC_RAM,
    I2C_INT_DS1307_RTC_RAM_TXN,

}I2c_tests_t;



class I2CCallback_t : public Callback
{
public:
    I2CCallback_t():XferComplete(0){;}
    void CallbackFunction(){ XferComplete++;}    
    uint32_t get_XferComplete() {return XferComplete; }
public:
    volatile uint32_t XferComplete;
};

