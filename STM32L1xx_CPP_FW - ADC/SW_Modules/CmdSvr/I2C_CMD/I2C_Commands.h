


//#include"system.h"
#include"CmdSvr.h"
#include"i2c.h"
#include "CPP_HAL.h" 

extern CmdSvrInterface_t I2C_CmdSvrInterface;

/* I2C Command Table*/
enum I2C_CdmID
{
  I2C_CMDID_ADD_REQUEST,
  I2C_CMDID_ADD_RESPONSE,
  I2C_CMDID_SUB_REQUEST,
  I2C_CMDID_SUB_RESPONSE,
  I2C_CMDID_MUL_REQUEST,
  I2C_CMDID_MUL_RESPONSE,
  I2C_CMDID_DIV_REQUEST,
  I2C_CMDID_DIV_RESPONSE,
};
struct I2C_Add_Cmd_Response
{
  CmdStatus_t Status;
  uint8_t ModuleId;
  uint8_t CmdId;
  uint8_t Len;
  uint8_t Result;
  uint8_t CRC8;  
};

struct I2C_Add_Cmd_Request
{  
  uint8_t ModuleId;
  uint8_t CmdId;
  uint8_t Len;
  uint8_t Param1;
  uint8_t Param2;
  uint8_t CRC8;  
};

typedef union 
{
  struct I2C_Add_Cmd_Request;
  struct I2C_Add_Cmd_Response;
  
}I2C_Cmds_t;

void I2C_CmdHandler();

void I2C_Cmd_Interface_Init();

void I2C_CmdSvrCallback();

//void I2C_CmdSvr_Callback(HAL::I2c::I2CStatus_t status);


class I2C_CmdSvr_Callback_t: public HAL::I2c::i2cCallback
{
  public:
  I2C_CmdSvr_Callback_t(){};
  virtual ~I2C_CmdSvr_Callback_t(){};
  virtual void CallbackFunction(HAL::I2c::I2CStatus_t I2CStatus);
};
