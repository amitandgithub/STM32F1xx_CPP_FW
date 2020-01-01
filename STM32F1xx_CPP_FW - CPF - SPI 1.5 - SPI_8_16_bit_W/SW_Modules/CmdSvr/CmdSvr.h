


#ifndef CmdSvr_h
#define CmdSvr_h


#include"CPP_HAL.h"

enum
{
  CMDSVR_INTERFACE_I2C,
  //CMDSVR_INTERFACE_SPI,
  //CMDSVR_INTERFACE_UART,
  CMDSVR_INTERFACE_MAX
};

typedef enum
{
  MODULE_ID_I2C,  
  MODULE_ID_MAX, 
}ModuleID_t;


typedef enum
{
  CMD_STATUS_OK,
  CMD_STATUS_INVALID_PARAMS,
  CMD_STATUS_LEN_ERROR,
  CMD_STATUS_CRC_ERROR,
  CMD_STATUS_INVALID_MODULEID,
  CMD_STATUS_INVALID_CMDID,  
  CMD_STATUS_ACK,
  CMD_STATUS_PROCESSING, // 7
  CMD_STATUS_PROCESSING_DONE, // 8
  CMD_STATUS_READY,  // 9
}CmdStatus_t;

typedef struct
{
  uint8_t ModuleID;
  uint8_t CmdID;
  uint8_t Len;
}CmdHdr_t;

typedef struct
{
  CmdStatus_t   Status;
  uint8_t       ModuleID;
  uint8_t       CmdID;
  uint8_t       Len;
}RspHdr_t;

typedef struct  CmdSvrInterface_s CmdSvrInterface_t;

typedef void (*CmdHandler)();

typedef void (*CmdSvrCallback_t)();

typedef void (*CmdSvrInterfaceInit_t)();

typedef struct CmdSvrInterface_s 
{
  HAL::Buf_t                 Request;
  HAL::Buf_t                 Response;
  CmdStatus_t           Status;
  CmdSvrInterfaceInit_t CmdSvrInterfaceInit;
  CmdSvrCallback_t      ResponseCallback;  
}CmdSvrInterface_t;




CmdStatus_t Cmdsvr_Init();

CmdStatus_t CmdSvr_GetStatus(CmdSvrInterface_t *pCmdSvrInterface);

CmdStatus_t Cmdsvr_AuthenticateCmd(CmdSvrInterface_t *pCmdSvrInterface);

void Cmdsvr_DispatchCmd(CmdSvrInterface_t *pCmdSvrInterface);

void Cmdsvr_DispatchResponse(CmdSvrInterface_t *pCmdSvrInterface);

void CmdSvr_Run(void);

void CmdSvr_Scan(CmdSvrInterface_t *pCmdInterface);





















#endif