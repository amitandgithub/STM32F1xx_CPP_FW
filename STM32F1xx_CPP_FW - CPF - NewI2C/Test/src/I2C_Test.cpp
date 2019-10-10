

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
//char text[100][30];

// RTC DS1307 
#if I2C_POLL

void I2c_Poll_Tests()
{
  uint32_t I2c_test_id = 0;
  
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  I2CDevIntr.HwInit();
  
  I2CDevIntr.ScanBus(RxBuf,20);
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_POLL_TX_1_RX_1;
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
  I2CDevIntr.XferPoll(SlaveAddress,TxBuf,2);       
  
  TxBuf[0] = 8;
  
  for(uint8_t i = 1; i<70; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    {            
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
      
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_POLL_TX_1_RX_1;
        RepeatedStart = 1;	
        printf("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        return;
      }
      break;
      
    default: break;
    }
    
    MemSet(RxBuf,0,100);
    LL_mDelay(50);
  }
}

void I2c_Intr_Tests()
{
  uint32_t I2c_test_id = 0;
  uint32_t count;
  //sprintf(text[0], "Sum of %d and %d is %d", 1, 2, 10);
  
  I2CDevIntr.HwInit();
  RepeatedStart = 0;	
  //I2CDevIntr.ScanBus(RxBuf,20);
  SlaveAddress = 0x68<<1;
  I2c_test_id = I2C_INT_TX_1_RX_1_TXN;
  TxBuf[0] = 0x00;
  TxBuf[1] = 0x00;
  I2CDevIntr.XferPoll(SlaveAddress,TxBuf,2);       
  TxBuf[0] = 8;
  
  for(uint8_t i = 1; i<70; i++) TxBuf[i] = i;
  
  while(1)
  {
    switch(I2c_test_id)
    {            
    case I2C_INT_TX_1_RX_1:
      TxLen = RxLen = 1;
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1,nullptr,0,0,&Status);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_1 = Pass"), STR("I2C_INT_TX_1_RX_1 = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_2;
      break;    
    case I2C_INT_TX_1_RX_2:
      TxLen = 1;
      RxLen = 2;
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_2 = Pass"), STR("I2C_INT_TX_1_RX_2 = Fail"));
      I2c_test_id = I2C_INT_TX_1_RX_3;
      break; 
    case I2C_INT_TX_1_RX_3:
      TxLen = 1;
      RxLen = 3;
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_1_RX_3 = Pass"), STR("I2C_INT_TX_1_RX_3 = Fail"));
      I2c_test_id = I2C_INT_TX_2_RX_2;
      break; 
    case I2C_INT_TX_2_RX_2:  
      TxLen = RxLen = 2;
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      Test_Condition(!(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_2_RX_2 = Pass"), STR("I2C_INT_TX_2_RX_2 = Fail"));
      I2c_test_id = I2C_INT_TX_3_RX_3;
      break;  
      
    case I2C_INT_TX_3_RX_3:  
      TxLen = RxLen = 3;
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart); 
      while(I2CDevIntr.GetState() != HAL::I2c::READY);			
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_3_RX_3 = Pass"), STR("I2C_INT_TX_3_RX_3 = Fail"));
      I2c_test_id = I2C_INT_TX_40_RX_40;
      break;
      
    case I2C_INT_TX_40_RX_40: 
      TxLen = RxLen = 40;           
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,TxLen+1);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);
      I2CDevIntr.XferIntr(SlaveAddress,TxBuf,1,RxBuf,RxLen,RepeatedStart);  
      while(I2CDevIntr.GetState() != HAL::I2c::READY);			
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40 = Pass"), STR("I2C_INT_TX_40_RX_40 = Fail"));
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY); 
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY); 
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY); 
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY); 
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY); 
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
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      
      I2CDevIntr.XferIntr(&Transaction);
      while(count == I2CCallback.get_XferComplete());
      
      Transaction.TxLen = 1;
      Transaction.TxBuf = TxBuf;
      Transaction.RxBuf = RxBuf;
      Transaction.RxLen = RxLen;
      
      I2CDevIntr.XferIntr(&Transaction);
      while(I2CDevIntr.GetState() != HAL::I2c::READY);            
      
      Test_Condition( !(std::memcmp( (const void*) &TxBuf[1],(const void*) RxBuf, TxLen )), STR("I2C_INT_TX_40_RX_40_TXN = Pass"), STR("I2C_INT_TX_40_RX_40_TXN = Fail"));
      I2c_test_id = I2C_INT_TX_QUEUE;			
      break;
    case I2C_INT_TX_QUEUE:
      int Posts=0;
      count = I2CCallback.get_XferComplete();
      TxLen = RxLen = 4;   
      Transaction.SlaveAddress = SlaveAddress;
      Transaction.TxBuf = TxBuf;
      Transaction.TxLen = TxLen+1;
      Transaction.RxBuf = nullptr;
      Transaction.RxLen = 0;
      Transaction.RepeatedStart = RepeatedStart;
      Transaction.pStatus = &Status;
      Transaction.XferDoneCallback = &I2CCallback;            
      Status = I2CDevIntr.Post(&Transaction);
      while(I2CDevIntr.Post(&Transaction) != I2c::I2C_TXN_QUEUE_ERROR)
      {
        Posts++;
      }
      printf("Q posts = %d \n",Posts);
      
      while(I2CCallback.get_XferComplete() < (count+Posts));
      
      Test_Condition( 1, STR("I2C_INT_TX_QUEUE = Pass"), STR("I2C_INT_TX_QUEUE = Fail"));
    default:         
      if(RepeatedStart == 0)
      {
        I2c_test_id = I2C_INT_TX_1_RX_1;
        RepeatedStart = 1;	
        printf("\n\nRe-Testing test cases with repeated start \n\n");
      }
      else
      {
        return;
      }
      break;
      
    }
    
    MemSet(RxBuf,0,100);
    LL_mDelay(50);
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


#endif //I2C_POLL