/******************
** CLASS: SpiPoll
**
** DESCRIPTION:
**  Implements the SpiPoll class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiPoll.h
**
******************/
#ifndef SpiPoll_h
#define SpiPoll_h


#include"SpiBase.h"

namespace HAL
{    

  
  class SpiPoll : public SpiBase
  {
    public:
      
    SpiStatus_t     Tx(uint8_t data);   
    
    SpiStatus_t     Tx(uint16_t TxBuf, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     Tx(uint32_t TxBuf, XferMode_t mode = SPI_8BIT);
      
    SpiStatus_t     TxPoll(uint8_t* TxBuf, uint32_t TxLen, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     RxPoll(uint8_t* RxBuf, uint32_t RxLen, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     TxRxPoll(uint8_t* TxBuf, uint8_t* RxBuf, uint32_t Len, XferMode_t mode = SPI_8BIT);
    
    SpiStatus_t     XferPoll(Transaction_t const *pTransaction);    
  };  
}

#endif //SpiPoll_h