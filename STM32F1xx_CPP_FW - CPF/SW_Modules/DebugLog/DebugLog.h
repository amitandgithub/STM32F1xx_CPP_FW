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

#define DBG_LOG_WITHOUT_TIMESTAMP

namespace Utils
{
    
    template< typename T = uint32_t,
    uint32_t ModuleIDBits  = 5U, 
    uint32_t LogValueBits  = 7U,
    uint32_t TimeStampBits = 20U,
    uint32_t MaxLogEntries = 1000U >                  
        class DebugLog
        {      
        public:
            static_assert( (ModuleIDBits+LogValueBits+TimeStampBits) <= (sizeof(T)*8), "Data type of DebugLog is too small !");
            
            using DebugLog_tm = DebugLog<T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >;
            typedef T DebugLog_t;
            
            typedef enum : DebugLog_t
            {
                DBG_LOG_MODULE_ID_I2C,
                DBG_LOG_MODULE_ID_SPI,            
            }ModuleId_t;                        
            
            typedef enum
            {
                STATUS_OK,
                STATUS_MODULE_ID_REGISTERED,
                STATUS_MODULE_ID_UNREGISTERED,
                STATUS_MODULE_ID_NOT_AVAILABLE,            
            }Status_t;
            
            typedef union
            {
                struct _bits 
                {
                    DebugLog_t TimeStamp : TimeStampBits;   // if Tick is 100us then it overflows after 104 seconds
                    DebugLog_t LogValue  : LogValueBits;    // LogValue can be from 0 to 127 max
                    DebugLog_t ModuleID  : ModuleIDBits;    // Module id can be 0 to 31 max           
                }bits;
                uint8_t Array[sizeof(struct _bits)];     
                
            }DebugLogElement_t;      
            
            static constexpr DebugLog_t TIMESTAMP_MASK = (
            (~((DebugLog_t)0))>>( (sizeof(DebugLog_t)*8) - (LogValueBits+ModuleIDBits))) << TimeStampBits ;
            
            static const uint32_t MAX_LOGS = 1024U;
            static const DebugLog_t LOG_MODULE_ID_MAX = 32;
            
        public:
            static DebugLog*   GetInstance();
            
            Status_t        Init               ( );
            
            Status_t        Register           (DebugLog_t* pModuleID);
            
            inline void     Enable             (DebugLog_t ModuleID);
            
            inline void     Disable            (DebugLog_t ModuleID);
            
            inline void     Log                (DebugLog_t ModuleID_LogValue);
            
            static constexpr DebugLog_t Make   (DebugLog_t ModuleID, DebugLog_t LogValue){return (ModuleID<<(LogValueBits+TimeStampBits) | LogValue<<TimeStampBits) & TIMESTAMP_MASK;}
            
        private:       
            uint32_t    GetCurrentTicks(){return 0x00012345U;}
            
            DebugLog();
            
            ~DebugLog(){};
            
        private:        
            static DebugLog*    _Instance;
            DebugLog_t          _ModuleEnableMask;
            DebugLog_t          _RegisteredModuleIDs; 
            uint32_t            _LogIndex;
            DebugLog_t          _Entries[MaxLogEntries]; 
            
        };
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Enable(DebugLog_t ModuleID)
    {
        _ModuleEnableMask |= ((DebugLog_t)1U << POSITION_VAL(ModuleID));
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Disable(DebugLog_t ModuleID)
    {
        _ModuleEnableMask &= ~((DebugLog_t)1U << POSITION_VAL(ModuleID));
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Log(DebugLog_t ModuleID_LogValue)
    {
        if(_ModuleEnableMask & ModuleID_LogValue)
        {
            // DBG_Q.Write( ModuleID | LogValue | GetCurrentTicks());
            if(_LogIndex == MaxLogEntries)
            {
                _LogIndex = 0;
            }
#ifdef DBG_LOG_WITHOUT_TIMESTAMP
            
            _Entries[_LogIndex] = ModuleID_LogValue;           
#else
            _Entries[_LogIndex] = ModuleID_LogValue | (GetCurrentTicks() & TIMESTAMP_MASK);
#endif
            _LogIndex++;            
        }
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    DebugLog()
        :_ModuleEnableMask(0),_RegisteredModuleIDs(0),_LogIndex(0)
    {

    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::Status_t DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Init()
    {
        
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >* DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    GetInstance()
    {
        if(_Instance == nullptr)
        {
            static DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries > _DebugLog;
            _Instance = &_DebugLog;               
        }
        return _Instance;
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::Status_t DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Register(DebugLog_t* pModuleID)
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

#endif //DebugLog_h
