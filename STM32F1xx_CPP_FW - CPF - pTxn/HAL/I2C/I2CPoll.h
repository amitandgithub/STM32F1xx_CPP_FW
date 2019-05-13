/******************
** CLASS: I2CPoll
**
** DESCRIPTION:
**  Implements the I2C in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2CPoll.h
**
******************/
#ifndef I2CPoll_h
#define I2CPoll_h

#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"stm32f1xx_ll_i2c.h"

namespace HAL
{
#define I2C_DEBUG 
    
    class I2CPoll
    {
    public:
        using Pin_t = HAL::Gpio::Pin_t;
        typedef uint32_t Hz_t ;
        //typedef uint32_t I2CStatus_t;
        typedef I2C_TypeDef* I2Cx_t;
        
        typedef enum
        {
            I2C_OK,
            I2C_BUSY_TIMEOUT,
            I2C_START_TIMEOUT,
            I2C_ADDR_TIMEOUT,
            I2C_BTF_TIMEOUT,
            I2C_RXNE_TIMEOUT,
            I2C_TXE_TIMEOUT,
            I2C_STOP_TIMEOUT,
            I2C_ACK_FAIL,
            I2C_NACK_WHILE_WAIT_FOR_STOP,
            I2C_STOPF_WHILE_WAIT_FOR_RXNE,
            I2C_STOPF_WHILE_WAIT_FOR_TXE,
            I2C_NACK_WHILE_WAIT_FOR_ADDR,
            I2C_NACK_WHILE_WAIT_FOR_BTF,
            I2C_INVALID_PARAMS,
            I2C_BUS_ERROR,
        }I2CStatus_t;
        
        static const uint32_t I2C_TIMEOUT       = 50000U;
        static const uint16_t I2C_DIR_WRITE     = 0xfffeU;
        static const uint16_t I2C_DIR_READ      = 0x0001U;
        static const uint16_t I2C_SLAVE_ADDRESS = 0x0c;
        
        I2CPoll(Pin_t scl, Pin_t sda, Hz_t Hz = 400000U);
        
        ~I2CPoll(){};
        
        I2CStatus_t HwInit(void *pInitStruct = nullptr);

        I2CStatus_t HwDeinit();
        
        inline void ClockEnable();
        
        inline void ClockDisable();
        
        bool DetectSlave(uint8_t SlaveAddress);
            
        void ScanBus(uint8_t* pFoundDevices, uint8_t size);
        
        inline void Start();
        
        inline void Stop();
        
        inline void SendAddress(uint8_t SlaveAddress);
        
        inline void WriteDataRegister(uint8_t data);
        
        inline uint8_t ReadDataRegister();
        
        I2CStatus_t MasterTx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t MasterRx(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t SlaveTx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t SlaveRx(uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        inline bool Busy(uint32_t timeout);
        
        inline bool StartConditionGenerated(uint32_t timeout);
        
        inline bool SlaveAddressSent(uint32_t timeout);
        
        inline bool TransmitterEmpty(uint32_t timeout);
        
        inline bool TransferDone(uint32_t timeout);
        
        inline void SoftReset();
        
        inline bool ACKFail(uint32_t timeout);
        
        inline bool DataAvailableRXNE(uint32_t timeout);   
        
        inline bool StopCondition(uint32_t timeout);
        
        I2CStatus_t AddressMatch();
        
    private:
        GpioOutput      _sclPin;
        GpioOutput      _sdaPin;
        Hz_t            _hz;
        I2Cx_t          _I2Cx;
        I2CStatus_t     _I2CStatus;
        
    };
    
    void I2CPoll::Start()
    {
        LL_I2C_GenerateStartCondition(_I2Cx);
    }
    
    void I2CPoll::Stop()
    {
        LL_I2C_GenerateStopCondition(_I2Cx);
    }
    
    void I2CPoll::SendAddress(uint8_t SlaveAddress)
    {
        
    }
    
    void I2CPoll::WriteDataRegister(uint8_t data)
    {
        LL_I2C_TransmitData8(_I2Cx, data);
    }
    
    uint8_t I2CPoll::ReadDataRegister()
    {
        return LL_I2C_ReceiveData8(_I2Cx);
    }
    
    bool I2CPoll::Busy(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BUSY(_I2Cx);
    }
    
    bool I2CPoll::StartConditionGenerated(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_SB(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_SB(_I2Cx);
    }
    
    bool I2CPoll::SlaveAddressSent(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(_I2Cx) == 0U) )
        {
            /* Check if a NACK is detected */
            if(LL_I2C_IsActiveFlag_AF(_I2Cx) == 1) 
            {
                /* Generate Stop */
                _I2Cx->CR1 |= I2C_CR1_STOP;
      
                /* Clear NACK Flag */
                LL_I2C_ClearFlag_AF(_I2Cx);
                
                return false;
            }
        }    
        return (bool)LL_I2C_IsActiveFlag_ADDR(_I2Cx);
    } 
    bool I2CPoll::TransmitterEmpty(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(_I2Cx) == 0U) )
        {
            /* Check if a NACK is detected */
            if(LL_I2C_IsActiveFlag_AF(_I2Cx) == 1) 
            {
                /* Clear STOP Flag */
                LL_I2C_ClearFlag_AF(_I2Cx);
                return false;
            }
        }
        return (bool)LL_I2C_IsActiveFlag_TXE(_I2Cx);
    }
    
    bool I2CPoll::TransferDone(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(_I2Cx) == 0U) )
        {
            /* Check if a NACK is detected */
            if(LL_I2C_IsActiveFlag_AF(_I2Cx) == 1) 
            {
                /* Clear NACK Flag */
                LL_I2C_ClearFlag_AF(_I2Cx);
                return false;
            }
        }
        return (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx);
    }
    void I2CPoll::SoftReset()
    {
            LL_I2C_EnableReset(_I2Cx);
            LL_I2C_DisableReset(_I2Cx);
            HwInit();
    }
    bool I2CPoll::ACKFail(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_AF(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_AF(_I2Cx);
    }
    
    bool I2CPoll::DataAvailableRXNE(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(_I2Cx) == 0U) )
        {
            /* Check if a STOPF is detected */
            if(LL_I2C_IsActiveFlag_STOP(_I2Cx) == 1)
            {
                /* Clear STOP Flag */
                LL_I2C_ClearFlag_STOP(_I2Cx);
                return false;
            }
        }
        return (bool)LL_I2C_IsActiveFlag_RXNE(_I2Cx);
    }
    
     bool I2CPoll::StopCondition(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_STOP(_I2Cx) == 0U) )
        {
             /* Check if a NACK is detected */
            if(LL_I2C_IsActiveFlag_AF(_I2Cx) == 1) 
            {
                /* Clear NACK Flag */
                LL_I2C_ClearFlag_AF(_I2Cx);
                return false;
            }
        }
        return (bool)LL_I2C_IsActiveFlag_STOP(_I2Cx);
    }
    

    
#if defined (I2C_DEBUG)
#define I2C_DEBUG_LOG(log) (_I2CStatus = (log))  // 108 bytes of ROM
#else 
#define I2C_DEBUG_LOG(log)
#endif
    
}
#endif //I2CPoll_h
