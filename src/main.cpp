/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **  Distribution: The file is distributed "as is", without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. This file may only be built (assembled or compiled and linked)
 **  using the Atollic TrueSTUDIO(R) product. The use of this file together
 **  with other tools than Atollic TrueSTUDIO(R) is not permitted.
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "proto/grSim_Commands.pb.h"
#include "proto/pb_decode.h"
#include "proto/pb_encode.h"
#include "radio/commands.h"

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

#include "radio/bsp.h"

uint8_t scanned[256];



#include <stm32f4xx_wwdg.h>

uint8_t read_register(uint8_t address);

void Timer_Init(){
	//TIM6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=83;
	TIM_TimeBaseStructure.TIM_Period=999;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler=0;
	TIM_TimeBaseStructure.TIM_Period=0xFFFF;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	TIM_Cmd(TIM2,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_Cmd(TIM3,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	TIM_Cmd(TIM4,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	TIM_Cmd(TIM5,ENABLE);

}

void TimerEncM0_Init(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	//TIM_DeInit(TIM3);

	TIM_Cmd(TIM3,ENABLE);
	//TIM_SetCounter(TIM3, (uint32_t) 0);
	//TIM_SetAutoreload(TIM3, 9999);//0x226=550
}
void TimerEncM1_Init(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	TIM_InternalClockConfig(TIM2);

	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	//TIM_DeInit(TIM3);

	TIM_Cmd(TIM2,ENABLE);
	//TIM_SetCounter(TIM3, (uint32_t) 0);
	//TIM_SetAutoreload(TIM3, 9999);//0x226=550
}
void TimerEncM2_Init(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);


	//TIM_DeInit(TIM3);

	TIM_Cmd(TIM5,ENABLE);
	//TIM_SetCounter(TIM3, (uint32_t) 0);
	//TIM_SetAutoreload(TIM3, 9999);//0x226=550
}
void TimerEncM3_Init(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);


	//TIM_DeInit(TIM3);

	TIM_Cmd(TIM4,ENABLE);
	//TIM_SetCounter(TIM3, (uint32_t) 0);
	//TIM_SetAutoreload(TIM3, 9999);//0x226=550
}

void TIM6_NVIC_Init(){
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0C;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0C;

	NVIC_Init(&NVIC_InitStructure);
}

IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);
void ConfigInt(){
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);


	EXTI_InitStruct.EXTI_Line = EXTI_Line2;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PA0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

