

#include"I2CMaster.h"
#include"INA219.h"

using namespace BSP;
using namespace HAL;

static I2CMaster I2CDev(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
static INA219 INA219_Dev(&I2CDev,0x80U);
static INA219::Power_t Power;
static float Curr;
void INA219_Test()
{
    int i = sizeof(INA219); // 20 
    INA219_Dev.HwInit();
    INA219_Dev.SetCalibration_16V_400mA();
    
    while(1)
    {
       INA219_Dev.Run(&Power);
       LL_mDelay(300);
    }
}