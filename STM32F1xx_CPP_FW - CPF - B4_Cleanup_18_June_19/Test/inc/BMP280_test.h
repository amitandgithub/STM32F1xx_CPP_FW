

#include"I2CPoll.h"
#include"I2CIntr.h"
#include"BMP280.h"

using namespace BSP;
using namespace HAL;



#if 1
void BMP280_Test()
{
#if 0
    static I2CPoll BME280_I2CDevPoll(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
    static BMP280 BMP280_Dev(&BME280_I2CDevPoll,0XEC);
#else    
    static I2CIntr BME280_I2CDevIntr(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
    static BMP280 BMP280_Dev(&BME280_I2CDevIntr,0XEC);
#endif
    volatile static float t,p,a;
    BMP280_Dev.HwInit();
    
    /* Default settings from datasheet. */
    BMP280_Dev.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                    BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    BMP280::FILTER_X16,      /* Filtering. */
                    BMP280::STANDBY_MS_500); /* Standby time. */
    
    while(1)
    {
        
        
        t = BMP280_Dev.readTemperature();
        p = BMP280_Dev.readPressure();
        a = BMP280_Dev.readAltitude(1013.25);
        LL_mDelay(2000);
       
    }
}
#endif