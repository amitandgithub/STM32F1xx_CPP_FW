/******************
** FILE: Spi.cpp
**
** DESCRIPTION:
**  Spi implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Spi.h"
#include "DigitalIO.h"
namespace HAL
{
  
  Spi::Spi(SpiPort_t SpiPort, Hz_t Hz) :
    m_SPIState(SPI_RESET)
#if SPI_MASTER_DMA || SPI_SLAVE_DMA
      ,m_SPI_DMA_Tx_Callback(this),
       m_SPI_DMA_Rx_Callback(this)
#endif
      {
//        m_Transaction.TxLen = 0;
//        m_Transaction.RxLen = 0;
//        m_Transaction.Spi_Baudrate = SPI_BAUDRATE_DIV2;
        m_Baudrate = SPI_BAUDRATE_DIV2;
        
        if(SpiPort  == SPI1_A5_A6_A7)
        {
          m_SPIx = SPI1;    
#if SPI_MASTER_DMA || SPI_SLAVE_DMA
          m_Curent_DMA_Tx_Channel = SPI1_TX_DMA_CHANNEL;
          m_Curent_DMA_Rx_Channel = SPI1_RX_DMA_CHANNEL;
#endif
        }
        else if(SpiPort  == SPI1_A15_B3_B4_B5)
        {          
          m_SPIx = nullptr; // will be initalized and remapped in HwInit    
#if SPI_MASTER_DMA || SPI_SLAVE_DMA
          m_Curent_DMA_Tx_Channel = SPI1_TX_DMA_CHANNEL;
          m_Curent_DMA_Rx_Channel = SPI1_RX_DMA_CHANNEL;
#endif
        } 
        else if(SpiPort  == SPI2_B13_B14_B15)
        {
          m_SPIx = SPI2;
#if SPI_MASTER_DMA || SPI_SLAVE_DMA          
          m_Curent_DMA_Tx_Channel = SPI2_TX_DMA_CHANNEL;
          m_Curent_DMA_Rx_Channel = SPI2_RX_DMA_CHANNEL;
#endif
        }
        else
        {
          while(1); // Fatal error
        }

        m_SPIState = SPI_RESET;
      }    
    
    Spi::SpiStatus_t Spi::HwInit(bool Slave, XferMode_t mode)
    {     
      LL_SPI_InitTypeDef SPI_InitStruct;
      
#if SPI_DEBUG
      DebugLogInstance.Enable(Utils::DebugLog<DBG_LOG_TYPE>::DBG_LOG_MODULE_ID_SPI);
#endif            
      /* Set SPI_InitStruct fields to default values */
      SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
      SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
      SPI_InitStruct.DataWidth         = (mode == SPI_8BIT) ? LL_SPI_DATAWIDTH_8BIT : LL_SPI_DATAWIDTH_16BIT;
      SPI_InitStruct.ClockPolarity     = LL_SPI_POLARITY_LOW;
      SPI_InitStruct.ClockPhase        = LL_SPI_PHASE_1EDGE;
      SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;//LL_SPI_NSS_SOFT;//LL_SPI_NSS_HARD_OUTPUT;
      SPI_InitStruct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
      SPI_InitStruct.BitOrder          = LL_SPI_MSB_FIRST;
      SPI_InitStruct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
      SPI_InitStruct.CRCPoly           = 10U;         
      
      if(m_SPIx == SPI1)
      {
        if(Slave)
        {
          SlavePinsHwInit(A4,A5,A6,A7);
          SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
          SPI_InitStruct.NSS = LL_SPI_NSS_HARD_INPUT;
        }
        else
        {
          MasterPinsHwInit(A5,A6,A7);
        }       
        //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_SPI1);
      }
      else if(m_SPIx == SPI2)
      {        
        if(Slave)
        {
          SlavePinsHwInit(B12,B13,B14,B15);
          SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
          SPI_InitStruct.NSS = LL_SPI_NSS_HARD_INPUT;
        }
        else
        {
          MasterPinsHwInit(B13,B14,B15);
        } 
        //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
        HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_SPI2);
      }
      else if(m_SPIx == nullptr)
      {
        AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP; // Remap (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)
        
        m_SPIx = SPI1;  
        
        // NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5
        if(Slave)
        {
          SlavePinsHwInit(A15,B3,B4,B5);
          SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
          SPI_InitStruct.NSS = LL_SPI_NSS_HARD_INPUT;
        }
        else
        {
          MasterPinsHwInit(B3,B4,B5);
        }       
        //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
        HAL::ClockManager::Enable(HAL::ClockManager::CLOCK_SPI1);        
      }
      else
      {
        while(1);
      }
      
      if(m_SPIx == nullptr)
      {
        // Remap 
      }
      Disable();
      LL_SPI_Init(m_SPIx, &SPI_InitStruct);    
      
