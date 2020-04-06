

#include"I2C_Test.h"
#include "System.h"

//I2c i2c1(Gpio::B6, Gpio::B7,100000U);
I2c::I2CStatus_t Status;
uint8_t RepeatedStart = 0;
uint16_t SlaveAddress = 0x0010;
uint8_t TxBuf[100];
uint8_t TxBuf_AT24C128[] = {0,0,'A','m','i','t',' ','C','h','a','u','d','h','a','r','y',0};
uint8_t RxBuf[100];
uint16_t TxLen,RxLen;
HAL::I2c::Transaction_t Transaction;
I2CCallback_t I2CCallback;
uint32_t Callbackcount;


bool Test_Condition(bool condition, char* PassStr, char* FailStr)
{
  if(condition)
  {
    I2C_PRINT(PassStr);
    I2C_PRINT("\n\r");
    return true;
  }
  else
  {
    I2C_PRINT(FailStr);
    I2C_PRINT("\n\r");
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
  i2c1.HwInit(400000);
  
  i2c1.SetSlaveTxDefaultByte(0xff);
  
  i2c1.SetSlaveCallback(&I2C_Slave_DMA_Callback);
  
  while(i2c1.SlaveStartListening_DMA(&I2C_Slave_TxBuf, &I2C_Slave_RxBuf) != HAL::I2c::I2C_OK);
}

// from I2c ISR - I2C_Slave_RxBuf.Idx =  I2C_Slave_RxBuf.Len - DMA_Counter;
void I2C_Slave_DMA_Callback_t::CallbackFunction(HAL::I2c::I2CStatus_t I2CStatus)
{
  if(I2CStatus == HAL::I2c::I2C_SLAVE_RX_DONE) 
  {    
    Total_Rx_Bytes = Total_Rx_Bytes + I2C_Slave_RxBuf.Idx;
    I2C_Slave_RxBuf.Buf[0] += 1; 
    //i2c1.LoadRxDmaChannel(I2C_Slave_RxBuf.Buf,50);

//    LL_DMA_DisableChannel(DMA1, 7);
//    LL_DMA_SetDataLength(DMA1, 7, 50); 
//    LL_DMA_EnableChannel(DMA1, 7);
    
    i2c1.LoadRxDmaChannel(50);
    //i2c1.SetDMADataLen_Ch7(50);
  }
  else if(I2CStatus == HAL::I2c::I2C_SLAVE_TX_DONE)
  {           
    Total_Tx_Bytes = Total_Tx_Bytes + I2C_Slave_TxBuf.Idx;
    
    //i2c1.LoadTxDmaChannel(I2C_Slave_TxBuf.Buf,50);

//    LL_DMA_DisableChannel(DMA1, 6);
//    LL_DMA_SetDataLength(DMA1, 6, 50); 
//    LL_DMA_EnableChannel(DMA1, 6);
    i2c1.LoadTxDmaChannel(50);
    
    //i2c1.SetDMADataLen_Ch6(50);
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

void I2c_Tests_AT24C128()
{
  
  uint32_t I2c_test_id = 0;
  
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  i2c1.HwInit(400000);
  
  i2c1.ScanBus(RxBuf,20);
  SlaveAddress = 0xA0;
  
#if I2C_MASTER_INTR
  I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
#elif I2C_MASTER_DMA
  I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;
#else
  I2c_test_id = I2C_POLL_TX_1_RX_1;
#endif
  
  TxBuf[0] = 0;
  TxBuf[1] = 0;
  for(uint8_t i = 2; i<62; i++) TxBuf[i] = 'A'+ i - 2;
  
  while(1)
  {
    switch(I2c_test_id)
    { 
    case I2C_POLL_TX_1_RX_1:
      TxLen = RxLen = 1;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_2;      
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = RxLen = 2;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;  
      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = RxLen = 3;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart);   
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_40_RX_40;
      break;
      
    case I2C_POLL_40_RX_40: 
      TxLen = RxLen = 40;           
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+2);
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart);            
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_POLL_40_RX_40 = Pass"), STR("I2C_POLL_40_RX_40 = Fail"));
#if I2C_MASTER_INTR
      I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
      break;
    case I2C_INT_TX_1_RX_1_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 1;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_1_TXN = Pass"), STR("I2C_INT_TX_1_RX_1_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_2_TXN;
      break;    
    case I2C_INT_TX_1_RX_2_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_2_TXN = Pass"), STR("I2C_INT_TX_1_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_3_TXN;
      break; 
    case I2C_INT_TX_1_RX_3_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_3_TXN = Pass"), STR("I2C_INT_TX_1_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN;
      break;
    case I2C_INT_TX_2_RX_2_TXN:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN = Pass"), STR("I2C_INT_TX_2_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN;
      break;            
    case I2C_INT_TX_3_RX_3_TXN:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN = Pass"), STR("I2C_INT_TX_3_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN;
      break;    
    case I2C_INT_TX_40_RX_40_TXN:
      // This test case tests the Transaction and the Callback functionality
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN = Pass"), STR("I2C_INT_TX_40_RX_40_TXN = Fail"));
#endif
      
#if I2C_MASTER_DMA
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;			
      break;
    case I2C_INT_TX_2_RX_2_TXN_DMA: 
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Pass"), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN_DMA;
      break;            
    case I2C_INT_TX_3_RX_3_TXN_DMA:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Pass"), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN_DMA;
      break;
    case I2C_INT_TX_40_RX_40_TXN_DMA:
      // This test case tests the Transaction and the Callback functionality
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+2;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 2;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      LL_mDelay(AT24C128_READ_WRITE_DELAY);
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Pass"), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Fail"));
      
#endif   //I2C_MASTER_DMA  
    default:         
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        I2C_PRINT("\n\n\rRe-Testing test cases with repeated start \n\n\r");
      }
      else
      {
        //I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        return;
      }
      
      break;
      
    }
    TxBuf[0] += 1;
    
    if(TxBuf[1] == 255) TxBuf[1] = 0;
    
    MemSet(RxBuf,0,sizeof(RxBuf));
    LL_mDelay(100);
  }
}


