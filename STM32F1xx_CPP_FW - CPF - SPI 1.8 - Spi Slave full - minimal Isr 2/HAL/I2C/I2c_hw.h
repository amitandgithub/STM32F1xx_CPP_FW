
#ifndef I2C_HW_H
#define I2C_HW_H

#include"chip_peripheral.h"

#define I2C_DATA_REG(_I2C_)             (_I2C_->DR)

#define I2C_GENERATE_START(_I2C_)       _I2C_->CR1 |= I2C_CR1_START

#define I2C_GENERATE_STOP(_I2C_)        _I2C_->CR1 |= I2C_CR1_STOP

#define I2C_ENABLE_ACK(_I2C_)           _I2C_->CR1 |= I2C_CR1_ACK

#define I2C_DISABLE_ACK(_I2C_)          _I2C_->CR1 &= ~I2C_CR1_ACK
  
#define I2C_ENABLE_POS(_I2C_)           _I2C_->CR1 |= I2C_CR1_POS

#define I2C_DISABLE_POS(_I2C_)          _I2C_->CR1 &= ~I2C_CR1_POS

#define I2C_ENABLE(_I2C_)               _I2C_->CR1 |= I2C_CR1_PE

#define I2C_CLOCK_ENABLE(_I2C_)         CLK->PCKENR |= ((uint8_t)CLK_Peripheral_I2C)

#define I2C_CLOCK_DISABLE(_I2C_)        CLK->PCKENR &= (uint8_t)(~(uint8_t)CLK_Peripheral_I2C)

#define I2C_DISABLE(_I2C_)              _I2C_->CR1 &= ~I2C_CR1_PE

#define I2C_SOFT_RESET(_I2C_)        
  
#define I2C_CLEAR_ADDR(_I2C_)           LL_I2C_ClearFlag_ADDR(_I2C_)

#define I2C_CLEAR_AF(_I2C_)             LL_I2C_ClearFlag_AF(_I2C_)

#define I2C_CLEAR_ARLO(_I2C_)           LL_I2C_ClearFlag_ARLO(_I2C_)

#define I2C_CLEAR_BERR(_I2C_)           LL_I2C_ClearFlag_BERR(_I2C_)

#define I2C_CLEAR_OVR(_I2C_)            LL_I2C_ClearFlag_OVR(_I2C_)

#define I2C_CLEAR_STOPF(_I2C_)          LL_I2C_ClearFlag_STOP(_I2C_)

#define I2C_ENABLE_INT_BUF(_I2C_)       SET_BIT(_I2C_->CR2, I2C_CR2_ITBUFEN)

#define I2C_DISABLE_INT_BUF(_I2C_)      CLEAR_BIT(_I2C_->CR2, I2C_CR2_ITBUFEN)

#define I2C_ENABLE_INT_EVT(_I2C_)       SET_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN)

#define I2C_DISABLE_INT_EVT(_I2C_)      CLEAR_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN)

#define I2C_ENABLE_INT_EVT_ERR(_I2C_)       SET_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN)

#define I2C_DISABLE_INT_EVT_ERR(_I2C_)      CLEAR_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITERREN)

#define I2C_ENABLE_INT_EVT_BUF_ERR(_I2C_)       SET_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN)

#define I2C_DISABLE_INT_EVT_BUF_ERR(_I2C_)      CLEAR_BIT(_I2C_->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN)

#define I2C_SET_STOPF(_I2C_,FLAG)       _I2C_->CR1 |= FLAG

#define I2C_EVENT_SB(REG)       REG & I2C_SR1_SB

#define I2C_EVENT_ADDR(REG)     REG & I2C_SR1_ADDR

#define I2C_EVENT_RXNE(REG)     REG & I2C_SR1_RXNE

#define I2C_EVENT_BTF(REG)      REG & I2C_SR1_BTF

#define I2C_EVENT_TXE(REG)      REG & I2C_SR1_TXE

#define I2C_EVENT_STOPF(REG)    REG & I2C_SR1_STOPF

#define I2C_EVENT_ADD10(REG)    REG & I2C_SR1_ADD10

#define I2C_EVENT_AF(REG)       REG & I2C_SR1_AF

#define I2C_GET_EVENT(_I2C_)    POSITION_VAL(_I2C_->SR1)

#define I2C_ENABLE_ACK_AND_START(_I2C_) _I2C_->CR1 |= I2C_CR1_ACK | I2C_CR1_START

#define WAIT_FOR_ADDR_FLAG_TO_SET(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR1, I2C_SR1_ADDR, 0, TIMEOUT)

#define WAIT_FOR_SB_FLAG_TO_SET(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR1, I2C_SR1_SB, 0, TIMEOUT)

#define WAIT_FOR_TXE_FLAG_TO_SET(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR1, I2C_SR1_TXE, 0, TIMEOUT)

#define WAIT_FOR_RXNE_FLAG_TO_SET(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR1, I2C_SR1_RXNE, 0, TIMEOUT)

#define WAIT_FOR_BTF_FLAG_TO_SET(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR1, I2C_SR1_BTF, 0, TIMEOUT)

#define WAIT_FOR_STOP_FLAG_TO_CLEAR(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->CR1, I2C_CR1_STOP, I2C_CR1_STOP, TIMEOUT)

#define WAIT_FOR_BUSY_FLAG_TO_CLEAR(_I2C_,TIMEOUT) WaitOnFlag(&_I2C_->SR2, I2C_SR2_BUSY, I2C_SR2_BUSY, TIMEOUT)



#define I2C_DMA_ENABLE(_I2C_)               _I2C_->CR2 |= I2C_CR2_DMAEN

#define I2C_DMA_DISABLE(_I2C_)              _I2C_->CR2 &= ~I2C_CR2_DMAEN

#define I2C_DMA_LAST_BIT_ENABLE(_I2C_)      _I2C_->CR2 |= I2C_CR2_LAST

#define I2C_DMA_LAST_BIT_DISABLE(_I2C_)     _I2C_->CR2 &= ~I2C_CR2_LAST



/* Non Hardware Stuff, general header file stuff */

#define I2C_BUF_BYTE_IN(__I2C_BUF)              (*__I2C_BUF.RxBuf++) = I2C_DATA_REG(m_I2Cx); __I2C_BUF.RxLen--

#define I2C_BUF_BYTE_OUT(__I2C_BUF)             I2C_DATA_REG(m_I2Cx) = (*__I2C_BUF.TxBuf++); __I2C_BUF.TxLen--

#define I2C_SLAVE_BUF_BYTE_IN(__I2C_BUF)        __I2C_BUF.RxBuf->Buf[__I2C_BUF.RxBuf->Idx++] = I2C_DATA_REG(m_I2Cx)

#define I2C_SLAVE_BUF_BYTE_OUT(__I2C_BUF)        I2C_DATA_REG(m_I2Cx) = __I2C_BUF.TxBuf->Buf[__I2C_BUF.TxBuf->Idx++] 



#endif //I2C_HW_H