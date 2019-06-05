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
        
        class DMA_Channel1_ISR : public InterruptSource
        {
        public:
            DMA_Channel1_ISR(DMA* This) : _this(This){};
            ~DMA_Channel1_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[0]) _this->_DMA1Callbacks[0]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        class DMA_Channel2_ISR : public InterruptSource
        {
        public:
            DMA_Channel2_ISR(DMA* This) : _this(This){};
            ~DMA_Channel2_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[1]) _this->_DMA1Callbacks[1]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        class DMA_Channel3_ISR : public InterruptSource
        {
        public:
            DMA_Channel3_ISR(DMA* This) : _this(This){};
            ~DMA_Channel3_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[2]) _this->_DMA1Callbacks[2]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        class DMA_Channel4_ISR : public InterruptSource
        {
        public:
            DMA_Channel4_ISR(DMA* This) : _this(This){};
            ~DMA_Channel4_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[3]) _this->_DMA1Callbacks[3]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        class DMA_Channel5_ISR : public InterruptSource
        {
        public:
            DMA_Channel5_ISR(DMA* This) : _this(This){};
            ~DMA_Channel5_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[4]) _this->_DMA1Callbacks[4]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        class DMA_Channel6_ISR : public InterruptSource
        {
        public:
            DMA_Channel6_ISR(DMA* This) : _this(This){};
            ~DMA_Channel6_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[5]) _this->_DMA1Callbacks[5]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        class DMA_Channel7_ISR : public InterruptSource
        {
        public:
            DMA_Channel7_ISR(DMA* This) : _this(This){};
            ~DMA_Channel7_ISR(){};
            virtual void ISR( IRQn_Type event ){if(_this->_DMA1Callbacks[6]) _this->_DMA1Callbacks[6]->CallbackFunction();}    
        private:
            DMA* _this;            
        };
        
        
        static DMACallback_t   _DMA1Callbacks[7];
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
        DMA_Channel1_ISR    _Ch1_ISR;
        DMA_Channel2_ISR    _Ch2_ISR;
        DMA_Channel3_ISR    _Ch3_ISR;
        DMA_Channel4_ISR    _Ch4_ISR;
        DMA_Channel5_ISR    _Ch5_ISR;
        DMA_Channel6_ISR    _Ch6_ISR;
        DMA_Channel7_ISR    _Ch7_ISR;
        
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