void I2c_Full_EEPROM_POLL()
{
  uint16_t block;
  RxLen = TxLen = 64;
  TxBuf[0] = 0;
  
  i2c1.HwInit(400000);
  
  for(uint8_t i = 2; i<65; i++) TxBuf[i] = i - 2;
  
  for(block = 0; block < 0xffu; block++)
  {    
    TxBuf[0]  = block;
    i2c1.XferPoll(0xA0,TxBuf,TxLen+2);
    LL_mDelay(AT24C128_READ_WRITE_DELAY);
    
    if(block%17 == 0)
    {
      I2C_PRINT("\n.");
    }
    else
      I2C_PRINT(".");
    
  }  
  
  for(block = 0; block < 0xffu; block++)
  {    
    TxBuf[0]  = block;
    i2c1.XferPoll(0xA0,TxBuf,TxLen+2);
    MemSet(RxBuf,0,sizeof(RxBuf));
    LL_mDelay(AT24C128_READ_WRITE_DELAY);
    i2c1.XferPoll(0xA0,TxBuf,2,RxBuf,RxLen); 
    
    
    if( !std::memcmp( (const void*) &TxBuf[2],(const void*) RxBuf, TxLen ) )
      I2C_PRINT(".");
    else
    {
      I2C_PRINT("Block = %d",block);
      I2C_PRINT(" ->Fail \n");
    }
    
    
  }
  I2C_PRINT("\n Done \n");
}

