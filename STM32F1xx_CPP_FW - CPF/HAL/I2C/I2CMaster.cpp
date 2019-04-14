/******************
** FILE: I2CMaster.cpp
**
** DESCRIPTION:
**  I2CMaster implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2CMaster.h"


namespace HAL
{
    
    I2CMaster::I2CMaster(Pin_t scl, Pin_t sda, Hz_t Hz) :
        _sclPin(scl,GpioOutput::AF_OPEN_DRAIN),
        _sdaPin(sda,GpioOutput::AF_OPEN_DRAIN),
        _hz(Hz)
    {
        if((scl == Gpio::B6) && (sda == Gpio::B7))
        {
            _I2Cx = I2C1;
        }
        else if((scl == Gpio::B10) && (sda == Gpio::B11))
        {
             _I2Cx = I2C2;
        }
        else if((scl == Gpio::B8) && (sda == Gpio::B9))
        {
            // initialize with nullptr to configure it later in HwInit() with pin remap
             _I2Cx = nullptr;
        }
        else
        {
            while(1); // Fatal Error
        }
        
    }
    
    I2CMaster::I2CStatus_t I2CMaster::HwInit(void *pInitStruct)
    {
        LL_I2C_InitTypeDef I2C_InitStruct;
        
        /* Set I2C_InitStruct fields to default values */
        I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
        I2C_InitStruct.ClockSpeed      = _hz;
        I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
        I2C_InitStruct.OwnAddress1     = 0U;
        I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
        I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
  
        _sclPin.HwInit();
        _sdaPin.HwInit();
        
        _sclPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
        _sdaPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
        
        if(_I2Cx == nullptr)
        {
            // Remap B8 and B9 Pin in I2C mode 
        }
        
        ClockEnable();
        
        if(pInitStruct == nullptr)
        {
            LL_I2C_Init(_I2Cx, &I2C_InitStruct);
        }
        else
        {
            LL_I2C_Init(_I2Cx, (LL_I2C_InitTypeDef*)pInitStruct);
        }        
        
        return 0;
        
    }
    void I2CMaster::ClockEnable()
    {
        if(_I2Cx == I2C1)
        {
            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
        }
        else if(_I2Cx == I2C1)
        {
            LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
        }        
    }
    
    void I2CMaster::ClockDisable()
    {
        if(_I2Cx == I2C1)
        {
            LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
        }
        else if(_I2Cx == I2C1)
        {
            LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
        }        
    }
    
    I2CMaster::I2CStatus_t I2CMaster:: Send(uint8_t SlaveAddress,uint8_t* pdata, uint8_t len)
    {
        uint32_t XferSize = len;
        
        if( (len == 0) || (pdata == nullptr) )
            return 1;
        
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true ) 
            return 1;
        
        /* Send Slave Address */
        
         /* Generate Start */
        Start();
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
            return 1;
        
        /* Send slave address */
        WriteDataRegister(SlaveAddress);
        
        /* Wait until ADDR flag is set */
         if(SlaveAddressSent(I2C_TIMEOUT) == false)
            return 1;
         
        /* Clear ADDR flag */
         LL_I2C_ClearFlag_ADDR(_I2Cx);
         
        /* Wait until TXE flag is set */
         if(TransmitterEmpty(I2C_TIMEOUT) == false)
             return 1;
         
         while(XferSize > 0U)
         {
             /* Wait until TXE flag is set */
             if(TransmitterEmpty(I2C_TIMEOUT) == false)
             {
                 if(TransferDone(I2C_TIMEOUT) == false)
                 {
                     
                 }
                 
                 return 1;
             }
                 
             
             /* Write data to DR */
             WriteDataRegister(*pdata++);
             
             /* Wait until BTF flag is set */
             if(TransferDone(I2C_TIMEOUT) == false)
             {
                 if(LL_I2C_IsActiveFlag_AF(_I2Cx))
                 {
                      /* Generate Stop */
                      Stop();
                 }
             }
                 return 1;
             
             /* Write data to DR */
             WriteDataRegister(*pdata++);
             
             XferSize--;
             
         }
         
         
         /* Generate Stop */
         Stop();
         
        return 0;
    }
    
    I2CMaster::I2CStatus_t I2CMaster::ReadByte(uint8_t SlaveAddress,uint8_t* pdata)
    {
        return 0;
    }
    bool I2CMaster::DetectSlave(uint8_t SlaveAddress)
    {
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true ) 
            return 1;
        
        /* Generate Start */
        Start();
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
            return 1;
        
        /* Send slave address */
        WriteDataRegister(SlaveAddress);
        
        /* Wait until ADDR flag is set */
        if(SlaveAddressSent(I2C_TIMEOUT) == false)
        {
           // LL_I2C_ClearFlag_AF(_I2Cx);
            //LL_I2C_GenerateStopCondition(_I2Cx);
            return 1;
        }
        
         return 0;
    }
    
     void I2CMaster::ScanBus(uint8_t* pFoundDevices, uint8_t size)
     {
         uint8_t slave, i=0;
         
         for(slave = 0; slave < 255; slave++)
         {
             if(DetectSlave(slave) == 0)
             {
                 pFoundDevices[i++ % size] = slave;
             }
             else
             {
                 LL_I2C_ClearFlag_AF(_I2Cx);
                 LL_I2C_GenerateStopCondition(_I2Cx);
             }
         }
        
     }
    
    
    
    
    
}