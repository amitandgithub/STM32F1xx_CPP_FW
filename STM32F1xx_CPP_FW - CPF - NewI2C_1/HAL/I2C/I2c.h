/******************
** CLASS: I2c
**
** DESCRIPTION:
**  Implements the I2C in polling mode
**
** CREATED: 4/13/2019, by Amit Chaudhary
**
** FILE: I2c.h
**
******************/
#ifndef I2c_h
#define I2c_h

#include"GpioOutput.h"
#include"CPP_HAL.h"
#include"I2c_hw.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DMA.h"

namespace HAL
{    
  
#define I2C_POLL  1
#define I2C_INT   1
#define I2C_DMA   1
  
  /* This flag enables the Slave receiver in DMA mode instead of Interrupt mode*/
#define I2C_SLAVE_IN_DMA_MODE 1
  
#define I2C_DEBUG 1 
#define I2C_LOG_STATES_SIZE 1500
  
#define I2C_RX_METHOD_1 
  
#define I2C_BUF_BYTE_IN(__I2C_BUF)              (*__I2C_BUF->RxBuf++) = I2C_DATA_REG(m_I2Cx); __I2C_BUF->RxLen--

#define I2C_BUF_BYTE_OUT(__I2C_BUF)             I2C_DATA_REG(m_I2Cx) = (*__I2C_BUF->TxBuf++); __I2C_BUF->TxLen--

#define I2C_SLAVE_BUF_BYTE_IN(__I2C_BUF)        __I2C_BUF.RxBuf->Buf[__I2C_BUF.RxBuf->Idx++] = I2C_DATA_REG

#define I2C_SLAVE_BUF_BYTE_OUT(__I2C_BUF)        I2C_DATA_REG = __I2C_BUF.TxBuf->Buf[__I2C_BUF.TxBuf->Idx++]
  
  class I2c : public InterruptSource
  {
  public:
    
    static const uint32_t I2C_TIMEOUT           = 5000U;
    static const uint16_t I2C_DIR_WRITE         = 0xfffeU;
    static const uint16_t I2C_DIR_READ          = 0x0001U;
    static const uint16_t I2C_OWN_SLAVE_ADDRESS = 0x08U<<1U;
    
    static const uint32_t I2C1_RX_DMA_CHANNEL = 7;
    static const uint32_t I2C1_TX_DMA_CHANNEL = 6;
    static const uint32_t I2C2_RX_DMA_CHANNEL = 5;
    static const uint32_t I2C2_TX_DMA_CHANNEL = 4;
    
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
    
    using Pin_t = HAL::Gpio::Pin_t;
    typedef uint32_t Hz_t ;
    typedef I2C_TypeDef* I2Cx_t;
    typedef Callback* I2CCallback_t;        
    using I2CSlaveRxQueue_t = Utils::Queue<uint8_t,64U> ;
    using I2CSlaveTxQueue_t = Utils::Queue<uint8_t,64U> ;
    
    typedef Buf_t i2cBuf_t;
    
    //typedef void(*i2cMasterCallback_t)(I2CStatus_t Status);
    
    //typedef void(*i2cSlaveCallback_t)(I2CStatus_t Status);
    
    typedef struct
    {
      uint16_t                SlaveAddress;
      uint8_t                 RepeatedStart;
      uint16_t                 TxLen;
      uint16_t                 RxLen; 
      uint8_t*                TxBuf; 
      uint8_t*                RxBuf;
      I2CCallback_t           XferDoneCallback;
    }MasterTxn_t;
    
    typedef struct
    {
      i2cBuf_t*           TxBuf;
      i2cBuf_t*           RxBuf;
      uint8_t             DefaultByte;
      I2CCallback_t       XferDoneCallback;
    }SlaveTxn_t;
    
    typedef enum 
    {
      I2C_RX_QUEUE_FULL_CALLBACK,
      I2C_TX_QUEUE_EMPTY_CALLBACK,
      I2C_SLAVE_TX_COMPLETE_CALLBACK,
      I2C_SLAVE_RX_COMPLETE_CALLBACK,            
    }I2CCallbackType_t;
    
    
    
    typedef enum 
    {
      SB = 0U,
      ADDR = 1U,
      BTF = 2U,
      ADD10 = 3U,
      STOPF = 4U,
      RSVD = 5U,
      RXNE = 6U,
      TXE = 7U,
      BERR = 8U,
      ARLO = 9U,
      AF = 10U,
      OVR = 11U,
      PECERR = 12U
    }I2C_Interrupts_t;
    
