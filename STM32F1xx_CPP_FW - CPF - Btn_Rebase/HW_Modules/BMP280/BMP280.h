/*!
*  @file Adafruit_BMP280.h
*
*  This is a library for the Adafruit BMP280 Breakout.
*
*  Designed specifically to work with the Adafruit BMP280 Breakout.
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
#ifndef __BMP280_H__
#define __BMP280_H__

#include <math.h>       /* pow */
#include"I2c.h"

/*!
*  I2C ADDRESS/BITS/SETTINGS
*/
#define BMP280_ADDRESS (0x77)     /**< The default I2C address for the sensor. */
#define BMP280_ADDRESS_ALT (0x76)  /**< Alternative I2C address for the sensor. */
#define BMP280_CHIPID (0x58)      /**< Default chip ID. */

/*!
* Registers available on the sensor.
*/
enum {
    BMP280_REGISTER_DIG_T1 = 0x88,
    BMP280_REGISTER_DIG_T2 = 0x8A,
    BMP280_REGISTER_DIG_T3 = 0x8C,
    BMP280_REGISTER_DIG_P1 = 0x8E,
    BMP280_REGISTER_DIG_P2 = 0x90,
    BMP280_REGISTER_DIG_P3 = 0x92,
    BMP280_REGISTER_DIG_P4 = 0x94,
    BMP280_REGISTER_DIG_P5 = 0x96,
    BMP280_REGISTER_DIG_P6 = 0x98,
    BMP280_REGISTER_DIG_P7 = 0x9A,
    BMP280_REGISTER_DIG_P8 = 0x9C,
    BMP280_REGISTER_DIG_P9 = 0x9E,
    BMP280_REGISTER_CHIPID = 0xD0,
    BMP280_REGISTER_VERSION = 0xD1,
    BMP280_REGISTER_SOFTRESET = 0xE0,
    BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
    BMP280_REGISTER_CONTROL = 0xF4,
    BMP280_REGISTER_CONFIG = 0xF5,
    BMP280_REGISTER_PRESSUREDATA = 0xF7,
    BMP280_REGISTER_TEMPDATA = 0xFA,
    
    BME280_REGISTER_DIG_H1 = 0xA1,
    BME280_REGISTER_DIG_H2 = 0xE1,
    BME280_REGISTER_DIG_H3 = 0xE3,
    BME280_REGISTER_DIG_H4 = 0xE4,
    BME280_REGISTER_DIG_H5 = 0xE5,
    BME280_REGISTER_DIG_H6 = 0xE7,
    
    BME280_REGISTER_CHIPID = 0xD0,
    BME280_REGISTER_VERSION = 0xD1,
    BME280_REGISTER_SOFTRESET = 0xE0,
    
    BME280_REGISTER_CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0
    
    BME280_REGISTER_CONTROLHUMID = 0xF2,
    BME280_REGISTER_STATUS = 0XF3,
    BME280_REGISTER_CONTROL = 0xF4,
    BME280_REGISTER_CONFIG = 0xF5,
    BME280_REGISTER_PRESSUREDATA = 0xF7,
    BME280_REGISTER_TEMPDATA = 0xFA,
    BME280_REGISTER_HUMIDDATA = 0xFD
};

/*!
*  Struct to hold calibration data.
*/
typedef struct {
    uint16_t dig_T1; /**< dig_T1 cal register. */
    int16_t dig_T2;  /**<  dig_T2 cal register. */
    int16_t dig_T3;  /**< dig_T3 cal register. */
    
    uint16_t dig_P1; /**< dig_P1 cal register. */
    int16_t dig_P2;  /**< dig_P2 cal register. */
    int16_t dig_P3;  /**< dig_P3 cal register. */
    int16_t dig_P4;  /**< dig_P4 cal register. */
    int16_t dig_P5;  /**< dig_P5 cal register. */
    int16_t dig_P6;  /**< dig_P6 cal register. */
    int16_t dig_P7;  /**< dig_P7 cal register. */
    int16_t dig_P8;  /**< dig_P8 cal register. */
    int16_t dig_P9;  /**< dig_P9 cal register. */
    
    uint8_t dig_H1; /**< dig_H1 cal register. */
    int16_t dig_H2; /**< dig_H2 cal register. */
    uint8_t dig_H3; /**< dig_H3 cal register. */
    int16_t dig_H4; /**< dig_H4 cal register. */
    int16_t dig_H5; /**< dig_H5 cal register. */
    int8_t dig_H6;  /**< dig_H6 cal register. */
} bmp280_calib_data;

/**
* Driver for the Adafruit BMP280 barometric pressure sensor.
*/
class BMP280 
{
public:
    
    using I2CDev = HAL::I2c*;
    typedef uint8_t byte;
    /** Oversampling rate for the sensor. */
    enum sensor_sampling {
        /** No over-sampling. */
        SAMPLING_NONE = 0x00,
        /** 1x over-sampling. */
        SAMPLING_X1 = 0x01,
        /** 2x over-sampling. */
        SAMPLING_X2 = 0x02,
        /** 4x over-sampling. */
        SAMPLING_X4 = 0x03,
        /** 8x over-sampling. */
        SAMPLING_X8 = 0x04,
        /** 16x over-sampling. */
        SAMPLING_X16 = 0x05
    };
    
