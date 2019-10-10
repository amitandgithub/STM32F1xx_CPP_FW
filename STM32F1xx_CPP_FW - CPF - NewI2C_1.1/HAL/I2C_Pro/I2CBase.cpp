/******************
** FILE: I2CBase.cpp
**
** DESCRIPTION:
**  I2C common features implementation
**
** CREATED: 6/20/2019, by Amit Chaudhary
******************/

#include "I2CBase.h"

namespace HAL
{
#ifdef I2C_DEBUG 
    I2CBase::I2CLogs_t  I2CBase::I2CStates[I2C_LOG_STATES_SIZE];
    uint32_t            I2CBase::I2CStates_Idx;
#endif
        
    
    I2CBase::I2CBase(Pin_t scl, Pin_t sda, Hz_t Hz) :
        m_sclPin(scl,GpioOutput::AF_OPEN_DRAIN),
        m_sdaPin(sda,GpioOutput::AF_OPEN_DRAIN),
        m_hz(Hz)
        {
            if((scl == Gpio::B6) && (sda == Gpio::B7))
            {
                m_I2Cx = I2C1;
            }
            else if((scl == Gpio::B10) && (sda == Gpio::B11))
            {
                m_I2Cx = I2C2;
            }
            else if((scl == Gpio::B8) && (sda == Gpio::B9))
            {
                // initialize with nullptr to configure it later in HwInit() with pin remap
                m_I2Cx = nullptr;
            }
            else
            {
                while(1); // Fatal Error
            }
            
            m_I2CStatus = I2C_OK;
            
        }
        
        I2CBase::I2CStatus_t I2CBase::HwInit(void *pInitStruct)
        {
            LL_I2C_InitTypeDef I2C_InitStruct;
            
            /* Set I2C_InitStruct fields to default values */
            I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
            I2C_InitStruct.ClockSpeed      = m_hz;
            I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_2;
            I2C_InitStruct.OwnAddress1     = 0U;
            I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
            I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
            
            LL_I2C_SetOwnAddress2(m_I2Cx,I2C_OWN_SLAVE_ADDRESS);
            
            m_sclPin.HwInit();
            m_sdaPin.HwInit();
            
            m_sclPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            m_sdaPin.SetPinSpeed(GpioOutput::GPIO_OUTPUT_SPEED_HIGH);
            
            if(m_I2Cx == nullptr)
            {
                // Remap B8 and B9 Pin in I2C mode 
            }
            
            ClockEnable();
            
            if(pInitStruct == nullptr)
            {
                LL_I2C_Init(m_I2Cx, &I2C_InitStruct);
            }
            else
            {
                LL_I2C_Init(m_I2Cx, (LL_I2C_InitTypeDef*)pInitStruct);
            }        
            
            return I2C_OK;
            
        }
        void I2CBase::ClockEnable()
        {
            if(m_I2Cx == I2C1)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(m_I2Cx == I2C2)
            {
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
        void I2CBase::ClockDisable()
        {
            if(m_I2Cx == I2C1)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
            }
            else if(m_I2Cx == I2C2)
            {
                LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C2);
            }        
        }
        
}