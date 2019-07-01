/******************
** CLASS: I2C
**
** DESCRIPTION:
**  I2C class Implementation
**
** CREATED: 8/14/2018, by Amit Chaudhary
**
** FILE: I2C_Drv.cpp
**
******************/
#ifndef I2C_Drv_hpp
#define I2C_Drv_hpp

#include "I2C_Base.hpp"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

//#define HAL_I2C_MODULE_ENABLED

namespace Peripherals
{
    
class I2C_Master  : public I2C_Base
{
public:
    
    I2C_Master(I2CPort_t I2CPort, I2CBaudRate_t I2CBaudRate = BAUD_100K);
  
    ~I2C_Master(){HwDeInit();}
  
    Status_t     HwInit          ();
  
    Status_t     HwDeInit        (){return HAL_I2C_DeInit(&m_hi2c);}
  
    Status_t     HwConfig        (void* pConfigStruct){return 1;}
  
    Status_t     Send            (uint16_t Addrs, uint8_t *pTxBuf, uint16_t Len);
  
    Status_t     Read            (uint16_t Addrs, uint8_t *pRxBuf, uint16_t Len);
  
    Status_t     Xfer            (uint16_t Addrs, uint8_t *pTxBuf, uint16_t TxLen,
                                               uint8_t *pRxBuf, uint16_t RxLen);
    Status_t     Abort           ();
  
    Status_t     Reset           ();
  
    Status_t     GetState        (){return HAL_I2C_GetState(&m_hi2c);}
  
    Status_t     GetErrorStatus  ();
  
    Status_t     Scan            (uint16_t *DevAddress, uint16_t len );
  
private:
    I2C_HandleTypeDef       m_hi2c;
    I2CPort_t               m_I2CPort;   
  
};


}
#endif //I2C_Drv_hpp
