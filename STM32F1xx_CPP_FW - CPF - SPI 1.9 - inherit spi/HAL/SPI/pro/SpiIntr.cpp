/******************
** CLASS: SpiIntr
**
** DESCRIPTION:
**  Implements the SpiIntr class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiIntr.cpp
**
*/

#include"SpiIntr.h"

namespace HAL
{   
  
  SpiIntr::SpiIntr(): m_CurrentIsr(nullptr)
  {
    
  }  
  
  Spi::SpiStatus_t Spi::HwInit(bool Slave, XferMode_t mode)
  {    
    if(m_SPIx == SPI1)
    {      
      InterruptManagerInstance.RegisterDeviceInterrupt(SPI1_IRQn,0,this);
    }       
    else if(m_SPIx == SPI2)
    {
      InterruptManagerInstance.RegisterDeviceInterrupt(SPI2_IRQn,0,this);
    }   
    else
    {
      while(1); // Fatal Error
    }
    
    return SPI_OK;            
  }        
  
  SpiIntr::SpiStatus_t SpiIntr::TxIntr(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
  {     
    SpiStatus_t SpiStatus = CheckAndLoadTxn(TxBuf,TxLen,TxBuf,0,mode,Spi_Baudrate,XferDoneCallback,pCS);
    
    if(SpiStatus != SPI_OK) return SpiStatus;
    
    m_SPIState = SPI_MASTER_TX;  
    
    if(mode == SPI_8BIT)
    {
      m_CurrentIsr = &SpiIntr::Master_Tx_8bit_Isr;
      
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
      
      SPI_BYTE_OUT_8_BIT(m_Transaction); 
    }
    else
    {
      m_CurrentIsr = &SpiIntr::Master_Tx_16bit_Isr;
      
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
      
      SPI_BYTE_OUT_16_BIT(m_Transaction); 
    }
    
    EnableInterrupt(SPI_CR2_TXEIE);
    
    return SPI_OK;        
  }
  
  SpiIntr::SpiStatus_t SpiIntr::RxIntr(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode,Spi_Baudrate_t Spi_Baudrate, SPICallback_t XferDoneCallback,GpioOutput* pCS)
  {      
    SpiStatus_t SpiStatus = CheckAndLoadTxn(RxBuf,0,RxBuf,RxLen,mode,Spi_Baudrate,XferDoneCallback,pCS);
    
    if(SpiStatus != SPI_OK) return SpiStatus;
    
    LL_SPI_ClearFlag_OVR(m_SPIx);
    
    m_SPIState = SPI_MASTER_RX; 
    
    if(mode == SPI_8BIT)
    {
      m_CurrentIsr = &SpiIntr::Master_Rx_8bit_Isr;         
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
    }
    else
    {
      m_CurrentIsr = &SpiIntr::Master_Rx_16bit_Isr;
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
    }
    m_SPIx->DR = 0xff;
    if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
    
    EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );
    
    return SPI_OK;        
  }
  
  SpiIntr::SpiStatus_t SpiIntr::TxRxIntr(Transaction_t const * pTransaction)
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
  
  SpiIntr::SpiStatus_t SpiIntr::XferIntr(Transaction_t const *pTransaction)
  {      
    m_SpiStatus = CheckAndLoadTxn(pTransaction);
    
    if(m_SpiStatus != SPI_OK) return m_SpiStatus;
    
    if(m_Transaction.TxLen)
    {        
      m_SPIState = SPI_MASTER_TX; 
      
      if(m_Transaction.Mode == SPI_8BIT)
      {
        m_CurrentIsr = &SpiIntr::Master_Tx_8bit_Isr;   
        
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
        
        SPI_BYTE_OUT_8_BIT(m_Transaction); 
      }
      else
      {
        m_CurrentIsr = &SpiIntr::Master_Tx_16bit_Isr;   
        
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
        
        SPI_BYTE_OUT_16_BIT(m_Transaction); 
      }           
    }
    else
    {
      m_SPIState = SPI_MASTER_RX; 
      
      if(m_Transaction.Mode == SPI_8BIT)
      {
        m_CurrentIsr = &SpiIntr::Master_Rx_8bit_Isr;   
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);
      }
      else
      {
        m_CurrentIsr = &SpiIntr::Master_Rx_16bit_Isr; 
        LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_16BIT);
      }
      m_SPIx->DR = 0xff;
      if(SPI_WAIT_FOR_RXNE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;                
    }
    LL_SPI_ClearFlag_OVR(m_SPIx);
    EnableInterrupt(SPI_CR2_RXNEIE | SPI_CR2_ERRIE );  
    
    return m_SpiStatus; 
  }
  
  void SpiIntr::Master_Tx_8bit_Isr()
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
  
  void SpiIntr::Master_Tx_16bit_Isr()
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
  
  void SpiIntr::Master_Rx_8bit_Isr()
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
  
  void SpiIntr::Master_Rx_16bit_Isr()
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
  
  void SpiIntr::TxnDoneHandler_INTR()
  {     
    if(m_Transaction.RxLen > 0)
    {
      m_SPIState = SPI_MASTER_RX;
      
      if(m_Transaction.Mode == SPI_8BIT)
      {
        m_CurrentIsr = &SpiIntr::Master_Rx_8bit_Isr;   
      }
      else
      {
        m_CurrentIsr = &SpiIntr::Master_Rx_16bit_Isr; 
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
#if 0 //SPI_MASTER_Q  
    Transaction_t const* _pCurrentTxn;  
    if(m_SPITxnQueue.AvailableEnteries())
    {
      m_SPITxnQueue.Read(&_pCurrentTxn);        
      XferIntr(_pCurrentTxn);     
    }
#endif
  }
  
  void SpiIntr::ISR()
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
  
  
}