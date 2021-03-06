/******************
** CLASS: SPI 
**
** DESCRIPTION:
**  SPI implementation in Interrupt mode
**
** CREATED: 9/2/2018, by Amit Chaudhary
**
** FILE: SPI.h
**
******************/
#ifndef SPI_DMA_h
#define SPI_DMA_h


#include "SPI_Base.h"
#include "Interrupt.hpp"
#include "GpioOutput.hpp"

namespace Peripherals
{


class SPI_DMA : public SPI_Base, public Interrupt 
{
public:
//    using Queue = Queue<Transaction_t*,10> ;
    
    static const uint32_t SPI_POLL_DELAY = 0xFFUL;
    
    SPI_DMA (SPIx_t spix, GpioOutput* CS, HZ_t hz = 100000UL,
             SPI_HandleTypeDef* phspi_x = nullptr, 
             DMA_HandleTypeDef* phdma_spi1_rx_x = nullptr,
             DMA_HandleTypeDef* phdma_spi1_tx_x = nullptr);
    
    virtual ~SPI_DMA (){HwDeinit();};
    
    virtual       Status_t        HwInit          ();
    
    virtual       Status_t        HwDeinit        (){ return HAL_SPI_DeInit(m_phspi_1);}
    
    virtual       Status_t        Tx              (uint8_t* pTxBuf, uint16_t TxLen);
    
    virtual       Status_t        Rx              (uint8_t* pRxBuf, uint16_t RxLen);  
    
    virtual       Status_t        TxRx            (uint8_t* pTxBuf, uint8_t* pRxBuf, uint16_t Len); 
    
    virtual       Status_t        Xfer            (Transaction_t* aTransaction);
    
    virtual       Status_t        Post            (Transaction_t aTransaction);
                  
    virtual       Status_t        Post            (Transaction_t* pTransaction); 
                  
                  void            Run             ();
             
                  SPIx_t           GetSPIx         (){return m_spix;};
                  


private: 
    SPIx_t m_spix;
    uint32_t m_hz;
};

}
#endif //SPI_DMA_h