#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)			
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
      
      if(m_SPIx == SPI1)
      {
#if SPI_MASTER_INTR || SPI_SLAVE_INTR       
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI1_IRQn,0,this);
#endif
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA 
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel3_IRQn,0,&m_SPI_DMA_Tx_Callback);
#endif
#if SPI_SLAVE_DMA     
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel2_IRQn,0,&m_SPI_DMA_Rx_Callback); 
#endif        
      }                
      else if(m_SPIx == SPI2)
      {
#if SPI_MASTER_INTR || SPI_SLAVE_INTR      
        InterruptManagerInstance.RegisterDeviceInterrupt(SPI2_IRQn,0,this);
#endif
#if  SPI_MASTER_DMA  || SPI_SLAVE_DMA    
        dma1.HwInit();
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel5_IRQn,0,&m_SPI_DMA_Tx_Callback);
#endif
#if SPI_SLAVE_DMA     
        InterruptManagerInstance.RegisterDeviceInterrupt(DMA1_Channel4_IRQn,0,&m_SPI_DMA_Rx_Callback);
#endif  
      }   
      else
      {
        while(1); // Fatal Error
      }
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA       
      dma1.XferConfig(&DMAConfig, m_Curent_DMA_Tx_Channel);
      dma1.XferConfig(&DMAConfig, m_Curent_DMA_Rx_Channel); 
      dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Tx_Channel);
      dma1.EnableTransferCompleteInterrupt(m_Curent_DMA_Rx_Channel);
#endif         
      Enable();      
      //m_Transaction.Mode = mode;      
      m_SpiStatus = SPI_OK;
      m_SPIState = SPI_READY;      
      return SPI_OK;            
    }        
    
      void Spi::MasterPinsHwInit(Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin)
      {   
        DigitalIO::SetOutputMode(ClkPort,ClkPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_HIGH); // sck.HwInit(OUTPUT_AF_PUSH_PULL);
        DigitalIO::SetInputMode(MisoPort,MisoPin,INPUT_PULLUP); //miso.HwInit(INPUT_PULLUP);
        DigitalIO::SetOutputMode(MosiPort,MosiPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_HIGH); // sck.HwInit(OUTPUT_AF_PUSH_PULL);
      }
    
      void Spi::SlavePinsHwInit(Port_t ChipSelectPort, PIN_t ChipSelectPin, Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin)
      {    
        DigitalIO::SetInputMode(ChipSelectPort,ChipSelectPin,INPUT_PULLDOWN); //ChipSelect(ChipSelectPort,ChipSelectPin);
        DigitalIO::SetInputMode(ClkPort,ClkPin,INPUT_PULLDOWN); //sck(ClkPort,ClkPin);
        DigitalIO::SetOutputMode(MisoPort,MisoPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_SPEED_FREQ_HIGH); // miso(MisoPort,MisoPin);
        DigitalIO::SetInputMode(MosiPort,MosiPin,INPUT_PULLDOWN); //mosi(MosiPort,MosiPin);
      }    
    
