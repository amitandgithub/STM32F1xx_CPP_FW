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
        
        DMA(uint32_t dma);    
        
        ~DMA();
        
        DMAStatus_t HwInit(void *pInitStruct = nullptr);
        
        DMAStatus_t HwDeinit();
        
        void ClockEnable();
        
        void ClockDisable();
        
        DMAStatus_t XferConfig(DMAConfig_t* DMAConfig, uint32_t Channel);
        
        DMAStatus_t Load(uint32_t Channel, uint32_t PeripheralAddress, uint32_t MemoryAddress, uint32_t DataLength, uint32_t DataDirection );
        
        void EnableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_HT(m_DMAx, Channel);}
        
        void EnableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_EnableIT_TC(m_DMAx, Channel);}
        
         void EnableTransferErrorInterrupt(uint32_t Channel){LL_DMA_EnableIT_TE(m_DMAx, Channel);}
        
        void DisableHalfTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_HT(m_DMAx, Channel);}
        
        void DisableTransferCompleteInterrupt(uint32_t Channel){LL_DMA_DisableIT_TC(m_DMAx, Channel);}
        
        void DisableTransferErrorInterrupt(uint32_t Channel){LL_DMA_DisableIT_TE(m_DMAx, Channel);}
        
        void SetDataLen(uint32_t Channel, uint32_t Len);
        
        uint32_t GetDataLen(uint32_t Channel);
        
        inline void Start();
        
        inline void Abort();

        DMA_TypeDef*        m_DMAx;            
    };
    
    
    inline void DMA::Start()
    {
        
    }
    
    inline void DMA::Abort()
    {
        
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