    /** Operating mode for the sensor. */
    enum sensor_mode {
        /** Sleep mode. */
        MODE_SLEEP = 0x00,
        /** Forced mode. */
        MODE_FORCED = 0x01,
        /** Normal mode. */
        MODE_NORMAL = 0x03,
        /** Software reset. */
        MODE_SOFT_RESET_CODE = 0xB6
    };
    
    /** Filtering level for sensor data. */
    enum sensor_filter {
        /** No filtering. */
        FILTER_OFF = 0x00,
        /** 2x filtering. */
        FILTER_X2 = 0x01,
        /** 4x filtering. */
        FILTER_X4 = 0x02,
        /** 8x filtering. */
        FILTER_X8 = 0x03,
        /** 16x filtering. */
        FILTER_X16 = 0x04
    };
    
    /** Standby duration in ms */
//    enum standby_duration {
//        /** 1 ms standby. */
//        STANDBY_MS_1 = 0x00,
//        /** 63 ms standby. */
//        STANDBY_MS_63 = 0x01,
//        /** 125 ms standby. */
//        STANDBY_MS_125 = 0x02,
//        /** 250 ms standby. */
//        STANDBY_MS_250 = 0x03,
//        /** 500 ms standby. */
//        STANDBY_MS_500 = 0x04,
//        /** 1000 ms standby. */
//        STANDBY_MS_1000 = 0x05,
//        /** 2000 ms standby. */
//        STANDBY_MS_2000 = 0x06,
//        /** 4000 ms standby. */
//        STANDBY_MS_4000 = 0x07
//    };
    
      enum standby_duration {
    STANDBY_MS_0_5 = 0b000,
    STANDBY_MS_10 = 0b110,
    STANDBY_MS_20 = 0b111,
    STANDBY_MS_62_5 = 0b001,
    STANDBY_MS_125 = 0b010,
    STANDBY_MS_250 = 0b011,
    STANDBY_MS_500 = 0b100,
    STANDBY_MS_1000 = 0b101
  };
    BMP280(I2CDev pI2CDrv,uint8_t BMP280_Address);
    
    ~BMP280(){};
    
    uint8_t Tx(uint8_t* TxBuf, uint32_t TxLen)
    { 
        uint8_t status;
        status = m_pI2CDrv->XferPoll( m_BMP280_Address, TxBuf, TxLen);
        return status;
    }
    
    uint8_t Rx(uint8_t* RxBuf, uint32_t RxLen)
    {
        uint8_t status;
        status = m_pI2CDrv->XferPoll( m_BMP280_Address,0,0, RxBuf, RxLen);
        return status;
    }
    
    uint8_t HwInit();
     
    //bool begin(uint8_t addr = BMP280_ADDRESS, uint8_t chipid = BMP280_CHIPID);
    
    uint8_t getChipID(){return ChipID;}
    
    float readTemperature();
    
    float readPressure(void);
    
    float readHumidity(void);
    
    float readAltitude(float seaLevelhPa = 1013.25);
    
    void takeForcedMeasurement();
    
//    void setSampling(sensor_mode mode = MODE_NORMAL,
//                     sensor_sampling tempSampling = SAMPLING_X16,
//                     sensor_sampling pressSampling = SAMPLING_X16,
//                     sensor_filter filter = FILTER_OFF,
//                     standby_duration duration = STANDBY_MS_1);
    
    void setSampling(sensor_mode mode = MODE_NORMAL,
                   sensor_sampling tempSampling = SAMPLING_X16,
                   sensor_sampling pressSampling = SAMPLING_X16,
                   sensor_sampling humSampling = SAMPLING_X16,
                   sensor_filter filter = FILTER_OFF,
                   standby_duration duration = STANDBY_MS_0_5);
    
    
private:
    /** Encapsulates the config register */
    struct config {
        /** Inactive duration (standby time) in normal mode */
        uint8_t t_sb : 3;
        /** Filter settings */
        uint8_t filter : 3;
        /** Unused - don't set */
        uint8_t none : 1;
        /** Enables 3-wire SPI */
        uint8_t spi3w_en : 1;
        /** Used to retrieve the assembled config register's byte value. */
       uint8_t get() { return (t_sb << 5) | (filter << 2) | spi3w_en; }
    };
    
    /** Encapsulates trhe ctrl_meas register */
    struct ctrl_meas {
        /** Temperature oversampling. */
        uint8_t osrs_t : 3;
        /** Pressure oversampling. */
        uint8_t osrs_p : 3;
        /** Device mode */
        uint8_t mode : 2;
        /** Used to retrieve the assembled ctrl_meas register's byte value. */
        uint8_t get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
    };
    
      struct ctrl_hum {
    /// unused - don't set
    unsigned int none : 5;

    // pressure oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_h : 3; ///< pressure oversampling

    /// @return combined ctrl hum register
    unsigned int get() { return (osrs_h); }
  };
    
    void readCoefficients(void);
    uint8_t spixfer(uint8_t x);
    void write8(byte reg, byte value);
    uint8_t read8(byte reg);
    uint16_t read16(byte reg);
    uint32_t read24(byte reg);
    int16_t readS16(byte reg);
    uint16_t read16_LE(byte reg);
    int16_t readS16_LE(byte reg);

    int32_t             t_fine;
    bmp280_calib_data   _bmp280_calib;
    config              _configReg;
    ctrl_meas           _measReg;    
    ctrl_hum            _humReg;
    I2CDev              m_pI2CDrv;
    int8_t              m_BMP280_Address;
    uint8_t             buf[5];
    uint8_t             ChipID;
};

#endif
