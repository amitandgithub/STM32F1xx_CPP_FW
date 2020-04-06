
/******************
** CLASS: IrDecoder
**
** DESCRIPTION:
**  Implements the IrDecoder driver
**
** CREATED: 7 Mar 2020, by Amit Chaudhary
**
** FILE: IrDecoder.h
**
******************/


#ifndef IrDecoder_h
#define IrDecoder_h


#include"CPP_HAL.h"
#include"Gpt.h"
#include"PulseIn.h"
#include"Callback.h"

#define HDR_MARK_POS 1
#define HDR_SPACE_POS 2

#define IR_TOERANCE 100
#define IN_RANGE(Val, Threshold, Tolerance) ( (Val < Threshold + Tolerance ) && (Val > Threshold - Tolerance ) )
namespace HAL
{
  
  template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
    class IrDecoder : public Callback 
    {
    public:       
      
      IrDecoder();

      ~IrDecoder(){};
      
      void HwInit();

      bool Decode(uint32_t* pResult);
      
      bool DecodeRaw(RawIrFrmae_t* pRawIrFrmae);
      
      bool Start(){ return PulseTimer.Start(&m_PulseBuf);}
      
      void Stop(){PulseTimer.Stop();}
      
      virtual void CallbackFunction();
      
      void GetRawFrame(uint16_t* Buf, uint16_t* Len){ Buf = m_Buf; *Len = m_PulseBuf.Idx;  }
      
      void Sync(uint32_t delay){ HAL::usDelay(delay); }
      
    private:
      uint8_t           m_Status;
      uint16_t          m_Buf[size];
      HAL::PulseBuf_t   m_PulseBuf;
      T                 PulseTimer;      
    };
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    IrDecoder()
    {
      m_PulseBuf.Buf = m_Buf;
      m_PulseBuf.Len = size;
      m_PulseBuf.Idx = 0;
      m_PulseBuf.pCallback = this;
      m_Status = 0;
    }
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    HwInit()
    {
      PulseTimer.HwInit(__LL_TIM_CALC_PSC(72000000, 1000000),0xffff,true);      
    }
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      bool IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    Decode(uint32_t* pResult)
    {
      uint32_t Result = 0;
      uint32_t Mask = ( 1UL << (IR_BITS-1) );
      uint8_t i;
      
      if(m_Status == 1)  // IR packet received
      {
        if( IN_RANGE(m_Buf[HDR_MARK_POS],HDR_MARK,IR_TOERANCE) && IN_RANGE(m_Buf[HDR_SPACE_POS],HDR_SPACE,IR_TOERANCE) ) // Header atched
        {
          // Decode Bits
          for(i = 3; i < IR_BITS*2+2 ; i += 2)  
          {
            if( IN_RANGE(m_Buf[i],BIT_MARK,IR_TOERANCE) )
            {
              
              if(IN_RANGE(m_Buf[i+1],ONE_SPACE,IR_TOERANCE))
              {
                Result |= Mask;  
              }
            }
            else
            {
              //Sync( ONE_SPACE*(IR_BITS - i) );
              //Sync(ONE_SPACE*IR_BITS);
              // Error in packed, reset to receive new packet
              m_Status = 0;
              *pResult = Result;
              m_PulseBuf.Idx = 0;              
              Start();              
              return false;
            }
            Mask = Mask >> 1;
          }
          // Packet decoded succesfully, get set for new packet
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;
          Start();
          return true;
        }
        else
        {
         // Sync(ONE_SPACE*IR_BITS);
          // Header is corrupted discard the packed, reset to receive new packet
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;
          Start();
          return false;
        }        
      }
      return false;      
    }    
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    CallbackFunction()
    {
      m_Status = 1;      
    }
   
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      bool IrDecoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    DecodeRaw(RawIrFrmae_t* pRawIrFrmae)
    {
      if( (pRawIrFrmae == nullptr) && (m_Status == 0) ) return false;
      
      pRawIrFrmae->HeaderMark   = m_Buf[HDR_MARK_POS]; 
      pRawIrFrmae->HeaderSpace  = m_Buf[HDR_SPACE_POS]; 
      pRawIrFrmae->BitMark      = m_Buf[2]; 
      
      for(uint8_t i = 3; i < m_PulseBuf.Idx; i += 2)  
      {
        //if()
      }
      return true;      
    }
    
} // namespace BSP

