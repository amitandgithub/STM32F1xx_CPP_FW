/******************
** FILE: Spi.cpp
**
** DESCRIPTION:
**  Spi implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Spi.h"

namespace HAL
{
  
  Spi::Spi(SpiPort_t SpiPort, Hz_t Hz) :
    m_hz(Hz),
    m_SPIState(SPI_RESET)
#if SPI_MASTER_DMA
      ,m_I2C1_DMA_Rx_Callback(this),
      m_I2C1_DMA_Tx_Callback(this),
      m_I2C2_DMA_Rx_Callback(this),
      m_I2C2_DMA_Tx_Callback(this)
#endif
      {
        if(SpiPort  == SPI1_A4_A5_A6_A7)
        {
          m_SPIx = SPI1;
        }
        else if(SpiPort  == SPI2_B12_B13_B14_B15)
        {
          m_SPIx = SPI1;
        }
        else
        {
          while(1); // Fatal Error
        }  
        
        m_SpiStatus = SPI_OK;
      }    
    
    Spi::SpiStatus_t Spi::HwInit(void *pInitStruct)
    {        	
      
      LL_SPI_InitTypeDef SPI_InitStruct;
#if SPI_DEBUG
      DebugLogInstance.Enable(Utils::DebugLog<DBG_LOG_TYPE>::DBG_LOG_MODULE_ID_I2C);
#endif            
      /* Set SPI_InitStruct fields to default values */
      SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;//LL_SPI_HALF_DUPLEX_TX;//LL_SPI_FULL_DUPLEX;
      SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
      SPI_InitStruct.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
      SPI_InitStruct.ClockPolarity     = LL_SPI_POLARITY_LOW;
      SPI_InitStruct.ClockPhase        = LL_SPI_PHASE_1EDGE;
      SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;//LL_SPI_NSS_HARD_OUTPUT;
      SPI_InitStruct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV4;
      SPI_InitStruct.BitOrder          = LL_SPI_MSB_FIRST;
      SPI_InitStruct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
      SPI_InitStruct.CRCPoly           = 10U;         
      
      if(m_SPIx == SPI1)
      {
        DigitalOut<A5,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> sck;
        DigitalIn<A6,INPUT_PULLUP>                                  miso;
        DigitalOut<A7,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> mosi;
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
      }
      else if(m_SPIx == SPI2)
      {
        DigitalOut<A5,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> sck;
        DigitalOut<A6,INPUT_PULLUP>      miso;
        DigitalOut<A7,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz> mosi;
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
      }
      else
      {
        while(1);
      }
      
      if(m_SPIx == nullptr)
      {
        // Remap 
      }
      
      if(pInitStruct == nullptr)
      {
        LL_SPI_Init(m_SPIx, &SPI_InitStruct);
      }
      else
      {
        LL_SPI_Init(m_SPIx, (LL_SPI_InitTypeDef*)pInitStruct);
      }     
      
#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)			
      DMA::DMAConfig_t DMAConfig; 
      
      /* Set DMA_InitStruct fields to default values */
      DMAConfig.PeriphOrM2MSrcAddress  = 0;
      DMAConfig.MemoryOrM2MDstAddress  = 0;
      DMAConfig.Direction 			 = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
      DMAConfig.Mode					 = LL_DMA_MODE_NORMAL;//LL_DMA_MODE_NORMAL;
      DMAConfig.PeriphOrM2MSrcIncMode  = LL_DMA_MEMORY_NOINCREMENT;
      DMAConfig.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
      DMAConfig.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
      DMAConfig.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
      DMAConfig.NbData                 = 0;
      DMAConfig.Priority				 = LL_DMA_PRIORITY_LOW;
#endif            
      
      if(m_SPIx == SPI1)
      {
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR || SPI_SLAVE_DMA       
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI1_IRQn,0,this);
        //        EnableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
#endif
#if SPI_MASTER_DMA   
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel7_IRQn,0,&m_I2C1_DMA_Rx_Callback);
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel6_IRQn,0,&m_I2C1_DMA_Tx_Callback);
        InterruptManagerInstance.EnableInterrupt(DMA1_Channel6_IRQn);
        InterruptManagerInstance.EnableInterrupt(DMA1_Channel7_IRQn);
        dma1.XferConfig(&DMAConfig, I2C1_TX_DMA_CHANNEL);
        dma1.XferConfig(&DMAConfig, I2C1_RX_DMA_CHANNEL);  
        dma1.EnableTransferCompleteInterrupt(I2C1_TX_DMA_CHANNEL);
        dma1.EnableTransferCompleteInterrupt(I2C1_RX_DMA_CHANNEL);
