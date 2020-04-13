/******************
** FILE: Uart.cpp
**
** DESCRIPTION:
**  Uart implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Uart.h"

namespace HAL
{
  
  Uart::Uart(UartPort_t uartport,uint32_t Baudrate) : m_Baudrate(Baudrate),m_UartPort(uartport)
#if UART_INTR || UART_DMA 
  ,m_Transaction{0,nullptr,nullptr}
#endif
#if UART_DMA
  ,m_UART_DMA_Tx_Callback(this),
  m_UART_DMA_Rx_Callback(this)
#endif
  {
    
  }
  
  void Uart::PinsHwInit(Port_t TxPort, PIN_t TxPin,Port_t RxPort, PIN_t RxPin)
  {
#if 0
    GpioOutput      Tx(TxPort,TxPin);
    GpioInput       Rx(RxPort,RxPin);
    
    Tx.HwInit(OUTPUT_AF_PUSH_PULL);
    Rx.HwInit(INPUT_PULLDOWN);
#else
    DigitalIO::SetInputMode(RxPort,RxPin,INPUT_PULLUP); //Rx.HwInit(INPUT_PULLDOWN);
    DigitalIO::SetOutputMode(TxPort,TxPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_HIGH); // Tx.HwInit(OUTPUT_AF_PUSH_PULL);
    
#endif
  }
  
  Uart::UartStatus_t Uart::HwInit(uint32_t Baudrate, uint32_t Stop, uint32_t Parity)
  {   
    LL_USART_InitTypeDef USART_InitStruct;
    m_Baudrate = Baudrate;
    /* Set USART_InitStruct fields to default values */
    USART_InitStruct.BaudRate            = m_Baudrate;
    USART_InitStruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits            = Stop;
    USART_InitStruct.Parity              = Parity ;
    USART_InitStruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
#if defined(USART_CR1_OVER8)
    USART_InitStruct.OverSampling        = LL_USART_OVERSAMPLING_16;
#endif /* USART_OverSampling_Feature */
    
#if UART_DMA    
    DMA::DMAConfig_t DMAConfig;       
    /* Set DMA_InitStruct fields to default values */
    DMAConfig.PeriphOrM2MSrcAddress   = 0;
    DMAConfig.MemoryOrM2MDstAddress   = 0;
    DMAConfig.Direction               = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    DMAConfig.Mode                    = LL_DMA_MODE_NORMAL;//LL_DMA_MODE_NORMAL;
    DMAConfig.PeriphOrM2MSrcIncMode   = LL_DMA_MEMORY_NOINCREMENT;
    DMAConfig.MemoryOrM2MDstIncMode   = LL_DMA_MEMORY_INCREMENT;
    DMAConfig.PeriphOrM2MSrcDataSize  = LL_DMA_PDATAALIGN_BYTE;
    DMAConfig.MemoryOrM2MDstDataSize  = LL_DMA_MDATAALIGN_BYTE;
    DMAConfig.NbData                  = 0;
    DMAConfig.Priority                = LL_DMA_PRIORITY_LOW;
#endif 
    
    if(m_UartPort == UART1_A9_A10)
    {
      m_UARTx = USART1;
    }
    else if(m_UartPort == UART1_B6_B7)
    {
      m_UARTx = USART1;
      AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
    }
    else if(m_UartPort == UART2_A2_A3)
    {
      m_UARTx = USART2;
    }
    else if(m_UartPort == UART2_D5_D6)
    {
      m_UARTx = USART2;
      AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;
    }
#if defined(USART3)
    else if(m_UartPort == UART3_B10_B11)
    {
      m_UARTx = USART3;
    }
    else if(m_UartPort == UART3_C10_C11)
    {
      m_UARTx = USART3;
      AFIO->MAPR |= AFIO_MAPR_USART3_REMAP; 
    }
    else if(m_UartPort == UART3_D8_D9)
    {
      m_UARTx = USART3;
      AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_FULLREMAP; 
    }
#endif
#if defined(USART4)
    else if(m_UartPort == UART4_C10_C11)
    {
      m_UARTx = USART4;
    }
#endif
#if defined(USART5)
    else if(m_UartPort == UART1_A9_A10)
    {
      m_UARTx = USART5;
    }
#endif
    else 
    {
      while(1);
    }
    
    if(m_UARTx == USART1)
    {
      //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
      HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_USART1);
      
      if(m_UartPort == UART1_B6_B7)
      {
        PinsHwInit(B6,B7);
      }
      else                   
      {
        PinsHwInit(A9,A10);
      }
        
      