//SETANDO GPIOS DE SAIDA
IO_Pin_STM32 M0_MAL(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_7, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M0_MBL(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_13, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M1_MAL(IO_Pin::IO_Pin_Mode_OUT, GPIOE, GPIO_Pin_6, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M1_MBL(IO_Pin::IO_Pin_Mode_OUT, GPIOE, GPIO_Pin_4, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M2_MAL(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M2_MBL(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_1, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M3_MAL(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_11, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 M3_MBL(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_12, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 CA(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 CB(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_10, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 CT(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_0, GPIO_PuPd_UP, GPIO_OType_PP);

//SETA PINOS DE SCK, MISO E MOSI COMO OUTPUT PARA TESTE BASICO DE SPI
//IO_Pin_STM32 CLK2(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_13, GPIO_PuPd_UP, GPIO_OType_PP);
//IO_Pin_STM32 MISO2(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_14, GPIO_PuPd_UP, GPIO_OType_PP);
//IO_Pin_STM32 MOSI2(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_15, GPIO_PuPd_UP, GPIO_OType_PP);

//SETANDO GPIOS DE ENTRADA (MAS COMO SAIDA)
//IO_Pin_STM32 MPU_INT(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_PP);
//IO_Pin_STM32 NRF_IRQ(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_5, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 S2(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_11, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 S1(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_1, GPIO_PuPd_UP, GPIO_OType_PP);

//SETANDO GPIOS DE PWM
void InitPWM(){

	//timers
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

	//pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//inicializando os pinos
	GPIO_InitTypeDef gpioStructure;

	gpioStructure.GPIO_Pin = GPIO_Pin_8;
	gpioStructure.GPIO_Mode = GPIO_Mode_AF;
	gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructure.GPIO_OType = GPIO_OType_PP;
	gpioStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpioStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);

	gpioStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructure.GPIO_Mode = GPIO_Mode_AF;
	gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructure.GPIO_OType = GPIO_OType_PP;
	gpioStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &gpioStructure);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

	gpioStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	gpioStructure.GPIO_Mode = GPIO_Mode_AF;
	gpioStructure.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructure.GPIO_OType = GPIO_OType_PP;
	gpioStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &gpioStructure);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);

	//inicializando os timers

	//TIM8
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/1/1000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000-1;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse        = 0;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM8, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE);

	TIM_Cmd(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);


	//TIM1

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/1/1000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000-1;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse        = 0;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

	//TIM9

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/1/1000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000-1;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse        = 0;
	TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM9, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_OC2Init(TIM9, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM9, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM9, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM9, ENABLE);

	TIM_Cmd(TIM9, ENABLE);
}

void configADC(){
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;

	/* Enable peripheral clocks *************************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	//RCC_APB2PeriphClockCmd(ADC1_CLK, ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* ADC1 regular channel18 (VBAT) configuration ******************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 1, ADC_SampleTime_15Cycles);

	/* Enable VBAT channel */
	ADC_VBATCmd(ENABLE);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);


}

void Config_ADC(){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);


ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  //DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  /* Enable ADCx, DMA and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);


  /* Configure ADC3 Channelx pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  //DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  /* DMA2_Stream0 enable */
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//ADC_DMAAccessMode_2;//ADC_Direct_memory_access_mode_for_multi_mode
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels 10, 11 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_3Cycles);
  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}

//setando os pinos de data e clock do i2c
IO_Pin_STM32 I2C_A_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
IO_Pin_STM32 I2C_A_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_OD, GPIO_AF_I2C1);
I2C_STM32 i2c_a(I2C_A_SDA_PIN, I2C_A_SCL_PIN, I2C1, 100000, 0x4000);

static int k=0;
void testLEDs();
void testGPIOsON();
void testGPIOsOFF();
void testPWM();
void testGPIOsINOn();
void testGPIOsINOff();
void testGPIOsANA();
void ShowCountDownDisplay();
int ShowNumber(int number);
void controle();

int main(void){
	LIS3DSH_CSN.Set();

	/*
	 * SPI: baud rate prescaler: 128 (fazendo a frequencia ficar menor que 1Mhz - pro MPU9250 funcionar)
	 * 		SPI configurada com CPOL = CPHA = 0
	 * 		SPI_I2S_DeInit(_spi);
	 * 		declaração do spi em /src/hal_stm32/spi_stm32.cpp
	 *
	 * I2C: clock de 100000.
	 * 		declaração do i2c em /src/hal_stm32/i2c_stm32.cpp
	 *
	 *
	 * 	clock do sistema:
	 * frequência do stm32f407: 168MHz
	 *
	 * prescaler APB2: 2
	 * prescaler AHB: 1
	 *
	 *
	 * */

	//setando os pinos de Chip Select
	IO_Pin_STM32 MPU9250_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_10, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 SDCARD_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_3, GPIO_PuPd_UP, GPIO_OType_PP);
	//IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);

	//setando os pinos de clock, miso e mosi do SPI2.
	IO_Pin_STM32 SPI2_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_13, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI2);
	IO_Pin_STM32 SPI2_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_14, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI2);
	IO_Pin_STM32 SPI2_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_15, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI2);

	SPI_STM32 spi_mpu(SPI2, MPU9250_CE_PIN, SPI_BaudRatePrescaler_128);
	SPI_STM32 spi_sdcard(SPI2, SDCARD_CE_PIN, SPI_BaudRatePrescaler_128);

	IO_Pin_STM32 SPI1_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_5, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI1_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_6, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI1_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_7, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 NRF24_SS_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_0, GPIO_PuPd_NOPULL, GPIO_OType_PP);
	IO_Pin_STM32 NRF24_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_12, GPIO_PuPd_NOPULL, GPIO_OType_PP);
	IO_Pin_STM32 NRF24_IRQN_PIN(IO_Pin::IO_Pin_Mode_IN, GPIOC, GPIO_Pin_5, GPIO_PuPd_UP, GPIO_OType_OD);

	SPI_STM32 spi_nrf(SPI1, NRF24_SS_PIN, SPI_BaudRatePrescaler_32);

	led_c.On();
	led_d.On();
	led_a.On();
	led_b.On();


	InitPWM();
	Timer_Init();

	ConfigInt();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	TimerEncM0_Init();
	TimerEncM1_Init();
	TimerEncM2_Init();
	TimerEncM3_Init();

	//TESTE NRF24
	uint8_t buf_nrf[]={0xFF,0XFF};
	spi_nrf.Assert();
	spi_nrf.WriteBuffer(buf_nrf,1);
	uint8_t resp_nrf = spi_nrf.WriteBuffer(buf_nrf+1,1);
	spi_nrf.Release();
	resp_nrf++;

	//TESTE STMPE
	uint8_t i2c_ta_ai = i2c_a.ReadRegByte(0x82, 0x00);
	i2c_ta_ai++;


	//TESTE MPU
	uint8_t buf[]={0xF5,0XFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf,1);
	uint8_t resp_mpu = spi_mpu.WriteBuffer(buf+1,1);
	spi_mpu.Release();
	resp_mpu++;

	// testGPIOsON();
	//testGPIOsOFF();
	testPWM();

	while(1){

		//ShowCountDownDisplay();

	}
}

