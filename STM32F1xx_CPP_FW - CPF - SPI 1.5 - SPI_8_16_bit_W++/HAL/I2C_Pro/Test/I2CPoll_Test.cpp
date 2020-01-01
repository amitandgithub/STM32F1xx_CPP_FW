

#include "CPP_HAL.h"
#include "I2CPoll.h"
using namespace HAL;

void I2CPoll_Master_TxRx();
void I2CPoll_Master_Scan_Bus();

I2CPoll I2CDev_Poll(Gpio::B6, Gpio::B7);
static uint16_t slaveaddress = 0x08<<1;
static uint8_t TxRx[20];


void I2CPoll_Test()
{
    static uint8_t testID = 0;
    TxRx[18]  = (uint8_t)sizeof(I2CPoll);
    
    I2CDev_Poll.HwInit();
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  I2CPoll_Master_Scan_Bus(); break;        
        
        case 1:  I2CPoll_Master_TxRx(); break;
        
        case 2:  break;
        
        default: break;
        
        }
        
        LL_mDelay(1);
    }
}


static void I2CPoll_Master_TxRx()
{    
    TxRx[0] = 'a';      
    I2CDev_Poll.TxPoll(slaveaddress,TxRx,1);      
    LL_mDelay(1000);        
    I2CDev_Poll.RxPoll(slaveaddress,TxRx,16);        
    LL_mDelay(1000);
}

static void I2CPoll_Master_Scan_Bus()
{
    static uint8_t I2CDevicesAvailable[10];
    I2CDev_Poll.ScanBus(I2CDevicesAvailable,10);   
}


