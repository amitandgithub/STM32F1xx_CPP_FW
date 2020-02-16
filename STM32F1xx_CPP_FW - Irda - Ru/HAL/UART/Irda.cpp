/******************
** FILE: Irda.cpp
**
** DESCRIPTION:
**  Irda implementation
**
** CREATED: 8/11/2019, by Amit Chaudhary
******************/

#include "Irda.h"

namespace HAL
{
  
  
  Irda::Irda(UartPort_t uartport,uint32_t Baudrate) : Uart(uartport,Baudrate)
  {
    
  }
  
  
  Irda::UartStatus_t Irda::HwInit()
  {
    Uart::HwInit(115200);
    
    Disable();
    
    /*
    In IrDA
      mode, the following bits must be kept cleared:
      • LINEN, STOP and CLKEN bits in the USART_CR2 register,
      • SCEN and HDSEL bits in the USART_CR3 register.
    */

    CLEAR_BIT(Uart::m_UARTx->CR2, USART_CR2_STOP | USART_CR2_LINEN | USART_CR2_CLKEN);
    
    //CLEAR_BIT(Uart::m_UARTx->CR1, USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE);
    
    CLEAR_BIT(Uart::m_UARTx->CR3, USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_RTSE | USART_CR3_CTSE);  
  
    Enable();
    
  return Uart::UART_OK;
    
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}