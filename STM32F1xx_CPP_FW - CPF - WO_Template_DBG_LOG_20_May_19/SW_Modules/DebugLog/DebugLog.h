/******************
** CLASS: DebugLog
**
** DESCRIPTION:
**  Debug Logging functionality
**
** CREATED: 5/17/2019, by Amit Chaudhary
**
** FILE: DebugLog.h
**
******************/
#ifndef DebugLog_h
#define DebugLog_h

#include <cstdint>
#include "CPP_HAL.h"

namespace Utils
{
    
    typedef enum : uint8_t
    {
        DBG_LOG_MODULEID_I2C,
        DBG_LOG_MODULEID_SPI,            
    }ModuleID_t;
    
    class DebugLog
    {      
        
        typedef uint32_t DebugLog_t;
             
        typedef enum
        {
            STATUS_OK,
            STATUS_MODULE_ID_REGISTERED,
            STATUS_MODULE_ID_UNREGISTERED,
            STATUS_MODULE_ID_NOT_AVAILABLE,            
        }Status_t;
        
        typedef union
        {
            struct bit 
            {
                DebugLog_t TimeStamp : 20;   // if Tick is 100us then it overflows after 104 seconds
                DebugLog_t LogValue  : 7;    // LogValue can be from 0 to 127 max
                DebugLog_t ModuleID  : 5;    // Module id can be 0 to 31 max           
            };
            DebugLog_t Element;           
        }DebugLogElement_t;      
        
        static const uint32_t MAX_LOGS = 1024U;
        static const DebugLog_t LOG_MODULE_ID_MAX = 32;
        
    public:
        static DebugLog*   GetInstance();
        
        Status_t        Init               ( );
        
        Status_t        Register           (DebugLog_t* pModuleID);
        
        inline void     Enable             (DebugLog_t ModuleID);
        
        inline void     Disable            (DebugLog_t ModuleID);
        
        inline void     Log                (DebugLog_t ModuleID, DebugLog_t LogValue);
        
        inline void     Log                (DebugLog_t ModuleID);
        
        static constexpr DebugLog_t Make   (DebugLog_t ModuleID, DebugLog_t LogValue){return (ModuleID<<27U | LogValue<<20U) & 0xFFF00000U;}
        
    private:       
        uint32_t    GetCurrentTicks(){return 0x00012345U;}
        
        DebugLog();
        
        ~DebugLog(){};
        
    private:        
        static DebugLog*    _Instance;
        DebugLog_t          _ModuleEnableMask;
        DebugLog_t          _RegisteredModuleIDs;        
    };
    
    
        void DebugLog::Enable(DebugLog_t ModuleID)
    {
        _ModuleEnableMask |= ((DebugLog_t)1U << POSITION_VAL(ModuleID));
    }
    
    void DebugLog::Disable(DebugLog_t ModuleID)
    {
        _ModuleEnableMask &= ~((DebugLog_t)1U << POSITION_VAL(ModuleID));
    }
    
    void DebugLog::Log(DebugLog_t ModuleID, DebugLog_t LogValue)
    {
        if(ModuleID & _ModuleEnableMask)
        {
            // DBG_Q.Write( ModuleID | LogValue | GetCurrentTicks());
        }
    }
    
    
}

#endif //DebugLog_h
