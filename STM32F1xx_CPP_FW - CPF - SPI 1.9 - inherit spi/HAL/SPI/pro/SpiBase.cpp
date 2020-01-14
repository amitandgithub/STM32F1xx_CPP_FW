/******************
** CLASS: SpiBase
**
** DESCRIPTION:
**  Implements the Spi Base class
**
** CREATED: 13/1/2020, by Amit Chaudhary
**
** FILE: SpiBase.cpp
**
*/

#include"SpiBase.h"

namespace HAL
{   


  SpiBase::SpiBase(SpiPort_t SpiPort, Hz_t Hz) :
    m_hz(Hz),
    m_SPIState(SPI_RESET)
      {
        m_Transaction.TxLen = 0;
        m_Transaction.RxLen = 0;
        m_Transaction.Spi_Baudrate = SPI_BAUDRATE_DIV2;
        m_Baudrate = SPI_BAUDRATE_DIV2;
        
        if(SpiPort  == SPI1_A5_A6_A7)
        {
          m_SPIx = SPI1;    
        }
        else if(SpiPort  == SPI2_B13_B14_B15)
        {
          m_SPIx = SPI2;
        }
        else
        {
          while(1); // Fatal Error
        }  

        m_SPIState = SPI_RESET;
      }    
    
    SpiBase::SpiStatus_t SpiBase::HwInit(bool Slave, XferMode_t mode)
    {     
      LL_SPI_InitTypeDef SPI_InitStruct;
      
      /* Set SPI_InitStruct fields to default values */
      SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
      SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
      SPI_InitStruct.DataWidth         = (mode == SPI_8BIT) ? LL_SPI_DATAWIDTH_8BIT : LL_SPI_DATAWIDTH_16BIT;
      SPI_InitStruct.ClockPolarity     = LL_SPI_POLARITY_LOW;
      SPI_InitStruct.ClockPhase        = LL_SPI_PHASE_1EDGE;
      SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;//LL_SPI_NSS_SOFT;//LL_SPI_NSS_HARD_OUTPUT;
      SPI_InitStruct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
      SPI_InitStruct.BitOrder          = LL_SPI_MSB_FIRST;
      SPI_InitStruct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
      SPI_InitStruct.CRCPoly           = 10U;         
      
      if(m_SPIx == SPI1)
      {
        if(Slave)
        {
          SlavePinsHwInit<A4,A5,A6,A7>();
          SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
          SPI_InitStruct.NSS = LL_SPI_NSS_HARD_INPUT;
        }
        else
        {
          MasterPinsHwInit<A5,A6,A7>();
        }       
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
      }
      else if(m_SPIx == SPI2)
      {        
        if(Slave)
        {
          SlavePinsHwInit<B12,B13,B14,B15>();
          SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
          SPI_InitStruct.NSS = LL_SPI_NSS_HARD_INPUT;
        }
        else
        {
          MasterPinsHwInit<B13,B14,B15>();
        } 
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
      }
      else
      {
        while(1);
      }
      
      if(m_SPIx == nullptr)
      {
        // Remap 
      }
      Disable();
      LL_SPI_Init(m_SPIx, &SPI_InitStruct);    
      Enable();      
      m_Transaction.Mode = mode;      
      m_SpiStatus = SPI_OK;
      m_SPIState = SPI_READY;
      
      return SPI_OK;            
    }        
    
    template<Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin>
      void SpiBase::MasterPinsHwInit()
      {
        DigitalOut<ClkPort,ClkPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz>        sck;
        DigitalIn<MisoPort,MisoPin,INPUT_PULLUP>                                        miso;
        DigitalOut<MosiPort,MosiPin,OUTPUT_AF_PUSH_PULL,LL_GPIO_MODE_OUTPUT_50MHz>      mosi;
        
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();      
      }
    
    template<Port_t ChipSelectPort, PIN_t ChipSelectPin, Port_t ClkPort, PIN_t ClkPin,Port_t MisoPort, PIN_t MisoPin,Port_t MosiPort, PIN_t MosiPin>
      void SpiBase::SlavePinsHwInit()
      {
        // These templates generate less code than none template class
        DigitalIn<ChipSelectPort,ChipSelectPin,INPUT_PULLDOWN>  ChipSelect;          
        DigitalIn<ClkPort,ClkPin,INPUT_PULLDOWN>                sck;
        DigitalOut<MisoPort,MisoPin,OUTPUT_AF_PUSH_PULL>        miso;
        DigitalIn<MosiPort,MosiPin,INPUT_PULLDOWN>              mosi;
        
        ChipSelect.HwInit();
        sck.HwInit();
        miso.HwInit();
        mosi.HwInit();        
      }

}