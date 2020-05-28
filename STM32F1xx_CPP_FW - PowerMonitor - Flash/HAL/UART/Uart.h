/******************
** CLASS: Uart
**
** DESCRIPTION:
**  Implements the Uart driver
**
** CREATED: 8/11/2019, by Amit Chaudhary
**
** FILE: Uart.h
**
******************/
#ifndef Uart_h
#define Uart_h

#include"chip_peripheral.h"
#include"CPP_HAL.h"
#include"InterruptManager.h"
#include"Callback.h"
#include"Queue.h"
#include"DebugLog.h"
#include"DigitalOut.h"
#include"GpioOutput.h"
#include"GpioInput.h"
#include"ClockManager.h"

#include"DMA.h"
#include <stdio.h>


extern HAL::DMA dma1;
extern Utils::DebugLog<DBG_LOG_TYPE> DebugLogInstance;
extern HAL::InterruptManager InterruptManagerInstance;   


namespace HAL
{    
  
#define UART_POLL       1  // 502,
#define UART_INTR       0  // 808    - 502
#define UART_DMA        0  // 1'380  - 502

  
  class Uart : public InterruptSource
  {
  public:
    static const uint32_t UART1_TX_DMA_CHANNEL = 4;
    static const uint32_t UART1_RX_DMA_CHANNEL = 5;
    static const uint32_t UART2_RX_DMA_CHANNEL = 6;
    static const uint32_t UART2_TX_DMA_CHANNEL = 7;
    static const uint32_t UART3_TX_DMA_CHANNEL = 2;
    static const uint32_t UART3_RX_DMA_CHANNEL = 3;
    static const uint32_t UART_TIMEOUT = 100000U;
    
    typedef enum  : uint8_t
    {
    UART_OK,
    UART_DMA_ERROR,
    UART_SLAVE_TX_DONE,
    UART_SLAVE_RX_DONE,
    UART_SLAVE_TXRX_DONE,
    UART_SLAVE_TX_DMA_DONE,
    UART_SLAVE_RX_DMA_DONE,
    UART_SLAVE_TXRX_DMA_DONE,
    UART_BUSY,
    UART_INVALID_PARAMS,
    UART_TXE_TIMEOUT,
    UART_RXNE_TIMEOUT,
    UART_TC_TIMEOUT,
    UART_TXN_POSTED,
    UART_TXN_QUEUE_ERROR,
  }UartStatus_t;
    
    typedef USART_TypeDef* UARTx_t;    

    typedef enum
    {
      UART_RESET,
      UART_READY,
      UART_TX_BUSY,
      UART_RX_BUSY,
    }UartState_t;      
    
    enum
    {
      IT_RXNE = USART_CR1_RXNEIE,
      IT_TXE = USART_CR1_TXEIE,
      IT_PE  = USART_CR1_PEIE,
      IT_ERR = USART_CR3_EIE,
    };
    
    class uartCallback
    {
    public:
      virtual void CallbackFunction(UartStatus_t Status) = 0;
    };
    
    typedef uartCallback UARTCallback_t;  
    
    typedef struct
    {
      uint16_t          Len;
      uint8_t*          Buf;     
      UARTCallback_t*   XferDoneCallback;
    }Transaction_t; 
    
    Uart(UartPort_t uartport,uint32_t Baudrate);
    
    ~Uart(){};

    UartStatus_t HwInit(uint32_t Baudrate = 9600, uint32_t Stop = LL_USART_STOPBITS_1, uint32_t Parity = LL_USART_PARITY_NONE);
    
    UartStatus_t HwDeinit();
    
    void Enable(){LL_USART_Enable(m_UARTx);}
    
    void Disable(){LL_USART_Disable(m_UARTx);}
    
    void EnableInterrupt(uint32_t InterruptFlags){SET_BIT(m_UARTx->CR1, InterruptFlags);}
    
    void DisableInterrupt(uint32_t InterruptFlags){CLEAR_BIT(m_UARTx->CR1, InterruptFlags);}
    
    void PinsHwInit(Port_t TxPort, PIN_t TxPin,Port_t RxPort, PIN_t RxPin);
    
    UartState_t GetState(){return m_UARTState;}  
    
    void SetBaudrate(uint32_t Baudrate, uint32_t OverSampling = LL_USART_OVERSAMPLING_16);
    
    UartStatus_t     TxPoll(uint8_t data);
    
    UartStatus_t     TxPoll(uint8_t data1,uint8_t data2);
    
    UartStatus_t     TxPoll(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4);
      
    UartStatus_t     TxPoll(uint8_t* TxBuf, uint16_t TxLen);
    
    UartStatus_t     RxPoll(uint8_t* RxBuf, uint16_t RxLen);
    
    virtual void ISR();
    
    UartStatus_t     TxIntr(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    UartStatus_t     RxIntr(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback = nullptr);    

    
#if UART_DMA   
    
    UartStatus_t     TxDMA(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    UartStatus_t     RxDMA(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback = nullptr);
    
    void LoadRxDmaChannel(uint8_t* Buf, uint32_t len);
    
    void LoadTxDmaChannel(uint8_t* Buf, uint32_t len);  
    
    class UART_DMA_Rx_Callback : public InterruptSource
    {
    public:
      UART_DMA_Rx_Callback(Uart* This):_this(This){};
      virtual void ISR(){_this->UART_DMA_Rx_Done_Handler();}
    private:
      Uart* _this;
    }; 
    
    class UART_DMA_Tx_Callback : public InterruptSource
    {
    public:
      UART_DMA_Tx_Callback(Uart* This):_this(This){};
      virtual void ISR(){_this->UART_DMA_Tx_Done_Handler();}
    private:
      Uart* _this;
    }; 
    
    void UART_DMA_Tx_Done_Handler();
    
    void UART_DMA_Rx_Done_Handler();
#endif
    
  protected:  
    UartPort_t                  m_UartPort;
    UARTx_t                     m_UARTx;    
    uint32_t                    m_Baudrate;
    volatile UartState_t        m_UARTState;         /* It must be volatile becoz it is shared between ISR and main loop */   
    volatile UartStatus_t       m_UARTStatus;        /* It must be volatile becoz it is shared between ISR and main loop */
    
#if UART_INTR || UART_DMA
    Transaction_t               m_Transaction;
#endif
    
#if UART_DMA 
    uint8_t                     m_Curent_DMA_Tx_Channel;
    uint8_t                     m_Curent_DMA_Rx_Channel;
    UART_DMA_Tx_Callback        m_UART_DMA_Tx_Callback;
    UART_DMA_Rx_Callback        m_UART_DMA_Rx_Callback;
#endif 
  };
  
} 

#define UART_TXE(_UART_) _UART_->SR & USART_SR_TXE
  
#define UART_RXNE(_UART_) _UART_->SR & USART_SR_RXNE

#define UART_TC(_UART_) _UART_->SR & USART_SR_TC

#define UART_WAIT_FOR_TXE_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_TXE, 0, TIMEOUT)
  
#define UART_WAIT_FOR_RXNE_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_RXNE, 0, TIMEOUT)

#define UART_WAIT_FOR_TC_FLAG_TO_SET(_UART_,TIMEOUT) WaitOnFlag(&_UART_->SR, USART_SR_TC, 0, TIMEOUT)

#define UART_BYTE_IN(__BUF)      *__BUF.Buf++ = m_UARTx->DR; __BUF.Len--

#define UART_BYTE_OUT(__BUF)     m_UARTx->DR = *__BUF.Buf++; __BUF.Len--

#endif