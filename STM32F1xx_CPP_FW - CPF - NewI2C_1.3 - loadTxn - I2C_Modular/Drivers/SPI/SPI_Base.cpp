/******************
** FILE: SPI_Base.cpp
**
** DESCRIPTION:
**  SPI Base Class functions
**
** CREATED: 9/2/2018, by Amit Chaudhary
******************/
#include "SPI_Base.h"

namespace Peripherals
{
SPI_Base::Transaction_t* SPI_Base::m_pCurentTransaction_SPI1;
SPI_Base::Transaction_t* SPI_Base::m_pCurentTransaction_SPI2;
    
SPI_Base* SPI_Base::m_pSPI1_Obj;
SPI_Base* SPI_Base::m_pSPI2_Obj; 

SPI_Base::Queue1 SPI_Base::SPI1_Q;
SPI_Base::Queue2 SPI_Base::SPI2_Q;

SPI_Base::Queue1* SPI_Base::m_pSPI1_Q = &SPI_Base::SPI1_Q;
SPI_Base::Queue2* SPI_Base::m_pSPI2_Q = &SPI_Base::SPI2_Q;



uint32_t SPI_Base::SPI1_Status ;
uint32_t SPI_Base::SPI2_Status ;

SPI_Base::Callback_t                            SPI_Base::SPI1_TxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI1_RxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI1_TxRxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI1_TxHalfDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI1_RxHalfDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI1_TxRxHalfDoneCallback;

SPI_Base::Callback_t                            SPI_Base::SPI2_TxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI2_RxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI2_TxRxDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI2_TxHalfDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI2_RxHalfDoneCallback;
SPI_Base::Callback_t                            SPI_Base::SPI2_TxRxHalfDoneCallback;

DMA_HandleTypeDef* SPI_Base::m_phdma_spi1_rx;
DMA_HandleTypeDef* SPI_Base::m_phdma_spi1_tx;
DMA_HandleTypeDef* SPI_Base::m_phdma_spi2_rx;
DMA_HandleTypeDef* SPI_Base::m_phdma_spi2_tx;

DMA_HandleTypeDef SPI_Base::m_hdma_spi1_rx;
DMA_HandleTypeDef SPI_Base::m_hdma_spi1_tx;
DMA_HandleTypeDef SPI_Base::m_hdma_spi2_rx;
DMA_HandleTypeDef SPI_Base::m_hdma_spi2_tx;

Peripherals::GpioOutput* SPI_Base::m_pChipSelect_SPI1;
Peripherals::GpioOutput* SPI_Base::m_pChipSelect_SPI2;

SPI_HandleTypeDef  SPI_Base::m_hspi_1; 
SPI_HandleTypeDef  SPI_Base::m_hspi_2; 

SPI_HandleTypeDef* SPI_Base::m_phspi_1;
SPI_HandleTypeDef* SPI_Base::m_phspi_2;

Status_t SPI_Base::SPI_Init (SPIx_t SPIx, HZ_t HZ, SPI_HandleTypeDef* phSPI)
{
      GPIO_InitTypeDef GPIO_InitStruct;  
      
      if( (phSPI == nullptr) )
         return HAL_ERROR;
        
      phSPI->Init.BaudRatePrescaler = SetFrequency(HZ);
      
    /* SPI1 parameter configuration*/
    if(SPIx == SPI1_A4_A5_A6_A7)
    {   
        m_pChipSelect_SPI1->HwInit(); 
       
        m_phspi_1->Instance = SPI1;
        
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();
        
        
        /**SPI1 GPIO Configuration    
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI 
        */
        GPIO_InitStruct.Pin     = GPIO_PIN_5|GPIO_PIN_7;
        GPIO_InitStruct.Mode    = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin     = GPIO_PIN_6;
        GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull    = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
        m_phspi_1->Init.Mode                = phSPI->Init.Mode ;
        m_phspi_1->Init.Direction           = phSPI->Init.Direction;
        m_phspi_1->Init.DataSize            = phSPI->Init.DataSize;
        m_phspi_1->Init.CLKPolarity         = phSPI->Init.CLKPolarity;
        m_phspi_1->Init.CLKPhase            = phSPI->Init.CLKPhase;
        m_phspi_1->Init.NSS                 = phSPI->Init.NSS;
        m_phspi_1->Init.BaudRatePrescaler   = phSPI->Init.BaudRatePrescaler;
        m_phspi_1->Init.FirstBit            = phSPI->Init.FirstBit;
        m_phspi_1->Init.TIMode              = phSPI->Init.TIMode;
        m_phspi_1->Init.CRCCalculation      = phSPI->Init.CRCCalculation;
        m_phspi_1->Init.CRCPolynomial       = phSPI->Init.CRCPolynomial;
        
        if (HAL_SPI_Init(m_phspi_1) != HAL_OK)
        {
            return HAL_ERROR;
        }    
        
    }
    else if(SPIx == SPI1_A15_B3_B4_B5)
    {   
        m_pChipSelect_SPI1->HwInit(); 
       
        m_phspi_1->Instance = SPI1;
        
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();
        
        
        /**SPI1 GPIO Configuration    
        PA15     ------> SPI1_NSS
        PB3     ------> SPI1_SCK
        PB4     ------> SPI1_MISO
        PB5     ------> SPI1_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
        m_phspi_1->Init.Mode                = phSPI->Init.Mode ;
        m_phspi_1->Init.Direction           = phSPI->Init.Direction;
        m_phspi_1->Init.DataSize            = phSPI->Init.DataSize;
        m_phspi_1->Init.CLKPolarity         = phSPI->Init.CLKPolarity;
        m_phspi_1->Init.CLKPhase            = phSPI->Init.CLKPhase;
        m_phspi_1->Init.NSS                 = phSPI->Init.NSS;
        m_phspi_1->Init.BaudRatePrescaler   = phSPI->Init.BaudRatePrescaler;
        m_phspi_1->Init.FirstBit            = phSPI->Init.FirstBit;
        m_phspi_1->Init.TIMode              = phSPI->Init.TIMode;
        m_phspi_1->Init.CRCCalculation      = phSPI->Init.CRCCalculation;
        m_phspi_1->Init.CRCPolynomial       = phSPI->Init.CRCPolynomial;
        
        if (HAL_SPI_Init(m_phspi_1) != HAL_OK)
        {
            return HAL_ERROR;
        }    
        
    }
    else if(SPIx == SPI2_B12_B13_B14_B15)
    {   
        m_pChipSelect_SPI2->HwInit(); 
       
        m_phspi_2->Instance = SPI2;
        
        __HAL_RCC_GPIOB_CLK_ENABLE();
        
        /* Peripheral clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();
        
        
        /**SPI1 GPIO Configuration    
        PA4     ------> SPI1_NSS
        PA5     ------> SPI1_SCK
        PA6     ------> SPI1_MISO
        PA7     ------> SPI1_MOSI 
        */
        GPIO_InitStruct.Pin     = GPIO_PIN_13|GPIO_PIN_15;
        GPIO_InitStruct.Mode    = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin     = GPIO_PIN_14;
        GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull    = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
        m_phspi_2->Init.Mode                = phSPI->Init.Mode ;
        m_phspi_2->Init.Direction           = phSPI->Init.Direction;
        m_phspi_2->Init.DataSize            = phSPI->Init.DataSize;
        m_phspi_2->Init.CLKPolarity         = phSPI->Init.CLKPolarity;
        m_phspi_2->Init.CLKPhase            = phSPI->Init.CLKPhase;
        m_phspi_2->Init.NSS                 = phSPI->Init.NSS;
        m_phspi_2->Init.BaudRatePrescaler   = phSPI->Init.BaudRatePrescaler;
        m_phspi_2->Init.FirstBit            = phSPI->Init.FirstBit;
        m_phspi_2->Init.TIMode              = phSPI->Init.TIMode;
        m_phspi_2->Init.CRCCalculation      = phSPI->Init.CRCCalculation;
        m_phspi_2->Init.CRCPolynomial       = phSPI->Init.CRCPolynomial;
        
        if (HAL_SPI_Init(m_phspi_2) != HAL_OK)
        {
            return HAL_ERROR;
        }    
        
    }
    else
    {
        return HAL_ERROR;
    }

    return HAL_OK;
    
}     

uint32_t SPI_Base::SetFrequency(HZ_t HZ)
{
    const uint8_t MIN = 8, MAX = 1;
    const uint32_t SysClockFreq = HAL_RCC_GetSysClockFreq();
    const uint32_t Freq_LUT[]     = { SPI_BAUDRATEPRESCALER_2,  SPI_BAUDRATEPRESCALER_2,
                                      SPI_BAUDRATEPRESCALER_4,  SPI_BAUDRATEPRESCALER_8,
                                      SPI_BAUDRATEPRESCALER_16, SPI_BAUDRATEPRESCALER_32,
                                      SPI_BAUDRATEPRESCALER_64, SPI_BAUDRATEPRESCALER_128,
                                      SPI_BAUDRATEPRESCALER_256};
    
    if(HZ >= SysClockFreq)
        return Freq_LUT[MAX];    
    
    for( uint32_t i = 0; i <= 8U; i++ )
    {
        if( (SysClockFreq/(1<<i)) < HZ )
        {
            return Freq_LUT[i];
        }
    }
    return Freq_LUT[MIN];
}

void SPI_Base::SPI1__IRQHandler()
{
   HAL_SPI_IRQHandler(m_phspi_1);
}

void SPI_Base::SPI2__IRQHandler()
{
   HAL_SPI_IRQHandler(m_phspi_2);
}

/* IRQ handler for SP1 DMA RX */
void SPI_Base::DMA_Ch2_IRQHandler()
{
   HAL_DMA_IRQHandler(m_phspi_1->hdmarx);
}
/* IRQ handler for SP1 DMA TX */
void SPI_Base::DMA_Ch3_IRQHandler()
{
   HAL_DMA_IRQHandler(m_phspi_1->hdmatx);
}
/* IRQ handler for SP2 DMA RX */
void SPI_Base::DMA_Ch4_IRQHandler()
{
   HAL_DMA_IRQHandler(m_phspi_2->hdmarx);
}
/* IRQ handler for SP2 DMA TX */
void SPI_Base::DMA_Ch5_IRQHandler()
{
   HAL_DMA_IRQHandler(m_phspi_2->hdmatx);
}

uint32_t SPI_Base::GetStatus(SPIx_t SPIx)
{
    if( SPIx == SPI1_A4_A5_A6_A7 )
    {
        return SPI_Base::SPI1_Status;
    }
    
    return SPI_Base::SPI2_Status;
}

uint8_t SPI_Base::Poll_TxRx(SPI_TypeDef* SPIx,uint8_t data) 
{
    if(SPIx)
    {
        SPIx->CR1 |=  SPI_CR1_SPE;
        
        SPIx->DR = data;
        
        while ((SPIx->SR & SPI_FLAG_RXNE) == 0);
        
        return SPIx->DR;
    }
    return 0xFF;
}


}



