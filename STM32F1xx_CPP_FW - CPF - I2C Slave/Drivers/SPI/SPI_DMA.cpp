/******************
** FILE: SPI_DMA.cpp
**
** DESCRIPTION:
**  SPI implementation in Interrupt mode
**
** CREATED: 9/2/2018, by Amit Chaudhary
******************/

#include "SPI_DMA.h"
#include "Queue.h"

namespace Peripherals
{


    
SPI_DMA::SPI_DMA (SPIx_t spix, GpioOutput* pCS, HZ_t hz, 
                  SPI_HandleTypeDef* phspi_x,
                  DMA_HandleTypeDef* phdma_spix_rx,
                  DMA_HandleTypeDef* phdma_spix_tx)
                  : m_hz(hz),
                    m_spix(spix)

{
    
    
     if((m_spix == SPI1_A4_A5_A6_A7) || (m_spix == SPI1_A15_B3_B4_B5) )
     {
        m_pChipSelect_SPI1 = pCS;
        
        m_phspi_1 = phspi_x;
        m_phdma_spi1_rx = phdma_spix_rx;
        m_phdma_spi1_tx = phdma_spix_tx;
        
        if(phspi_x          == nullptr)     m_phspi_1 = &m_hspi_1;
        if(m_phdma_spi1_rx  == nullptr)     m_phdma_spi1_rx = &m_hdma_spi1_rx;
        if(m_phdma_spi1_tx  == nullptr)     m_phdma_spi1_tx = &m_hdma_spi1_tx;
        
        if( (m_pChipSelect_SPI1 == nullptr)  )
        {
            while(1); // Fatal Error
        }
        SPI_Base::m_pSPI1_Obj = this;
     }
     else if(m_spix == SPI2_B12_B13_B14_B15)
     {
        m_pChipSelect_SPI2 = pCS;
         
        m_phspi_2 = phspi_x;
        m_phdma_spi2_rx = phdma_spix_rx;
        m_phdma_spi2_tx = phdma_spix_tx;
        
        if(phspi_x          == nullptr)     m_phspi_2 = &m_hspi_1;
        if(m_phdma_spi2_rx  == nullptr)     m_phdma_spi2_rx = &m_hdma_spi2_rx;
        if(m_phdma_spi2_tx  == nullptr)     m_phdma_spi2_tx = &m_hdma_spi2_tx;
        
        if( (m_pChipSelect_SPI2 == nullptr)  )
        {
            while(1); // Fatal Error
        }
        SPI_Base::m_pSPI2_Obj = this;
     }
     else
     {
         while(1); // Fatal error
     }
}


Status_t SPI_DMA::HwInit ()
{    
    Status_t Status;
    SPI_HandleTypeDef hspi;
    
    hspi.Init.Mode                = SPI_MODE_MASTER;
    hspi.Init.Direction           = SPI_DIRECTION_2LINES;
    hspi.Init.DataSize            = SPI_DATASIZE_8BIT;
    hspi.Init.CLKPolarity         = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase            = SPI_PHASE_1EDGE;
    hspi.Init.NSS                 = SPI_NSS_HARD_OUTPUT;
    hspi.Init.BaudRatePrescaler   = SPI_BAUDRATEPRESCALER_128;//SPI_BAUDRATEPRESCALER_2;
    hspi.Init.FirstBit            = SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode              = SPI_TIMODE_DISABLE;
    hspi.Init.CRCCalculation      = SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial       = 10;
        
    Status = SPI_Base::SPI_Init( m_spix, m_hz,&hspi );
    
    if(Status != HAL_OK)
        return HAL_ERROR;
    
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
        
    if( (m_spix == SPI1_A4_A5_A6_A7) || (m_spix == SPI1_A15_B3_B4_B5) )
    { 

        
        /* SPI1 DMA Init */
        /* SPI1_RX Init */
        m_phdma_spi1_rx->Instance                           = DMA1_Channel2;
        m_phdma_spi1_rx->Init.Direction                     = DMA_PERIPH_TO_MEMORY;
        m_phdma_spi1_rx->Init.PeriphInc                     = DMA_PINC_DISABLE;
        m_phdma_spi1_rx->Init.MemInc                        = DMA_MINC_ENABLE;
        m_phdma_spi1_rx->Init.PeriphDataAlignment           = DMA_PDATAALIGN_BYTE;
        m_phdma_spi1_rx->Init.MemDataAlignment              = DMA_MDATAALIGN_BYTE;
        m_phdma_spi1_rx->Init.Mode                          = DMA_NORMAL;
        m_phdma_spi1_rx->Init.Priority                      = DMA_PRIORITY_LOW;
        
        if (HAL_DMA_Init(m_phdma_spi1_rx) != HAL_OK)
        {
            //_Error_Handler(__FILE__, __LINE__);
             return HAL_ERROR;
        }
        
        __HAL_LINKDMA(m_phspi_1,hdmarx,*m_phdma_spi1_rx);
        
        /* SPI1_TX Init */
        m_phdma_spi1_tx->Instance                           = DMA1_Channel3;
        m_phdma_spi1_tx->Init.Direction                     = DMA_MEMORY_TO_PERIPH;
        m_phdma_spi1_tx->Init.PeriphInc                     = DMA_PINC_DISABLE;
        m_phdma_spi1_tx->Init.MemInc                        = DMA_MINC_ENABLE;
        m_phdma_spi1_tx->Init.PeriphDataAlignment           = DMA_PDATAALIGN_BYTE;
        m_phdma_spi1_tx->Init.MemDataAlignment              = DMA_MDATAALIGN_BYTE;
        m_phdma_spi1_tx->Init.Mode                          = DMA_NORMAL;
        m_phdma_spi1_tx->Init.Priority                      = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(m_phdma_spi1_tx) != HAL_OK)
        {
           // _Error_Handler(__FILE__, __LINE__);
            return HAL_ERROR;
        }
        
        __HAL_LINKDMA(m_phspi_1,hdmatx,*m_phdma_spi1_tx);
        
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(SPI1__IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(SPI1_IRQHandler));
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(DMA_Ch2_IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(DMA1_Channel2_IRQn),0,0);
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(DMA_Ch3_IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(DMA1_Channel3_IRQn),0,0);
        SPI1_Status |= SPI_INIT_DONE; 
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        /* SPI2 DMA Init */
        /* SPI2_RX Init */
        m_phdma_spi2_rx->Instance                   = DMA1_Channel4;
        m_phdma_spi2_rx->Init.Direction             = DMA_PERIPH_TO_MEMORY;
        m_phdma_spi2_rx->Init.PeriphInc             = DMA_PINC_DISABLE;
        m_phdma_spi2_rx->Init.MemInc                = DMA_MINC_ENABLE;
        m_phdma_spi2_rx->Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        m_phdma_spi2_rx->Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        m_phdma_spi2_rx->Init.Mode                  = DMA_NORMAL;
        m_phdma_spi2_rx->Init.Priority              = DMA_PRIORITY_LOW;
        
        if (HAL_DMA_Init(m_phdma_spi2_rx) != HAL_OK)
        {
            //_Error_Handler(__FILE__, __LINE__);
            return HAL_ERROR;
        }
        
        __HAL_LINKDMA(m_phspi_2,hdmarx,*m_phdma_spi2_rx);
        
        /* SPI2_TX Init */
        m_phdma_spi2_tx->Instance                   = DMA1_Channel5;
        m_phdma_spi2_tx->Init.Direction             = DMA_MEMORY_TO_PERIPH;
        m_phdma_spi2_tx->Init.PeriphInc             = DMA_PINC_DISABLE;
        m_phdma_spi2_tx->Init.MemInc                = DMA_MINC_ENABLE;
        m_phdma_spi2_tx->Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        m_phdma_spi2_tx->Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        m_phdma_spi2_tx->Init.Mode                  = DMA_NORMAL;
        m_phdma_spi2_tx->Init.Priority              = DMA_PRIORITY_LOW;
        
        if (HAL_DMA_Init(m_phdma_spi2_tx) != HAL_OK)
        {
           // _Error_Handler(__FILE__, __LINE__);
            return HAL_ERROR;
        }
        
        __HAL_LINKDMA(m_phspi_2,hdmatx,*m_phdma_spi2_tx);
        
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(SPI2__IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(SPI2_IRQHandler));
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(DMA_Ch4_IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(DMA1_Channel4_IRQn),0,0);
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(DMA_Ch5_IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(DMA1_Channel5_IRQn),0,0); 
        SPI2_Status |= SPI_INIT_DONE; 
    }
    else
    {
        while(1); // Fatal Error
    }
    return Status;    
}

