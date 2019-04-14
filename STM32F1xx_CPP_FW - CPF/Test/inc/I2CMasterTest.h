
#include"I2CMaster.h"

//This tests the GpioOutput as well

void I2CMaster_Test()
{
    static uint8_t I2CDevicesAvailable[10];
    //int i = sizeof(I2CMaster);
    I2CMaster I2CDev(Gpio::B6, Gpio::B7);
    
    I2CDev.HwInit();
    
    while(1)
    {
       I2CDev.ScanBus(I2CDevicesAvailable,10);
       LL_mDelay(1000);
    }
}