void testGPIOsON(){

	/*while(i<7000000){
		i++;
	}
	i=0;*/
	M0_MAL.Set();
	M0_MBL.Set();
	M1_MAL.Set();
	M1_MBL.Set();
	M2_MAL.Set();
	M2_MBL.Set();
	M3_MAL.Set();
	M3_MBL.Set();
	CA.Set();
	CB.Set();
	CT.Set();
	led_c.Toggle();
}

void testGPIOsOFF(){
	int i=0;
	M0_MAL.Reset();
	M0_MBL.Reset();
	M1_MAL.Reset();
	M1_MBL.Reset();
	M2_MAL.Reset();
	M2_MBL.Reset();
	M3_MAL.Reset();
	M3_MBL.Reset();
	CA.Reset();
	CB.Reset();
	CT.Reset();
}

void testLEDs(){
	int i=0;
	while(i<6000000){
		i++;
	}
	i=0;
	led_c.Toggle();
	led_d.Toggle();
	led_a.Toggle();
	led_b.Toggle();
}

void testPWM(){
	TIM1->CCR1 = 100;
	TIM1->CCR2 = 200;
	TIM1->CCR3 = 300;
	TIM1->CCR4 = 400;
	TIM8->CCR1 = 500;
	TIM8->CCR2 = 600;
	TIM8->CCR3 = 700;
	TIM8->CCR4 = 800;
	TIM9->CCR1 = 900;
}

void testGPIOsINOn(){
	int i=0;
	//aqui testados como output
	//MPU_INT.Set();
	//NRF_IRQ.Set();
	S2.Set();
	S1.Set();
}

void testGPIOsINOff(){
	int i=0;
	//aqui testados como output
//	MPU_INT.Reset();
	//NRF_IRQ.Reset();
	S2.Reset();
	S1.Reset();
}

void testGPIOsANA(){

}

uint16_t encoderCount0[10];
uint16_t encoderCount1[10];
uint16_t encoderCount2[10];
uint16_t encoderCount3[10];

float convert = 2*3.14159265*0.028*1000/(10*400*8);

int speed0[10];
int speed1[10];
int speed2[10];
int speed3[10];

void controle(){
	int currentCount0, currentSpd;
	int currentCount1, currentCount2, currentCount3;

	currentCount0 = TIM_GetCounter(TIM3);
	currentCount1 = TIM_GetCounter(TIM2);
	currentCount2 = TIM_GetCounter(TIM4);
	currentCount3 = TIM_GetCounter(TIM5);

	for (int i=9; i>0; i--){
		encoderCount0[i]=encoderCount0[i-1];
		speed0[i]=speed0[i-1];

		encoderCount1[i]=encoderCount1[i-1];
		speed1[i]=speed1[i-1];

		encoderCount2[i]=encoderCount2[i-1];
		speed2[i]=speed2[i-1];

		encoderCount3[i]=encoderCount3[i-1];
		speed3[i]=speed3[i-1];
	}

	encoderCount0[0]=currentCount0;
	encoderCount1[0]=currentCount1;
	encoderCount2[0]=currentCount2;
	encoderCount3[0]=currentCount3;

	currentSpd = 1000*(encoderCount0[0] - encoderCount0[9])*convert; //x1000 para nao ter necessidade de utilizar float
	speed0[0]=currentSpd;

	currentSpd = 1000*(encoderCount1[0] - encoderCount1[9])*convert; //x1000 para nao ter necessidade de utilizar float
	speed1[0]=currentSpd;

	currentSpd = 1000*(encoderCount2[0] - encoderCount2[9])*convert; //x1000 para nao ter necessidade de utilizar float
	speed2[0]=currentSpd;

	currentSpd = 1000*(encoderCount3[0] - encoderCount3[9])*convert; //x1000 para nao ter necessidade de utilizar float
	speed3[0]=currentSpd;

}

