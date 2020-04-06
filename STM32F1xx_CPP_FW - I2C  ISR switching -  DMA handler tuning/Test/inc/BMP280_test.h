

#include"I2c.h"
#include"BMP280.h"

//using namespace BSP;
using namespace HAL;
#include "System.h"


#if 1
void BMP280_Test()
{
   
    //static I2c BME280_I2CDevIntr(HAL::Gpio::B6, HAL::Gpio::B7, 100000U);
    static BMP280 BMP280_Dev(&i2c1,0XEC);
    volatile static float t,p,h,a;
    BMP280_Dev.HwInit();
    
//    /* Default settings from datasheet. */
//    BMP280_Dev.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
//                    BMP280::SAMPLING_X2,     /* Temp. oversampling */
//                    BMP280::SAMPLING_X16,    /* Pressure oversampling */
//                    BMP280::FILTER_X16,      /* Filtering. */
//                    BMP280::STANDBY_MS_500); /* Standby time. */

    /* Default settings from datasheet. */
    BMP280_Dev.setSampling();
    
   // while(1)
    {
        
        
        t = BMP280_Dev.readTemperature();
        p = BMP280_Dev.readPressure();
        h = BMP280_Dev.readHumidity();        
        a = BMP280_Dev.readAltitude(1013.25);
        printf("\nTemperature = %f\n",t);
        printf("Pressure    = %f\n",p);
        if(BMP280_Dev.getChipID() == 0x60)
        printf("Humidity    = %f\n",h);
        printf("Altitude    = %f\n\n",a);
        //LL_mDelay(3000);
       
    }
}
#endif