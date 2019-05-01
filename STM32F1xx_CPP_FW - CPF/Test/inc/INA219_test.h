

#include"I2CPoll.h"
#include"I2CIntr.h"
#include"INA219.h"

using namespace BSP;
using namespace HAL;



static INA219::Power_t Power;

void INA219_Test()
{
#if 1
    static I2CPoll I2CDevPoll(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
    static I2CIntr I2CDevIntr(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
    static INA219 INA219_Dev(&I2CDevIntr,0x80U);

    INA219_Dev.HwInit();
    INA219_Dev.SetCalibration_16V_400mA();
    
    while(1)
    {
       INA219_Dev.Run(&Power);
       LL_mDelay(300);
    }
#endif
}