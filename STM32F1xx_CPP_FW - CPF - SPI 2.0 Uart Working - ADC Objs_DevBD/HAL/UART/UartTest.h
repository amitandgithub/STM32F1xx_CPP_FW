

#include "Uart.h"


void Uart_DMA_Test();

extern HAL::Uart uart1;
uint8_t Uart_RxBuf[20] = "Amit Chaudhary\n\r";

volatile static uint8_t uart_Rx_Done=0;

class UartCallback_t : public HAL::Uart::uartCallback
{
public:
  virtual void CallbackFunction(HAL::Uart::UartStatus_t Status)
  {
    uart_Rx_Done = 1;
    Uart_RxBuf[sizeof("Amit Chaudhary\n\r") - 1] = '\r';
    Uart_RxBuf[sizeof("Amit Chaudhary\n\r")] = '\n';
  }
};

UartCallback_t UartCallback;

void Uart_Test()
{  
  static uint16_t Error = 0;
  static bool InitDone;  
  if(InitDone == false)
  {
    uart1.HwInit();
    InitDone = true;
  }
  
  uart1.TxPoll(0x31,0x32);
  uart1.TxPoll((uint8_t)'\n');
  uart1.TxPoll(0x33,0x34,0x35,0x36);
  uart1.TxPoll((uint8_t)'\n');
  while(1)
  {
    // Poll test
   if( uart1.TxPoll((uint8_t*)"Amit Chaudhary\n\r", sizeof("Amit Chaudhary\n\r")) != HAL::Uart::UART_OK) Error++;   
    
#if UART_INTR
    // Interrupt Test
    uart_Rx_Done = 0;
    uart1.RxIntr(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
    while(uart_Rx_Done == 0);    
    
    LL_mDelay(500);
    uart1.TxIntr(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
    while(uart1.GetState() != HAL::Uart::UART_READY );
    LL_mDelay(500); 
#endif

#if UART_DMA
  // DMA Test
  uart_Rx_Done = 0;
  uart1.RxDMA(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
  while(uart_Rx_Done == 0);    

  uart1.TxDMA(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
  while(uart1.GetState() != HAL::Uart::UART_READY );  
#endif
 
  }
  
}

