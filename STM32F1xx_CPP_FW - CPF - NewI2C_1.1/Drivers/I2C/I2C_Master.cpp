/******************
** FILE: I2C_Drv.cpp
**
** DESCRIPTION:
**  I2C class Implementation
**
** CREATED: 8/14/2018, by Amit Chaudhary
******************/

#include "I2C_Master.hpp"

namespace Peripherals
{
#define I2C_TIMEOUT_FLAG          35U         /*!< Timeout 35 ms             */
#define I2C_TIMEOUT_BUSY_FLAG     25U         /*!< Timeout 25 ms             */
    
I2C_Master::I2C_Master(I2CPort_t I2CPort, I2CBaudRate_t I2CBaudRate) : m_I2CPort(I2CPort)
{
    if( (I2CPort == I2C1_SCL_B6_SDA_B7) || (I2CPort == I2C1_SCL_B8_SDA_B9) )
    {
        m_hi2c.Instance = I2C1;
    }
    else if(I2CPort == I2C2_SCL_B11_SDA_B12)
    {
        m_hi2c.Instance = I2C2; 
    }
    else
    {
        m_hi2c.Instance = I2C1;
    }
    if(I2CBaudRate == BAUD_100K )
    {
        m_hi2c.Init.ClockSpeed = 100000;
    }
    else if(I2CBaudRate == BAUD_400K)
    {
        m_hi2c.Init.ClockSpeed = 400000;
    }
    else
    {
        m_hi2c.Init.ClockSpeed = 100000;
    }
        
}

Status_t I2C_Master::HwInit()
{    
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    if(m_hi2c.Instance==I2C1)
    {  
        /* Peripheral clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();
        
        if(m_I2CPort == I2C1_SCL_B6_SDA_B7)
        {
            /**I2C1 GPIO Configuration    
            PB6     ------> I2C1_SCL
            PB7     ------> I2C1_SDA 
            */
            GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
            
        }
        else if(m_I2CPort == I2C1_SCL_B8_SDA_B9)
        {
            /**I2C1 GPIO Configuration    
            PB8     ------> I2C1_SCL
            PB9     ------> I2C1_SDA 
            */
            GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);           
        }        
        
        
    }
    else if(m_hi2c.Instance==I2C2)
    {
        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE(); 
        /**I2C1 GPIO Configuration    
        PB10     ------> I2C1_SCL
        PB11     ------> I2C1_SDA 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);              
       
    }
    
    m_hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
    m_hi2c.Init.OwnAddress1 = 0;
    m_hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    m_hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    m_hi2c.Init.OwnAddress2 = 0;
    m_hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    m_hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&m_hi2c) != HAL_OK)
    {
        //_Error_Handler(__FILE__, __LINE__);
        return -1;
    }
    return 0;
}
  
Status_t I2C_Master::Send (uint16_t Addrs, uint8_t *pTxBuf, uint16_t Len)
{
    return HAL_I2C_Master_Transmit(&m_hi2c, Addrs, pTxBuf, Len, I2C_TIMEOUT_FLAG);
}

Status_t I2C_Master::Read (uint16_t Addrs, uint8_t *pRxBuf, uint16_t Len)
{
    return HAL_I2C_Master_Receive(&m_hi2c, Addrs, pRxBuf, Len, I2C_TIMEOUT_FLAG);
}

Status_t I2C_Master::Scan(uint16_t *DevAddress, uint16_t len )
{
    uint16_t idx = 0;
   if( DevAddress && (len>0))
   {
    for(uint16_t i = 0; i< 256; i++)
    {
        if (HAL_OK == HAL_I2C_IsDeviceReady(&m_hi2c, i<<1, 2, 2))
            DevAddress[idx++ % len] = i;
    }  
   return HAL_OK;
   }
   return HAL_ERROR;
  
}





    
    
}