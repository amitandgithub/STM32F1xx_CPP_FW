

#include"I2C_Test.h"



I2c I2CDevIntr(Gpio::B6, Gpio::B7,100000U);
I2c::I2CStatus_t Status;
uint8_t RepeatedStart = 0;
uint16_t SlaveAddress = 0x0010;
uint8_t TxBuf[100];
uint8_t RxBuf[100];
uint16_t TxLen,RxLen;
HAL::I2c::Transaction_t Transaction;
I2CCallback_t I2CCallback;


void I2c_Tests()
{

  uint32_t I2c_test_id = 0;
  
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  I2CDevIntr.HwInit();
  
  I2CDevIntr.ScanBus(RxBuf,20);
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_POLL_TX_1_RX_1;//I2C_POLL_TX_1_RX_1;
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
#if I2C_POLL
  I2CDevIntr.XferPoll(SlaveAddress,TxBuf,2); 
#endif
  TxBuf[0] = 8;
  int Posts=0;
  for(uint8_t i = 1; i<70; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    { 
#if I2C_POLL
    case I2C_POLL_TX_1_RX_1:
      TxLen = RxLen = 1;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_2;
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = RxLen = 2;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;  
      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = RxLen = 3;
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);   
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_INT_POLL_40_RX_40;
      break;
      
    case I2C_INT_POLL_40_RX_40: 
      TxLen = RxLen = 40;           
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      I2CDevIntr.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);            
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_POLL_40_RX_40 = Pass"), STR("I2C_INT_POLL_40_RX_40 = Fail"));
#endif
#if I2C_MASTER_INTR
      I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
      break;
case I2C_INT_TX_1_RX_1_TXN:
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 1;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_1_TXN = Pass"), STR("I2C_INT_TX_1_RX_1_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_2_TXN;
      break;    
    case I2C_INT_TX_1_RX_2_TXN:
      count = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_2_TXN = Pass"), STR("I2C_INT_TX_1_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_3_TXN;
      break; 
    case I2C_INT_TX_1_RX_3_TXN:
      count = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_3_TXN = Pass"), STR("I2C_INT_TX_1_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN;
      break;
    case I2C_INT_TX_2_RX_2_TXN:  
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN = Pass"), STR("I2C_INT_TX_2_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN;
      break;            
    case I2C_INT_TX_3_RX_3_TXN:  
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN = Pass"), STR("I2C_INT_TX_3_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN;
      break;    
    case I2C_INT_TX_40_RX_40_TXN:
      // This test case tests the Transaction and the Callback functionality
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN = Pass"), STR("I2C_INT_TX_40_RX_40_TXN = Fail"));
#endif
#if I2C_MASTER_Q && ((I2C_MASTER_INTR) || (I2C_MASTER_DMA))
      I2c_test_id = I2C_INT_TX_QUEUE;			
      break;
    case I2C_INT_TX_QUEUE:

      Posts=0;
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 4;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      Status = I2CDevIntr.Post(&Transaction);
      while(I2CDevIntr.Post(&Transaction) != I2c::I2C_TXN_QUEUE_ERROR)
      {
        Posts++;
      }      
      while(I2CCallback.get_XferComplete() < (count+Posts))
      {
        printf("Q posts = %d \t ISR Count = %d \n",Posts, (count+Posts));
      }
      
      Test_Condition( 1, STR("I2C_INT_TX_QUEUE = Pass"), STR("I2C_INT_TX_QUEUE = Fail"));
      
#endif
#if I2C_MASTER_DMA
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;			
      break;
    case I2C_INT_TX_2_RX_2_TXN_DMA: 
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferDMA(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferDMA(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Pass"), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN_DMA;
      break;            
    case I2C_INT_TX_3_RX_3_TXN_DMA:  
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferDMA(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferDMA(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Pass"), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN_DMA;
      break;
      case I2C_INT_TX_40_RX_40_TXN_DMA:
      // This test case tests the Transaction and the Callback functionality
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferDMA(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferDMA(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Pass"), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Fail"));
      I2c_test_id = 200;
      break;
#endif     
    default:         
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        printf("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        return;
      }
      
      break;
      
    }
    
    MemSet(RxBuf,0,100);
    LL_mDelay(1);
  }
}


bool Test_Condition(bool condition, char* PassStr, char* FailStr)
{
  if(condition)
  {
    printf(PassStr);
    printf("\n");
    return true;
  }
  else
  {
    printf(FailStr);
    printf("\n");
    return true;
  }   
  
}

void MemSet(uint8_t* mem, uint8_t data, uint32_t size)
{
  while(size--) *mem++ = data;
}

#if I2C_SLAVE_DMA
uint32_t Total_Rx_Bytes = 0;
uint32_t Total_Tx_Bytes = 0;
uint16_t I2C_Slave_Tx_Done_Count = 0;        
uint8_t I2C_Slave_DMA_Array[50];
I2c::i2cBuf_t I2C_Slave_TxBuf = {&I2C_Slave_DMA_Array[0],50,0};
I2c::i2cBuf_t I2C_Slave_RxBuf = {&I2C_Slave_DMA_Array[0],50,0};

I2C_Slave_DMA_Callback_t I2C_Slave_DMA_Callback;

void I2C_Slave_DMA_Test()
{  
  I2CDevIntr.HwInit();
  
  I2CDevIntr.SetSlaveTxDefaultByte(0xff);
  
  I2CDevIntr.SetSlaveCallback(&I2C_Slave_DMA_Callback);
    
  while(I2CDevIntr.SlaveStartListening_DMA(&I2C_Slave_TxBuf, &I2C_Slave_RxBuf) != HAL::I2c::I2C_OK);
}

// from I2c ISR - I2C_Slave_RxBuf.Idx =  I2C_Slave_RxBuf.Len - DMA_Counter;
void I2C_Slave_DMA_Callback_t::CallbackFunction(HAL::I2c::I2CStatus_t I2CStatus)
 {
  if(I2CStatus == HAL::I2c::I2C_SLAVE_RX_DONE) 
  {    
    Total_Rx_Bytes = Total_Rx_Bytes + I2C_Slave_RxBuf.Idx;
    I2C_Slave_RxBuf.Buf[0] += 1; 
    I2CDevIntr.ReloadRxDmaChannel((uint8_t*)&I2C_Slave_RxBuf,50);
  }
  else if(I2CStatus == HAL::I2c::I2C_SLAVE_TX_DONE)
  {           
      Total_Tx_Bytes = Total_Tx_Bytes + I2C_Slave_TxBuf.Idx;
      I2CDevIntr.ReloadTxDmaChannel((uint8_t*)&I2C_Slave_RxBuf,50);
  }
  else
  {
    while(1);
  }  
 }

#endif //  I2C_SLAVE_DMA

void I2c_Slave_Tests()
{
#if I2C_SLAVE_INTR
  Cmdsvr_Init();
  while(1)
  {
    CmdSvr_Run(); 
  }
#elif I2C_SLAVE_DMA
  I2C_Slave_DMA_Test();
      while(1);
#endif
  
}