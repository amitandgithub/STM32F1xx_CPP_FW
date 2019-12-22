/******************
** CLASS: I2CBase
**
** DESCRIPTION:
**  I2C common features implementation
**
** CREATED: 6/20/2019, by Amit Chaudhary
**
** FILE: I2CBase.h
**
******************/
#ifndef I2CBase_h
#define I2CBase_h


#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"stm32f1xx_ll_i2c.h"

namespace HAL
{
    
#define I2C_DEBUG 
#define I2C_LOG_STATES_SIZE 1500
    
    class I2CBase
    {
    public:
        
        using Pin_t = HAL::Gpio::Pin_t;
        typedef uint32_t Hz_t ;
        typedef I2C_TypeDef* I2Cx_t;
        
        typedef enum
        {
            NONE,
            I2C_LOG_STOPF_FLAG,
            I2C_LOG_STOPF_NOT_CLEARED,
            I2C_LOG_START_MASTER_TX,
            I2C_LOG_START_MASTER_RX,
            I2C_LOG_START_MASTER_TXRX,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1_DMA,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2_DMA,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2,
            I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER,
            I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0,
            I2C_LOG_ADDR_INTR_MASTER_TX_REPEATED_START,
            I2C_LOG_ADDR_INTR_MASTER_TX_STOP,
            I2C_LOG_TXE,
            I2C_LOG_TXE_DONE,  
            I2C_LOG_REPEATED_START,
            I2C_LOG_BTF_MASTER_TX_GT_0,
            I2C_LOG_BTF_MASTER_TX_REPEATED_START,
            I2C_LOG_BTF_MASTER_TX_STOP,
            I2C_LOG_BTF_MASTER_RX_SIZE_3,
            I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED,
            I2C_LOG_BTF_MASTER_RX_SIZE_GT_3,
            I2C_LOG_RXNE_MASTER_SIZE_GT_3,
            I2C_LOG_RXNE_MASTER,
            I2C_LOG_RXNE_MASTER_SIZE_1,
            I2C_LOG_RXNE_MASTER_SIZE_2,
            I2C_LOG_RXNE_MASTER_SIZE_2_OR_3,
            // I2C_LOG_BTF_MASTER_RXNE_LAST,
            I2C_LOG_RXNE_MASTER_LAST,
            I2C_LOG_BTF_MASTER_BERR,
            I2C_LOG_BTF_MASTER_ACK_FAIL,
            I2C_LOG_BTF_MASTER_ARB_LOST,
            I2C_LOG_SB_MASTER_TX,
            I2C_LOG_SB_MASTER_RX,
            I2C_LOG_SB_MASTER_RX_2,
            I2C_LOG_SB_MASTER_RX_REPEATED_START,
            I2C_LOG_STOP_MASTER_RX_WITHOUT_REPEATED_START,
            I2C_LOG_STOP_XFER_DONE,
            I2C_LOG_BTF_MASTER_DATA_OVR,
            I2C_LOG_START_SLAVE_RX,
            I2C_LOG_BTF_SLAVE_RX,
            I2C_LOG_RXNE_SLAVE_RX,
            I2C_LOG_BTF_SLAVE_RX_QUEUED,
            I2C_LOG_BTF_SLAVE_RX_QUEUE_FULL,
            I2C_LOG_RXNE_SLAVE_RX_QUEUED,
            I2C_LOG_RXNE_SLAVE_RX_QUEUE_FULL,
            I2C_LOG_AF_SLAVE_ACK_FAIL,
            I2C_LOG_TxE_SLAVE_TX,
            I2C_LOG_TxE_SLAVE_TX_QUEUE_EMPTY,
            I2C_LOG_BTF_SLAVE_TX,
            I2C_LOG_BTF_SLAVE_TX_QUEUE_EMPTY,
            I2C_LOG_MASTER_BTF_XFER_DONE,
            I2C_LOG_ADDR_XFER_DONE,
            I2C_LOG_TXN_QUEUED,
            I2C_LOG_TXN_DEQUEUED,
            I2C_LOG_TXN_QUEUE_EMPTY,
            I2C_LOG_TXN_QUEUE_ERROR,
            I2C_LOG_TXN_DONE,
            I2C_LOG_TXN_DONE_ALL,
            I2C_LOG_DMA_TX_DONE,
            I2C_LOG_DMA_HALF_TX_DONE,
            I2C_LOG_DMA_TX_ERROR,
            I2C_LOG_ADDR_INTR_MASTER_TX_DMA,
            I2C_LOG_ADDR_INTR_MASTER_RX_DMA,
            I2C_LOG_BTF_MASTER_RX_DMA_STOP,
            I2C_LOG_BTF_MASTER_TX_DMA_STOP,
            I2C_LOG_START_MASTER_TX_DMA,
            I2C_LOG_START_MASTER_RX_DMA,
            I2C_LOG_REPEATED_START_MASTER_RX_DMA,
            I2C_LOG_DMA_HALF_RX_DONE,
            I2C_LOG_DMA_RX_ERROR,
            I2C_LOG_DMA_RX_DONE,
            I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_1,
            I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_2,
            I2C_LOG_DMA_TX_BTF_TIMEOUT,
            I2C_LOG_DMA_REPEATED_START,
            I2C_LOG_DMA_STOP_GENERATED,
            I2C_LOG_DMA_TXN_DONE,
        }I2CLogs_t;
        
