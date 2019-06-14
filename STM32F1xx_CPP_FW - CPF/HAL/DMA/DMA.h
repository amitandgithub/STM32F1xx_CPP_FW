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
    class DMA 
    {
    public:
        
        typedef LL_DMA_InitTypeDef DMAConfig_t;
        typedef uint32_t DMAStatus_t;
        typedef uint32_t DMAState_t;
        typedef Callback* DMACallback_t;
        
        DMAStatus_t HwInit(void *pInitStruct = nullptr);
        
        DMAStatus_t HwDeinit();
        
        void ClockEnable();
        
        void ClockDisable();
        
        DMAStatus_t XferConfig(DMAConfig_t* DMAConfig, uint32_t Channel);
        
        DMAStatus_t Xfer(uint32_t Channel, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength, uint32_t DataDirection );
        
        void EnableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_HT(_DMAx, Channel);}
        
        void EnableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_TC(_DMAx, Channel);}
        
         void EnableTransferErrorInterrupt(uint32_t Channel){LL_DMA_EnableIT_TE(_DMAx, Channel);}
        
        void DisableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_HT(_DMAx, Channel);}
        
        void DisableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_TC(_DMAx, Channel);}
        
        void DisableTransferErrorInterrupt(uint32_t Channel){LL_DMA_DisableIT_TE(_DMAx, Channel);}
        

        
        inline void Start();
        
        inline void Abort();
        
        void RegisterCallback(uint32_t Channel, DMACallback_t ChannelCB);
        
        void UnRegisterCallback(uint32_t Channel);
        
        inline DMAState_t GetState();

        static inline DMA* GetInstance(uint32_t dma);
        
        static DMACallback_t   _DMA1Callbacks[7];
        
#if defined (DMA2)
        static DMACallback_t   _DMA2Callbacks[7];
#endif
        DMA_TypeDef*        _DMAx;   
    private:        
        DMA(uint32_t dma);        
        ~DMA();
        static DMA*         _DMA1Instance;
#if defined (DMA2)
        static DMA*         _DMA2Instance;
#endif
            
    };
    
    
    inline void DMA::Start()
    {
        
    }
    
    inline void DMA::Abort()
    {
        
    }
    
    inline DMA::DMAState_t DMA::GetState()
    {
        return 0;
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


//        class DMA1_Channel1_ISR : public InterruptSource
//        {
//        public: 
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[0]) DMA::_DMA1Callbacks[0]->CallbackFunction();}            
//        };        
//        
//        class DMA1_Channel2_ISR : public InterruptSource
//        {
//        public:
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[1]) DMA::_DMA1Callbacks[1]->CallbackFunction();}           
//        };
//        
//        class DMA1_Channel3_ISR : public InterruptSource
//        {
//        public: 
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[2]) DMA::_DMA1Callbacks[2]->CallbackFunction();}            
//        };        
//        
//        class DMA1_Channel4_ISR : public InterruptSource
//        {
//        public:
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[3]) DMA::_DMA1Callbacks[3]->CallbackFunction();}           
//        };
//        
//        class DMA1_Channel5_ISR : public InterruptSource
//        {
//        public: 
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[4]) DMA::_DMA1Callbacks[4]->CallbackFunction();}            
//        };        
//        
//        class DMA1_Channel6_ISR : public InterruptSource
//        {
//        public:
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[5]) DMA::_DMA1Callbacks[5]->CallbackFunction();}           
//        };
//        
//        class DMA1_Channel7_ISR : public InterruptSource
//        {
//        public: 
//            virtual void ISR( IRQn_Type event ){if(DMA::_DMA1Callbacks[6]) DMA::_DMA1Callbacks[6]->CallbackFunction();}            
//        };
        //        class DMA1_Channel2_ISR : public InterruptSource
        //        {
        //        public:
        //            DMA1_Channel2_ISR(DMA* This) : _this(This){};
        //            ~DMA1_Channel2_ISR(){};
        //            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[1]) _this->_DMA1Callbacks[1]->CallbackFunction();}    
        //        private:
        //            DMA* _this;            
        //        };