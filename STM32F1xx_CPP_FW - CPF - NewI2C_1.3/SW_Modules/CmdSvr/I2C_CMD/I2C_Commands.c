


#include"I2C_Commands.h"


#define I2C_BUF_SIZE (sizeof(I2C_Add_Cmd_Response)+5)

static HAL::I2c I2CDev(HAL::Gpio::B6, HAL::Gpio::B7,100000U);

uint8_t I2c_Cmd_request [I2C_BUF_SIZE];
uint8_t I2c_Cmd_response[I2C_BUF_SIZE] = {CMD_STATUS_READY,};

I2C_CmdSvr_Callback_t I2C_CmdSvr_Callback;

CmdSvrInterface_t I2C_CmdSvrInterface = 
{
  {I2c_Cmd_request,I2C_BUF_SIZE,0},
  {I2c_Cmd_response,I2C_BUF_SIZE,0},
  CMD_STATUS_READY,
  I2C_Cmd_Interface_Init,
  I2C_CmdSvrCallback
};
  
void I2C_Cmd_Interface_Init()
{
  I2CDev.HwInit();
  
  I2CDev.SetSlaveTxDefaultByte(0xff);
  
  I2CDev.SetSlaveCallback(&I2C_CmdSvr_Callback);
    
  while(I2CDev.SlaveStartListening(&I2C_CmdSvrInterface.Response, &I2C_CmdSvrInterface.Request) != HAL::I2c::I2C_OK);

}

void I2C_CmdHandler()
{
  
  struct I2C_Add_Cmd_Response* ReponseData = (struct I2C_Add_Cmd_Response*)I2c_Cmd_response;
  struct I2C_Add_Cmd_Request* RequestData =  (struct I2C_Add_Cmd_Request*)I2c_Cmd_request;
  
   
  switch(RequestData->CmdId)
  {
  case I2C_CMDID_ADD_REQUEST:    
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 + RequestData->Param2;
    break;
  case I2C_CMDID_SUB_REQUEST:    
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 - RequestData->Param2;
    break;
  case I2C_CMDID_MUL_REQUEST:
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 * RequestData->Param2;
    break;
  case I2C_CMDID_DIV_REQUEST: 
    ReponseData->CmdId  = I2C_CMDID_ADD_RESPONSE;
    ReponseData->Len    = 1;
    ReponseData->Result = RequestData->Param1 / RequestData->Param2;
    break;
  default: break;
  }  
}


void I2C_CmdSvrCallback()
{
  I2C_CmdSvrInterface.Request.Idx = 0;
  I2c_Cmd_response[0] = CMD_STATUS_PROCESSING_DONE;
  I2C_CmdSvrInterface.Status = CMD_STATUS_PROCESSING_DONE;  
  I2C_CmdSvrInterface.Response.Len = I2C_BUF_SIZE;
}

void I2C_CmdSvr_Callback_t::CallbackFunction(HAL::I2c::I2CStatus_t I2CStatus)
 {
    if((I2CStatus == HAL::I2c::I2C_SLAVE_RX_DONE) )
  {    
    if((I2C_CmdSvrInterface.Status == CMD_STATUS_READY) && (I2C_CmdSvrInterface.Request.Idx >= sizeof(RspHdr_t)))
    {
      //I2C_CmdSvrInterface.Response.Buf = CmdSvr_Buf;
      I2C_CmdSvrInterface.Status = CMD_STATUS_PROCESSING;
      I2c_Cmd_response[0] = CMD_STATUS_PROCESSING;
      I2C_CmdSvrInterface.Response.Len = 1;
    }
  }
  else if(I2CStatus == HAL::I2c::I2C_SLAVE_TX_DONE)
  {           
    if(I2C_CmdSvrInterface.Status == CMD_STATUS_PROCESSING_DONE)
    {
//      I2C_CmdSvrInterface.Request.Idx = 0;
      if(I2C_CmdSvrInterface.Response.Idx >= 1)
      {
        I2C_CmdSvrInterface.Status = CMD_STATUS_READY;
        I2c_Cmd_response[0] = CMD_STATUS_READY;
      }
    }
    I2C_CmdSvrInterface.Response.Idx = 0;    
  }
  else
  {
    while(1);
  }  
 }


//void I2C_CmdSvr_Callback(HAL::I2c::I2CStatus_t status)
//{  
//  if((status == HAL::I2c::I2C_SLAVE_RX_DONE) )
//  {    
//    if((I2C_CmdSvrInterface.Status == CMD_STATUS_READY) && (I2C_CmdSvrInterface.Request.Idx >= sizeof(RspHdr_t)))
//    {
//      //I2C_CmdSvrInterface.Response.Buf = CmdSvr_Buf;
//      I2C_CmdSvrInterface.Status = CMD_STATUS_PROCESSING;
//      I2c_Cmd_response[0] = CMD_STATUS_PROCESSING;
//      I2C_CmdSvrInterface.Response.Len = 1;
//    }
//  }
//  else if(status == HAL::I2c::I2C_SLAVE_TX_DONE)
//  {           
//    if(I2C_CmdSvrInterface.Status == CMD_STATUS_PROCESSING_DONE)
//    {
////      I2C_CmdSvrInterface.Request.Idx = 0;
//      if(I2C_CmdSvrInterface.Response.Idx >= 1)
//      {
//        I2C_CmdSvrInterface.Status = CMD_STATUS_READY;
//        I2c_Cmd_response[0] = CMD_STATUS_READY;
//      }
//    }
//    I2C_CmdSvrInterface.Response.Idx = 0;    
//  }
//  else
//  {
//    while(1);
//  }  
//}