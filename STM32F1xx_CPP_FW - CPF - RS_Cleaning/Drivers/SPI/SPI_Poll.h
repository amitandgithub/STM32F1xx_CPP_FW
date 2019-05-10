/******************
** CLASS: SPI 
**
** DESCRIPTION:
**  SPI implementation in polling mode
**
** CREATED: 9/2/2018, by Amit Chaudhary
**
** FILE: SPI.h
**
******************/
#ifndef SPI_h
#define SPI_h


#include "SPI_Base.h"

namespace Peripherals
{
    
class SPI_Poll : public SPI_Base
{
public:
    static const uint32_t SPI_POLL_DELAY = 0x1000UL;
    
    SPI_Poll (SPIx_t spix, GpioOutput* CS, HZ_t hz = 100000UL,SPI_HandleTypeDef* phspi_x = nullptr);
    
    virtual ~SPI_Poll (){HwDeinit();};
    
    virtual       Status_t        HwInit          ();
    
    virtual       Status_t        HwDeinit        (){ return 1;}
    
    virtual       Status_t        Tx            (uint8_t* pTxBuf, uint16_t TxLen);
    
    virtual       Status_t        Rx            (uint8_t* pRxBuf, uint16_t RxLen);  
    
    virtual       Status_t        TxRx          (uint8_t* pTxBuf, uint8_t* pRxBuf, uint16_t Len); 
    
    virtual       Status_t        Xfer          (Transaction_t* aTransaction);
    
    virtual       Status_t        Post          (Transaction_t* pTransaction);
    
    virtual       Status_t        Post          (Transaction_t aTransaction);
    
                  SPIx_t          GetSPIx         (){return m_spix;};
    
private: 
    SPIx_t          m_spix;
    HZ_t            m_hz;  
};

}
#endif //SPI_h
