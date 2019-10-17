/******************
** FILE: SPI.cpp
**
** DESCRIPTION:
**  SPI implementation in polling mode
**
** CREATED: 9/2/2018, by Amit Chaudhary
******************/

#include "SPI_Poll.h"

namespace Peripherals
{
  


SPI_Poll::SPI_Poll (SPIx_t spix, GpioOutput* pCS, HZ_t hz,SPI_HandleTypeDef* phspi_x) : m_hz(hz), m_spix(spix)
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



Status_t SPI_Poll::HwInit ()
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
    
    SPI1_Status |= SPI_INIT_DONE; 
    
    return Status;
    
}

Status_t SPI_Poll::Tx(uint8_t* pTxBuf, uint16_t TxLen)
{
    Status_t Status = 0;
    
    if((pTxBuf == nullptr) || (TxLen == 0U) )   return HAL_ERROR;    
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Transmit(m_phspi_1,pTxBuf, TxLen, SPI_POLL_DELAY);
        m_pChipSelect_SPI1->On();
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Transmit(m_phspi_2,pTxBuf, TxLen, SPI_POLL_DELAY);
        m_pChipSelect_SPI2->On();
    }
    
    return Status;
}

Status_t SPI_Poll::Rx(uint8_t* pRxBuf, uint16_t RxLen)
{
    Status_t Status = 0;
    
    if((pRxBuf == nullptr) || (RxLen == 0U) )   return HAL_ERROR;    
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_Receive(m_phspi_1,pRxBuf, RxLen, SPI_POLL_DELAY);
        m_pChipSelect_SPI1->On();
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
        m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_Receive(m_phspi_2,pRxBuf, RxLen, SPI_POLL_DELAY);
        m_pChipSelect_SPI2->On();
    }
    
    return Status;

}

Status_t SPI_Poll::TxRx(uint8_t* pTxBuf, uint8_t* pRxBuf, uint16_t Len)
{
    Status_t Status = 0;
    
    if((pTxBuf == nullptr) || (pRxBuf == nullptr) || (Len == 0U) )    return HAL_ERROR;  
    
    if(m_spix == SPI1_A4_A5_A6_A7)
    {
        //m_pChipSelect_SPI1->Off();
        Status = HAL_SPI_TransmitReceive(m_phspi_1,pTxBuf, pRxBuf, Len, SPI_POLL_DELAY);
       // m_pChipSelect_SPI1->On();
    }
    else if(m_spix == SPI2_B12_B13_B14_B15)
    {
       // m_pChipSelect_SPI2->Off();
        Status = HAL_SPI_TransmitReceive(m_phspi_2,pTxBuf, pRxBuf, Len, SPI_POLL_DELAY);
        //m_pChipSelect_SPI2->On();
    }
    
    return Status;
}

Status_t SPI_Poll::Xfer(Transaction_t* pTransaction)
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

Status_t SPI_Poll::Post(Transaction_t* pTransaction)
{
    
    if( m_spix == SPI1_A4_A5_A6_A7 )
    {
        m_pCurentTransaction_SPI1 = pTransaction;
       
        return Xfer(m_pCurentTransaction_SPI1);
    }    
    else
    {
        m_pCurentTransaction_SPI2 = pTransaction;
        
        return Xfer(m_pCurentTransaction_SPI2);
    
    }
    
}









}