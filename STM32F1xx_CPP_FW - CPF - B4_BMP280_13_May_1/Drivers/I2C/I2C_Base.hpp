/******************
** CLASS: I2C_Base
**
** DESCRIPTION:
**  I2C common functions
**
** CREATED: 8/18/2018, by Amit Chaudhary
**
** FILE: I2C_Base.hpp
**
******************/
#ifndef I2C_Base_hpp
#define I2C_Base_hpp

#include "Peripheral.hpp"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"

namespace Peripherals
{
    
class I2C_Base : public Peripheral
{
public:
    typedef enum
    {
		I2C1_SCL_B6_SDA_B7      = 0,
		I2C1_SCL_B8_SDA_B9      = 1,
		I2C2_SCL_B11_SDA_B12    = 2,       
    }I2CPort_t;
    
    typedef enum
    {
		BAUD_100K      = 0,
		BAUD_400K      = 1,
		BAUD_1M        = 2,       
    }I2CBaudRate_t;
  
    I2C_Base(){;}
  
  virtual ~I2C_Base(){;}
  
  virtual      Status_t     HwInit          ();
  
  virtual      Status_t     HwDeInit        (){return 1;}
  
  virtual      Status_t     HwConfig        (void* pConfigStruct);
  
  virtual      Status_t     Abort           () = 0;
  
  virtual      Status_t     Reset           () = 0;
  
  virtual      Status_t     GetState        () = 0;
  
  virtual      Status_t     GetErrorStatus  () = 0;
  
};


}


#endif //I2C_Base_hpp
