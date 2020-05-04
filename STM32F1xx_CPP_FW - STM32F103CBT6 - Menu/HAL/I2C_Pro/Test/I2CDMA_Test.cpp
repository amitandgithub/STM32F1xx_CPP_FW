

#include "CPP_HAL.h"
#include "I2CDMA.h"
using namespace HAL;

I2CDMA I2CDev_DMA(Gpio::B6, Gpio::B7);
static uint16_t slaveaddress = 0x08<<1;
static uint8_t TxRx[20];

void I2CDMA_Test()
{
    I2CDMA::I2CStatus_t Status;
    static uint8_t testID = 0;
    TxRx[18]  = (uint8_t)sizeof(I2CDMA); // 52 bytes
    
    I2CDev_DMA.HwInit();
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  //I2CDev_DMA.MasterTx(0x80,TxRx,3,&Status);  
                    break;
        
        case 1:   break;
        
        case 2:  break;
        
        default: break;
        
        }
        
        LL_mDelay(1);
    }
}



