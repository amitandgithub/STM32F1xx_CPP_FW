
#include"I2CIntr.h"

I2CIntr I2CDevIntr(Gpio::B6, Gpio::B7,100000U);


static uint8_t Intr_TxRx[20];

void I2CIntr_Test()
{
    uint8_t testID = 0;        
    uint16_t slaveaddress = 0x08<<1;
    uint8_t name[] = "Amit Chaudhary is a good Boy";
    name[sizeof(name)/sizeof(uint8_t)-1] = '\n';
    I2CDevIntr.HwInit();
    //I2CDevIntr.StartListening();
    I2CDevIntr.InteruptControl(HAL::I2CIntr::I2C_INTERRUPT_ENABLE_ALL);
    
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:  
           // if (I2CDevIntr.GetState() != I2CIntr::READY) 
           //     return;
            I2CDevIntr.MasterTx(slaveaddress,name,sizeof(name)/sizeof(uint8_t)); 
            break;        
            
        case 1:   break;
        
        case 2:   break;
        
        default: break;
        
        }
        
        //LL_mDelay(2000);
    }
}