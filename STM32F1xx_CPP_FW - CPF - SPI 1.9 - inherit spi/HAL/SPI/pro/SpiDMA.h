/******************
** CLASS: SpiDMA
**
** DESCRIPTION:
**  Implements the Spi Base class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiBase.h
**
******************/
#ifndef SpiDMA_h
#define SpiDMA_h


#include"SpiBase.h"

namespace HAL
{    

  
  class SpiDMA 
  {
    public:
    
    SpiDMA();
      
    void LoadRxDmaChannel(uint8_t* Buf, uint32_t len){dma1.Load(m_Curent_DMA_Rx_Channel, (uint32_t)&(m_SPIx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_PERIPH_TO_MEMORY, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);}
    
    void LoadTxDmaChannel(uint8_t* Buf, uint32_t len){dma1.Load(m_Curent_DMA_Tx_Channel, (uint32_t)&(m_SPIx->DR),(uint32_t)Buf,len, LL_DMA_DIRECTION_MEMORY_TO_PERIPH, m_Transaction.Mode == SPI_8BIT ? LL_DMA_PDATAALIGN_BYTE : LL_DMA_PDATAALIGN_HALFWORD);}    
    
    class SPI1_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Tx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };
    
    class SPI2_DMA_Tx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Tx_Callback(Spi* This):_this(This){};
      virtual void ISR( ) ;
    private:
      Spi* _this;
    };
    
    class SPI1_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI1_DMA_Rx_Callback(Spi* This):_this(This){};
      virtual void ISR() ;
    private:
      Spi* _this;
    };    
    
    class SPI2_DMA_Rx_Callback : public InterruptSource
    {
    public:
      SPI2_DMA_Rx_Callback(Spi* This):_this(This){};
      virtual void ISR( ) ;
    private:
      Spi* _this;
    };
    
    void SPI1_DMA_SlaveTx_Done_Handler();
    
    void SPI1_DMA_SlaveRx_Done_Handler();
    
    void SPI2_DMA_SlaveTx_Done_Handler();
    
    void SPI2_DMA_SlaveRx_Done_Handler();  
    
    void SrartListeningTXDma(uint8_t* Buf, uint16_t Len);
    
    void SrartListeningRXDma(uint8_t* Buf, uint16_t Len);    
  };  
}

#endif //SpiDMA_h