#endif //IrDecoder_h

// Another way if we want to share the Timer with other modules. in this case we need not to create the timer object inside IrDecoder class
// instead we can pass the Timer type and address to be used inside IrDecoder module, but it generates 86 more bytes of code 
#if 0

   template<typename T, T* Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
    class IrDecoder : public Callback 
    {
    public:       
      
      IrDecoder();

      ~IrDecoder(){};
      
      void HwInit();

      bool Decode(uint32_t* pResult);
      
      bool DecodeRaw(RawIrFrmae_t* pRawIrFrmae);
      
      bool Start(){ return PulseTimer->Start(&m_PulseBuf);}
      
      void Stop(){PulseTimer->Stop();}
      
      virtual void CallbackFunction();
      
      void GetRawFrame(uint16_t* Buf, uint16_t* Len){ Buf = m_Buf; *Len = m_PulseBuf.Idx;  }
      
      void Sync(uint32_t delay){ HAL::usDelay(delay); }
      
    private:
      uint8_t           m_Status;
      uint16_t          m_Buf[size];
      HAL::PulseBuf_t   m_PulseBuf;
      T*                PulseTimer;      
    };
    
    template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    IrDecoder(): PulseTimer((T*)Addreess)
    {
      m_PulseBuf.Buf = m_Buf;
      m_PulseBuf.Len = size;
      m_PulseBuf.Idx = 0;
      m_PulseBuf.pCallback = this;
      m_Status = 0;
    }
    
    template< typename T,  uint32_t Addreess,uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    HwInit()
    {
      PulseTimer->HwInit(__LL_TIM_CALC_PSC(72000000, 1000000),0xffff,true);      
    }
    
    template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      bool IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    Decode(uint32_t* pResult)
    {
      uint32_t Result = 0;
      uint32_t Mask = ( 1UL << (IR_BITS-1) );
      uint8_t i;
      
      if(m_Status == 1)  // IR packet received
      {
        if( IN_RANGE(m_Buf[HDR_MARK_POS],HDR_MARK,IR_TOERANCE) && IN_RANGE(m_Buf[HDR_SPACE_POS],HDR_SPACE,IR_TOERANCE) ) // Header atched
        {
          // Decode Bits
          for(i = 3; i < IR_BITS*2+2 ; i += 2)  
          {
            if( IN_RANGE(m_Buf[i],BIT_MARK,IR_TOERANCE) )
            {
              
              if(IN_RANGE(m_Buf[i+1],ONE_SPACE,IR_TOERANCE))
              {
                Result |= Mask;  
              }
            }
            else
            {
              //Sync( ONE_SPACE*(IR_BITS - i) );
              //Sync(ONE_SPACE*IR_BITS);
              m_Status = 0;
              *pResult = Result;
              m_PulseBuf.Idx = 0;              
              Start();              
              return false;
            }
            Mask = Mask >> 1;
          }
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;
          Start();
          return true;
        }
        else
        {
         // Sync(ONE_SPACE*IR_BITS);
          m_Status = 0;
          *pResult = Result;
          m_PulseBuf.Idx = 0;
          Start();
          return false;
        }        
      }
      return false;      
    }    
    
    template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    CallbackFunction()
    {
      m_Status = 1;      
    }
   
    template< typename T, uint32_t Addreess, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      bool IrDecoder<T,Addreess,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    DecodeRaw(RawIrFrmae_t* pRawIrFrmae)
    {
      if( (pRawIrFrmae == nullptr) && (m_Status == 0) ) return false;
      
      pRawIrFrmae->HeaderMark   = m_Buf[HDR_MARK_POS]; 
      pRawIrFrmae->HeaderSpace  = m_Buf[HDR_SPACE_POS]; 
      pRawIrFrmae->BitMark      = m_Buf[2]; 
      
      for(uint8_t i = 3; i < m_PulseBuf.Idx; i += 2)  
      {
        //if()
      }
      return true;      
    }
#endif