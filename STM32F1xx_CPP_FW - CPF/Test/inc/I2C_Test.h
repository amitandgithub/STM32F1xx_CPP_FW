
#include"INA219.h"
#include"I2c.h"
#include<stdio.h> 
#include<cstring>
using namespace HAL;

#define I2C_PRINT printf

void I2c_Poll_Tests();
bool Test_Condition(bool condition, char* PassStr, char* FailStr);
void MemSet(uint8_t* mem, uint8_t data, uint32_t size);

#define STR(x) ((char*)x)


typedef enum
{
    I2C_POLL_TX_1_RX_1,
    I2C_POLL_TX_1_RX_2,
    I2C_POLL_TX_1_RX_3,
    I2C_POLL_TX_2_RX_2,
    I2C_POLL_TX_3_RX_3,
    I2C_POLL_DS1307_RTC_TIME,
    I2C_POLL_DS1307_RTC_RAM,
    I2C_POLL_DS1307_RTC_RAM_REPEATED_START,
    
    I2C_INT_TX_1_RX_1,
    I2C_INT_TX_2_RX_2,
    I2C_INT_TX_3_RX_3,

}I2c_tests_t;


