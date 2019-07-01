
#include"INA219.h"
#include"I2c.h"
#include<stdio.h> 
#include<cstring>
using namespace HAL;

#define I2C_PRINT printf

void I2c_Poll_Tests();
bool Test_Condition(bool condition, char* PassStr, char* FailStr);
void MemSet(uint8_t* mem, uint8_t data, uint32_t size);
void DS1307_Read(uint8_t MemAddress, uint8_t* rxBuf, uint8_t rxLen );
void DS1307_Write(uint8_t* txBuf, uint8_t txLen );

//void DS1307_Write(uint8_t MemAddress, uint8_t Data );
//uint8_t DS1307_Read(uint8_t MemAddress);

#define STR(x) ((char*)x)


typedef enum
{
    I2C_POLL_TX_1_RX_1,
    I2C_POLL_TX_2_RX_2,
    I2C_POLL_TX_3_RX_3,
    I2C_POLL_DS1307_RTC_TIME,
    I2C_POLL_DS1307_RTC_RAM,
    
    I2C_INT_TX_1_RX_1,
    I2C_INT_TX_2_RX_2,
    I2C_INT_TX_3_RX_3,

}I2c_tests_t;