extern "C"
{
 /* Transmit complete callback*/
    
using SPI_Base = Peripherals::SPI_Base;

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    // 721 Cycles total
    if(hspi->Instance == SPI1)
    {
        
        SPI_Base::m_pChipSelect_SPI1->On();
        
        SPI_Base::SPI1_Status &= ~SPI_Base::SPI_BUSY;
        
        if(SPI_Base::m_pCurentTransaction_SPI1) 
        {
            SPI_Base::m_pCurentTransaction_SPI1->TxnStatus.Event |= SPI_Base::SPI_TX_COMPLETE; 
            SPI_Base::m_pCurentTransaction_SPI1->TxnStatus.TimeUnits = SPI_Base::SPI_TIME_MILLI_SEC;
            SPI_Base::m_pCurentTransaction_SPI1->TxnStatus.TimeValue = 0;//HAL_GetTick() - SPI_Base::m_pCurentTransaction_SPI1->TxnStatus.TimeValue;            
        }
            
        if(SPI_Base::m_pSPI1_Q->IsQueueEmpty() == false)                          // 28 cycles
        {
            SPI_Base::m_pSPI1_Q->Read(SPI_Base::m_pCurentTransaction_SPI1);       // 77 cycles
            SPI_Base::m_pSPI1_Obj->Xfer(SPI_Base::m_pCurentTransaction_SPI1);     // 518 cycles
        }
        
        if(SPI_Base::SPI1_TxDoneCallback != nullptr)
            SPI_Base::SPI1_TxDoneCallback(); 
       
        
    }
    else if(hspi->Instance == SPI2)
    {
        SPI_Base::SPI2_Status &= ~SPI_Base::SPI_BUSY;
        
        SPI_Base::m_pChipSelect_SPI2->On();
        
        if(SPI_Base::m_pCurentTransaction_SPI2) 
        {
            SPI_Base::m_pCurentTransaction_SPI2->TxnStatus.Event |= SPI_Base::SPI_TX_COMPLETE; 
            SPI_Base::m_pCurentTransaction_SPI2->TxnStatus.TimeUnits = 1;
            SPI_Base::m_pCurentTransaction_SPI2->TxnStatus.TimeValue = 200;            
        }
         
        if(SPI_Base::m_pSPI2_Q->IsQueueEmpty() == false)
        {
            SPI_Base::m_pSPI2_Q->Read(SPI_Base::m_pCurentTransaction_SPI2);
            SPI_Base::m_pSPI2_Obj->Xfer(SPI_Base::m_pCurentTransaction_SPI2);
        }
        
        if(SPI_Base::SPI2_TxDoneCallback != nullptr)
            SPI_Base::SPI2_TxDoneCallback();
        
       
    }

}
 /* Receive complete callback*/
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        
        SPI_Base::m_pChipSelect_SPI1->On();
        SPI_Base::SPI1_Status &= ~SPI_Base::SPI_BUSY;
        if(SPI_Base::SPI1_RxDoneCallback != nullptr)
            SPI_Base::SPI1_RxDoneCallback();
        
        
    }
    else if(hspi->Instance == SPI2)
    {
         SPI_Base::m_pChipSelect_SPI2->On();
          SPI_Base::SPI2_Status &= ~SPI_Base::SPI_BUSY;
        if(SPI_Base::SPI2_RxDoneCallback != nullptr)
            SPI_Base::SPI2_RxDoneCallback(); 
        
       
    }    
}
 /* Transmit and Receive complete callback*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        SPI_Base::m_pChipSelect_SPI1->On();
        SPI_Base::SPI1_Status &= ~SPI_Base::SPI_BUSY;
        if(SPI_Base::SPI1_TxRxDoneCallback != nullptr)
            SPI_Base::SPI1_TxRxDoneCallback();
        
        
    }
    else if(hspi->Instance == SPI2)
    {
        SPI_Base::m_pChipSelect_SPI2->On();
        SPI_Base::SPI2_Status &= ~SPI_Base::SPI_BUSY;
        if(SPI_Base::SPI2_TxRxDoneCallback != nullptr)
            SPI_Base::SPI2_TxRxDoneCallback();   
        
       
    }   
}

 /* Transmit complete Half callback*/
void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        if(SPI_Base::SPI1_TxHalfDoneCallback != nullptr)
            SPI_Base::SPI1_TxHalfDoneCallback();
    }
    else if(hspi->Instance == SPI2)
    {
        if(SPI_Base::SPI2_TxHalfDoneCallback != nullptr)
            SPI_Base::SPI2_TxHalfDoneCallback();   
    }

}
 /* Receive complete Half callback*/
void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        if(SPI_Base::SPI1_RxHalfDoneCallback != nullptr)
            SPI_Base::SPI1_RxHalfDoneCallback();
    }
    else if(hspi->Instance == SPI2)
    {
        if(SPI_Base::SPI2_RxHalfDoneCallback != nullptr)
            SPI_Base::SPI2_RxHalfDoneCallback();   
    }    
}
 /* Transmit and Receive Half complete callback*/
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi->Instance == SPI1)
    {
        if(SPI_Base::SPI1_TxRxHalfDoneCallback != nullptr)
            SPI_Base::SPI1_TxRxHalfDoneCallback();
    }
    else if(hspi->Instance == SPI2)
    {
        if(SPI_Base::SPI2_TxRxHalfDoneCallback != nullptr)
            SPI_Base::SPI2_TxRxHalfDoneCallback();   
    }   
}


}
