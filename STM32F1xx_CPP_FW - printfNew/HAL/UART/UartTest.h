

#include "Uart.h"
#include "Irda.h"

#define UART_X uart1
void Uart_DMA_Test();

extern HAL::Uart uart1;
HAL::Irda Irda_Uart2(UART1_A9_A10,9600);

uint8_t Uart_RxBuf[200] = "Amit Chaudhary\n\r";

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
  static bool InitDone;  
  if(InitDone == false)
  {
    //Irda_Uart2.HwInit();
    UART_X.HwInit();
    InitDone = true;
  }
  
//  UART_X.TxPoll(0x31,0x32);
//  UART_X.TxPoll((uint8_t)'\n');
//  UART_X.TxPoll(0x33,0x34,0x35,0x36);
//  UART_X.TxPoll((uint8_t)'\n');
  while(1)
  {
    // Poll test
   //if( UART_X.TxPoll((uint8_t*)"Amit Chaudhary\n\r", sizeof("Amit Chaudhary\n\r")) != HAL::Uart::UART_OK) Error++;   
    
#if UART_INTR
    // Interrupt Test
    uart_Rx_Done = 0;
    UART_X.RxIntr(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
    while(uart_Rx_Done == 0);    
    
    LL_mDelay(500);
    UART_X.TxIntr(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
    while(UART_X.GetState() != HAL::Uart::UART_READY );
    LL_mDelay(500); 
#endif

#if UART_DMA
  // DMA Test
  uart_Rx_Done = 0;
  UART_X.RxDMA(Uart_RxBuf, sizeof("Amit Chaudhary")-1,&UartCallback);
  while(uart_Rx_Done == 0);    

  UART_X.TxDMA(Uart_RxBuf, sizeof("Amit Chaudhary\n\r"));    
  while(UART_X.GetState() != HAL::Uart::UART_READY );  
#endif
 
  }
  
}