#if UART_INTR 
      InterruptManagerInstance.RegisterDeviceInterrupt(USART1_IRQn,0,this);
#endif
      
#if UART_DMA 
      dma1.HwInit();
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,&m_UART_DMA_Tx_Callback);
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_UART_DMA_Rx_Callback);
      m_Curent_DMA_Tx_Channel = UART1_TX_DMA_CHANNEL;
      m_Curent_DMA_Rx_Channel = UART1_RX_DMA_CHANNEL;
#endif
    }
    else if(m_UARTx == USART2)
    {
      //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
      HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_USART2);
      
      
      if(m_UartPort == UART2_D5_D6)
      {
        PinsHwInit(D5,D6);
      }
      else                   
      {
        PinsHwInit(A2,A3);
      }
      
#if UART_INTR 
      InterruptManagerInstance.RegisterDeviceInterrupt(USART2_IRQn,0,this);
#endif
      
#if UART_DMA 
      dma1.HwInit();
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel6_IRQn,0,&m_UART_DMA_Rx_Callback);
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel7_IRQn,0,&m_UART_DMA_Tx_Callback);
      m_Curent_DMA_Tx_Channel = UART2_TX_DMA_CHANNEL;
      m_Curent_DMA_Rx_Channel = UART2_RX_DMA_CHANNEL;
#endif
      
    }
#if defined(USART3)
    else if(m_UARTx == USART3)
    {
      //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
      HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_USART3);      
      
      if(m_UartPort == UART3_C10_C11)
      {
        PinsHwInit(C10,C11);
      }
      else if(m_UartPort == UART3_D8_D9)
      {
        PinsHwInit(D8,D9);
      }
      else                   
      {
        PinsHwInit(B10,B11);
      }
      
#if UART_INTR 
      InterruptManagerInstance.RegisterDeviceInterrupt(USART3_IRQn,0,this);
#endif
      
#if UART_DMA 
      dma1.HwInit();
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel2_IRQn,0,&m_UART_DMA_Tx_Callback);
      InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,&m_UART_DMA_Rx_Callback);
      m_Curent_DMA_Tx_Channel = UART3_TX_DMA_CHANNEL;
      m_Curent_DMA_Rx_Channel = UART3_RX_DMA_CHANNEL;
#endif
    }
#endif
#if defined(USART4)
    else if(m_UARTx == USART4)
    {
      //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
      HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_USART4);
      PinsHwInit(C10,C11);
#if UART_INTR 
      InterruptManagerInstance.RegisterDeviceInterrupt(USART4_IRQn,0,this);
#endif
    }
#endif
#if defined(USART5)
    else if(m_UARTx == USART5)
    {
      
    }
#endif
    else 
    {
      while(1);
    }
    
    Disable();
    LL_USART_Init(m_UARTx,&USART_InitStruct);
    Enable();
    
#if UART_DMA     
    dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
    dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel); 
    dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
    dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
