#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

#include <radio/bsp.h>
#include <radio/serialnumber.h>
#include <hal_stm32/interrupt_stm32.h>
#include <radio/version.h>
#include <list>

extern "C"{
	#include "usb_dcd_int.h"
	#include "usb_hcd_int.h"
}

LED led_a(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_12));
LED led_b(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_13));
LED led_c(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_14));
LED led_d(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_15));

IO_Pin_STM32 Botao(IO_Pin::IO_Pin_Mode_IN, GPIOA, GPIO_Pin_0, GPIO_PuPd_NOPULL);

IO_Pin_STM32 USB_DP(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_12, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_OTG_FS);
IO_Pin_STM32 USB_DM(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_11, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_OTG_FS);

//USB_DEVICE_CLASS_CDC_RNDIS usb_device_class_cdc_rndis(1);

std::list<USB_DEVICE_CLASS*> USB_DEVICE_CLASS::_usb_device_classes_list;
USB_DEVICE_CLASS_CDC_VCP usb_device_class_cdc_vcp({"RoboIME Serial Port"},1);
USB_STM32 usb(0x29BC, 0x2000, "IME", "RoboIME", SerialNumberGetHexaString());
/*
IO_Pin_STM32 SPI1_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_5, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 SPI1_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_6, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 SPI1_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_7, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 NRF24_SS_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_4, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 NRF24_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_3, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 NRF24_IRQN_PIN(IO_Pin::IO_Pin_Mode_IN, GPIOC, GPIO_Pin_5, GPIO_PuPd_UP, GPIO_OType_OD);

SPI_STM32 spi_nrf(SPI1, NRF24_SS_PIN, SPI_BaudRatePrescaler_16);

NRF24L01P nrf24(spi_nrf, NRF24_SS_PIN, NRF24_CE_PIN, NRF24_IRQN_PIN);
*/
IO_Pin_STM32 LIS3DSH_CSN(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_3, GPIO_PuPd_NOPULL, GPIO_OType_OD);

//INTERRUPT_STM32 nrf24_irqn_exti_interrupt(NRF24_IRQN_PIN.GetIRQChannel(), 0x0C, 0x0C, DISABLE);

IO_Pin_STM32 sw1(IO_Pin::IO_Pin_Mode_IN, GPIOD, GPIO_Pin_6, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 sw2(IO_Pin::IO_Pin_Mode_IN, GPIOD, GPIO_Pin_7, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 sw3(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_PP);

INTERRUPT_STM32 timer_robot(TIM6_DAC_IRQn, 0x0C, 0x0C, ENABLE);

extern "C" void EXTI9_5_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line5)){
		EXTI_ClearITPendingBit(EXTI_Line5);
		//nrf24.InterruptCallback();
	}
}
/*
extern "C" void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	}
}*/

//IO_Pin_STM32 I2C_A_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
//IO_Pin_STM32 I2C_A_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_OD, GPIO_AF_I2C1);
//I2C_STM32 i2c_a(I2C_A_SDA_PIN, I2C_A_SCL_PIN, I2C1, 100000, 0x4000);