#if SPI_POLL    
    
    Spi::SpiStatus_t Spi::Tx(uint8_t data)
    {     
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);    
      
      m_SPIx->DR = data;            
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::Tx(uint16_t data, XferMode_t mode)
    {      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      
      if(mode == SPI_8BIT)
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);       
        
        m_SPIx->DR = data;          
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
        
        m_SPIx->DR = data>>8;          
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      }
      else
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);       
        m_SPIx->DR = data;          
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      }
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
//    Spi::SpiStatus_t Spi::Tx(uint16_t data, XferMode_t mode)
//    {           
//      SpiStatus_t SpiStatus = Tx((uint8_t)data, mode);
//      
//      if(SpiStatus != SPI_OK ) return SpiStatus;
//      
//      return Tx((uint8_t)(data>>8), mode);
//    }
//    
    
    Spi::SpiStatus_t Spi::Tx(uint32_t data, XferMode_t mode)
    {           
      SpiStatus_t SpiStatus = Tx((uint16_t)data, mode);
      
      if(SpiStatus != SPI_OK ) return SpiStatus;
      
      return Tx((uint16_t)(data>>16), mode);
    }
    
    Spi::SpiStatus_t Spi::TxPoll(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode)
    {      
      if(TxBuf == nullptr)  return SPI_INVALID_PARAMS;  
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      
      if(mode == SPI_8BIT)
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);       
        
        while(TxLen != 0)
        {        
          m_SPIx->DR = *TxBuf++;
          TxLen--;
          
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
        }
      }
      else
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);       
        
        while(TxLen != 0)
        {        
          m_SPIx->DR = *((uint16_t*)TxBuf);
          TxBuf += sizeof(uint16_t);
          TxLen--; 
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
        }
      }
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::RxPoll(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode)
    {      
      if(RxBuf == nullptr) return SPI_INVALID_PARAMS;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      if(mode == SPI_8BIT)
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        
        while(RxLen != 0)
        {
          m_SPIx->DR = 0xFF;
          
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
          
          *RxBuf++ = m_SPIx->DR;        
          RxLen--;
        }
      }
      else
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        
        while(RxLen != 0)
        {
          m_SPIx->DR = 0xff;
          
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
          
          *((uint16_t*)RxBuf) = m_SPIx->DR; RxBuf += sizeof(uint16_t);
          RxLen--;
        }        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;   
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    uint8_t Spi::TxRxPoll8Bit(uint8_t data)
    {            
      SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT + 100000);
      
      m_SPIx->DR = data;
      
      SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT + 100000);    
      
      SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT + 100000);
      
       return (uint8_t)m_SPIx->DR;      
    }
    
    Spi::SpiStatus_t Spi::TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, XferMode_t mode)
    {      
      if((TxBuf == nullptr) || (RxBuf == nullptr)) return SPI_INVALID_PARAMS;        
      
      if(mode == SPI_8BIT)
      {        
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        
        while(Len != 0)
        {        
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
          
          m_SPIx->DR = *TxBuf++;
          
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
          
          *RxBuf++ = m_SPIx->DR;        
          Len--;
        }
      }
      else
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        
        while(Len != 0)
        {        
          if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
          
          m_SPIx->DR = *((uint16_t*)TxBuf); TxBuf += sizeof(uint16_t);
          
          if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_RXNE_TIMEOUT; 
          
          *((uint16_t*)RxBuf) = m_SPIx->DR; RxBuf += sizeof(uint16_t);       
          Len--;
        }
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    Spi::SpiStatus_t Spi::XferPoll(Transaction_t const *pTransaction)
    {
      SpiStatus_t SpiStatus;
      
      if(pTransaction == nullptr) return SPI_INVALID_PARAMS;
      
      if(pTransaction->TxLen)
      {
        SpiStatus = TxPoll(pTransaction->TxBuf,pTransaction->TxLen);
      }
      
      if(pTransaction->RxLen)
      {
        SpiStatus =  RxPoll(pTransaction->RxBuf,pTransaction->RxLen);
      }
      return SpiStatus;
    }
    
#endif // SPI_POLL
    
#if SPI_MASTER_INTR
    
    Spi::SpiStatus_t Spi::TxIntr(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {     
      SpiStatus_t SpiStatus = CheckAndLoadTxn(TxBuf,TxLen,TxBuf,0,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      m_SPIState = SPI_MASTER_TX;  
      
      if(mode == SPI_8BIT)
      {
        m_CurrentIsr = &Spi::Master_Tx_8bit_Isr;
        
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        
        SPI_BYTE_OUT_8_BIT(m_Transaction); 
      }
      else
      {
        m_CurrentIsr = &Spi::Master_Tx_16bit_Isr;
        
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        
        SPI_BYTE_OUT_16_BIT(m_Transaction); 
      }
      
      EnableInterrupt(SPI_CR2_TXEIE);
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::RxIntr(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {      
      SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_RX; 
      
      if(mode == SPI_8BIT)
      {
        m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;         
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
      }
      else
      {
        m_CurrentIsr = &Spi::Master_Rx_16bit_Isr;
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
      }
      m_SPIx->DR = 0xff;
      if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::TxRxIntr(Transaction_t const * pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);   
      
      m_SPIState = SPI_MASTER_TXRX;  
      
      if(m_Transaction.Mode == SPI_8BIT)
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        
        SPI_BYTE_OUT_8_BIT(m_Transaction); 
      }
      else
      {
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        
        SPI_BYTE_OUT_16_BIT(m_Transaction); 
      }
      
      EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
      
      return m_SpiStatus; 
    } 
    
    Spi::SpiStatus_t Spi::XferIntr(Transaction_t const *pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      if(m_Transaction.TxLen)
      {        
        m_SPIState = SPI_MASTER_TX; 
        
        if(m_Transaction.Mode == SPI_8BIT)
        {
          m_CurrentIsr = &Spi::Master_Tx_8bit_Isr;   
          
          LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
          
          SPI_BYTE_OUT_8_BIT(m_Transaction); 
        }
        else
        {
          m_CurrentIsr = &Spi::Master_Tx_16bit_Isr;   
          
          LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
          
          SPI_BYTE_OUT_16_BIT(m_Transaction); 
        }           
      }
      else
      {
        m_SPIState = SPI_MASTER_RX; 
        
        if(m_Transaction.Mode == SPI_8BIT)
        {
          m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;   
          LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        }
        else
        {
          m_CurrentIsr = &Spi::Master_Rx_16bit_Isr; 
          LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        }
        m_SPIx->DR = 0xff;
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;                
      }
      LL_SPI_ClearFlag_OVR(m_SPIx);
      EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );  
      
      return m_SpiStatus; 
    }
    
    void Spi::Master_Tx_8bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_8_BIT(m_Transaction);           
      }
      else
      {               
        TxnDoneHandler_INTR();            
      }
    }
    
    void Spi::Master_Tx_16bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_16_BIT(m_Transaction); 
      }            
      else
      {               
        TxnDoneHandler_INTR();            
      }
    }
    
    void Spi::Master_Rx_8bit_Isr()
    {
      if(m_Transaction.RxLen > 0)
      {    
        SPI_BYTE_IN_8_BIT(m_Transaction);  
        
        m_SPIx->DR = 0xBA;
      }
      else
      {               
        TxnDoneHandler_INTR();            
      }
    }
    
    void Spi::Master_Rx_16bit_Isr()
    {
      if(m_Transaction.RxLen > 0)
      {    
        SPI_BYTE_IN_16_BIT(m_Transaction);   
        
        m_SPIx->DR = 0xBA;        
      }
      else
      {               
        TxnDoneHandler_INTR();            
      }
    }    
    
    void Spi::TxnDoneHandler_INTR()
    {     
      if(m_Transaction.RxLen > 0)
      {
        m_SPIState = SPI_MASTER_RX;
        
        if(m_Transaction.Mode == SPI_8BIT)
        {
          m_CurrentIsr = &Spi::Master_Rx_8bit_Isr;   
        }
        else
        {
          m_CurrentIsr = &Spi::Master_Rx_16bit_Isr; 
        }
        LL_SPI_ClearFlag_OVR(m_SPIx);
        m_SPIx->DR = 0xff; // clock out to receive data
        return;
      }
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))
      {
        m_SpiStatus = SPI_BUSY_TIMEOUT;
      }
      else
      {
        m_SpiStatus = SPI_OK;
      }
      
      SPI_CS_HIGH();
      
      if( m_Transaction.XferDoneCallback) 
        m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      
      m_SPIState = SPI_READY; 
      DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
