
/******************
** CLASS: IrEncoder
**
** DESCRIPTION:
**  Implements the IrEncoder driver
**
** CREATED: 9 Mar 2020, by Amit Chaudhary
**
** FILE: IrEncoder.h
**
******************/


#ifndef IrEncoder_h
#define IrEncoder_h


#include"CPP_HAL.h"
#include"Gpt.h"
#include"PulseIn.h"
#include"Callback.h"

#define HDR_MARK_POS 1
#define HDR_SPACE_POS 2

#define IR_TOERANCE 100
#define IN_RANGE(Val, Threshold, Tolerance) ( (Val < Threshold + Tolerance ) && (Val > Threshold - Tolerance ) )
namespace BSP
{
  
  template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE>
    class IrEncoder : public Callback 
    {
    public: 
      
      IrEncoder(){};

      ~IrEncoder(){};
      
      void HwInit();
      
      void mark(uint32_t time);
      
      void space(uint32_t time);

      void Encode(uint32_t Data);
      
      void EncodeRaw(uint32_t buf[], uint32_t len, uint32_t hz);
      
      virtual void CallbackFunction();
      
    private:
      T PulseTimer;      
    };
    
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    HwInit()
    {
      PulseTimer.HwInit();      
    }
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    Encode(uint32_t data)
    {      
      // Header
      mark(HDR_MARK);      
      space(HDR_SPACE);
      
      // Data
      for (uint32_t  mask = 1UL << (IR_BITS - 1);  mask;  mask >>= 1)
      {
        mark(BIT_MARK); 
        
        if (data & mask)
        {        
          space(ONE_SPACE);
        }
        else
        {        
          space(ZERO_SPACE);
        }
      }
      
      // Footer
      mark(BIT_MARK);
      space(0);  // Always end with the LED off
    }    
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    CallbackFunction()
    {
 
    }    
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    mark(uint32_t time)
    {
      PulseTimer.Start();
      if (time > 0) HAL::usDelay(time);
    }
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    space(uint32_t time)
    {
      PulseTimer.Stop();
      if(time > 0) HAL::usDelay(time);
    }
    
    template< typename T, uint16_t size, uint16_t IR_BITS,  uint16_t HDR_MARK,  uint16_t HDR_SPACE,  uint16_t BIT_MARK,  uint16_t ONE_SPACE, uint16_t ZERO_SPACE, uint16_t RPT_SPACE >
      void IrEncoder<T,size,IR_BITS,HDR_MARK,HDR_SPACE,BIT_MARK,ONE_SPACE,ZERO_SPACE,RPT_SPACE>::
    EncodeRaw(uint32_t buf[], uint32_t len, uint32_t hz)
    {
      PulseTimer.SetFrequency(hz);
      
      for(uint16_t i = 0; i < len; i++)
      {
        if(i & 1) space(buf[i]);
        else mark(buf[i]);
      }      
      space(0);
    }
    
    
    
    
} // namespace BSP

#endif //IrEncoder_h