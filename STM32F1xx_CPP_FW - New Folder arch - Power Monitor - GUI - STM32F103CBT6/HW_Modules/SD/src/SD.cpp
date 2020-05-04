/******************
** FILE: SD.cpp
**
** DESCRIPTION:
**  SD card functionality
**
** CREATED: 10/31/2018, by Amit Chaudhary
******************/

#include "SD.h"


namespace BSP
{

    

SD::SD(SPIDrv_t*   pSpiDriverSD , Port_t CSPort, PIN_t CSPin) : m_pSpiDriverSD(pSpiDriverSD), m_pSD_CS(CSPort,CSPin)
{


}

void SD::HwInit()
{                                         
    m_pSpiDriverSD->HwInit(); 
    m_pSD_CS.HwInit();
}

uint8_t SD::spi_txrx(uint8_t data)
{
    return m_pSpiDriverSD->TxRxPoll(data,1); 
}



} // Namespace Peripherals