/*TOP SECRET - SHIU
USART_STM32 usart_onewire(USART6, 115200, USART_WordLength_8b, USART_StopBits_2, USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx, USART_HardwareFlowControl_None, 1);
ONEWIRE onewire(usart_onewire,ONEWIRE_PULL_UP,1000);



IO_Pin_STM32 I2C_B_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOC, GPIO_Pin_9, GPIO_PuPd_UP, GPIO_OType_OD, GPIO_AF_I2C3);
IO_Pin_STM32 I2C_B_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_OD, GPIO_AF_I2C3);
I2C_STM32 i2c_b(I2C_B_SDA_PIN, I2C_B_SCL_PIN, I2C3, 100000, 0x4000);

IO_Pin_STM32 USART_GPS_RX_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_3, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_USART2);
IO_Pin_STM32 USART_GPS_TX_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_USART1);
USART_STM32 usart_gps(USART1, USART2);



IO_Pin_STM32 MIC_PIN(IO_Pin::IO_Pin_Mode_ANALOG, GPIOA, GPIO_Pin_6);
ADC_STM32 ADC_MIC(16000, 512, ADC1, ADC_Channel_6, ADC_SampleTime_56Cycles);


RF_POWER_CTRL rf_power_ctrl(mcp4561_b, ad7992, rf_power_ctrl_memory);

IO_Pin_STM32 PA_VDD_EN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_11, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 SPI1_SDN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_13, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 SPI1_NIRQ(IO_Pin::IO_Pin_Mode_IN, GPIOB, GPIO_Pin_6, GPIO_PuPd_UP, GPIO_OType_PP, 0, EXTI_Trigger_Falling, EXTI_Mode_Interrupt);
SI446X modem(spi, PA_VDD_EN, SPI1_SDN, SPI1_NIRQ, rf_power_ctrl);
//NULLMODEM modem;

Buttons buttons(10);


INTERRUPT_STM32 i2s_dma_interrupt(i2s.GetIRQChannel(), 0x0B, 0x0B, ENABLE);
INTERRUPT_STM32 adc_mic_dma_interrupt(ADC_MIC.GetIRQChannel(), 0x0B, 0x0B, ENABLE);
INTERRUPT_STM32 spi_nirq_exti_interrupt(SPI1_NIRQ.GetIRQChannel(), 0x0C, 0x0C, ENABLE);
INTERRUPT_STM32 onewire_dma(DMA2_Stream6_IRQn, 0x0F, 0x0F, ENABLE);
//INTERRUPT_STM32 onewire_usart(USART6_IRQn, 0x0E, 0x0E, ENABLE);

//IP_STACK ip_stack;

const char *SCPI_IDN[4]{BUILD_MANUFACTURER, BUILD_MODEL, SerialNumberGetHexaString(), BUILD_VERSION};
SCPI SCPI_USB(SCPI_IDN);

extern "C" void led_a_on(){ led_a.On();}
extern "C" void led_a_off(){ led_a.Off();}
extern "C" void led_a_toggle(){ led_a.Toggle();}

void SI446X::GeneratePoll(){
	static uint8_t firsttime=1;
	if(firsttime){
		firsttime=0;
		EXTI_InitTypeDef EXTI_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		EXTI_InitStructure.EXTI_LineCmd=ENABLE;
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_Line=EXTI_Line0;
		EXTI_Init(&EXTI_InitStructure);
	}
	EXTI_GenerateSWInterrupt(EXTI_Line6);
}

void RF_POWER_CTRL::GeneratePoll(){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}

extern "C" void USART6_IRQHandler(){
	uint8_t flag=USART_GetFlagStatus(USART6, USART_FLAG_FE);
	if(flag){
		USART_ReceiveData(USART6);
	}
}


extern "C" void DMA1_Stream7_IRQHandler(){
	led_c.On();
	if(DMA_GetITStatus(DMA1_Stream7, DMA_IT_HTIF7)){
		DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_HTIF7);
		sound.ProcessSpeaker();
	}
	led_c.Off();
}

extern "C" void DMA2_Stream6_IRQHandler(){
	if(DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6)){
		DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
		onewire.Callback();
	}
}


extern "C" void DMA2_Stream0_IRQHandler(){
	if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0)){
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
		sound.ProcessMicrophone();
	}
}


extern "C" void OTG_HS_IRQHandler(void){
	USBD_OTG_ISR_Handler (&USB_OTG_dev);
	USBH_OTG_ISR_Handler (&USB_OTG_dev);
}


extern "C" void OTG_HS_WKUP_IRQHandler(void){
	if(USB_OTG_dev.cfg.low_power){
		*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
}

*/

extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern "C" void OTG_FS_IRQHandler(void){
	USBD_OTG_ISR_Handler (&USB_OTG_dev);
	USBH_OTG_ISR_Handler (&USB_OTG_dev);
}
extern "C" void OTG_FS_WKUP_IRQHandler(void){
	if(USB_OTG_dev.cfg.low_power){
		*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
}
