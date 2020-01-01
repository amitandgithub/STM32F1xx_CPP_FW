
#include"I2CPoll.h"

I2CPoll I2CDev(Gpio::B6, Gpio::B7);

uint16_t slaveaddress = 0x08<<1;
static uint8_t TxRx[20];

static void I2CPoll_Master_TxRx()
{    
    TxRx[0] = 'a';      
    I2CDev.MasterTx(slaveaddress,TxRx,1);      
    LL_mDelay(1000);        
    I2CDev.MasterRx(slaveaddress,TxRx,16);        
    LL_mDelay(1000);
}

static void I2CPoll_Slave_RxTx()
{    
    if(I2CDev.AddressMatch() == HAL::I2CPoll::I2C_OK)
    {
//        I2CDev.SlaveRx(TxRx,1);
//        LL_mDelay(1000);
//        TxRx[0] += 1; 
//        I2CDev.SlaveTx(TxRx,1);                 
          LL_mDelay(1);     
    }
    
}

static void I2CPoll_Master_Scan_Bus()
{
    static uint8_t I2CDevicesAvailable[10];
    I2CDev.ScanBus(I2CDevicesAvailable,10);   
}
void I2CPoll_Test()
{
    static uint8_t testID = 0;        
    
    I2CDev.HwInit();
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  I2CPoll_Master_Scan_Bus(); break;        
        
        case 1:  I2CPoll_Master_TxRx(); break;
        
        case 2:  I2CPoll_Slave_RxTx(); break;
        
        default: break;
        
        }
        
        LL_mDelay(1);
    }
}