    typedef enum
    {														  
      I2C_LOG_STOPF_FLAG                           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,1),
      I2C_LOG_STOPF_NOT_CLEARED                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,2),
      I2C_LOG_START_MASTER_TX                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,3),
      I2C_LOG_START_MASTER_RX                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,4),
      I2C_LOG_START_MASTER_TXRX                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,5),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_0           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,6),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1_DMA       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,7),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_1           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,8),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2_DMA       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,9),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_2           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,10),
      I2C_LOG_ADDR_INTR_MASTER_RX_SIZE_OTHER       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,11),
      I2C_LOG_ADDR_INTR_MASTER_TX_SIZE_GT_0        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,12),
      I2C_LOG_ADDR_INTR_MASTER_TX_REPEATED_START   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,13),
      I2C_LOG_ADDR_INTR_MASTER_TX_STOP             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,14),
      I2C_LOG_TXE_GT_0                              =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,15),
      I2C_LOG_TXE_DONE                             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,16),
      I2C_LOG_REPEATED_START                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,17),
      I2C_LOG_BTF_MASTER_TX_GT_0                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,18),
      I2C_LOG_BTF_MASTER_TX_REPEATED_START         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,19),
      I2C_LOG_BTF_MASTER_TX_STOP                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,20),
      I2C_LOG_BTF_MASTER_RX_SIZE_3                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,21),
      I2C_LOG_BTF_MASTER_RX_SIZE_2_STOPED          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,22),
      I2C_LOG_BTF_MASTER_RX_SIZE_GT_3              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,23),
      I2C_LOG_RXNE_MASTER_SIZE_GT_3                	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,24),
      I2C_LOG_RXNE_MASTER                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,25),
      I2C_LOG_RXNE_MASTER_SIZE_1                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,26),
      I2C_LOG_RXNE_MASTER_SIZE_2                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,27),
      I2C_LOG_RXNE_MASTER_SIZE_2_OR_3              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,28),
      I2C_LOG_BTF_MASTER_RXNE_LAST              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,29),
      I2C_LOG_RXNE_MASTER_LAST                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,30),
      I2C_LOG_BTF_MASTER_BERR                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,31),
      I2C_LOG_BTF_MASTER_ACK_FAIL                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,32),
      I2C_LOG_BTF_MASTER_ARB_LOST                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,33),
      I2C_LOG_SB_MASTER_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,34),
      I2C_LOG_SB_MASTER_RX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,35),
      I2C_LOG_SB_MASTER_RX_2                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,36),
      I2C_LOG_SB_MASTER_RX_REPEATED_START          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,37),
      I2C_LOG_STOP_MASTER_RX_WITHOUT_REPEATED_START	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,38),
      I2C_LOG_STOP_XFER_DONE                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,39),
      I2C_LOG_BTF_MASTER_DATA_OVR                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,40),
      I2C_LOG_START_SLAVE_RX                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,41),
      I2C_LOG_BTF_SLAVE_RX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,42),
      I2C_LOG_RXNE_SLAVE_RX                        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,43),
      I2C_LOG_BTF_SLAVE_RX_QUEUED                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,44),
      I2C_LOG_BTF_SLAVE_RX_QUEUE_FULL              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,45),
      I2C_LOG_RXNE_SLAVE_RX_QUEUED                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,46),
      I2C_LOG_RXNE_SLAVE_RX_QUEUE_FULL             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,47),
      I2C_LOG_AF_SLAVE_ACK_FAIL                    	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,48),
      I2C_LOG_TxE_SLAVE_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,49),
      I2C_LOG_TxE_SLAVE_TX_QUEUE_EMPTY             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,50),
      I2C_LOG_BTF_SLAVE_TX                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,51),
      I2C_LOG_BTF_SLAVE_TX_QUEUE_EMPTY             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,52),
      I2C_LOG_MASTER_BTF_XFER_DONE                 	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,53),
      I2C_LOG_ADDR_XFER_DONE                       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,54),
      I2C_LOG_TXN_QUEUED                           	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,55),
      I2C_LOG_TXN_DEQUEUED                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,56),
      I2C_LOG_TXN_QUEUE_EMPTY                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,57),
      I2C_LOG_TXN_QUEUE_ERROR                      	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,58),
      I2C_LOG_TXN_DONE                             	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,59),
      I2C_LOG_TXN_DONE_ALL                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,60),
      I2C_LOG_DMA_TX_DONE                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,61),
      I2C_LOG_DMA_HALF_TX_DONE                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,62),
      I2C_LOG_DMA_TX_ERROR                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,63),
      I2C_LOG_ADDR_INTR_MASTER_TX_DMA              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,64),
      I2C_LOG_ADDR_INTR_MASTER_RX_DMA              	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,65),
      I2C_LOG_BTF_MASTER_RX_DMA_STOP               	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,66),
      I2C_LOG_BTF_MASTER_TX_DMA_STOP               	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,67),
      I2C_LOG_START_MASTER_TX_DMA                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,68),
      I2C_LOG_START_MASTER_RX_DMA                  	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,69),
      I2C_LOG_REPEATED_START_MASTER_RX_DMA         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,70),
      I2C_LOG_DMA_HALF_RX_DONE                     	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,71),
      I2C_LOG_DMA_RX_ERROR                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,72),
      I2C_LOG_DMA_RX_DONE                          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,73),
      I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_1       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,74),
      I2C_LOG_ADDR_INTR_MASTER_RX_DMA_SIZE_2       	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,75),
      I2C_LOG_DMA_TX_BTF_TIMEOUT                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,76),
      I2C_LOG_DMA_REPEATED_START                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,77),
      I2C_LOG_DMA_STOP_GENERATED                   	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,78),
      I2C_LOG_DMA_TXN_DONE                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,79),
      I2C_LOG_SB                                    =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,80),
      I2C_LOG_ADDR                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,81),
      I2C_LOG_BTF                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,82),
      I2C_LOG_RXNE                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,83),
      I2C_LOG_TXE                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,84),
      I2C_LOG_STOPF                                 =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,85),
      I2C_LOG_BERR                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,86),
      I2C_LOG_ARLO                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,87),
      I2C_LOG_AF                                    =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,88),
      I2C_LOG_OVR                                   =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,89),
      I2C_LOG_PECERR                                =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,90),
      I2C_LOG_ADD10                                	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,91),
      I2C_LOG_START_TIMEOUT                        	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,92),
      I2C_LOG_START_DONE                            =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,93),
      I2C_LOG_ADDR_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,94),
      I2C_LOG_ADDR_SENT                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,95),
      I2C_LOG_TXE_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,96),
      I2C_LOG_TX_DONE                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,97),
      I2C_LOG_RX_DONE                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,98),
      I2C_LOG_RXNE_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,99),
      I2C_LOG_BTF_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,100),
      I2C_LOG_RX_1                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,101),
      I2C_LOG_RX_2                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,102),
      I2C_LOG_RX_GT_2                               =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,103),
      I2C_LOG_RX_1_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,104),
      I2C_LOG_RX_2_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,105),
      I2C_LOG_RX_3_DONE                             =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,106),
      I2C_LOG_START                                 =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,107),
      I2C_LOG_STOP                                  =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,108),
      I2C_LOG_STOP_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,109),
      I2C_LOG_BUSY_TIMEOUT                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,110),
      I2C_LOG_ACK_FAIL                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,111),
      I2C_LOG_SLAVE_RX_DONE_WITH_DEFAULT_BYTE     =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,112),
      I2C_LOG_TXE_DEFAULT_BYTE                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,113),
      I2C_LOG_ADDR_SLAVE_RX_SELECTED              =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,114),
      I2C_LOG_ADDR_SLAVE_TX_SELECTED      =	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,115),
      I2C_LOG_SLAVE_RX_BYTE_IN          	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,116),/*
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,117),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,118),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,119),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,120),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,121),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,122),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,123),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,124),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,125),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,126),
      I2C_LOG_                         	=	DBG_LOG_CREATE_ID(DBG_LOG_MODULE_ID_I2C,127) */           
    }I2CLogs_t;
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
      SLAVE_TX_DMA,
      SLAVE_RX_DMA,
    }I2CState_t;      
    
    typedef struct
    {
      uint16_t                SlaveAddress;
      uint8_t                 RepeatedStart;
      volatile I2CStatus_t*   pStatus;
      uint16_t                TxLen;
      uint16_t                RxLen; 
      uint8_t*                TxBuf; 
      uint8_t*                RxBuf;
      I2CCallback_t           XferDoneCallback;
    }Transaction_t;
    
    using I2CTxnQueue_t = Utils::Queue<MasterTxn_t*,10U> ;
    
    typedef enum 
    {
      I2C_EVENT_INTERRUPT_ENABLE,
      I2C_EVENT_INTERRUPT_DISABLE,
      I2C_EVENT_INTERRUPT_BUFFER_ENABLE,
      I2C_EVENT_INTERRUPT_BUFFER_DISABLE,
      I2C_ERROR_INTERRUPT_ENABLE,
      I2C_ERROR_INTERRUPT_DISABLE,
      I2C_INTERRUPT_ENABLE_ALL,
      I2C_INTERRUPT_DISABLE_ALL,
      
    }I2CInterrupt_t;      
    
    I2c(Pin_t scl, Pin_t sda, Hz_t Hz = 100000U);
    
    ~I2c(){};
    
    I2CStatus_t HwInit(void *pInitStruct = nullptr);
    
    I2CStatus_t HwDeinit();
    
    inline void ClockEnable();
    
    inline void ClockDisable();
    
    bool WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout);
    
    I2CStatus_t SendSlaveAddress(uint16_t SlaveAddress, uint8_t Repeatedstart);
    
    inline I2CState_t GetState();
    
    inline uint8_t SlaveRxDataAvailable();
    
    bool DetectSlave(uint8_t SlaveAddress);
    
    void ScanBus(uint8_t* pFoundDevices, uint8_t size);
    
    inline void Start();
    
    void Interrupt_Tx_Done_Handler(uint32_t StopFlag);
    
    void Interrupt_Rx_Done_Handler();
    
    void DMA_Tx_Done_Handler();
    
    void DMA_Rx_Done_Handler();
    
    void TxnDoneHandler(uint32_t StopFlag);
    
    void LoadNextTransaction();
    
    inline void SendAddress(uint8_t SlaveAddress);
    
    inline void WriteDataRegister(uint8_t data);
    
    inline uint8_t ReadDataRegister();             
    
    I2CStatus_t     TxPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen);        
    
    I2CStatus_t     RxPoll(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen);
    
    I2CStatus_t MasterTx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t MasterTx_Intr(Transaction_t* pTransaction);
    
    I2CStatus_t MasterRx_Intr(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr );
    
    I2CStatus_t MasterRx_Intr(Transaction_t* pTransaction);
    
    I2CStatus_t MasterTxRx_Intr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen,
                                uint8_t RepeatedStart,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t MasterTxRx_Intr(Transaction_t* pTransaction);
    
    I2CStatus_t Post(MasterTxn_t* pTransaction, uint32_t Mode = 0);
    
    I2CStatus_t SlaveTx_Intr(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );
    
    I2CStatus_t SlaveRx_Intr(uint8_t* pdata, uint32_t len, I2CCallback_t XferDoneCallback = nullptr );
    
    I2CStatus_t MasterTx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t MasterRx_DMA(uint16_t SlaveAddress,uint8_t* RxBuf, uint32_t RxLen,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);        
    
    I2CStatus_t MasterTxRx_DMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen,
                               uint8_t RepeatedStart,volatile I2CStatus_t* pStatus, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t MasterTxRx_DMA(Transaction_t* pTransaction);
    
    
    I2CStatus_t     XferPoll(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf=nullptr, uint32_t RxLen=0,uint8_t RepeatedStart=0);
    
    I2CStatus_t     XferPoll(Transaction_t* pTransaction);
    
    I2CStatus_t     XferIntr(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf=nullptr, uint32_t RxLen=0,uint8_t RepeatedStart=0,volatile I2CStatus_t* pStatus=nullptr, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t     XferIntr(Transaction_t* pTransaction);
    
    I2CStatus_t     XferIntr(MasterTxn_t* pTransaction);
    
    I2CStatus_t     XferDMA(uint16_t SlaveAddress,uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf=nullptr, uint32_t RxLen=0,uint8_t RepeatedStart=0,volatile I2CStatus_t* pStatus=nullptr, I2CCallback_t XferDoneCallback = nullptr);
    
    I2CStatus_t     XferDMA(Transaction_t* pTransaction);
    
    inline uint8_t ReadRxData();
    
    inline bool StartConditionGenerated(uint32_t timeout);
    
    inline bool Busy(uint32_t timeout);
    
    inline bool SlaveAddressSent(uint32_t timeout);
    
    inline bool TransmitterEmpty(uint32_t timeout);
    
    inline bool TransferDone(uint32_t timeout);
    
    inline void SoftReset();
    
    inline bool ACKFail(uint32_t timeout);
    
    inline bool DataAvailableRXNE(uint32_t timeout);   
    
    inline bool StopCondition(uint32_t timeout);
    
    inline void SetRxFullCallback(I2CCallback_t I2CCallback);
    
    void SetCallback(I2CCallbackType_t I2CCallbackType, I2CCallback_t I2CCallback);
    
    I2CStatus_t AddressMatch();
    
    virtual void ISR( IRQn_Type event );
    
    void InteruptControl(I2CInterrupt_t I2CInterrupt);
    
    void InterruptControl(bool enable, uint16_t InterruptFlag){ enable ? SET_BIT(m_I2Cx->CR2, InterruptFlag) : CLEAR_BIT(m_I2Cx->CR2, InterruptFlag) ;}
    
    void Enable_EVT_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN);}
    
    void Enable_BUF_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITBUFEN);}
    
    void Enable_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITERREN);}
    
    void Enable_EVT_BUF_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);}
    
    void Enable_EVT_ERR_Interrupt(){SET_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);}
    
    void Disable_EVT_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN);}
    
    void Disable_BUF_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITBUFEN);}
    
    void Disable_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITERREN);}
    
    void Disable_EVT_BUF_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);}
    
    void Disable_EVT_ERR_Interrupt(){CLEAR_BIT(m_I2Cx->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);}
    
    inline I2CStatus_t StartListening();
    
    inline bool StopFlagCleared(uint32_t timeout);
    
    void I2C1_DMA_Tx_Done_Handler();
    
    void I2C1_DMA_Rx_Done_Handler();
