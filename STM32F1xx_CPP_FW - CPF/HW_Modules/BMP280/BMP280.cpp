/*!
*  @file BMP280.cpp
*
*  This is a library for the BMP280 orientation sensor
*
*  Designed specifically to work with the Adafruit BMP280 Sensor.
*
*  Pick one up today in the adafruit shop!
*  ------> https://www.adafruit.com/product/2651
*
*  These sensors use I2C to communicate, 2 pins are required to interface.
*
*  Adafruit invests time and resources providing this open source code,
*  please support Adafruit andopen-source hardware by purchasing products
*  from Adafruit!
*
*  K.Townsend (Adafruit Industries)
*
*  BSD license, all text above must be included in any redistribution
*/

#include "BMP280.h"

#define BMP280_CHIP_ID1                      UINT8_C(0x56)
#define BMP280_CHIP_ID2                      UINT8_C(0x57)
#define BMP280_CHIP_ID3                      UINT8_C(0x58)


BMP280::BMP280(I2CDev pI2CDrv,uint8_t BMP280_Address): m_pI2CDrv(pI2CDrv), m_BMP280_Address(BMP280_Address)
{
    
}

uint8_t BMP280::HwInit()
{
    
    
    m_pI2CDrv->HwInit();
    
    ChipID = read8(BMP280_REGISTER_CHIPID);
    
    printf("BMP/BME ChipID : 0x%x",ChipID);

        
        
    readCoefficients();
   // write8(BMP280_REGISTER_CONTROL, 0x3F); /* needed? */
    setSampling();
    
    return 1;
}



/*!
* Sets the sampling config for the device.
* @param mode
*        The operating mode of the sensor.
* @param tempSampling
*        The sampling scheme for temp readings.
* @param pressSampling
*        The sampling scheme for pressure readings.
* @param filter
*        The filtering mode to apply (if any).
* @param duration
*        The sampling duration.
*/
//void BMP280::setSampling(sensor_mode mode,
//                         sensor_sampling tempSampling,
//                         sensor_sampling pressSampling,
//                         sensor_filter filter,
//                         standby_duration duration) {
//                             _measReg.mode = mode;
//                             _measReg.osrs_t = tempSampling;
//                             _measReg.osrs_p = pressSampling;
//                             
//                             _configReg.filter = filter;
//                             _configReg.t_sb = duration;
//                             
//                             write8(BMP280_REGISTER_CONFIG, _configReg.get());
//                             write8(BMP280_REGISTER_CONTROL, _measReg.get());
//                         }
                         
                         
                         void BMP280::setSampling(sensor_mode mode,
                                                           sensor_sampling tempSampling,
                                                           sensor_sampling pressSampling,
                                                           sensor_sampling humSampling,
                                                           sensor_filter filter,
                                                           standby_duration duration) 
                         {
                           _measReg.mode = mode;
                           _measReg.osrs_t = tempSampling;
                           _measReg.osrs_p = pressSampling;
                           
                           _humReg.osrs_h = humSampling;
                           _configReg.filter = filter;
                           _configReg.t_sb = duration;
                           
                           // you must make sure to also set REGISTER_CONTROL after setting the
                           // CONTROLHUMID register, otherwise the values won't be applied (see
                           // DS 5.4.3)
                           write8(BME280_REGISTER_CONTROLHUMID, _humReg.get());
                           write8(BME280_REGISTER_CONFIG, _configReg.get());
                           write8(BME280_REGISTER_CONTROL, _measReg.get());
                        }
  
                         /**************************************************************************/
                         /*!
                         @brief  Writes an 8 bit value over I2C/SPI
                         */
                         /**************************************************************************/
                         void BMP280::write8(byte reg, byte value) 
                         {
                             buf[0] = reg;
                             buf[1] = value;
                             Tx(buf, 2, &I2C_Status);
                         }
                         
                         /*!
                         *  @brief  Reads an 8 bit value over I2C/SPI
                         *  @param  reg
                         *          selected register
                         *  @return value from selected register
                         */
                         uint8_t BMP280::read8(byte reg) 
                         {
                             buf[0] = reg;
                             Tx(buf, 1, &I2C_Status);
                             Rx(&buf[1], 1, &I2C_Status);                             
                             return buf[1];
                         }
                         
                         /*!
                         *  @brief  Reads a 16 bit value over I2C/SPI
                         */
                         uint16_t BMP280::read16(byte reg) 
                         {
                             uint16_t value;
                             buf[0] = reg;
                             Tx(buf, 1, &I2C_Status) ;
                             Rx(&buf[1], 2, &I2C_Status) ;    
                             value = (uint16_t)((buf[1] << 8) | buf[2]);
                             return value;
                         }
                         
                         uint16_t BMP280::read16_LE(byte reg) {
                             uint16_t temp = read16(reg);
                             return (temp >> 8) | (temp << 8);
                         }
                         
                         /*!
                         *   @brief  Reads a signed 16 bit value over I2C/SPI
                         */
                         int16_t BMP280::readS16(byte reg) 
                         { 
                             return (int16_t)read16(reg); 
                         }
                         
                         int16_t BMP280::readS16_LE(byte reg) 
                         {
                             return (int16_t)read16_LE(reg);
                         }
                         
                         /*!
                         *  @brief  Reads a 24 bit value over I2C/SPI
                         */
                         uint32_t BMP280::read24(byte reg) 
                         {
                             uint32_t value;
                             buf[0] = reg;
                             Tx(buf, 1, &I2C_Status) ;
                             Rx(&buf[1], 3, &I2C_Status) ;                             
                             value = buf[1];
                             value <<= 8;
                             value |= buf[2];
                             value <<= 8;
                             value |= buf[3];                             
                             return value;
                         }
                         
                         /*!
                         *  @brief  Reads the factory-set coefficients
                         */
                         void BMP280::readCoefficients() 
                         {
                             _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
                             _bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
                             _bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);
                             _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
                             _bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
                             _bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
                             _bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
                             _bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
                             _bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
                             _bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
                             _bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
                             _bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
                             
                              _bmp280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
                              _bmp280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
                              _bmp280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
                              _bmp280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) |
                                                     (read8(BME280_REGISTER_DIG_H4 + 1) & 0xF);
                              _bmp280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5 + 1) << 4) |
                                                     (read8(BME280_REGISTER_DIG_H5) >> 4);
                              _bmp280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
                             