        typedef enum : uint8_t
        {
            I2C_OK,
            I2C_BUSY,
            I2C_BUSY_TIMEOUT,
            I2C_START_TIMEOUT,
            I2C_ADDR_TIMEOUT,
            I2C_BTF_TIMEOUT,
            I2C_RXNE_TIMEOUT,
            I2C_TXE_TIMEOUT,
            I2C_STOP_TIMEOUT,
            I2C_ACK_FAIL,
            I2C_ARB_LOST,
            I2C_INTR_PENDING,
            I2C_BUS_ERROR,
            I2C_DATA_OVR,
            I2C_INVALID_PARAMS,
            I2C_XFER_DONE,
            I2C_TXN_POSTED,
            I2C_TXN_QUEUE_ERROR,
        }I2CStatus_t;
        
        typedef enum
        {
            RESET,
            BUSY,
            READY,
            MASTER_TX,
            MASTER_RX,
            SLAVE_TX,
            SLAVE_RX,
            SLAVE_RX_LISTENING,
            MASTER_RX_REPEATED_START,
            MASTER_TX_ACK_FAIL,
            MASTER_TX_DMA,
            MASTER_RX_DMA,
        }I2CState_t; 
        
        static const uint32_t I2C_TIMEOUT           = 5000U;
        static const uint16_t I2C_DIR_WRITE         = 0xfffeU;
        static const uint16_t I2C_DIR_READ          = 0x0001U;
        static const uint16_t I2C_OWN_SLAVE_ADDRESS = 0x08U<<1U;
        
        I2CBase(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);        
        ~I2CBase(){};     
        
        I2CStatus_t     HwInit(void *pInitStruct = nullptr);        
        I2CStatus_t     HwDeinit();        
        void            ClockEnable();        
        void            ClockDisable();        
        void            ScanBus(uint8_t* pFoundDevices, uint8_t size);        
        void            SetSpeed(uint32_t speed);
        inline void     SoftReset();
        inline bool     Busy(uint32_t timeout);
        inline bool     StopFlagCleared(uint32_t timeout);
        I2CStatus_t     GetStatus(){return m_I2CStatus;}        
        I2CState_t      GetState(){return m_I2CState;}             
        
    protected:
        GpioOutput              m_sclPin;
        GpioOutput              m_sdaPin;
        Hz_t                    m_hz;
        I2Cx_t                  m_I2Cx;        
        volatile I2CState_t     m_I2CState;     /* It must be volatile becoz it might be shared between ISR and main loop */
        volatile I2CStatus_t    m_I2CStatus;    /* It must be volatile becoz it might be shared between ISR and main loop */
        
    public:  
        //inline void log(I2CLogs_t I2CLog) { I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = I2CLog ;}
        
#ifdef I2C_DEBUG
        static I2CLogs_t       I2CStates[I2C_LOG_STATES_SIZE];
        static uint32_t        I2CStates_Idx;
#endif
        
    };
    
    void I2CBase::SoftReset()
    {
        LL_I2C_EnableReset(m_I2Cx);
        LL_I2C_DisableReset(m_I2Cx);
        HwInit();
    }
    
    bool I2CBase::Busy(uint32_t timeout)
    {
        while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(m_I2Cx) == 1U) );
        return (bool)LL_I2C_IsActiveFlag_BUSY(m_I2Cx);
    }
    
    bool I2CBase::StopFlagCleared(uint32_t timeout)
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
    
#ifdef I2C_DEBUG     

#define I2C_LOG_STATES(log) (I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = (log))
#define I2C_DEBUG_LOG(log) (m_I2CStatus = (log))  // 108 bytes of ROM
//#define I2C_LOG_STATES(_log) log(_log)
    
#else
    
#define I2C_LOG_STATES(log)
#define I2C_DEBUG_LOG(log)
    
#endif

}
#endif //I2CBase.h

/*
 2 261 bytes of readonly  code memory
     95 bytes of readonly  data memory
  2 375 bytes of readwrite data memory
  
  + I2CPoll_Test();code 3172; data 35 
  
  5 433 bytes of readonly  code memory
    183 bytes of readonly  data memory
  2 410 bytes of readwrite data memory
  
  
  
   2 261 bytes of readonly  code memory
     95 bytes of readonly  data memory
  2 375 bytes of readwrite data memory
  
  + I2CInt_Test();code 5500; data 1529 (Local)
  
    7 761 bytes of readonly  code memory
    195 bytes of readonly  data memory
   3 904 bytes of readwrite data memory
   
   
   
      2 261 bytes of readonly  code memory
     95 bytes of readonly  data memory
	2 375 bytes of readwrite data memory
  
  + I2CInt_Test();code 5720; data 2213 (global)
   
     7 981 bytes of readonly  code memory
    199 bytes of readonly  data memory
  4 588 bytes of readwrite data memory
  
  
  
    2 261 bytes of readonly  code memory
    95 bytes of readonly  data memory
	2 375 bytes of readwrite data memory
  
  + I2CInt_Test()
  + I2CPoll_Test();
  //code 7248; data 2248 
  
    9 509 bytes of readonly  code memory
    199 bytes of readonly  data memory
  4 623 bytes of readwrite data memory
  
  
   + I2CInt_Test()
   + I2CPoll_Test();
   + I2CDMA_Test();
  
    11 539 bytes of readonly  code memory
     321 bytes of readonly  data memory
   5 005 bytes of readwrite data memory
   //code 9278; data 2630 
   
   
   
   
     8 655 bytes of readonly  code memory
    321 bytes of readonly  data memory
  3 445 bytes of readwrite data memory
  
  +I2C_DEBUG
   //code 1292; data 1504
  
    9 947 bytes of readonly  code memory
    321 bytes of readonly  data memory
  4 949 bytes of readwrite data memory
*/
