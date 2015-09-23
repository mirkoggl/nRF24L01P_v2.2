/*
 * nRF24L01P_SPI.c
 *
 *  Created on: 17/dic/2014
 *      Author: Mirko
 */

#include "nRF24L01P_SPI.h"

void nRF24L01P_SPI_Init() {
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	//SPI
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;

	RCC_AHB1PeriphClockCmd(NRF24L01P_SPI_GPIO_RCC, ENABLE);

	GPIO_InitStruct.GPIO_Pin = NRF24L01P_SPI_SCK_PIN | NRF24L01P_SPI_MISO_PIN | NRF24L01P_SPI_MOSI_PIN;
	GPIO_Init(NRF24L01P_SPI_GPIO_PORT, &GPIO_InitStruct);

	GPIO_PinAFConfig(NRF24L01P_SPI_GPIO_PORT, NRF24L01P_SPI_SCK_PINSOURCE, NRF24L01P_SPI_GPIO_AF);
	GPIO_PinAFConfig(NRF24L01P_SPI_GPIO_PORT, NRF24L01P_SPI_MISO_PINSOURCE, NRF24L01P_SPI_GPIO_AF);
	GPIO_PinAFConfig(NRF24L01P_SPI_GPIO_PORT, NRF24L01P_SPI_MOSI_PINSOURCE, NRF24L01P_SPI_GPIO_AF);

	RCC_APB1PeriphClockCmd(NRF24L01P_SPI_RCC, ENABLE);

	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_BaudRatePrescaler = NRF24L01P_SPI_PRESCALER;
	SPI_InitStruct.SPI_DataSize = NRF24L01P_SPI_DATASIZE;
	SPI_InitStruct.SPI_Direction = NRF24L01P_SPI_DIRECTION;
	SPI_InitStruct.SPI_FirstBit = NRF24L01P_SPI_FIRSTBIT;
	SPI_InitStruct.SPI_Mode = NRF24L01P_SPI_MODE;

	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;

	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;

	SPI_Init(NRF24L01P_SPI, &SPI_InitStruct);
	SPI_Cmd(NRF24L01P_SPI, ENABLE);
}

void nRF24L01P_InitPin(){
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_AHB1PeriphClockCmd(NRF24L01P_CSN_RCC | NRF24L01P_CE_RCC, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	//CSN Pin
	GPIO_InitStruct.GPIO_Pin = NRF24L01P_CSN_PIN;
	GPIO_Init(NRF24L01P_CSN_PORT, &GPIO_InitStruct);
	NRF24L01P_CSN_HIGH;

	//CE Pin
	GPIO_InitStruct.GPIO_Pin = NRF24L01P_CE_PIN;
	GPIO_Init(NRF24L01P_CE_PORT, &GPIO_InitStruct);
	NRF24L01P_CE_LOW;

	//IRQ Pin
	GPIO_InitStruct.GPIO_Pin = NRF24L01P_IRQ_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(NRF24L01P_IRQ_PORT, &GPIO_InitStruct);

	/* Connect EXTI Line to appropriate GPIO Pin */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(NRF24L01P_IRQ_EXTI_PORT, NRF24L01P_IRQ_GPIO_EXTI_LINE);

	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;


	/* Configure EXTI Line */
	EXTI_InitStructure.EXTI_Line = NRF24L01P_IRQ_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = NRF24L01P_IRQ_EXTI_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NRF24L01P_IRQ_EXTI_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	nRF24L01P_SPI_Init();
}

uint8_t SPI_Send(SPI_TypeDef* SPIx, uint8_t data) {
	//Fill output buffer with data
	SPIx->DR = data;
	//Wait for transmission to complete
	while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE));
	//Wait for received data to complete
	while (!SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE));
	//Wait for SPI to be ready
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY));
	//Return data from buffer
	return SPIx->DR;
}

void SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint8_t *dataIn, uint8_t count) {
	uint8_t i;
	for (i = 0; i < count; i++) {
		dataIn[i] = SPI_Send(SPIx, dataOut[i]);
	}
}

void SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint8_t count) {
	uint8_t i;
	for (i = 0; i < count; i++) {
		SPI_Send(SPIx, dataOut[i]);
	}
}

void SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t *dataIn, uint8_t dummy, uint8_t count) {
	uint8_t i;
	for (i = 0; i < count; i++) {
		dataIn[i] = SPI_Send(SPIx, dummy);
	}
}