#if I2C_DMA
    class I2C1_DMA_Rx_Callback : public Callback
    {
    public:
      I2C1_DMA_Rx_Callback(I2c* This):_This(This){};
      virtual void CallbackFunction();
    private:
      I2c* _This;
    };
    
    class I2C1_DMA_Tx_Callback : public Callback
    {
    public:
      I2C1_DMA_Tx_Callback(I2c* This):_This(This){};
      virtual void CallbackFunction();
    private:
      I2c* _This;
    };
    
    class I2C2_DMA_Rx_Callback : public Callback
    {
    public:
      I2C2_DMA_Rx_Callback(I2c* This):_This(This){};
      virtual void CallbackFunction();
    private:
      I2c* _This;
    };
    
    class I2C2_DMA_Tx_Callback : public Callback
    {
    public:
      I2C2_DMA_Tx_Callback(I2c* This):_This(This){};
      virtual void CallbackFunction();
    private:
      I2c* _This;
    };
#endif  
    inline void LogStates(I2CLogs_t log);
  private:
    GpioOutput              m_sclPin;
    GpioOutput              m_sdaPin;
    Hz_t                    m_hz;
    I2Cx_t                  m_I2Cx;            
    //Transaction_t           m_Transaction;
    MasterTxn_t*            m_MasterTxn;
    I2CCallback_t           m_TxQueueEmptyCallback;
    I2CCallback_t           m_RxQueueFullCallback;
    I2CCallback_t           m_SlaveTxDoneCallback;
    I2CCallback_t           m_SlaveRxDoneCallback;
    I2CTxnQueue_t           m_I2CTxnQueue;
    
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile I2CState_t     m_I2CState; 
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile I2CStatus_t    m_I2CStatus;
    
