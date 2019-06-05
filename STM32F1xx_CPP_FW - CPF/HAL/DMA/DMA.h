/******************
** CLASS: DMA
**
** DESCRIPTION:
**  Dma functionality implementation
**
** CREATED: 6/5/2019, by Amit Chaudhary
**
** FILE: DMA.h
**
******************/
#ifndef DMA_h
#define DMA_h


#include"CPP_HAL.h"
#include"stm32f1xx_ll_dma.h"
#include"InterruptManager.h"
#include"Callback.h"

namespace HAL
{    
    class DMA : public InterruptSource
    {
    public:
        
        typedef LL_DMA_InitTypeDef* DMAConfig_t;
        typedef uint32_t DMAStatus_t;
        typedef uint32_t DMAState_t;
        typedef Callback* DMACallback_t;
       
//        typedef struct
//        {
//            LL_DMA_InitTypeDef Params;            
//        }DMAConfig_t;
        
        DMAStatus_t HwInit(void *pInitStruct = nullptr);
        
        DMAStatus_t HwDeinit();
        
        DMAStatus_t XferConfig(DMAConfig_t DMAConfig, uint32_t Channel);
        
        DMAStatus_t Xfer(uint32_t Channel, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
        
        inline void Start();
        
        inline void Abort();
        
        void RegisterCallback(uint32_t Channel, DMACallback_t ChannelCB);
        
        void UnRegisterCallback(uint32_t Channel);
        
        inline DMAState_t GetState();
        
        inline DMA* GetInstance(uint32_t dma);
        
        virtual void ISR( IRQn_Type event );
        
    private:        
        DMA(uint32_t dma);        
        ~DMA();
        DMA*                _DMA1Instance;
#if defined (DMA2)
        DMA*                _DMA2Instance;
#endif
        DMA_TypeDef*        _DMAx;
        DMACallback_t       _HalfCompleteCB;
        DMACallback_t       _FullCompleteCB;
        DMACallback_t       _ErrorCB;   
        static DMACallback_t   _DMA1Callbacks[7];
    };
    
    
    inline void DMA::Start()
    {
        
    }
    
    inline void DMA::Abort()
    {
        
    }
    
    inline DMA::DMAState_t DMA::GetState()
    {
        
    }
    
    inline DMA* DMA::GetInstance(uint32_t dma)
    {
        if(dma == 1)
        {
            
            if(_DMA1Instance)
            {
                return _DMA1Instance;
            }
            else
            {
                static DMA DMAObj(1);
                _DMA1Instance = &DMAObj;
                return _DMA1Instance; 
            }
        }
        else
        {
#if defined (DMA2)
             if(_DMA2Instance)
            {
                return _DMA2Instance;
            }
            else
            {
                static DMAObj(2);
                _Instance = &DMAObj;
                return _DMA2Instance; 
            } 
#else
            while(1); // Fatal Error
#endif            
        }
             
            
    }
    
     
}
#endif //DMA_h
