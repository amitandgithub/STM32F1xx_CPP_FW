
#include"I2CPoll.h"

I2CPoll I2CDev(Gpio::B6, Gpio::B7);

static void I2CPoll_Master_TxRx()
{
    static uint8_t TxBuff[10], RxBuff[10];
    
    TxBuff[0] = 'a';        
    I2CDev.MasterTx(0x90,TxBuff,1);        
    LL_mDelay(100);        
    I2CDev.MasterRx(0x90,RxBuff,1);        
    printf("TxBuff[0] = %c \t RxBuff[0] = %c \n",TxBuff[0],RxBuff[0] );          
    LL_mDelay(100);
}

static void I2CPoll_Slave_RxTx()
{
    static uint8_t RxBuff[10];
    
    I2CDev.SlaveRx(RxBuff,1);        
    LL_mDelay(100);
    RxBuff[0] += 1; 
    I2CDev.SlaveTx(RxBuff,1);                 
    LL_mDelay(100);
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
    testID = 2;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  I2CPoll_Master_Scan_Bus(); break;        
        
        case 1:  I2CPoll_Master_TxRx(); break;
        
        case 2:  I2CPoll_Slave_RxTx(); break;
        
        default: break;
        
        }
        
        LL_mDelay(10);
    }
}