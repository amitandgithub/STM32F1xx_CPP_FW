/******************
** CLASS: SpiIntr
**
** DESCRIPTION:
**  Implements the SpiIntr class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiIntr.h
**
******************/
#ifndef SpiIntr_h
#define SpiIntr_h


#include"SpiBase.h"

namespace HAL
{    

  
  class SpiIntr : public InterruptSource, public SpiBase
  {
    public:
    
    SpiIntr();
      
    SpiStatus_t     TxIntr(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode = SPI_8BIT, Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     RxIntr(uint8_t* RxBuf, uint32_t RxLen,  XferMode_t mode = SPI_8BIT,Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr, GpioOutput* pCS = nullptr);
    
    SpiStatus_t     TxRxIntr(uint8_t* TxBuf, uint32_t TxLen, uint8_t* RxBuf, uint32_t RxLen, Spi_Baudrate_t Spi_Baudrate = SPI_BAUDRATE_DIV4, SPICallback_t XferDoneCallback = nullptr);
    
    SpiStatus_t     TxRxIntr(Transaction_t const * pTransaction);    
    
    SpiStatus_t     XferIntr(Transaction_t const *pTransaction);
    
    void TxnDoneHandler_INTR();
    
    void Master_Tx_8bit_Isr();
    
    void Master_Rx_8bit_Isr();
    
    void Master_Tx_16bit_Isr();
    
    void Master_Rx_16bit_Isr();
    
    virtual void ISR();
    
  private:
     CurrentIsr m_CurrentIsr;    
  };  
}

#endif //SpiIntr_h