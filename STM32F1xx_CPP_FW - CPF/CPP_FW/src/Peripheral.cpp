

#include "Peripheral.hpp"

namespace Peripherals
{


#if 0
Status_t Peripheral::HwClockEnable(Peripheral_t Peripheral)
{
    Status_t Status = 0;
	Peripheral_t BitAdjusted;

	if(Peripheral>= APB2Periph_AFIO && Peripheral<= APB2Periph_TIM11 )
	{
		RCC_APB2PeriphClockCmd( 1U<<Peripheral, ENABLE);
        Status = true;
	}
	else if (Peripheral>= Bsp::PeripheralBase::APB1Periph_TIM2 && Peripheral<= Bsp::PeripheralBase::APB1Periph_CEC )
	{
		BitAdjusted = static_cast<Bsp::PeripheralBase::Peripheral_t>(Peripheral - Bsp::PeripheralBase::APB1Periph_TIM2);
		RCC_APB1PeriphClockCmd( 1U<<BitAdjusted, ENABLE);
        Status = true;
	}
	else if (Peripheral>= Bsp::PeripheralBase::AHBPeriph_DMA1 && Peripheral<= Bsp::PeripheralBase::AHBPeriph_CRC )
	{
		BitAdjusted = static_cast<Bsp::PeripheralBase::Peripheral_t>(Peripheral - Bsp::PeripheralBase::AHBPeriph_DMA1);
		RCC_AHBPeriphClockCmd( 1U<<Peripheral, ENABLE);
        Status = true;
	}
	else
	{
		// Nothing to do

	}
    return Status;


}
  
Status_t Peripheral::HwClockDisble(Peripheral_t Peripheral)
{
    Status_t Status = 0;
	Bsp::PeripheralBase::Peripheral_t BitAdjusted;

	if(Peripheral>= Bsp::PeripheralBase::APB2Periph_AFIO && Peripheral<= Bsp::PeripheralBase::APB2Periph_TIM11 )
	{
		RCC_APB2PeriphClockCmd( 1U<<Peripheral, DISABLE);
        Status = true;
	}
	else if (Peripheral>= Bsp::PeripheralBase::APB1Periph_TIM2 && Peripheral<= Bsp::PeripheralBase::APB1Periph_CEC )
	{
		BitAdjusted = static_cast<Bsp::PeripheralBase::Peripheral_t>(Peripheral - Bsp::PeripheralBase::APB1Periph_TIM2);
		RCC_APB1PeriphClockCmd( 1U<<BitAdjusted, DISABLE);
        Status = true;
	}
	else if (Peripheral>= Bsp::PeripheralBase::AHBPeriph_DMA1 && Peripheral<= Bsp::PeripheralBase::AHBPeriph_CRC )
	{
		BitAdjusted = static_cast<Bsp::PeripheralBase::Peripheral_t>(Peripheral - Bsp::PeripheralBase::AHBPeriph_DMA1);
		RCC_AHBPeriphClockCmd( 1U<<Peripheral, DISABLE);
        Status = true;
	}
	else
	{
		// Nothing to do
	}
    return Status;
  
  
}


#endif



}