Status_t SPI_DMA::Tx(uint8_t* pTxBuf, uint16_t TxLen)
{
    Status_t Status = HAL_ERROR;
    
    if((pTxBuf == nullptr) || (TxLen == 0U) )   return HAL_ERROR;    
    
    if( (m_spix == SPI1_A4_A5_A6_A7) && (!(SPI_Base::SPI1_Status & SPI_Base::SPI_BUSY)) )
    {
        SPI_Base::SPI1_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Transmit_DMA(m_phspi_1,pTxBuf, TxLen);
        return Status;
    }
    else if( (m_spix == SPI2_B12_B13_B14_B15) && (!(SPI_Base::SPI2_Status & SPI_Base::SPI_BUSY)) )
    {
        SPI_Base::SPI2_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Transmit_DMA(m_phspi_2,pTxBuf, TxLen);
        return Status;
    }
    
    return Status;
}

Status_t SPI_DMA::Rx(uint8_t* pRxBuf, uint16_t RxLen)
{
    Status_t Status = HAL_ERROR;
    
    if((pRxBuf == nullptr) || (RxLen == 0U) )   return HAL_ERROR;    
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        SPI_Base::SPI1_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Receive_DMA(m_phspi_1,pRxBuf, RxLen);
        return Status;
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        SPI_Base::SPI2_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Receive_DMA(m_phspi_2,pRxBuf, RxLen);
        return Status;
    }
    
    return Status;
}