#endif
#if SPI_SLAVE_DMA     
        dma1.HwInit();
        dma1.XferConfig(&DMAConfig, I2C1_TX_DMA_CHANNEL);
        dma1.XferConfig(&DMAConfig, I2C1_RX_DMA_CHANNEL);  
#endif        
      }                
      else if(m_SPIx == SPI2)
      {
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR || SPI_SLAVE_DMA
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI2_IRQn,0,this);
        //        EnableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
#endif
#if (SPI_MASTER_DMA == 1) 
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_I2C2_DMA_Rx_Callback);
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,&m_I2C2_DMA_Tx_Callback);
        dma1.EnableTransferCompleteInterrupt(I2C2_TX_DMA_CHANNEL);
        dma1.EnableTransferCompleteInterrupt(I2C2_RX_DMA_CHANNEL);
#endif
      }   
      else
      {
        while(1); // Fatal Error
      }
      
      Enable();
      
      m_SpiStatus = SPI_OK;
      
      return SPI_OK;            
    }
    
    bool Spi::WaitOnFlag(volatile uint32_t* reg, uint32_t bitmask, uint32_t status, uint32_t timeout) // 35(0x25)bytes // stm32 0x28
    {
      while( ((*reg & bitmask) == status) && timeout-- );    
      return (bool)((*reg & bitmask) == status);
    }
    
    
#if SPI_POLL    
    
    Spi::SpiStatus_t Spi::TxPoll(uint8_t* TxBuf, uint32_t TxLen, uint8_t Options)
    {      
      if(TxBuf == nullptr)  return SPI_INVALID_PARAMS;           
      
      SetTransmissionMode(LL_SPI_HALF_DUPLEX_TX);
      
      while(TxLen != 0)
      {        
        m_SPIx->DR = *TxBuf++;
        TxLen--;
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::RxPoll(uint8_t* RxBuf, uint32_t RxLen, uint8_t Options)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;        
      
      while(RxLen != 0)
      {
        //if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIx->DR = 0xFF;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = m_SPIx->DR;        
        RxLen--;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, uint8_t Options)
    {      
      if((TxBuf == nullptr) || (RxBuf == nullptr)) return SPI_INVALID_PARAMS;        
      
      //SetTransmissionMode(LL_SPI_FULL_DUPLEX);
      
      while(Len != 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIx->DR = *TxBuf++;
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
        
        *RxBuf++ = m_SPIx->DR;        
        Len--;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    
#endif // SPI_POLL
    
#if SPI_MASTER_INTR
    
#if 0
    
    if (hspi->Init.Direction == SPI_DIRECTION_2LINES)
    {
      /* Enable TXE interrupt */
      __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE));
    }
    else
    {
      /* Enable TXE and ERR interrupt */
      __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_ERR));
    }
    
#endif
    
    Spi::SpiStatus_t Spi::TxIntr(uint8_t* TxBuf, uint32_t TxLen, SPICallback_t XferDoneCallback)
    {      
      if(TxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = nullptr;
      m_Transaction.RxLen = 0;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
      if(TxLen > 0)
      {        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
        
        m_SPIState = SPI_MASTER_TX;  
        
        // This clears the TXE flag
        m_SPIx->DR = *m_Transaction.TxBuf++;
        m_Transaction.TxLen--;
        
        //SPI_BYTE_OUT_8_BIT(m_Transaction); 
        
        EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }
      
      return SPI_INVALID_PARAMS;
    }
    
    Spi::SpiStatus_t Spi::RxIntr(uint8_t* RxBuf, uint32_t RxLen,SPICallback_t XferDoneCallback)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      m_Transaction.TxBuf = nullptr;
      m_Transaction.TxLen = 0;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.XferDoneCallback = XferDoneCallback;
      
       if(RxLen > 0)
      {       
        m_SPIState = SPI_MASTER_RX;
       
        m_SPIx->DR = 0xff;
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;

        EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        return SPI_OK;        
      }
      
      return SPI_INVALID_PARAMS;
    }
    
