/******************
** CLASS: Irda
**
** DESCRIPTION:
**  Implements the Irda driver
**
** CREATED: 4 Feb 2020, by Amit Chaudhary
**
** FILE: Irda.h
**
******************/
#ifndef Irda_h
#define Irda_h

#include"Uart.h"


namespace HAL
{    
  
  class Irda : public Uart
  {
  public:
    
    Irda(UartPort_t uartport,uint32_t Baudrate);
    
    ~Irda(){};
    
    UartStatus_t HwInit();
    
    UartStatus_t HwDeinit();  
    
    void Enable(){LL_USART_EnableIrda(Uart::m_UARTx);}
    
    void Disable(){LL_USART_DisableIrda(Uart::m_UARTx);}
    
  };
  
} 


#endif