void I2c_Tests()
{
  
  uint32_t I2c_test_id = 0;
  
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  i2c1.HwInit(400000);
  
  i2c1.ScanBus(RxBuf,20);
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_POLL_TX_1_RX_1;//I2C_POLL_TX_1_RX_1;
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
#if I2C_POLL
  i2c1.XferPoll(SlaveAddress,TxBuf,2); 
#endif
  TxBuf[0] = 8;

  for(uint8_t i = 1; i<70; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    { 
#if I2C_POLL
    case I2C_POLL_TX_1_RX_1:
      TxLen = RxLen = 1;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_1 = Pass"), STR("I2C_POLL_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_2;
      break;    
    case I2C_POLL_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_2 = Pass"), STR("I2C_POLL_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_1_RX_3;
      break; 
    case I2C_POLL_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_1_RX_3 = Pass"), STR("I2C_POLL_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_TX_2_RX_2;
      break; 
    case I2C_POLL_TX_2_RX_2:  
      TxLen = RxLen = 2;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart); 
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_2_RX_2 = Pass"), STR("I2C_POLL_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3;
      break;  
      
    case I2C_POLL_TX_3_RX_3:  
      TxLen = RxLen = 3;
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,2,RxBuf,RxLen,RepeatedStart);   
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_TX_3_RX_3 = Pass"), STR("I2C_POLL_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_POLL_40_RX_40;
      break;
      
    case I2C_POLL_40_RX_40: 
      TxLen = RxLen = 40;           
      i2c1.XferPoll(SlaveAddress,TxBuf,TxLen+1);
      i2c1.XferPoll(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);            
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_POLL_40_RX_40 = Pass"), STR("I2C_POLL_40_RX_40 = Fail"));
      I2c_test_id = I2C_POLL_TX_3_RX_3_AT24C128;
      break;
      
    case I2C_POLL_TX_3_RX_3_AT24C128:
      RxLen =  TxLen = 2 + sizeof(TxBuf_AT24C128);
      SlaveAddress = 0xA0;
      TxBuf_AT24C128[0] = 0;
      TxBuf_AT24C128[1] = 0;
      TxBuf_AT24C128[5] += 1;
      i2c1.XferPoll(SlaveAddress,TxBuf_AT24C128,TxLen);
      LL_mDelay(10);
      i2c1.XferPoll(SlaveAddress,TxBuf_AT24C128,2,RxBuf,RxLen,RepeatedStart);
      Test_Condition( !(std::memcmp( (const void*) &TxBuf_AT24C128[2],(const void*) RxBuf, RxLen-6 )), STR("I2C_POLL_TX_3_RX_3_AT24C128 = Pass"), STR("I2C_POLL_TX_3_RX_3_AT24C128 = Fail"));
      SlaveAddress = 0x68<<1;
#endif
#if I2C_MASTER_INTR
      I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
      break;
    case I2C_INT_TX_1_RX_1_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 1;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_1_TXN = Pass"), STR("I2C_INT_TX_1_RX_1_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_2_TXN;
      break;    
    case I2C_INT_TX_1_RX_2_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_2_TXN = Pass"), STR("I2C_INT_TX_1_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_3_TXN;
      break; 
    case I2C_INT_TX_1_RX_3_TXN:
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = 1;
      RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_3_TXN = Pass"), STR("I2C_INT_TX_1_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN;
      break;
    case I2C_INT_TX_2_RX_2_TXN:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN = Pass"), STR("I2C_INT_TX_2_RX_2_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN;
      break;            
    case I2C_INT_TX_3_RX_3_TXN:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN = Pass"), STR("I2C_INT_TX_3_RX_3_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN;
      break;    
    case I2C_INT_TX_40_RX_40_TXN:
      // This test case tests the Transaction and the Callback functionality
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferIntr(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferIntr(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN = Pass"), STR("I2C_INT_TX_40_RX_40_TXN = Fail"));
#endif
      
#if I2C_MASTER_DMA
      I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;			
      break;
    case I2C_INT_TX_2_RX_2_TXN_DMA: 
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 2;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Pass"), STR("I2C_INT_TX_2_RX_2_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3_TXN_DMA;
      break;            
    case I2C_INT_TX_3_RX_3_TXN_DMA:  
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 3;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY); 
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Pass"), STR("I2C_INT_TX_3_RX_3_TXN_DMA = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40_TXN_DMA;
      break;
    case I2C_INT_TX_40_RX_40_TXN_DMA:
      // This test case tests the Transaction and the Callback functionality
      Callbackcount = I2CCallback.get_XferComplete();
      TxLen = RxLen = 40;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      i2c1.XferDMA(&Transaction);
      while(Callbackcount == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      i2c1.XferDMA(&Transaction);
      while(i2c1.GetState() != HAL::I2c::I2C_READY);               
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Pass"), STR("I2C_INT_TX_40_RX_40_TXN_DMA = Fail"));
      
#endif   //I2C_MASTER_DMA  
    default:         
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        I2C_PRINT("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        //I2c_test_id = I2C_INT_TX_2_RX_2_TXN_DMA;
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        //return;
      }
      
      break;
      
    }
    
    MemSet(RxBuf,0,100);
    LL_mDelay(1);
  }
}