float CalcAccelZ(uint8_t buf1[], uint8_t buf2[], uint8_t buf3[], SPI_STM32 spi_mpu){

	uint16_t ACCEL_Z = 0x00;
	uint8_t ACCEL_ZOUT_H;
	uint8_t ACCEL_ZOUT_L;
	uint8_t ACCEL_CONFIG;
	float gravity = 0;
#define ACCEL_MAX 32766

	//uint8_t buf1[]={0xBF,0X3F};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf1,1);
	ACCEL_ZOUT_H = spi.WriteBuffer(buf1+1,1);
	spi_mpu.Release();

	//uint8_t buf2[]={0xC0,0xFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf2,1);
	ACCEL_ZOUT_L = spi.WriteBuffer(buf2+1,1);
	spi_mpu.Release();

	//uint8_t buf3[]={0x1C,0X1C};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf3,1);
	ACCEL_CONFIG = spi.WriteBuffer(buf3+1,1);
	spi_mpu.Release();

	ACCEL_Z = (ACCEL_ZOUT_H << 8) | (ACCEL_ZOUT_L & 0xFF);

	gravity = ((float)((int)ACCEL_Z))/((int)ACCEL_MAX)*2*9.81;

	return gravity;
}

void ShowCountDownDisplay(){
	IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);
	uint8_t numero[10];
	numero[0]=0x77;//10+20+40+04+02+01
	numero[1]=0x06;//02+04
	numero[2]=0xB5;//10+20+04+01+80
	numero[3]=0x97;//01+02+04+80+10
	numero[4]=0xC6;//40+80+04+02
	numero[5]=0xD3;//01+02+10+40+80
	numero[6]=0xF3;//01+02+10+20+40+80
	numero[7]=0x07;//01+02+04
	numero[8]=0xF7;//todos menos o 0x08
	numero[9]=0xD7;//01+02+08+10+40+04
	//int k=0;
	uint8_t a;
	int i=0;
	while(i<6000000){
		i++;
	}
	i=0;
	if(k==10){
		k=0;
	}

	a=numero[k];

	i2c_a.WriteRegByte(0x82, 0x04, ~0x04);
	i2c_a.WriteRegByte(0x82, 0x17, a);
	i2c_a.WriteRegByte(0x82, 0x13, a);
	i2c_a.WriteRegByte(0x82, 0x10, a);

	k++;

	/*if(ID_BUTTON.Read()==0)
		led_c.Toggle();
	else
		led_c.Off();*/

}

int ShowNumber(int number){

	uint8_t numero[10];
	numero[0]=0x77;//10+20+40+04+02+01
	numero[1]=0x06;//02+04
	numero[2]=0xB5;//10+20+04+01+80
	numero[3]=0x97;//01+02+04+80+10
	numero[4]=0xC6;//40+80+04+02
	numero[5]=0xD3;//01+02+10+40+80
	numero[6]=0xF3;//01+02+10+20+40+80
	numero[7]=0x07;//01+02+04
	numero[8]=0xF7;//todos menos o 0x08
	numero[9]=0xD7;//01+02+08+10+40+04
	//int k=0;
	uint8_t a;
	int i=0;

	a=numero[number];

	i2c_a.WriteRegByte(0x82, 0x04, ~0x04);
	i2c_a.WriteRegByte(0x82, 0x17, a);
	i2c_a.WriteRegByte(0x82, 0x13, a);
	i2c_a.WriteRegByte(0x82, 0x10, a);

	return 0;
}

int number=0;
extern "C" void EXTI2_IRQHandler(void) {
	//interrupcao

	//Make sure that interrupt flag is set
	if (EXTI_GetITStatus(EXTI_Line2)) {
		// Do your stuff when PA0 is changed */
		number++;
		if(number==0) ShowNumber(0);
		else if(number==1) {
			testGPIOsON();//pinos ok
			ShowNumber(1);
		}
		else if(number==2) {
			testGPIOsOFF();//pinos ok
			ShowNumber(2);
		}
		else if(number==3) {
			testPWM(); //pinos ok
			ShowNumber(3);
		}
		else if(number==4) {
			testGPIOsINOn(); //mpu_int em curto
			ShowNumber(4);
		}
		else if(number==5) {
			testGPIOsINOff(); //aparentemente ok = int_mpu em curto com gnd
			ShowNumber(5);
		}
		else if(number==6) {
			testGPIOsANA(); //
			ShowNumber(6);
		}
		if(number==7){
			number=0;
			ShowNumber(0);
		}
		// Clear interrupt flag
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

extern "C" void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		controle(); //funcao que atualiza os valores de controle chamada a cada 1ms
	}
}

extern uint32_t LocalTime;

extern "C" void SysTick_Handler(void){
	//	TimingDelay_Decrement();
	LocalTime++;
}

extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
	/* TODO, implement your code here */
	return -1;
}
