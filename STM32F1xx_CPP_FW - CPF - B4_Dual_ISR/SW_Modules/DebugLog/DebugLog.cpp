/******************
** FILE: DebugLog.cpp
**
** DESCRIPTION:
**  Debug Log functionality Imolementation
**
** CREATED: 5/18/2019, by Amit Chaudhary
******************/
#include "DebugLog.h"

namespace Utils
{
    
    DebugLog*    _Instance = nullptr;
    
    DebugLog::DebugLog()
    {
        
    }
    
    DebugLog::Status_t DebugLog::Init()
    {
        
    }
    
    DebugLog* DebugLog::GetInstance()
    {
        if(_Instance == nullptr)
        {
            static DebugLog _DebugLog;
            _Instance = &_DebugLog;               
        }
        return _Instance;
    }
    
    DebugLog::Status_t DebugLog::Register(DebugLog_t* pModuleID)
    {
        for(uint32_t count = 0; count < LOG_MODULE_ID_MAX; count++)
        {
            if((_RegisteredModuleIDs & count) == 0)
            {
                _RegisteredModuleIDs |= (DebugLog_t)1U << count;
                *pModuleID = (DebugLog_t)1U << count;
                return STATUS_MODULE_ID_REGISTERED;
            }            
        }
        
        return STATUS_MODULE_ID_NOT_AVAILABLE;
    }        

}





