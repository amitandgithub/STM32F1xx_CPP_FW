/******************
** FILE: SPI.cpp
**
** DESCRIPTION:
**  SPI implementation in Interrupt mode
**
** CREATED: 9/2/2018, by Amit Chaudhary
******************/

#include "SPI_IT.h"

namespace Peripherals
{

SPI_IT::SPI_IT (SPIx_t spix, GpioOutput* pCS, HZ_t hz, 
                  SPI_HandleTypeDef* phspi_x,
                  DMA_HandleTypeDef* phdma_spix_rx,
                  DMA_HandleTypeDef* phdma_spix_tx)
                  : m_hz(hz),
                    m_spix(spix)
{
    if((m_spix == SPI1_A4_A5_A6_A7) || (m_spix == SPI1_A15_B3_B4_B5) )
     {
        m_pChipSelect_SPI1 = pCS;
        
       if(phspi_x == nullptr) m_phspi_1 = &m_hspi_1;
        
        if( (m_pChipSelect_SPI1 == nullptr)  )
        {
            while(1); // Fatal Error
        }
        
     }
     else if(m_spix == SPI2_B12_B13_B14_B15)
     {
        m_pChipSelect_SPI2 = pCS;
        
        if(phspi_x == nullptr) m_phspi_2 = &m_hspi_2;
        
        if( (m_pChipSelect_SPI2 == nullptr) )
        {
            while(1); // Fatal Error
        }
     }
     else
     {
         while(1); // Fatal error
     }
}




Status_t SPI_IT::HwInit ()
{
    Status_t Status;
    SPI_HandleTypeDef hspi;
    
    hspi.Init.Mode                = SPI_MODE_MASTER;
    hspi.Init.Direction           = SPI_DIRECTION_2LINES;
    hspi.Init.DataSize            = SPI_DATASIZE_8BIT;
    hspi.Init.CLKPolarity         = SPI_POLARITY_LOW;
    hspi.Init.CLKPhase            = SPI_PHASE_1EDGE;
    hspi.Init.NSS                 = SPI_NSS_HARD_OUTPUT;
    hspi.Init.BaudRatePrescaler   = SPI_BAUDRATEPRESCALER_256;//SPI_BAUDRATEPRESCALER_2;
    hspi.Init.FirstBit            = SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode              = SPI_TIMODE_DISABLE;
    hspi.Init.CRCCalculation      = SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial       = 10;
        
    Status = SPI_Base::SPI_Init( m_spix, m_hz,&hspi );
    
    if(Status != HAL_OK)
        return HAL_ERROR;
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    { 
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(SPI1__IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(SPI1_IRQHandler));
        SPI1_Status |= SPI_INIT_DONE; 
    }
    else if(m_spix == SPI1_A15_B3_B4_B5)
    {
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(SPI1__IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(SPI1_IRQHandler));
        SPI1_Status |= SPI_INIT_DONE; 
    }
    else if(m_spix == SPI1_A15_B3_B4_B5)
    {
        Peripherals::Interrupt::RegisterInterrupt_Vct_Table(SPI2__IRQHandler,static_cast<Peripherals::Interrupt::IRQn>(SPI2_IRQHandler));
        SPI2_Status |= SPI_INIT_DONE; 
    }
    else
    {
        while(1); // Fatal Error
    }
    return Status;
}

Status_t SPI_IT::Tx(uint8_t* pTxBuf, uint16_t TxLen)
{
    
    Status_t Status = 0;
    
    if((pTxBuf == nullptr) || (TxLen == 0U) )   return HAL_ERROR;    
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Transmit_IT(m_phspi_1,pTxBuf, TxLen);
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Transmit_IT(m_phspi_2,pTxBuf, TxLen);
    }
    
    return Status;
    
}

Status_t SPI_IT::Rx(uint8_t* pRxBuf, uint16_t RxLen)
{
    Status_t Status = 0;
    
    if((pRxBuf == nullptr) || (RxLen == 0U) )   return HAL_ERROR;  
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Receive_IT(m_phspi_1,pRxBuf, RxLen);
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Receive_IT(m_phspi_2,pRxBuf, RxLen);
    }
    return Status;
}

Status_t SPI_IT::TxRx(uint8_t* pTxBuf, uint8_t* pRxBuf, uint16_t Len)
{
    Status_t Status = 0;
    
    if((pTxBuf == nullptr) || (pRxBuf == nullptr) || (Len == 0U) )    return HAL_ERROR;
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_TransmitReceive_IT(m_phspi_1,pTxBuf, pRxBuf, Len);
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_TransmitReceive_IT(m_phspi_2,pTxBuf, pRxBuf, Len);
    }
    return Status;
}
Status_t SPI_IT::Xfer(Transaction_t* pTransaction)
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

Status_t SPI_IT::Post(Transaction_t* pTransaction)
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


/*
Status_t SPI_IT::Xfer(Transaction_t* pTransaction)
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

 
Status_t SPI_IT::Post(Transaction_t* pTransaction)
{
#if 1
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        if(  (SPI_Base::SPI1_Status & SPI_Base::SPI_BUSY) == SPI_Base::SPI_BUSY  )
        {
            
            return m_pSPI1_Q->Write(pTransaction);
            
        }
        else
        {
            return Xfer(pTransaction);
        }
    }    
    else
    {
        if(  (SPI_Base::SPI2_Status & SPI_Base::SPI_BUSY) == SPI_Base::SPI_BUSY  )
        {
            
            return m_pSPI2_Q->Write(pTransaction);
            
        }
        else
        {
            return Xfer(pTransaction);
        }
    }
#elif 0  
    
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        return m_pSPI1_Q->Write(pTransaction);
    }
    else
    {
        return m_pSPI2_Q->Write(pTransaction);
    }
#endif
    
}

*/

}