#if SPI_MASTER_Q  
      Transaction_t const* _pCurrentTxn;  
      if(m_SPITxnQueue.AvailableEnteries())
      {
        m_SPITxnQueue.Read(&_pCurrentTxn);        
        XferIntr(_pCurrentTxn);     
      }
#endif
    }
    
    void Spi::ISR()
    {            
      if( SPI_RXNE(m_SPIx) )
      {
        (this->*m_CurrentIsr)();
      }
      else
      {
        if(LL_SPI_IsActiveFlag_OVR(m_SPIx))
        {
          LL_SPI_ClearFlag_OVR(m_SPIx);
        }
      }
    }
#else
    void Spi::ISR()
    { 
    }
#endif // SPI_MASTER_INTR
    

#if SPI_SLAVE_INTR
    
    void Spi::Slave_Tx_8bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_8_BIT(m_Transaction);            
      }
      else
      {               
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DONE;
        }        
        //LL_SPI_ClearFlag_OVR(m_SPIx);
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      }
    }
    
    void Spi::Slave_Tx_16bit_Isr()
    {
      if(m_Transaction.TxLen > 0)
      {    
        SPI_BYTE_OUT_16_BIT(m_Transaction);           
      }
      else
      {               
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DONE;
        }        
        //LL_SPI_ClearFlag_OVR(m_SPIx);
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      }
    }
    
    void Spi::Slave_Rx_8bit_Isr()
    {
      SPI_BYTE_IN_8_BIT(m_Transaction); 
      
      if(m_Transaction.RxLen == 0)
      {               
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );            
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DONE);     
      }     
    }   
    
    void Spi::Slave_Rx_16bit_Isr()
    {
      SPI_BYTE_IN_16_BIT(m_Transaction); 
      
      if(m_Transaction.RxLen == 0)
      {               
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );            
        if(m_Transaction.XferDoneCallback) m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DONE);     
      }     
    } 
    
    void Spi::SrartListeningTXIntr8bit(uint8_t* Buf, uint16_t Len)
    {    
      m_CurrentIsr = &Spi::Slave_Tx_8bit_Isr;
      m_Transaction.TxBuf = Buf;
      m_Transaction.TxLen = Len; 
      SPI_BYTE_OUT_8_BIT(m_Transaction);         
      EnableInterrupt(SPI_CR2_TXEIE);
    }
    
    void Spi::SrartListeningTXIntr16bit(uint8_t* Buf, uint16_t Len)
    {    
      m_CurrentIsr = &Spi::Slave_Tx_16bit_Isr;
      m_Transaction.TxBuf = Buf;
      m_Transaction.TxLen = Len; 
      SPI_BYTE_OUT_16_BIT(m_Transaction);         
      EnableInterrupt(SPI_CR2_TXEIE);
    }
    
    void Spi::SrartListeningRXIntr8bit(uint8_t* Buf, uint16_t Len)
    {     
      m_Transaction.RxBuf = Buf;
      m_Transaction.RxLen = Len;  
      m_CurrentIsr = &Spi::Slave_Rx_8bit_Isr;
      EnableInterrupt(SPI_CR2_RXNEIE);
    }
    
    void Spi::SrartListeningRXIntr16bit(uint8_t* Buf, uint16_t Len)
    {     
      m_Transaction.RxBuf = Buf;
      m_Transaction.RxLen = Len;  
      m_CurrentIsr = &Spi::Slave_Rx_16bit_Isr;
      EnableInterrupt(SPI_CR2_RXNEIE);
    } 
    
