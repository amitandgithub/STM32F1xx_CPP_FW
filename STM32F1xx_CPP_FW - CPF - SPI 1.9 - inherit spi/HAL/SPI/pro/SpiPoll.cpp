/******************
** CLASS: SpiPoll
**
** DESCRIPTION:
**  Implements the SpiPoll class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiPoll.cpp
**
*/

#include"SpiPoll.h"

namespace HAL
{   

 SpiPoll::SpiStatus_t SpiPoll::Tx(uint8_t data)
    {     
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT; 
      
      LL_SPI_SetDataWidth(m_SPIx,LL_SPI_DATAWIDTH_8BIT);    
      
      m_SPIx->DR = data;            
      
      if(SPI_WAIT_FOR_TXE_FLAG_TO_SET(m_SPIx,SPI_TIMEOUT)) return SPI_TXE_TIMEOUT;
      
      if(SPI_WAIT_FOR_BUSY_FLAG_TO_CLEAR(m_SPIx,SPI_TIMEOUT)) return SPI_BUSY_TIMEOUT;
      
      return SPI_OK;
    }
    
    SpiPoll::SpiStatus_t SpiPoll::Tx(uint16_t data, XferMode_t mode)
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
    
    SpiPoll::SpiStatus_t SpiPoll::Tx(uint32_t data, XferMode_t mode)
    {           
      SpiStatus_t SpiStatus = Tx((uint16_t)data, mode);
      
      if(SpiStatus != SPI_OK ) return SpiStatus;
      
      return Tx((uint16_t)(data>>16), mode);
    }
    
    SpiPoll::SpiStatus_t SpiPoll::TxPoll(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode)
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
    
    SpiPoll::SpiStatus_t SpiPoll::RxPoll(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode)
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
    
    SpiPoll::SpiStatus_t SpiPoll::TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, XferMode_t mode)
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
    
    SpiPoll::SpiStatus_t SpiPoll::XferPoll(Transaction_t const *pTransaction)
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


}