Status_t SPI_DMA::TxRx(uint8_t* pTxBuf, uint8_t* pRxBuf, uint16_t Len)
{
        Status_t Status = HAL_ERROR;
    
     if((pTxBuf == nullptr) || (pRxBuf == nullptr) || (Len == 0U) )    return HAL_ERROR;
    
    if( (m_spix == SPI1_A4_A5_A6_A7) && (!(SPI_Base::SPI1_Status & SPI_Base::SPI_BUSY)) )
    {
        SPI_Base::SPI1_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_TransmitReceive_DMA(m_phspi_1,pTxBuf, pRxBuf, Len);
        return Status;
    }
    else if( (m_spix == SPI2_B12_B13_B14_B15) && (!(SPI_Base::SPI2_Status & SPI_Base::SPI_BUSY)) )
    {
        SPI_Base::SPI2_Status |= SPI_Base::SPI_BUSY;
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_TransmitReceive_DMA(m_phspi_2,pTxBuf, pRxBuf, Len);
        return Status;
    }
    
    return Status;
}

Status_t SPI_DMA::Xfer(Transaction_t* pTransaction)
{
    Status_t Status = HAL_ERROR;
    if(pTransaction == nullptr)
    {
        return HAL_ERROR;           
    }    
    else if ( (pTransaction->TxBuf == nullptr) && (pTransaction->TxLen == 0) && (pTransaction->RxBuf == nullptr)  && (pTransaction->RxLen == 0))
    {
        return HAL_ERROR;
    }
    
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        m_pChipSelect_SPI1 = pTransaction->pCS;
    }
    else if( m_spix == SPI2_B12_B13_B14_B15 )
    {
        m_pChipSelect_SPI2 = pTransaction->pCS;
    }
    
    if( ((pTransaction->TxBuf != nullptr) || (pTransaction->TxLen != 0)) && ((pTransaction->RxBuf == nullptr)  || (pTransaction->RxLen == 0)) )
    {        
         Status = Tx(pTransaction->TxBuf,pTransaction->TxLen);       
    }
    else if( ((pTransaction->TxBuf == nullptr) || (pTransaction->TxLen == 0)) && ((pTransaction->RxBuf != nullptr)  || (pTransaction->RxLen != 0)) )
    {        
         Status = Rx(pTransaction->RxBuf,pTransaction->RxLen);   
    }
    else if((pTransaction->TxBuf != nullptr) && (pTransaction->TxLen != 0) && (pTransaction->RxBuf != nullptr)  && (pTransaction->RxLen != 0))
    {    
        if(pTransaction->TxLen == pTransaction->RxLen)
        {
            Status =  TxRx(pTransaction->TxBuf, pTransaction->RxBuf, pTransaction->TxLen);   
        }
        else
        {
            Status = Tx(pTransaction->TxBuf,pTransaction->TxLen);
            
            Status = Rx(pTransaction->RxBuf,pTransaction->RxLen); 
        }

    }
    
    return Status;
    
}
 
Status_t SPI_DMA::Post(Transaction_t* pTransaction)
{
    
#if 1
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        m_pCurentTransaction_SPI1 = pTransaction;
        
        if(  (SPI_Base::SPI1_Status & SPI_Base::SPI_BUSY) == SPI_Base::SPI_BUSY  )
        {
            
            return m_pSPI1_Q->Write(m_pCurentTransaction_SPI1);
            
        }
        else
        {
            return Xfer(m_pCurentTransaction_SPI1);
        }
    }    
    else
    {
        m_pCurentTransaction_SPI2 = pTransaction;
        
        if(  (SPI_Base::SPI2_Status & SPI_Base::SPI_BUSY) == SPI_Base::SPI_BUSY  )
        {
            
            return m_pSPI2_Q->Write(m_pCurentTransaction_SPI2);
            
        }
        else
        {
            return Xfer(m_pCurentTransaction_SPI2);
        }
    }
#elif 0  
    
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        return m_pSPI1_Q->Write(m_pCurentTransaction_SPI1);
    }
    else
    {
        return m_pSPI2_Q->Write(m_pCurentTransaction_SPI2);
    }
#endif
    
}

void SPI_DMA::Run()
{        
    
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        if(Peripherals::SPI_Base::m_pSPI1_Q->IsQueueEmpty() == false)
        {
            SPI_DMA::m_pSPI1_Q->Read(m_pCurentTransaction_SPI1);
            Xfer(m_pCurentTransaction_SPI1);
        }
    }
    else if( m_spix == SPI2_B12_B13_B14_B15 )
    {
        if(Peripherals::SPI_Base::m_pSPI2_Q->IsQueueEmpty() == false)
        {
            SPI_DMA::m_pSPI2_Q->Read(m_pCurentTransaction_SPI2);
            Xfer(m_pCurentTransaction_SPI2);
        }  
    }
    else
    {
        while(1); // Fatal Error
    }
    
}

}// Namespace Peripherals




