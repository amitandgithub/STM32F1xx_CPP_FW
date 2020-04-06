/******************
** CLASS: I2c
**
** DESCRIPTION:
**  Implements the I2C base class
**
** CREATED: 20 Mar 2020 by Amit Chaudhary
**
** FILE: I2c.h
**
******************/
#ifndef I2c_h
#define I2c_h

#include "CPP_HAL.h"
//#include "printf.h"
#include"I2c_hw.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DebugLog.h"
#include"DigitalOut.h"
#include"DMA.h"

extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;

#define I2C_DEBUG 1
#define I2C_LOG_STATES_SIZE 500

namespace HAL
{      
  class I2c
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
    
    typedef enum  : uint8_t
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
    I2C_STOP_DETECTED,
    I2C_ACK_FAIL,
    I2C_ARB_LOST,
    I2C_INTR_PENDING,
    I2C_BUS_ERROR,
    I2C_DATA_OVR,
    I2C_INVALID_PARAMS,
    I2C_XFER_DONE,
    I2C_SLAVE_TX_DONE,
    I2C_SLAVE_RX_DONE,
    I2C_SLAVE_RX_DONE_WITH_NACK,
    I2C_TXN_POSTED,
    I2C_TXN_QUEUE_ERROR,
  }I2CStatus_t;
  
  typedef enum
  {
    I2C1_B6_B7,
    I2C2_B10_B11,
    I2C1_B8_B9,
  }I2cPins_t;
  
  typedef enum
  {
    i2c1 = I2C1_BASE,
    i2c2 = I2C2_BASE
  }I2cPort_t;

    typedef uint32_t Hz_t ;
    typedef I2C_TypeDef* I2Cx_t;    
    typedef Buf_t i2cBuf_t;
    
    class i2cCallback
    {
    public:
      i2cCallback(){};
      virtual ~i2cCallback(){};
      virtual void CallbackFunction(I2CStatus_t I2CStatus) = 0;
    };
    
    typedef Callback* I2CCallback_t;
    
    typedef i2cCallback* I2CSlaveCallback_t; 
    
    typedef struct
    {
      uint16_t                SlaveAddress;
      uint8_t                 RepeatedStart;
      uint8_t                 TxLen;
      uint8_t                 RxLen; 
      uint8_t*                TxBuf; 
      uint8_t*                RxBuf;
      I2CCallback_t           XferDoneCallback;
    }MasterTxn_t;
    
    typedef struct
    {
      i2cBuf_t*             TxBuf;
      i2cBuf_t*             RxBuf;
      uint8_t               DefaultByte;
      I2CSlaveCallback_t    XferDoneCallback;
    }SlaveTxn_t;    
    
    typedef enum 
    {
      EVENT_SB        = 0U,
      EVENT_ADDR      = 1U,
      EVENT_BTF       = 2U,
      EVENT_ADD10     = 3U,
      EVENT_STOPF     = 4U,
      EVENT_RSVD      = 5U,
      EVENT_RXNE      = 6U,
      EVENT_TXE       = 7U,
      EVENT_BERR      = 8U,
      EVENT_ARLO      = 9U,
      EVENT_AF        = 10U,
      EVENT_OVR       = 11U,
      EVENT_PECERR    = 12U
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
      I2C_RESET,
      I2C_READY,
      I2C_MASTER_TX,
      I2C_MASTER_RX,
      I2C_SLAVE_TX,
      I2C_SLAVE_RX,
      I2C_SLAVE_RX_LISTENING,
      I2C_MASTER_RX_REPEATED_START,
      I2C_MASTER_TX_ACK_FAIL,
      I2C_MASTER_TX_DMA,
      I2C_MASTER_RX_DMA,
      I2C_SLAVE_TX_DMA,
      I2C_SLAVE_RX_DMA,
    }I2CState_t;      
    
    typedef struct
    {
      uint8_t           SlaveAddress;
      uint8_t           RepeatedStart;
      uint16_t          TxLen;
      uint16_t          RxLen; 
      uint8_t*          TxBuf; 
      uint8_t*          RxBuf;
      I2CCallback_t     XferDoneCallback;
    }Transaction_t; // 20 bytes
      
    
    //I2c(I2cPort_t I2cx);
    
    ~I2c(){};
    
    I2CStatus_t HwInit(I2Cx_t I2Cx, I2cPins_t I2cPins);
    
    I2CStatus_t HwDeinit(I2Cx_t I2Cx);
    
    void PinsHwInit(Port_t SclPort, PIN_t SclPin, Port_t SdaPort, PIN_t SdaPin);
    
    void ClockEnable(I2Cx_t I2Cx);
    
    void ClockDisable(I2Cx_t I2Cx);
    
    I2CStatus_t SendSlaveAddress(I2Cx_t I2Cx, uint16_t SlaveAddress, uint8_t Repeatedstart);
    
    I2CState_t GetState(){return m_I2CState;}
    
    I2CStatus_t CheckAndLoadTxn(I2Cx_t I2Cx, Transaction_t const *pTransaction);
    
    void TxnDoneHandler(I2Cx_t I2Cx, uint32_t StopFlag);
    
    void ScanBus(I2Cx_t I2Cx, uint8_t* pFoundDevices, uint8_t size);
    
    inline void SoftReset();  

    inline void LogStates(I2CLogs_t log);
    
  protected:
//    Hz_t                    m_hz;
//    I2Cx_t                  m_I2Cx;            
    Transaction_t           m_Transaction;
//    SlaveTxn_t              m_SlaveTxn;
//#if I2C_MASTER_Q 
//    I2CTxnQueue_t           m_I2CTxnQueue;
//#endif
    
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile I2CState_t     m_I2CState; 
    /* It must be volatile becoz it is shared between ISR and main loop */
    volatile I2CStatus_t    m_I2CStatus;
    
  protected:         
#if I2C_DEBUG
    I2CLogs_t       I2CStates[I2C_LOG_STATES_SIZE];
    volatile uint32_t        I2CStates_Idx;
    //Utils::DebugLog<DBG_LOG_TYPE>* dbg_log_instance;
#endif
 
  };   
  
  inline void I2c::SoftReset()
  {
//    LL_I2C_EnableReset(m_I2Cx);
//    LL_I2C_DisableReset(m_I2Cx);
//    HwInit();
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
