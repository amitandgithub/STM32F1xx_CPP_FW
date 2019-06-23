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

#define DBG_LOG_WITHOUT_TIMESTAMP 1
extern uint32_t count;
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
            
            typedef T DebugLog_t;
            
            typedef enum : DebugLog_t
            {
                DBG_LOG_MODULE_ID_DW,
                DBG_LOG_MODULE_ID_I2C,
                DBG_LOG_MODULE_ID_SPI
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
            
            Status_t        Init                ( );
            
            Status_t        Register            (DebugLog_t* pModuleID);
            
            inline void     Enable              (ModuleId_t ModuleID);
            
            inline void     Disable             (ModuleId_t ModuleID);
            
            inline void     Log                 (DebugLog_t ModuleID_ORed_LogValue);

                   void     Decode              ();

                   void     Decode              (ModuleId_t ModuleID);
            
            static constexpr DebugLog_t Create  (DebugLog_t ModuleID, DebugLog_t LogValue){return (ModuleID<<(LogValueBits+TimeStampBits) | LogValue<<TimeStampBits) & TIMESTAMP_MASK;}
            
        private:       
            uint32_t    GetCurrentTicks(){return 0x00012345U;}
            
            DebugLog();
            
            ~DebugLog(){};
            
        private:        
            static DebugLog*    m_Instance;
            DebugLog_t          m_ModuleEnableMask;
            DebugLog_t          m_RegisteredModuleIDs; 
            uint32_t            m_LogIndex;
            DebugLog_t          m_Entries[MaxLogEntries]; 
            uint32_t Overflow_Counter;
            
        };
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >* DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    m_Instance;
        
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Enable(ModuleId_t ModuleID)
    {
        m_ModuleEnableMask |= (ModuleID)<<(LogValueBits+TimeStampBits); //POSITION_VAL(ModuleID));
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Disable(ModuleId_t ModuleID)
    {
        m_ModuleEnableMask &= ~((DebugLog_t)1U << ModuleID); //POSITION_VAL(ModuleID));
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        void DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Log(DebugLog_t ModuleID_ORed_LogValue)
    {
        if(m_ModuleEnableMask & ModuleID_ORed_LogValue)
        {
            // DBG_Q.Write( ModuleID | LogValue | GetCurrentTicks());
            if(m_LogIndex == MaxLogEntries)
            {
                m_LogIndex = 0;
            }
            else if(m_LogIndex > MaxLogEntries)
            {
                m_LogIndex = 0;
                Overflow_Counter++;
                
                count++;
               // while(1);
            }
            //__disable_irq();
            
#if DBG_LOG_WITHOUT_TIMESTAMP
            
            m_Entries[m_LogIndex++] = ModuleID_ORed_LogValue;           
#else
            m_Entries[m_LogIndex++] = ModuleID_ORed_LogValue | (GetCurrentTicks() & TIMESTAMP_MASK);
#endif
             //__enable_irq(); 
             
        }
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    DebugLog()
        :m_ModuleEnableMask(0),m_RegisteredModuleIDs(0),m_LogIndex(0),Overflow_Counter(0)
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
        if(m_Instance == nullptr)
        {
            static DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries > m_DebugLog;
            m_Instance = &m_DebugLog;               
        }
        return m_Instance;
    }
    
    template< typename T,uint32_t ModuleIDBits, uint32_t LogValueBits,uint32_t TimeStampBits,uint32_t MaxLogEntries > 
        DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::Status_t DebugLog< T,ModuleIDBits,LogValueBits,TimeStampBits,MaxLogEntries >::
    Register(DebugLog_t* pModuleID)
    {
        for(uint32_t count = 0; count < LOG_MODULE_ID_MAX; count++)
        {
            if((m_RegisteredModuleIDs & count) == 0)
            {
                m_RegisteredModuleIDs |= (DebugLog_t)1U << count;
                *pModuleID = (DebugLog_t)1U << count;
                return STATUS_MODULE_ID_REGISTERED;
            }            
        }
        
        return STATUS_MODULE_ID_NOT_AVAILABLE;
    }   
    
    
}

#endif //DebugLog_h
