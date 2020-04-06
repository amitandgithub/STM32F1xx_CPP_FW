

//#include "system.h"

#include "CmdSvr.h"

#include"I2C_Commands.h"


CmdSvrInterface_t*const CmdInterfaceTable[CMDSVR_INTERFACE_MAX] = 
{
  &I2C_CmdSvrInterface,
};

static const CmdHandler ModuleHandlerTable[MODULE_ID_MAX] = 
{       
  I2C_CmdHandler,                                        
};

void Cmdsvr_DispatchCmd(CmdSvrInterface_t *pCmdSvrInterface)
{
  return ModuleHandlerTable[pCmdSvrInterface->Request.Buf[0]]();
}

void Cmdsvr_DispatchResponse(CmdSvrInterface_t *pCmdSvrInterface)
{  
  pCmdSvrInterface->ResponseCallback();
}

CmdStatus_t Cmdsvr_Init()
{
   uint8_t i;
  
  for(i = 0; i < CMDSVR_INTERFACE_MAX; i++ )
  {
    CmdInterfaceTable[i]->CmdSvrInterfaceInit();
  } 
  
  return CMD_STATUS_OK;    
}

void CmdSvr_Run(void)
{  
  uint8_t i;
  
  for(i = 0; i < CMDSVR_INTERFACE_MAX; i++ )
  {
    if(CmdInterfaceTable[i]->Status == CMD_STATUS_PROCESSING)
    {
      Cmdsvr_DispatchCmd(CmdInterfaceTable[i]);
      Cmdsvr_DispatchResponse(CmdInterfaceTable[i]);
    } 
  }  
}