#endif  
    
#if SPI_MASTER_Q && (SPI_MASTER_DMA || SPI_MASTER_INTR)
    
    Spi::SpiStatus_t Spi::Post(Transaction_t const *pTransaction, uint32_t Mode)
    {      
      if(m_SPIState == SPI_READY)
      {  
        if(Mode == 0)
        {
#if (SPI_MASTER_INTR == 1)
          return XferIntr(pTransaction); 
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
        else
        {
#if (SPI_MASTER_DMA == 1)
          return XferDMA(pTransaction);
#else
          return SPI_INVALID_PARAMS; 
#endif          
        }
      }
      else
      {      
        if( (pTransaction == nullptr) || ((pTransaction->TxBuf == nullptr) && (pTransaction->RxBuf == nullptr))  )
        {                       
          return SPI_INVALID_PARAMS;                
        }        
        if( !m_SPITxnQueue.Write(pTransaction) )
        {
          return SPI_TXN_POSTED;
        }
        else 
        {                  
          return SPI_TXN_QUEUE_ERROR;
        }           
      }
    }
#endif //SPI_MASTER_Q   
    
#if SPI_MASTER_INTR || SPI_MASTER_DMA || SPI_SLAVE_INTR  || SPI_SLAVE_DMA
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(Transaction_t const *pTransaction)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if((pTransaction == 0) || ( (!pTransaction->TxBuf) && (!pTransaction->RxBuf) ))  return SPI_INVALID_PARAMS;   
      
      memcpy(&m_Transaction,pTransaction,sizeof(Transaction_t));
      
      SetBaudrate();
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;  
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    }    
    
    Spi::SpiStatus_t Spi::CheckAndLoadTxn(uint8_t* TxBuf, uint32_t TxLen,uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {
      if(m_SPIState != SPI_READY) return SPI_BUSY;
      
      if( (TxBuf == nullptr) || (RxBuf == nullptr) || ((TxLen == 0) && (RxLen == 0)) ) return SPI_INVALID_PARAMS;
      
      SetBaudrate(Spi_Baudrate);
      
      m_Transaction.Mode = mode;
      m_Transaction.TxBuf = TxBuf;
      m_Transaction.TxLen = TxLen;
      m_Transaction.RxBuf = RxBuf;
      m_Transaction.RxLen = RxLen;
      m_Transaction.pChipselect = pCS;
      m_Transaction.XferDoneCallback = XferDoneCallback;     
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      SPI_CS_LOW();
      
      return SPI_OK;      
    }    
#endif    
    
#if SPI_MASTER_DMA    
    
    Spi::SpiStatus_t Spi::TxDMA(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {   
      SpiStatus_t SpiStatus = CheckAndLoadTxn(TxBuf,TxLen,TxBuf,0,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      m_SPIState = SPI_MASTER_TX_DMA;          
      
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen); 
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::RxDMA(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
    {      
      SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,mode,Spi_Baudrate,XferDoneCallback,pCS);
      
      if(SpiStatus != SPI_OK) return SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_RX_DMA;        
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_RX(m_SPIx);
      
      LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);   
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return SPI_OK;        
    }
    
    Spi::SpiStatus_t Spi::TxRxDMA(Transaction_t const * pTransaction)
    {      
      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      LL_SPI_ClearFlag_OVR(m_SPIx);
      
      m_SPIState = SPI_MASTER_TXRX_DMA;       
      
      /* Load DMA Tx transaction*/  
      LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);
      
      /* Load DMA Rx transaction*/    
      LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      LL_SPI_EnableDMAReq_RX(m_SPIx);
      
      return m_SpiStatus;        
    } 
    
    Spi::SpiStatus_t Spi::XferDMA(Transaction_t const * pTransaction)
    {      
      m_SpiStatus = CheckAndLoadTxn(pTransaction);
      
      if(m_SpiStatus != SPI_OK) return m_SpiStatus;
      
      if(pTransaction->TxLen > 0)
      {       
        LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
        m_SPIState = SPI_MASTER_TX_DMA;
      }
      else
      {            
        LL_SPI_ClearFlag_OVR(m_SPIx);
        
        /* Load Tx DMA for generating clock for Rx*/ 
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
        LL_SPI_EnableDMAReq_RX(m_SPIx);
        
        m_SPIState = SPI_MASTER_RX_DMA;
      }    
      
      LL_SPI_EnableDMAReq_TX(m_SPIx);
      
      return m_SpiStatus;        
    } 
    
    void Spi::TxnDoneHandler_DMA()
    {  
      SpiStatus_t SpiStatus;
      
      if( (m_SPIState == SPI_MASTER_TX_DMA) && (m_Transaction.RxLen > 0) )
      {
        /* Load Tx DMA for generating clock for Rx*/ 
        LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        /* Load DMA Rx transaction*/    
        LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
        
        LL_SPI_ClearFlag_OVR(m_SPIx);
        
        m_SPIState = SPI_MASTER_RX_DMA; 
        
        LL_SPI_EnableDMAReq_RX(m_SPIx);
        LL_SPI_EnableDMAReq_TX(m_SPIx);
      }
      else
      {       
        DisableInterrupt(SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE );    
        
        if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))           SpiStatus = SPI_TXE_TIMEOUT;          
        else if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT))         SpiStatus = SPI_BUSY_TIMEOUT;
        else SpiStatus = SPI_OK;
        
        SPI_CS_HIGH();
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SpiStatus);
        
