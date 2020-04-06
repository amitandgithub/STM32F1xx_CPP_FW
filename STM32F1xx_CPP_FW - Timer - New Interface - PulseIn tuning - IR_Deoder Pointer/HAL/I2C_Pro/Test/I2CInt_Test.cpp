

#include "CPP_HAL.h"
#include "I2CInt.h"
using namespace HAL;

I2CInt I2CDev_Int(Gpio::B6, Gpio::B7);
static uint16_t slaveaddress = 0x08<<1;
static uint8_t TxRx[20];

void I2CInt_Test()
{
    
    static uint8_t testID = 0;
    TxRx[18]  = (uint8_t)sizeof(I2CInt); // 248 bytes, map file 280 bytes
    
    I2CDev_Int.HwInit();
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:   break;        
        
        case 1:   break;
        
        case 2:  break;
        
        default: break;
        
        }
        
        LL_mDelay(1);
    }
}



