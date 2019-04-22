
#include"I2CIntr.h"

I2CIntr I2CDevIntr(Gpio::B6, Gpio::B7);

//uint16_t slaveaddress = 0x08<<1;
static uint8_t Intr_TxRx[20];

void I2CIntr_Test()
{
    uint8_t testID = 0;        
    
    I2CDevIntr.HwInit();
    I2CDevIntr.StartListening();
    I2CDevIntr.InteruptControl(HAL::I2CIntr::I2C_EVENT_INTERRUPT_ENABLE);
    I2CDevIntr.InteruptControl(HAL::I2CIntr::I2C_ERROR_INTERRUPT_ENABLE);
    
    testID = 0;
    
    while(1)
    {
        switch(testID)
        {
        case 0:   break;        
        
        case 1:   break;
        
        case 2:   break;
        
        default: break;
        
        }
        
        LL_mDelay(1);
    }
}