#endif // SPI_MASTER_INTR
    
    
    void Spi::ISR()
    {             
      
      if( SPI_RXNE(m_SPIx) )
      {
        if(m_SPIState == SPI_MASTER_TX)
        {
          if(m_Transaction.TxLen > 0)
          {
            SPI_BYTE_OUT_8_BIT(m_Transaction);       
          }
          else
          {
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
            }
            else
            {                      
              m_SPIState = SPI_READY;           
              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction();
            }
            DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
          }
        }
        else  if(m_SPIState == SPI_MASTER_RX)
        {
          if(m_Transaction.RxLen > 0)
          {            
            SPI_BYTE_IN_8_BIT(m_Transaction); 
            m_SPIx->DR = 0xff; // dummy write
          }
          else
          {
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
            }
            else
            {                      
              m_SPIState = SPI_READY;           
              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction();
            }
            DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
          }              
        }
        else  if(m_SPIState == SPI_MASTER_TXRX)
        {
          if((m_Transaction.TxLen > 0) || (m_Transaction.RxLen > 0))
          {
          
            if(m_Transaction.RxLen > 0)
            {
              SPI_BYTE_OUT_8_BIT(m_Transaction);         
            }
            
            if(m_Transaction.TxLen > 0)
            {
              SPI_BYTE_IN_8_BIT(m_Transaction);        
            }
          }
          else
          {
            if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
            {
              m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
            }
            else
            {                      
              m_SPIState = SPI_READY;           
              
              if( m_Transaction.XferDoneCallback) 
                m_Transaction.XferDoneCallback->CallbackFunction();
            }
            DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
          }        
        }
      }     
      else
      {
        
      }
    }
    
    void Spi::HalfDuplex8_Handler(SPI_Interrupts_t event)
    {
      if( SPI_TXE(m_SPIx) )
      {
        if(m_Transaction.TxLen > 0)
        {
          m_SPIx->DR = *m_Transaction.TxBuf++;
          m_Transaction.TxLen--;        
        }
        else
        {
          if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
          {
            m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
          }
          else
          {                      
            m_SPIState = SPI_READY;           
            
            if( m_Transaction.XferDoneCallback) 
              m_Transaction.XferDoneCallback->CallbackFunction();
          }
          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        }        
      }
      
      if( SPI_RXNE(m_SPIx) )
      {
        if(m_Transaction.RxLen > 0)
        {
          *m_Transaction.TxBuf++ = m_SPIx->DR;
          m_Transaction.RxLen--;        
        }
        else
        {
          if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
          {
            m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
          }
          else
          {                      
            m_SPIState = SPI_READY;           
            
            if( m_Transaction.XferDoneCallback) 
              m_Transaction.XferDoneCallback->CallbackFunction();
          }
          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        }        
      }
      
    }
    
    void Spi::FullDuplex8_Handler(SPI_Interrupts_t event)
    {      
      if( SPI_RXNE(m_SPIx) )
      {
        if(m_Transaction.TxLen > 0)
        {
          m_SPIx->DR = *m_Transaction.TxBuf++;
          m_Transaction.TxLen--;        
        }
        else
        {
          if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
          {
            m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
          }
          else
          {                      
            m_SPIState = SPI_READY;           
            
            if( m_Transaction.XferDoneCallback) 
              m_Transaction.XferDoneCallback->CallbackFunction();
          }
          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        }        
      }
      
    }
    
    void Spi::HalfDuplex16_Handler(SPI_Interrupts_t event)
    {
      
      
    }
    
    void Spi::FullDuplex16_Handler(SPI_Interrupts_t event)
    {
      
      
    }
    
    
}










//#if 0
//
//    void Spi::ISR()
//    {
//      
//      switch(SPI_GET_EVENT(m_SPIx))
//      {
//      case EVENT_RXNE :
////        if(m_Transaction.RxLen > 0)
////        {
////          *m_Transaction.TxBuf++ = m_SPIx->DR = 
////          m_Transaction.RxLen--;        
////        }
////        else
////        {
////          if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
////          {
////            m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
////          }
////          else
////          {                      
////            m_SPIState = SPI_READY;           
////            
////            if( m_Transaction.XferDoneCallback) 
////              m_Transaction.XferDoneCallback->CallbackFunction();
////          }
////          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
////        } 
//        //break;
//        
//      case EVENT_TXE :  
//        if(m_Transaction.TxLen > 0)
//        {
//          m_SPIx->DR = *m_Transaction.TxBuf++;
//          m_Transaction.TxLen--;        
//        }
//        else
//        {
//          if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
//          {
//            m_SPIState = SPI_ERROR_BUSY_TIMEOUT;
//          }
//          else
//          {                      
//            m_SPIState = SPI_READY;           
//            
//            if( m_Transaction.XferDoneCallback) 
//              m_Transaction.XferDoneCallback->CallbackFunction();
//          }
//          DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
//        } 
//        break;
//        
//      case EVENT_CHSIDE :
//        break;
//      case EVENT_UDR :
//        break;
//      case EVENT_CRCERR :
//        break;
//      case EVENT_MODF :
//        break;
//      case EVENT_OVR :
//        LL_SPI_ClearFlag_OVR(m_SPIx);
//        break;
//      case EVENT_BSY :
//        break;
//      default: while(1);      
//      }
//    } 
//
//
//#endif