#if SPI_MASTER_Q   
        // Check if any transaction pending in Txn Queue, if yes then load the next transaction
        Transaction_t const* _pCurrentTxn;
        if(m_SPITxnQueue.AvailableEnteries())
        {         
          SPI_CS_LOW();
          
          m_SPITxnQueue.Read(&_pCurrentTxn);
          memcpy(&m_Transaction,_pCurrentTxn,sizeof(Transaction_t));
          
          SetBaudrate();
          
          if(m_Transaction.TxLen)
          {          
            LoadTxDmaChannel(m_Transaction.TxBuf,m_Transaction.TxLen);            
            m_SPIState = SPI_MASTER_TX_DMA;           
          }
          else
          {          
            LL_SPI_ClearFlag_OVR(m_SPIx);
            
            /* Load Tx DMA for generating clock for Rx*/ 
            LoadTxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen);
            
            /* Load DMA Rx transaction*/    
            LoadRxDmaChannel(m_Transaction.RxBuf,m_Transaction.RxLen); 
            LL_SPI_EnableDMAReq_RX(m_SPIx);
            
            m_SPIState = SPI_MASTER_RX_DMA;             
          }
          
          LL_SPI_EnableDMAReq_TX(m_SPIx);           
          return;
        }       
#endif    // SPI_MASTER_Q    
        m_SPIState = SPI_READY;
      }                              
    }
    
   void Spi::SPI_DMA_MasterTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if(LL_DMA_GetMode(dma1.m_DMAx,m_Curent_DMA_Tx_Channel) == LL_DMA_MODE_CIRCULAR)
        {
         /* Disable all the Half Transfer interrupt */
          dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE);
          dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE);
        }        
        
        dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
        dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);
        
        LL_SPI_DisableDMAReq_TX(m_SPIx); 
        LL_SPI_DisableDMAReq_RX(m_SPIx);      
        
        TxnDoneHandler_DMA();     
      }          
      /* Transfer Error Interrupt management **************************************/
      else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;   
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }      
    }        
    
