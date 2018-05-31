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

void ShowDisplay();

uint8_t read_register(uint8_t address);

int main(void){
	LIS3DSH_CSN.Set();

	//setando pino de CS do mpu9250
	/*GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC ,&GPIO_InitStructure);*/

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
	IO_Pin_STM32 MPU9250_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_15, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 SDCARD_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_12, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);

	//setando os pinos de clock, miso e mosi do SPI.
	IO_Pin_STM32 SPI_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_5, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_6, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_7, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);

	//setando os pinos de data e clock do i2c
	IO_Pin_STM32 I2C_A_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
	IO_Pin_STM32 I2C_A_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_OD, GPIO_AF_I2C1);
	I2C_STM32 i2c_a(I2C_A_SDA_PIN, I2C_A_SCL_PIN, I2C1, 100000, 0x4000);

	led_c.On();
	led_d.On();

	uint32_t last_time;

	//MPU9250_CE_PIN.Reset();

	SPI_STM32 spi_mpu(SPI1, MPU9250_CE_PIN);
	SPI_STM32 spi_sdcard(SPI1, SDCARD_CE_PIN);

	uint8_t i2c_ta_ai = i2c_a.ReadRegByte(0x82, 0x00);
	//i2c_a.WriteRegByte(0x82, 0x04, ~0x04);
	//i2c_a.WriteRegByte(0x82, 0x17, 0x03);
	//i2c_a.WriteRegByte(0x82, 0x13, 0x03);
	//i2c_a.WriteRegByte(0x82, 0x10, 0x03);
	i2c_ta_ai++;

	uint8_t numero[10];
	numero[0]=0x77;//10+20+40+04+02+01
	numero[1]=0x60;//20+40
	numero[2]=0x3D;//10+20+08+04+01
	numero[3]=0x79;//10+20+40+08+01
	numero[4]=0x6A;//20+40+08+02
	numero[5]=0x5B;//10+02+08+40+01
	numero[6]=0x5F;//10+02+08+40+01+04
	numero[7]=0x70;//10+20+40
	numero[8]=0x7F;//todos menos o 0x80
	numero[9]=0x7B;//01+02+08+10+20+40

	uint8_t buf[]={0xF5,0XFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf,1);
	uint8_t resp = spi.WriteBuffer(buf+1,1);
	spi_mpu.Release();
	resp++;
/*
	while(0){
		uint8_t buf_pre_sed[]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		spi_sdcard.Release();
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		uint8_t buf_sd[]={0x40, 0x00, 0x00, 0x00, 0x00, 0x95, 0xFF, 0xFF};
		spi_sdcard.Assert();
		spi_sdcard.WriteBuffer(buf_sd,6);
		resp = spi.WriteBuffer(buf_sd+6,2);
		spi_sdcard.Release();
		resp++;
	}*/

/*	uint16_t ACCEL_Z = 0x00;
	uint8_t ACCEL_ZOUT_H;
	uint8_t ACCEL_ZOUT_L;
	uint8_t ACCEL_CONFIG;
	float gravity = 0;
	#define ACCEL_MAX 32766*/

	while(1){

		//led_d.Toggle();

		//if (GPIOA->IDR & (1<<15)) led_d.Toggle(); //chip select funcionou.
		//led_c.Toggle();

		//ShoWDisplay();

		uint8_t buf1[]={0xBF,0X3F};
		spi_mpu.Assert();
		spi_mpu.WriteBuffer(buf1,1);
		ACCEL_ZOUT_H = spi.WriteBuffer(buf1+1,1);
		spi_mpu.Release();

		uint8_t buf2[]={0xC0,0xFF};
		spi_mpu.Assert();
		spi_mpu.WriteBuffer(buf2,1);
		ACCEL_ZOUT_L = spi.WriteBuffer(buf2+1,1);
		spi_mpu.Release();

		uint8_t buf3[]={0x1C,0X1C};
		spi_mpu.Assert();
		spi_mpu.WriteBuffer(buf3,1);
		ACCEL_CONFIG = spi.WriteBuffer(buf3+1,1);
		spi_mpu.Release();

		ACCEL_Z = (ACCEL_ZOUT_H << 8) | (ACCEL_ZOUT_L & 0xFF);

		gravity = ((float)((int)ACCEL_Z))/((int)ACCEL_MAX)*2*9.81;

		//spi_sdcard.Assert();
		//if (GPIOC->IDR & (1<<12)) led_d.Toggle(); //chip select funcionou.
		//spi_sdcard.Release();
		//WHO_AM_I_MPU9250;
		//NRF24L01P.read_register(WHO_AM_I_MPU9250);
		/*
		 * if((GetLocalTime()-last_time)>1000){
			led_c.Toggle();
			led_d.Toggle();
			last_time = GetLocalTime();
		}
		*/


	}
}

int CalcAccelX(){

}

int CalcAccelY(){

}

int CalcAccelZ(uint8_t buf1[], uint8_t buf2[], uint8_t buf3[], ){

	uint16_t ACCEL_Z = 0x00;
	uint8_t ACCEL_ZOUT_H;
	uint8_t ACCEL_ZOUT_L;
	uint8_t ACCEL_CONFIG;
	float gravity = 0;
	#define ACCEL_MAX 32766


	uint8_t buf1[]={0xBF,0X3F};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf1,1);
	ACCEL_ZOUT_H = spi.WriteBuffer(buf1+1,1);
	spi_mpu.Release();

	uint8_t buf2[]={0xC0,0xFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf2,1);
	ACCEL_ZOUT_L = spi.WriteBuffer(buf2+1,1);
	spi_mpu.Release();

	uint8_t buf3[]={0x1C,0X1C};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf3,1);
	ACCEL_CONFIG = spi.WriteBuffer(buf3+1,1);
	spi_mpu.Release();

	ACCEL_Z = (ACCEL_ZOUT_H << 8) | (ACCEL_ZOUT_L & 0xFF);

	gravity = ((float)((int)ACCEL_Z))/((int)ACCEL_MAX)*2*9.81;

	return gravity;
}

void ShowDisplay(){
	IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);
	uint8_t numero[10];
		numero[0]=0x77;//10+20+40+04+02+01
		numero[1]=0x60;//20+40
		numero[2]=0x3D;//10+20+08+04+01
		numero[3]=0x79;//10+20+40+08+01
		numero[4]=0x6A;//20+40+08+02
		numero[5]=0x5B;//10+02+08+40+01
		numero[6]=0x5F;//10+02+08+40+01+04
		numero[7]=0x70;//10+20+40
		numero[8]=0x7F;//todos menos o 0x80
		numero[9]=0x7B;//01+02+08+10+20+40
	int k=0;
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

	if(ID_BUTTON.Read())
		led_c.Toggle();
	else
		led_c.Off();

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
