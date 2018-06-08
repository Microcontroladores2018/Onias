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
  TIM_DeInit(TIM6);

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_ClockDivision=0;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Prescaler=335;
  TIM_TimeBaseStructure.TIM_Period=4999;
  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM6,ENABLE);

}

IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);
void ConfigInt(){
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);

    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);


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

//SETANDO GPIOS DE ENTRADA (MAS COMO SAIDA)
IO_Pin_STM32 MPU_INT(IO_Pin::IO_Pin_Mode_OUT, GPIOB, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_PP);
IO_Pin_STM32 NRF_IRQ(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_5, GPIO_PuPd_UP, GPIO_OType_PP);
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
	GPIO_InitTypeDef gpioStructureA;
	gpioStructureA.GPIO_Pin = GPIO_Pin_8;
	gpioStructureA.GPIO_Mode = GPIO_Mode_AF;
	gpioStructureA.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructureA.GPIO_OType = GPIO_OType_PP;
	gpioStructureA.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOA, &gpioStructureA);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);

	GPIO_InitTypeDef gpioStructureE;
	gpioStructureE.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	gpioStructureE.GPIO_Mode = GPIO_Mode_AF;
	gpioStructureE.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructureE.GPIO_OType = GPIO_OType_PP;
	gpioStructureE.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOE, &gpioStructureE);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 | GPIO_PinSource11 | GPIO_PinSource13 | GPIO_PinSource14 , GPIO_AF_TIM1);

	GPIO_InitTypeDef gpioStructureC;
	gpioStructureC.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	gpioStructureC.GPIO_Mode = GPIO_Mode_AF;
	gpioStructureC.GPIO_Speed = GPIO_Speed_100MHz;
	gpioStructureC.GPIO_OType = GPIO_OType_PP;
	gpioStructureC.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOC, &gpioStructureC);
	GPIO_PinAFConfig(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9, GPIO_AF_TIM8);

	//inicializando os timers

	//TIM8
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure8;

	TIM_TimeBaseStructure8.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure8.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure8.TIM_Prescaler=(SystemCoreClock/20000000)-1;
	TIM_TimeBaseStructure8.TIM_Period=168000000/168000-1;

	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure8);
	TIM_OCInitTypeDef TIM_OCInitStructure8;
	TIM_OCInitStructure8.TIM_OCMode=TIM_OCMode_PWM1;

	TIM_OCInitStructure8.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure8.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure8.TIM_Pulse        = 0;
	TIM_OCInitStructure8.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure8.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM8, &TIM_OCInitStructure8);
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure8);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure8);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM8, &TIM_OCInitStructure8);
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE);

	TIM_Cmd(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);

	//TIM1
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure1;

	TIM_TimeBaseStructure1.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure1.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure1.TIM_Prescaler=(SystemCoreClock/20000000)-1;
	TIM_TimeBaseStructure1.TIM_Period=168000000/168000-1;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure1);
	TIM_OCInitTypeDef TIM_OCInitStructure1;
	TIM_OCInitStructure1.TIM_OCMode=TIM_OCMode_PWM1;

	TIM_OCInitStructure1.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure1.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure1.TIM_Pulse        = 0;
	TIM_OCInitStructure1.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure1.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure1);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure1);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure1);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure1);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

	//TIM9
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure9;

	TIM_TimeBaseStructure9.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure9.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure9.TIM_Prescaler=(SystemCoreClock/20000000)-1;
	TIM_TimeBaseStructure9.TIM_Period=168000000/168000-1;

	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure8);
	TIM_OCInitTypeDef TIM_OCInitStructure9;
	TIM_OCInitStructure9.TIM_OCMode=TIM_OCMode_PWM1;

	TIM_OCInitStructure9.TIM_OutputState  = TIM_OutputState_Enable;
	TIM_OCInitStructure9.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure9.TIM_Pulse        = 0;
	TIM_OCInitStructure9.TIM_OCPolarity   = TIM_OCPolarity_Low;
	TIM_OCInitStructure9.TIM_OCIdleState  = TIM_OCIdleState_Set;

	TIM_OC1Init(TIM9, &TIM_OCInitStructure9);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_OC2Init(TIM9, &TIM_OCInitStructure9);
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM9, &TIM_OCInitStructure9);
	TIM_OC3PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM9, &TIM_OCInitStructure9);
	TIM_OC4PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM9, ENABLE);

	TIM_Cmd(TIM9, ENABLE);
	TIM_CtrlPWMOutputs(TIM9,ENABLE);

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

	//setando os pinos de clock, miso e mosi do SPI.
	IO_Pin_STM32 SPI_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_13, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_14, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_15, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);

	led_c.On();
	led_d.On();
	led_a.On();
	led_b.On();

	SPI_STM32 spi_mpu(SPI1, MPU9250_CE_PIN);
	SPI_STM32 spi_sdcard(SPI2, SDCARD_CE_PIN);

	InitPWM();
	Timer_Init();

	ConfigInt();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	uint8_t i2c_ta_ai = i2c_a.ReadRegByte(0x82, 0x00);
	i2c_ta_ai++;

	uint8_t buf[]={0xF5,0XFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf,1);
	uint8_t resp = spi.WriteBuffer(buf+1,1);
	spi_mpu.Release();
	resp++;

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
	TIM1->CCR1 = 500;
	TIM1->CCR2 = 500;
	TIM1->CCR3 = 500;
	TIM1->CCR4 = 500;
	TIM8->CCR1 = 500;
	TIM8->CCR2 = 500;
	TIM8->CCR3 = 500;
	TIM8->CCR4 = 500;
	TIM9->CCR1 = 500;
}

void testGPIOsINOn(){
	int i=0;
	//aqui testados como output
	MPU_INT.Set();
	NRF_IRQ.Set();
	S2.Set();
	S1.Set();
}

void testGPIOsINOff(){
	int i=0;
	//aqui testados como output
	MPU_INT.Reset();
	NRF_IRQ.Reset();
	S2.Reset();
	S1.Reset();
}


void testGPIOsANA(){

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

void ShowDisplay(int passos){
	//mostra um certo numero no display
}
/*
extern "C" void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		CalculaSteps();
	}
}*/
int number=0;
extern "C" void EXTI2_IRQHandler(void) {
	//interrupcao

     //Make sure that interrupt flag is set
    if (EXTI_GetITStatus(EXTI_Line2)) {
        // Do your stuff when PA0 is changed */
        number++;
        if(number==0) ShowNumber(0);
        else if(number==1) {
        	testGPIOsON();
        	ShowNumber(1);
        }
        else if(number==2) {
        	testGPIOsOFF();
        	ShowNumber(2);
        }
        else if(number==3) {
        	testPWM();
        	ShowNumber(3);
        }
        else if(number==4) {
        	testGPIOsINOn();
        	ShowNumber(4);
        }
        else if(number==5) {
            testGPIOsINOff();
            ShowNumber(5);
        }
        else if(number==6) {
        	testGPIOsANA();
        	ShowNumber(6);
        }
        if(number==7){
        	number=0;
        }
        // Clear interrupt flag
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
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