#if I2C_DMA
    I2C1_DMA_Rx_Callback m_I2C1_DMA_Rx_Callback;
    I2C1_DMA_Tx_Callback m_I2C1_DMA_Tx_Callback;
    I2C2_DMA_Rx_Callback m_I2C2_DMA_Rx_Callback;
    I2C2_DMA_Tx_Callback m_I2C2_DMA_Tx_Callback;
    HAL::DMA*            m_DMAx;
#endif
    
  public:         
#ifdef I2C_DEBUG
    I2CLogs_t       I2CStates[I2C_LOG_STATES_SIZE];
    volatile uint32_t        I2CStates_Idx;
    Utils::DebugLog<DBG_LOG_TYPE>* dbg_log_instance;
#endif
    
  public:      
    I2CSlaveRxQueue_t   m_I2CSlaveRxQueue;
    I2CSlaveTxQueue_t   m_I2CSlaveTxQueue; 
  };   
  
  
  inline void I2c::SetRxFullCallback(I2CCallback_t I2CCallback)
  {
    m_RxQueueFullCallback = I2CCallback;
  }
  
  inline uint8_t I2c::ReadRxData()
  {
    uint8_t Data;
    m_I2CSlaveRxQueue.Read(Data);
    return Data;
  }
  
  bool I2c::StartConditionGenerated(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_SB(m_I2Cx) == 0U) );
    return (bool)LL_I2C_IsActiveFlag_SB(m_I2Cx);
  }
  
  inline uint8_t I2c::SlaveRxDataAvailable()
  {
    return m_I2CSlaveRxQueue.Available();
  }
  
  bool I2c::Busy(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_BUSY(m_I2Cx) == 1U) );
    return (bool)LL_I2C_IsActiveFlag_BUSY(m_I2Cx);
  }
  
  bool I2c::StopFlagCleared(uint32_t timeout)
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
  
  
  bool I2c::SlaveAddressSent(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_ADDR(m_I2Cx) == 0U) );
    return (bool)LL_I2C_IsActiveFlag_ADDR(m_I2Cx);
  } 
  bool I2c::TransmitterEmpty(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_TXE(m_I2Cx) == 0U) );
    return (bool)LL_I2C_IsActiveFlag_TXE(m_I2Cx);
  }
  
  bool I2c::TransferDone(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_BTF(m_I2Cx) == 0U) );
    return (bool)LL_I2C_IsActiveFlag_BTF(m_I2Cx);
  }
  void I2c::SoftReset()
  {
    LL_I2C_EnableReset(m_I2Cx);
    LL_I2C_DisableReset(m_I2Cx);
    HwInit();
  }
  bool I2c::ACKFail(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_AF(m_I2Cx) == 1U) );
    return (bool)LL_I2C_IsActiveFlag_AF(m_I2Cx);
  }
  
  bool I2c::DataAvailableRXNE(uint32_t timeout)
  {
    while( (timeout--) && (LL_I2C_IsActiveFlag_RXNE(m_I2Cx) == 0U) );
    return (bool)LL_I2C_IsActiveFlag_RXNE(m_I2Cx);
  }
  
  I2c::I2CStatus_t I2c::StartListening()
  {
    /* Wait until BUSY flag is reset */
    if( Busy(I2C_TIMEOUT) == true ) 
    {          
      //I2C_DEBUG_LOG(I2C_BUSY_TIMEOUT);
      return I2C_BUSY_TIMEOUT;                
    }        
    /* Disable Pos */
    m_I2Cx->CR1 &= ~I2C_CR1_POS;
    
    /* Enable Address Acknowledge */
    m_I2Cx->CR1 |= I2C_CR1_ACK;
    
    // m_I2CState = SLAVE_RX_LISTENING;
    m_I2CState = READY;
    
    Enable_EVT_BUF_ERR_Interrupt();
    
    return I2C_OK;
  }
  
  I2c::I2CState_t I2c::GetState()
  {
    return m_I2CState;
  }
  
  
  
#if I2C_DEBUG     
#pragma inline = forced
  void I2c::LogStates(I2CLogs_t log)
  {
#if 1
    I2CStates[I2CStates_Idx++] = log;
    if(I2CStates_Idx == I2C_LOG_STATES_SIZE )
      I2CStates_Idx = 0;
#else
    dbg_log_instance->Log(log);
#endif
  }
  
  //#define I2C_LOG_STATES(log) (I2CStates[I2CStates_Idx++  % I2C_LOG_STATES_SIZE] = (log))
#define I2C_DEBUG_LOG(log) (m_I2CStatus = (log))  // 108 bytes of ROM
#define I2C_LOG_STATES(log) LogStates(log)
#define I2C_LOG_EVENTS(log) LogStates(log)
  
#else
  
#define I2C_LOG_STATES(log)
#define I2C_DEBUG_LOG(log)
#define I2C_LOG_EVENTS(log)
  
#endif    
  
}
#endif //I2c_h