#endif 
    
    m_UARTStatus = UART_OK;
    m_UARTState = UART_READY;    
    return UART_OK;     
  }
  
  void Uart::SetBaudrate(uint32_t Baudrate,uint32_t OverSampling)
  {
    uint32_t periphclk = LL_RCC_PERIPH_FREQUENCY_NO;
    LL_RCC_ClocksTypeDef rcc_clocks;
    
    if(m_Baudrate == Baudrate) return;
    
    m_Baudrate = Baudrate;
    
    /*---------------------------- USART BRR Configuration -----------------------
    * Retrieve Clock frequency used for USART Peripheral
    */
    LL_RCC_GetSystemClocksFreq(&rcc_clocks);
    if (m_UARTx == USART1)
    {
      periphclk = rcc_clocks.PCLK2_Frequency;
    }
    else if (m_UARTx == USART2)
    {
      periphclk = rcc_clocks.PCLK1_Frequency;
    }
#if defined(USART3)
    else if (m_UARTx == USART3)
    {
      periphclk = rcc_clocks.PCLK1_Frequency;
    }
#endif /* USART3 */
#if defined(UART4)
    else if (m_UARTx == UART4)
    {
      periphclk = rcc_clocks.PCLK1_Frequency;
    }
#endif /* UART4 */
#if defined(UART5)
    else if (m_UARTx == UART5)
    {
      periphclk = rcc_clocks.PCLK1_Frequency;
    }
#endif /* UART5 */
    else
    {
      /* Nothing to do, as error code is already assigned to ERROR value */
    }
    
    /* Configure the USART Baud Rate :
    - valid baud rate value (different from 0) is required
    - Peripheral clock as returned by RCC service, should be valid (different from 0).
    */
    if ((periphclk != LL_RCC_PERIPH_FREQUENCY_NO)
        && (Baudrate != 0U))
    {
#if defined(USART_CR1_OVER8)
      LL_USART_SetBaudRate(m_UARTx,
                           periphclk,
                           OverSampling,
                           Baudrate);
#else
      LL_USART_SetBaudRate(m_UARTx,
                           periphclk,
                           Baudrate);
#endif /* USART_OverSampling_Feature */      
    }
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t data)
  {
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    
    m_UARTx->DR = data;
    
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT;
    
    if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TC_TIMEOUT;
    
    return UART_OK;     
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t data1, uint8_t data2)
  {
    UartStatus_t status = TxPoll(data1);
    
    if(status != UART_OK) return status;    
    
    return TxPoll(data2);
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t data1,uint8_t data2,uint8_t data3,uint8_t data4)
  {
    UartStatus_t status = TxPoll(data1,data2);
    
    if(status != UART_OK) return status;    
    
    return TxPoll(data3,data4);
  }
  
  Uart::UartStatus_t Uart::TxPoll(uint8_t* TxBuf, uint16_t TxLen)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    
    while(TxLen != 0)
    {        
      m_UARTx->DR = *TxBuf++;
      TxLen--;
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TXE_TIMEOUT; 
    }
    
    if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_TC_TIMEOUT;
    return UART_OK;     
  }
  
  Uart::UartStatus_t Uart::RxPoll(uint8_t* RxBuf, uint16_t RxLen)
  {    
    if((RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    while(RxLen != 0)
    {        
      *RxBuf++ = m_UARTx->DR;
      RxLen--;      
      if(UART_WAIT_FOR_RXNE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) return UART_RXNE_TIMEOUT; 
    }    
    return UART_OK;     
  }
  
#if UART_INTR 
  
  Uart::UartStatus_t Uart::TxIntr(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = TxLen;
    m_Transaction.Buf = TxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    EnableInterrupt(IT_TXE | IT_PE | IT_ERR);
    return UART_OK;
  }
  
  Uart::UartStatus_t Uart::RxIntr(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback)
  {
    if( (RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = RxLen;
    m_Transaction.Buf = RxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    EnableInterrupt(IT_RXNE | IT_PE | IT_ERR);
    return UART_OK;
  }
#endif
   
#if UART_INTR 
  
  void Uart::ISR()
  {
    if(UART_RXNE(m_UARTx))
    {
      UART_BYTE_IN(m_Transaction);
      
      if(m_Transaction.Len == 0)
      {
        DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
        
        if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT; 
        if(UART_WAIT_FOR_TC_FLAG_TO_SET(m_UARTx,UART_TIMEOUT))  m_UARTStatus = UART_TC_TIMEOUT;    
        
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);   
        
        m_UARTState = UART_READY;
      }
    }
    else if(UART_TXE(m_UARTx))
    {
      if(m_Transaction.Len > 0)
      {
        UART_BYTE_OUT(m_Transaction);
      }
      else
      {
        DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
        
        if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
        
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
        
        m_UARTState = UART_READY;
      }
    }
//    else if(UART_TC(m_UARTx))
//    {
//      DisableInterrupt(IT_TXE | IT_RXNE | IT_PE | IT_ERR );   
//      
//      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
//      
//      if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
//      
//      m_UARTState = UART_READY;
//    }
    else // Handle remaining events here
    {
      if(LL_USART_IsActiveFlag_ORE(m_UARTx))
      {
        LL_USART_ClearFlag_ORE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_nCTS(m_UARTx))
      {
        LL_USART_ClearFlag_nCTS(m_UARTx);
      }
      
       if(LL_USART_IsActiveFlag_LBD(m_UARTx))
      {
        LL_USART_ClearFlag_LBD(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_IDLE(m_UARTx))
      {
        LL_USART_ClearFlag_IDLE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_NE(m_UARTx))
      {
        LL_USART_ClearFlag_NE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_FE(m_UARTx))
      {
        LL_USART_ClearFlag_FE(m_UARTx);
      }
      
      if(LL_USART_IsActiveFlag_PE(m_UARTx))
      {
        LL_USART_ClearFlag_PE(m_UARTx);
      }        
    }
  } 
#else
  void Uart::ISR()
  {
  }
#endif  // UART_INTR

#if UART_DMA 
  
  Uart::UartStatus_t Uart::TxDMA(uint8_t* TxBuf, uint16_t TxLen,UARTCallback_t* pUARTCallback)
  {
    if( (TxBuf == nullptr) || (TxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = TxLen;
    m_Transaction.Buf = TxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    //EnableInterrupt(IT_TXE | IT_PE | IT_ERR);
    LoadTxDmaChannel(TxBuf,TxLen);
    LL_USART_EnableDMAReq_TX(m_UARTx);
    return UART_OK;
  }
  
  Uart::UartStatus_t Uart::RxDMA(uint8_t* RxBuf, uint16_t RxLen,UARTCallback_t* pUARTCallback)
  {
    if( (RxBuf == nullptr) || (RxLen == 0) ) return UART_INVALID_PARAMS;
    
    m_UARTState = UART_TX_BUSY;
    m_Transaction.Len = RxLen;
    m_Transaction.Buf = RxBuf;
    m_Transaction.XferDoneCallback = pUARTCallback; 
    //EnableInterrupt(IT_RXNE | IT_PE | IT_ERR);
    LoadRxDmaChannel(RxBuf,RxLen);
    LL_USART_EnableDMAReq_RX(m_UARTx);
    return UART_OK;
  }
  
  void Uart::UART_DMA_Tx_Done_Handler()
  {			      
    /* Transfer Complete Interrupt management ***********************************/
    if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
    {
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable all the DMA interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
      }                   
      
      dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
      
      LL_USART_DisableDMAReq_TX(m_UARTx);  
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
      else                                                    m_UARTStatus = UART_OK;
    }     
    /* Half Transfer Complete Interrupt management ******************************/
    else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_HC_FLAG))
    {        
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,UART1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable the half transfer interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
      }
      /* Clear the half transfer complete flag */
      dma1.ClearFlag(m_Curent_DMA_Tx_Channel, HAL::DMA::DMA_HC_FLAG);
    }      
    /* Transfer Error Interrupt management **************************************/
    else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
    {
      /* Disable all the DMA interrupt */
      m_UARTStatus = UART_DMA_ERROR;  
      dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
    }
    
    if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
    
    m_UARTState = UART_READY;      
  }
  
  void Uart::UART_DMA_Rx_Done_Handler()
  {			      
    /* Transfer Complete Interrupt management ***********************************/
    if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG))
    {
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Rx_Channel) == LL_DMA_MODE_CIRCULAR)
      {
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
      }                  
      
      dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
      
      LL_USART_DisableDMAReq_RX(m_UARTx); 
      
      if(UART_WAIT_FOR_TXE_FLAG_TO_SET(m_UARTx,UART_TIMEOUT)) m_UARTStatus = UART_TXE_TIMEOUT;   
      else                                                    m_UARTStatus = UART_OK;    
    }     
    /* Half Transfer Complete Interrupt management ******************************/
    else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_HC_FLAG))
    {        
      /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
      if(LL_DMA_GetMode(dma1.m_DMAx,UART1_RX_DMA_CHANNEL) == LL_DMA_MODE_CIRCULAR)
      {
        /* Disable the half transfer interrupt */
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
      }
      /* Clear the half transfer complete flag */
      dma1.ClearFlag(m_Curent_DMA_Rx_Channel, HAL::DMA::DMA_HC_FLAG);
    }      
    /* Transfer Error Interrupt management **************************************/
    else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TE_FLAG))
    {
      m_UARTStatus = UART_DMA_ERROR;        
      dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
    }     
    
    if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_UARTStatus);       
    
    m_UARTState = UART_READY;
  }  
  
  void Uart::LoadRxDmaChannel(uint8_t* Buf, uint32_t len)
  {
    dma1.Load(m_Curent_DMA_Rx_Channel, (uint32_t)&(m_UARTx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY, LL_DMA_PDATAALIGN_BYTE);
  }
  
  void Uart::LoadTxDmaChannel(uint8_t* Buf, uint32_t len)
  {
    dma1.Load(m_Curent_DMA_Tx_Channel, (uint32_t)&(m_UARTx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH, LL_DMA_PDATAALIGN_BYTE);
  }
  
//  void Uart::UART_DMA_Tx_Callback::ISR( )
//  {      
//    _this->UART_DMA_Tx_Done_Handler(); 
//  }
//  
//  void Uart::UART_DMA_Rx_Callback::ISR( )
//  {      
//    _this->UART_DMA_Rx_Done_Handler(); 
//  }
#endif // UART_DMA 
  
  
  
  
}