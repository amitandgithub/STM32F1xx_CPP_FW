/******************
** CLASS: I2CPoll
**
** DESCRIPTION:
**  I2C Basic features implementation
**
** CREATED: 6/19/2019, by Amit Chaudhary
**
** FILE: I2CPoll.h
**
******************/
#ifndef I2CBasic_h
#define I2CBasic_h

#include"I2CBase.h"

namespace HAL
{   
    
    class I2CPoll:public I2CBase
    {
    public:
        
        I2CPoll(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);        
        ~I2CPoll(){};     
        
        I2CStatus_t     HwInit(void *pInitStruct = nullptr);        
        I2CStatus_t     HwDeinit();               
        void            ScanBus(uint8_t* pFoundDevices, uint8_t size);               
        I2CStatus_t     TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen);        
        I2CStatus_t     RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen);        
        
    private:
        bool DetectSlave(uint8_t SlaveAddress);
        inline bool StartConditionGenerated(uint32_t timeout);        
        inline bool SlaveAddressSent(uint32_t timeout);        
        inline bool TransmitterEmpty(uint32_t timeout);        
        inline bool TransferDone(uint32_t timeout);       
        inline bool ACKFail(uint32_t timeout);        
        inline bool DataAvailableRXNE(uint32_t timeout);        
        inline bool StopCondition(uint32_t timeout);
        inline bool StopFlagCleared(uint32_t timeout);        
    };
    

    
    bool I2CPoll::StopFlagCleared(uint32_t timeout)
    {   
#if 1
        static int StopCountMax;
        uint32_t Timeout=0;
        
        while( (timeout--) && (m_I2Cx->CR1 & (I2C_CR1_STOP)) )
        {            
            Timeout++;          
        }
        
        if(StopCountMax<Timeout) 
            StopCountMax = Timeout;
#else
        while( (timeout--) && (m_I2Cx->CR1 & (I2C_CR1_STOP)) );        
#endif
        return (bool)(m_I2Cx->CR1 & (I2C_CR1_STOP));
    }
    
    bool I2CPoll::StartConditionGenerated(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_SB(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_SB(m_I2Cx);
    }
    
    bool I2CPoll::SlaveAddressSent(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_ADDR(m_I2Cx);
    } 
    bool I2CPoll::TransmitterEmpty(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_TXE(m_I2Cx);
    }
    
    bool I2CPoll::TransferDone(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx);
    }
    
    bool I2CPoll::ACKFail(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_AF(m_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx);
    }
    
    bool I2CPoll::DataAvailableRXNE(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_RXNE(m_I2Cx);
    }
    
    bool I2CPoll::StopCondition(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_STOP(m_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_STOP(m_I2Cx);
    }  
}
#endif //I2CBasic_h
