/******************
** FILE: I2CPoll.cpp
**
** DESCRIPTION:
**  I2CPoll class implementation
**
** CREATED: 6/19/2019, by Amit Chaudhary
******************/
#include "I2CPoll.h"

namespace HAL
{
    
    I2CPoll::I2CPoll(Pin_t scl, Pin_t sda, Hz_t Hz) : I2CBase(scl, sda, Hz)
    {
        
    }
    
    I2CPoll::I2CStatus_t I2CPoll::HwInit(void *pInitStruct)
    {            
        I2CBase::HwInit();
        return I2C_OK;            
    }
    
    I2CPoll::I2CStatus_t I2CPoll::TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen)
    {
        if(m_I2CState != READY)
            return I2C_BUSY;
        
        if( (TxLen == 0) || (TxBuf == nullptr) )
        {          
            I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
            m_I2CStatus = I2C_INVALID_PARAMS;
            return I2C_INVALID_PARAMS;                
        }          
        
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true )
        {
            I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;
        }
        
        /* Disable Pos */
        m_I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Generate Start */
        m_I2Cx->CR1 |= I2C_CR1_START;
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
        {         
            I2C_DEBUG_LOG(I2C_START_TIMEOUT);
            return I2C_START_TIMEOUT;                
        }            
        
        /* Send slave address */
        m_I2Cx->DR = SlaveAddress & I2C_DIR_WRITE;
        
        /* Wait until ADDR flag is set */
        if(SlaveAddressSent(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
            return I2C_ADDR_TIMEOUT;                
        }
        
        /* Clear ADDR flag */
        LL_I2C_ClearFlag_ADDR(m_I2Cx);
        
        while(TxLen > 0U)
        {
            /* Wait until TXE flag is set */
            if(TransmitterEmpty(I2C_TIMEOUT) == false)
            {       
                if( (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx) == true )
                {
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP; 
                    
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL; 
                }
                
                I2C_DEBUG_LOG(I2C_TXE_TIMEOUT);
                return I2C_TXE_TIMEOUT;
            }                 
            
            /* Write data to DR */
            m_I2Cx->DR = *TxBuf++;                
            TxLen--;     
            
            if( (bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx) == true)
            {
                /* Write data to DR */
                m_I2Cx->DR = *TxBuf++;                
                TxLen--;                    
            }
            
            /* Wait until BTF flag is set */
            if(TransferDone(I2C_TIMEOUT) == false)
            {
                if( (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx) == true )
                {
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;                 
                    I2C_DEBUG_LOG(I2C_ACK_FAIL);
                    return I2C_ACK_FAIL;
                }
                I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                return I2C_BTF_TIMEOUT;
            }                
        }                  
        
        /* Generate Stop */
        m_I2Cx->CR1 |= I2C_CR1_STOP;
        
        I2C_DEBUG_LOG(I2C_OK);
        return I2C_OK; 
    }
    
    I2CPoll::I2CStatus_t I2CPoll::RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen)
    {
        if(m_I2CState != READY)
            return I2C_BUSY;
        
        if( (RxLen == 0) || (RxBuf == nullptr) )
        {          
            I2C_DEBUG_LOG(I2C_INVALID_PARAMS);
            m_I2CStatus = I2C_INVALID_PARAMS;
            return I2C_INVALID_PARAMS;                
        }          
        
        /* Wait until BUSY flag is reset */
        if( Busy(I2C_TIMEOUT) == true )
        {
            I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
            return I2C_BUSY_TIMEOUT;
        }
        
        /* Disable Pos */
        m_I2Cx->CR1 &= ~I2C_CR1_POS;
        
        /* Generate Start */
        m_I2Cx->CR1 |= I2C_CR1_START;
        
        /* Check if Start condition generated successfully*/
        if(StartConditionGenerated(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_START_TIMEOUT);
            return I2C_START_TIMEOUT;                
        }
        
        /* Send slave address */
        m_I2Cx->DR = SlaveAddress | I2C_DIR_READ;
        
        /* Wait until ADDR flag is set */
        if(SlaveAddressSent(I2C_TIMEOUT) == false)
        {          
            I2C_DEBUG_LOG(I2C_ADDR_TIMEOUT);
            return I2C_ADDR_TIMEOUT;                
        }          
        
        if(RxLen == 1)
        {
            /* Disable Acknowledge */
            m_I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
            
            /* Generate Stop */
            m_I2Cx->CR1 |= I2C_CR1_STOP;
            
            /* Re-enable IRQs */
            __enable_irq();             
        }
        else if(RxLen == 2)
        {
            /* Enable Pos */
            m_I2Cx->CR1 |= I2C_CR1_POS;
            
            /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
            software sequence must complete before the current byte end of transfer */
            __disable_irq();
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
            
            /* Disable Acknowledge */
            m_I2Cx->CR1 &= ~I2C_CR1_ACK;
            
            /* Re-enable IRQs */
            __enable_irq(); 
        }
        else
        {
            /* Enable Acknowledge */
            m_I2Cx->CR1 |= I2C_CR1_ACK;
            
            /* Clear ADDR flag */
            LL_I2C_ClearFlag_ADDR(m_I2Cx);
        }
        
        while(RxLen > 0U)
        {
            if(RxLen <= 3U)
            {
                /* One byte */
                if(RxLen == 1U)
                {
                    /* Wait until RXNE flag is set */
                    if(DataAvailableRXNE(I2C_TIMEOUT) == false)      
                    {
                        I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                        return I2C_RXNE_TIMEOUT;
                    }
                    
                    /* Read data from DR */
                    (*RxBuf) = m_I2Cx->DR;
                    RxLen--;
                }
                /* Two bytes */
                else if(RxLen == 2U)
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
                    m_I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    /* Read data from DR */
                    (*RxBuf++)= m_I2Cx->DR;
                    RxLen--;
                    
                    /* Re-enable IRQs */
                    __enable_irq();
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
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
                    m_I2Cx->CR1 &= ~I2C_CR1_ACK;
                    
                    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
                    software sequence must complete before the current byte end of transfer */
                    __disable_irq();
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                    
                    /* Wait until BTF flag is set */
                    if(TransferDone(I2C_TIMEOUT) == false)
                    {         
                        I2C_DEBUG_LOG(I2C_BTF_TIMEOUT);
                        return I2C_BTF_TIMEOUT;                
                    }
                    
                    /* Generate Stop */
                    m_I2Cx->CR1 |= I2C_CR1_STOP;
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                    
                    /* Re-enable IRQs */
                    __enable_irq(); 
                    
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                }
            }
            else
            {
                /* Wait until RXNE flag is set */
                if(DataAvailableRXNE(I2C_TIMEOUT) == false)      
                {
                    I2C_DEBUG_LOG(I2C_RXNE_TIMEOUT);
                    return I2C_RXNE_TIMEOUT;
                }
                
                /* Read data from DR */
                (*RxBuf++) = m_I2Cx->DR;
                RxLen--;
                
                if((bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx) == true)
                {
                    /* Read data from DR */
                    (*RxBuf++) = m_I2Cx->DR;
                    RxLen--;
                }
            }
        }       
        
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
            m_I2Cx->CR1 |= I2C_CR1_START;
            
            /* Check if Start condition generated successfully*/
            if(StartConditionGenerated(I2C_TIMEOUT) == false)
            { 
                I2C_DEBUG_LOG(I2C_START_TIMEOUT);
                return I2C_START_TIMEOUT;                
            }
            
            /* Send slave address */
            m_I2Cx->DR = SlaveAddress;
            
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
                    LL_I2C_ClearFlag_AF(m_I2Cx);
                    LL_I2C_GenerateStopCondition(m_I2Cx);
                }
            }
        }
    
}