#endif //SPI_MASTER_DMA

#if (SPI_MASTER_DMA == 1) || (SPI_SLAVE_DMA == 1)
    
    void Spi::LoadRxDmaChannel(uint8_t* Buf, uint32_t len)
    {
      dma1.Load(m_Curent_DMA_Rx_Channel, (uint32_t)&(m_SPIx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);
    }
    
    void Spi::LoadTxDmaChannel(uint8_t* Buf, uint32_t len)
    {
      dma1.Load(m_Curent_DMA_Tx_Channel, (uint32_t)&(m_SPIx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);
    }
    
#endif   
    
#if SPI_SLAVE_DMA  
    
    //SPI1_RX_DMA_CHANNEL = 2;
    void Spi::SPI_DMA_SlaveRx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG))
      {        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_TC2(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_HT2(dma1.m_DMAx); 
        
        dma1.ClearFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
        
        LL_SPI_DisableDMAReq_RX(m_SPIx);        
        
         if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(SPI_SLAVE_RX_DMA_DONE);       
      }
      else if(dma1.GetFlag(m_Curent_DMA_Rx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;  
        dma1.DisableInterrupt(m_Curent_DMA_Rx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }
    }
    
     //SPI1_TX_DMA_CHANNEL = 3;
    void Spi::SPI_DMA_SlaveTx_Done_Handler()
    {		
      /* Transfer Complete Interrupt management ***********************************/
      if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG))
      {        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_TC3(dma1.m_DMAx);
        
        /* Clear the half transfer complete flag */
        //LL_DMA_ClearFlag_HT3(dma1.m_DMAx);
        
        dma1.ClearFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TC_FLAG | HAL::DMA::DMA_HC_FLAG);  
        
        LL_SPI_DisableDMAReq_TX(m_SPIx);       
        
        if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT))
        {
          m_SpiStatus =  SPI_TXE_TIMEOUT;
        }
        else
        {
          m_SpiStatus = SPI_SLAVE_TX_DMA_DONE;
        } 
        
        if( m_Transaction.XferDoneCallback) 
          m_Transaction.XferDoneCallback->CallbackFunction(m_SpiStatus);
      }
      else if(dma1.GetFlag(m_Curent_DMA_Tx_Channel,HAL::DMA::DMA_TE_FLAG))
      {
        /* Disable all the DMA interrupt */
        m_SpiStatus = SPI_DMA_ERROR;  
        dma1.DisableInterrupt(m_Curent_DMA_Tx_Channel,DMA_CCR_HTIE | DMA_CCR_TCIE | DMA_CCR_TEIE);
      }
    }  
    
    void Spi::SrartListeningTXDma(uint8_t* Buf, uint16_t Len)
    {
     // LL_SPI_ClearFlag_OVR(m_SPIx);
      m_SPIState = SPI_SLAVE_TX_DMA;
      LoadTxDmaChannel(Buf,Len); // oopps, 198 cycles 
      LL_SPI_EnableDMAReq_TX(m_SPIx);
    }
    
    void Spi::SrartListeningRXDma(uint8_t* Buf, uint16_t Len)
    {
      LL_SPI_ClearFlag_OVR(m_SPIx);
      m_SPIState = SPI_SLAVE_RX_DMA;
      LoadRxDmaChannel(Buf,Len);
      LL_SPI_EnableDMAReq_RX(m_SPIx);
    } 
    
    void Spi::SPI_DMA_Rx_Callback::ISR( )
    {
      _this->SPI_DMA_SlaveRx_Done_Handler(); 
    } 

#endif //SPI_SLAVE_DMA
    
#if SPI_MASTER_DMA  || SPI_SLAVE_DMA   
    
    void Spi::SPI_DMA_Tx_Callback::ISR( )
    {      
#if SPI_MASTER_DMA && SPI_SLAVE_DMA 
      if((_this->m_SPIState == SPI_MASTER_TX_DMA) || (_this->m_SPIState == SPI_MASTER_RX_DMA) )
      {
        _this->SPI_DMA_MasterTx_Done_Handler();    
      }
      else
      {
        _this->SPI_DMA_SlaveTx_Done_Handler(); 
      }
#elif SPI_MASTER_DMA
      _this->SPI_DMA_MasterTx_Done_Handler();    
#elif SPI_SLAVE_DMA
      _this->SPI_DMA_SlaveTx_Done_Handler(); 
#endif
    }     
    
#endif // SPI_MASTER_DMA || SPI_SLAVE_DMA   
    
   
    
 
    

    
}