//                             printf("_bmp280_calib.dig_T1 = %d \n",_bmp280_calib.dig_T1);
//                             printf("_bmp280_calib.dig_T2 = %d \n",_bmp280_calib.dig_T2);
//                             printf("_bmp280_calib.dig_T3 = %d \n",_bmp280_calib.dig_T3);
//                             
//                             printf("_bmp280_calib.dig_T1 = %x \n",_bmp280_calib.dig_T1);
//                             printf("_bmp280_calib.dig_T2 = %x \n",_bmp280_calib.dig_T2);
//                             printf("_bmp280_calib.dig_T3 = %x \n",_bmp280_calib.dig_T3);
                             
                         }
                         
                         /*!
                         * Reads the temperature from the device.
                         * @return The temperature in degress celcius.
                         */
                         float BMP280::readTemperature() 
                         {
                             int32_t var1, var2;
                             
                             int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);

                             adc_T >>= 4;
                             
                             var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *
                                     ((int32_t)_bmp280_calib.dig_T2)) >> 11;
                             
                             var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *
                                       ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
                                     ((int32_t)_bmp280_calib.dig_T3)) >> 14;
                             
                             t_fine = var1 + var2;
                             
                             float T = (t_fine * 5 + 128) >> 8;
                             return T / 100.0;
                         }
                         
                         /*!
                         * Reads the barometric pressure from the device.
                         * @return Barometric pressure in hPa.
                         */
                         float BMP280::readPressure() 
                         {
                             int64_t var1, var2, p;
                             
                             // Must be done first to get the t_fine variable set up
                             readTemperature();
                             
                             int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
                             adc_P >>= 4;
                             
                             var1 = ((int64_t)t_fine) - 128000;
                             var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
                             var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
                             var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
                             var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
                                 ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
                             var1 =
                                 (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;
                             
                             if (var1 == 0) {
                                 return 0; // avoid exception caused by division by zero
                             }
                             p = 1048576 - adc_P;
                             p = (((p << 31) - var2) * 3125) / var1;
                             var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
                             var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;
                             
                             p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
                             return (float)p / 256;
                         }
                         
                         /*!
                         * @brief Calculates the approximate altitude using barometric pressure and the
                         * supplied sea level hPa as a reference.
                         * @param seaLevelhPa
                         *        The current hPa at sea level.
                         * @return The approximate altitude above sea level in meters.
                         */
                         float BMP280::readAltitude(float seaLevelhPa) 
                         {
                             float altitude;
                             
                             float pressure = readPressure(); // in Si units for Pascal
                             pressure /= 100;
                             
                             altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
                             
                             return altitude;
                         }

                         /*!
                         *  @brief  Returns the humidity from the sensor
                         *  @returns the humidity value read from the device
                         */
                         float BMP280::readHumidity(void) 
                         {
                             if(ChipID != 0x60)
                                 return 0.0;
                             
                             readTemperature(); // must be done first to get t_fine
                             
                             int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
                             if (adc_H == 0x8000) // value in case humidity measurement was disabled
                                 return NAN;
                             
                             int32_t v_x1_u32r;
                             
                             v_x1_u32r = (t_fine - ((int32_t)76800));
                             
                             v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bmp280_calib.dig_H4) << 20) -
                                             (((int32_t)_bmp280_calib.dig_H5) * v_x1_u32r)) +
                                            ((int32_t)16384)) >>
                                           15) *
                                          (((((((v_x1_u32r * ((int32_t)_bmp280_calib.dig_H6)) >> 10) *
                                               (((v_x1_u32r * ((int32_t)_bmp280_calib.dig_H3)) >> 11) +
                                                ((int32_t)32768))) >>
                                              10) +
                                             ((int32_t)2097152)) *
                                            ((int32_t)_bmp280_calib.dig_H2) +
                                                8192) >>
                                           14));
                             
                             v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                                                        ((int32_t)_bmp280_calib.dig_H1)) >>
                                                       4));
                             
                             v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
                             v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
                             float h = (v_x1_u32r >> 12);
                             return h / 1024.0;
                         }
                         