/******************
** FILE: I2CPoll.cpp
**
** DESCRIPTION:
**  I2CPoll implementation in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
******************/

#include "I2CPoll.h"


namespace HAL
{
    
    I2CPoll::I2CPoll(Pin_t scl, Pin_t sda, Hz_t Hz) :
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
            
            _I2CStatus = I2C_OK;
            
        }
        
        I2CPoll::I2CStatus_t I2CPoll::HwInit(void *pInitStruct)
        {
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            /* Set I2C_InitStruct fields to default values */
            I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
            I2C_InitStruct.ClockSpeed      = _hz;
            I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
            I2C_InitStruct.OwnAddress1     = 0U;
            I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
            I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
            
            LL_I2C_SetOwnAddress2(_I2Cx,I2C_SLAVE_ADDRESS);
            
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
            
            return I2C_OK;
            
        }
        void I2CPoll::ClockEnable()
        {
            if(_I2Cx == I2C1)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(_I2Cx == I2C2)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
        void I2CPoll::ClockDisable()
        {
            if(_I2Cx == I2C1)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(_I2Cx == I2C2)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        I2CPoll::I2CStatus_t I2CPoll:: MasterTx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered)
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {          
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true )
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Generate Start */
            Start();
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            {         
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }            
            
            /* Send slave address */
            WriteDataRegister(SlaveAddress & I2C_DIR_WRITE);
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            while(XferSize > 0U)
            {
                /* Wait until TXE flag is set */
                if(TransmitterEmpty(I2C_TIMEOUT) == false)
                {       
                    if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                    {
                        /* Generate Stop */
                        Stop(); 
                        
                        I2C_DEBUG_LOG(I2C_ACK_FAIL);
                        return I2C_ACK_FAIL; 
                    }
                    if(pbytesXfered) 
                        *pbytesXfered = Received_data;
                    
                    I2C_DEBUG_LOG(I2C_TXE_TIMEOUT);
                    return I2C_TXE_TIMEOUT;
                }                 
                
                /* Write data to DR */
                //WriteDataRegister(*pdata++);
                _I2Cx->DR = *pdata++;                
                XferSize--; 
                Received_data++;     
                
                if( (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                {
                    /* Write data to DR */
                    _I2Cx->DR = *pdata++;                
                    XferSize--; 
                    Received_data++;                     
                }
                
                /* Wait until BTF flag is set */
                if(TransferDone(I2C_TIMEOUT) == false)
                {
                    if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                    {
                        /* Generate Stop */
                        Stop();                 
                        I2C_DEBUG_LOG(I2C_ACK_FAIL);
                        return I2C_ACK_FAIL;
                    }
                    I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                    return I2C_BTF_TIMEOUT;
                }                
            }
            
            if(pbytesXfered) 
                *pbytesXfered = Received_data;       
            
            /* Generate Stop */
            Stop();
            
            I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
            return I2C_OK;
        }
        
        I2CPoll::I2CStatus_t I2CPoll:: MasterRx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            //LL_I2C_DisableBitPOS(_I2Cx);
            
            /* Generate Start */
            Start();
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            WriteDataRegister(SlaveAddress | I2C_DIR_READ);
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            if(XferSize == 1)
            {
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                //LL_I2C_AcknowledgeNextData(_I2Cx,LL_I2C_NACK);
                
                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_irq();
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
                
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
                
                /* Re-enable IRQs */
                __enable_irq();             
            }
            else if(XferSize == 2)
            {
                /* Enable Pos */
                _I2Cx->CR1 |= I2C_CR1_POS;
                
                /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                software sequence must complete before the current byte end of transfer */
                __disable_irq();
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
                
                /* Disable Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                /* Re-enable IRQs */
                __enable_irq(); 
            }
            else
            {
                /* Enable Acknowledge */
                _I2Cx->CR1 |= I2C_CR1_ACK;
                
                /* Clear ADDR flag */
                LL_I2C_ClearFlag_ADDR(_I2Cx);
            }
            
            while(XferSize > 0U)
            {
                if(XferSize <= 3U)
                {
                    /* One byte */
                    if(XferSize == 1U)
                    {
                        /* Wait until RXNE flag is set */
                        if(DataAvailableRXNE(I2C_TIMEOUT) == true)      
                        {
                            I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                            return I2C_RXNE_TIMEOUT;
                        }
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                    /* Two bytes */
                    else if(XferSize == 2U)
                    {
                        /* Wait until BTF flag is set */
                        if(TransferDone(I2C_TIMEOUT) == false)
                        {         
                            I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;                
                        }
                        
                        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                        software sequence must complete before the current byte end of transfer */
                        __disable_irq();
                        
                        /* Generate Stop */
                        _I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Re-enable IRQs */
                        __enable_irq();
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                    /* 3 Last bytes */
                    else
                    {
                        /* Wait until BTF flag is set */
                        if(TransferDone(I2C_TIMEOUT) == false)
                        {          
                            I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;                
                        }
                        
                        /* Disable Acknowledge */
                        _I2Cx->CR1 &= ~I2C_CR1_ACK;
                        
                        /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                        software sequence must complete before the current byte end of transfer */
                        __disable_irq();
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Wait until BTF flag is set */
                        if(TransferDone(I2C_TIMEOUT) == false)
                        {         
                            I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                            return I2C_BTF_TIMEOUT;                
                        }
                        
                        /* Generate Stop */
                        _I2Cx->CR1 |= I2C_CR1_STOP;
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                        
                        /* Re-enable IRQs */
                        __enable_irq(); 
                        
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                }
                else
                {
                    /* Wait until RXNE flag is set */
                    if(DataAvailableRXNE(I2C_TIMEOUT) == true)      
                    {
                        I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                        return I2C_RXNE_TIMEOUT;
                    }
                    
                    /* Read data from DR */
                    *pdata++ = _I2Cx->DR;
                    XferSize--;
                    Received_data++;
                    
                    if(TransferDone(I2C_TIMEOUT) == true)
                    {
                        /* Read data from DR */
                        *pdata++ = _I2Cx->DR;
                        XferSize--;
                        Received_data++;
                    }
                }
            }       
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        I2CPoll::I2CStatus_t I2CPoll::SlaveRx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            while(XferSize>0)
            {
                /* Wait until RXNE flag is set */
                if(DataAvailableRXNE(I2C_TIMEOUT) == true)      
                {
                    /* Disable Address Acknowledge */
                    _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Read data from DR */
                *pdata++ = _I2Cx->DR;
                XferSize--;
                Received_data++;
                
                if( (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                {
                    /* Write data to DR */
                    *pdata++ = _I2Cx->DR;            
                    XferSize--; 
                    Received_data++;                     
                }
            }
            
            /* Wait until STOP flag is set */
            if(StopCondition(I2C_TIMEOUT) == false)
            {
                /* Disable Address Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                {              
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_STOP_TIMEOUT);
                return I2C_STOP_TIMEOUT;
            }
            
            /* Clear STOP flag */
            LL_I2C_ClearFlag_STOP(_I2Cx);
            
            /* Disable Address Acknowledge */
            _I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        I2CPoll::I2CStatus_t I2CPoll::SlaveTx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered )
        {
            uint32_t XferSize, Received_data = 0;
            
            if( (len == 0) || (pdata == nullptr) )
            {       
                I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
                return I2C_INVALID_PARAMS;                
            }
            
            XferSize = len;
            
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {          
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Disable Pos */
            _I2Cx->CR1 &= ~I2C_CR1_POS;
            
            /* Enable Address Acknowledge */
            _I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {          
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }          
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(_I2Cx);
            
            while(XferSize>0)
            {
                /* Wait until TXE flag is set */
                if(TransmitterEmpty(I2C_TIMEOUT) == true)      
                {
                    /* Disable Address Acknowledge */
                    _I2Cx->CR1 &= ~I2C_CR1_ACK;
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Write data to DR */
                _I2Cx->DR = *pdata++;
                XferSize--;
                Received_data++;
                
                if( (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx) == true)
                {
                    /* Write data to DR */
                    _I2Cx->DR = *pdata++;            
                    XferSize--; 
                    Received_data++;                     
                }
            }
            
             /* Wait until AF flag is set */
            if(ACKFail(I2C_TIMEOUT) == false)
            {
                /* Disable Address Acknowledge */
                _I2Cx->CR1 &= ~I2C_CR1_ACK;
                
                if( (bool)LL_I2C_IsActiveFlag_AF(_I2Cx) == true )
                {              
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_STOP_TIMEOUT);
                return I2C_STOP_TIMEOUT;
            }
            
            /* Clear AF flag */
            LL_I2C_ClearFlag_AF(_I2Cx);
            
            /* Disable Address Acknowledge */
            _I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        bool I2CPoll::DetectSlave(uint8_t SlaveAddress)
        {
            /* Wait until BUSY flag is reset */
            if( Busy(I2C_TIMEOUT) == true ) 
            {
                I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
                return I2C_BUSY_TIMEOUT;                
            }
            
            /* Generate Start */
            Start();
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            { 
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            WriteDataRegister(SlaveAddress);
            
            /* Wait until ADDR flag is set */
            if(SlaveAddressSent(I2C_TIMEOUT) == false)
            {
                I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
                return I2C_ADDR_TIMEOUT;                
            }
            
            I2C_DEBUG_LOG(I2C_OK);
            return I2C_OK;
        }
        
        void I2CPoll::ScanBus(uint8_t* pFoundDevices, uint8_t size)
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