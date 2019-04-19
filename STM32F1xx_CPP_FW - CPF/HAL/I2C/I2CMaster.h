/******************
** CLASS: I2CMaster
**
** DESCRIPTION:
**  Implements the I2C Master in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2CMaster.h
**
******************/
#ifndef I2CMaster_h
#define I2CMaster_h

#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"stm32f1xx_ll_i2c.h"

namespace HAL
{
#define I2C_DEBUG 
    
    class I2CMaster
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
            I2C_ACK_FAIL,
            I2C_INVALID_PARAMS,
            I2C_BUS_ERROR,
        }I2CStatus_t;
        
        static const uint32_t I2C_TIMEOUT       = 5000U;
        static const uint16_t I2C_DIR_WRITE     = 0xfff7U;
        static const uint16_t I2C_DIR_READ      = 0x0001U;
        
        I2CMaster(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
        
        ~I2CMaster(){};
        
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
        
        I2CStatus_t Send(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t Read(uint16_t SlaveAddress,uint8_t* pdata, uint32_t len, uint32_t* pbytesXfered = nullptr );
        
        I2CStatus_t ReadByte(uint8_t SlaveAddress,uint8_t* pdata);
        
        inline bool Busy(uint32_t timeout);
        
        inline bool StartConditionGenerated(uint32_t timeout);
        
        inline bool SlaveAddressSent(uint32_t timeout);
        
        inline bool TransmitterEmpty(uint32_t timeout);
        
        inline bool TransferDone(uint32_t timeout);
        
        inline void SoftReset(bool enable);
        
        inline bool ACKFail();
        
        inline bool DataAvailableRXNE(uint32_t timeout);
        
        
        
    private:
        GpioOutput      _sclPin;
        GpioOutput      _sdaPin;
        Hz_t            _hz;
        I2Cx_t          _I2Cx;
        I2CStatus_t     _I2CStatus;
        
    };
    
    void I2CMaster::Start()
    {
        LL_I2C_GenerateStartCondition(_I2Cx);
    }
    
    void I2CMaster::Stop()
    {
        LL_I2C_GenerateStopCondition(_I2Cx);
    }
    
    void I2CMaster::SendAddress(uint8_t SlaveAddress)
    {
        
    }
    
    void I2CMaster::WriteDataRegister(uint8_t data)
    {
        LL_I2C_TransmitData8(_I2Cx, data);
    }
    
    uint8_t I2CMaster::ReadDataRegister()
    {
        return LL_I2C_ReceiveData8(_I2Cx);
    }
    
    bool I2CMaster::Busy(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BUSY(_I2Cx);
    }
    
    bool I2CMaster::StartConditionGenerated(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_SB(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_SB(_I2Cx);
    }
    
    bool I2CMaster::SlaveAddressSent(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_ADDR(_I2Cx);
    } 
    bool I2CMaster::TransmitterEmpty(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_TXE(_I2Cx);
    }
    
    bool I2CMaster::TransferDone(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(_I2Cx) == 0U) );
        return (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx);
    }
    void I2CMaster::SoftReset(bool enable)
    {
        if(enable)
            LL_I2C_EnableReset(_I2Cx);
        else
            LL_I2C_DisableReset(_I2Cx);
    }
    bool I2CMaster::ACKFail()
    {
        return (bool)LL_I2C_IsActiveFlag_AF(_I2Cx);
    }
    
    bool I2CMaster::DataAvailableRXNE(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BTF(_I2Cx);
    }
    
#if defined (I2C_DEBUG)
#define I2C_DEBUG_LOG(log) (_I2CStatus = (log))  // 108 bytes of ROM
#else 
#define I2C_DEBUG_LOG(log)
#endif
    
}
#endif //I2CMaster_h
