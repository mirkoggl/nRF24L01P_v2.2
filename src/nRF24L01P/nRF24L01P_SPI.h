/*
 * nRF24L01P_SPI.h
 *
 *  Created on: 17/dic/2014
 *      Author: Alessandra
 */

#ifndef NRF24L01P_SPI_H_
#define NRF24L01P_SPI_H_

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"

/* SPI Configuration */
#define NRF24L01P_SPI						SPI3
#define NRF24L01P_SPI_RCC					RCC_APB1Periph_SPI3

#define NRF24L01P_SPI_PRESCALER				SPI_BaudRatePrescaler_32
#define NRF24L01P_SPI_DATASIZE				SPI_DataSize_8b
#define NRF24L01P_SPI_DIRECTION				SPI_Direction_2Lines_FullDuplex
#define NRF24L01P_SPI_FIRSTBIT 				SPI_FirstBit_MSB
#define NRF24L01P_SPI_MODE 					SPI_Mode_Master

#define NRF24L01P_SPI_GPIO_AF				GPIO_AF_SPI3
#define NRF24L01P_SPI_SCK_PIN				GPIO_Pin_10
#define NRF24L01P_SPI_MISO_PIN				GPIO_Pin_11
#define NRF24L01P_SPI_MOSI_PIN				GPIO_Pin_12
#define NRF24L01P_SPI_SCK_PINSOURCE			GPIO_PinSource10
#define NRF24L01P_SPI_MISO_PINSOURCE		GPIO_PinSource11
#define NRF24L01P_SPI_MOSI_PINSOURCE		GPIO_PinSource12

#define NRF24L01P_SPI_GPIO_RCC				RCC_AHB1Periph_GPIOC
#define NRF24L01P_SPI_GPIO_PORT				GPIOC

/* Pin options */
#define NRF24L01P_CSN_RCC					RCC_AHB1Periph_GPIOD
#define NRF24L01P_CSN_PORT					GPIOD
#define NRF24L01P_CSN_PIN					GPIO_Pin_7

#define NRF24L01P_CE_RCC					RCC_AHB1Periph_GPIOD
#define NRF24L01P_CE_PORT					GPIOD
#define NRF24L01P_CE_PIN					GPIO_Pin_8

#define NRF24L01P_IRQ_RCC					RCC_AHB1Periph_GPIOE
#define NRF24L01P_IRQ_PIN					GPIO_Pin_9
#define NRF24L01P_IRQ_PORT					GPIOE
#define NRF24L01P_IRQ_EXTI_PORT				EXTI_PortSourceGPIOE
#define NRF24L01P_IRQ_GPIO_EXTI_LINE 		EXTI_PinSource9
#define NRF24L01P_IRQ_EXTI_LINE 			EXTI_Line9
#define NRF24L01P_IRQ_EXTI_IRQ 				EXTI9_5_IRQn
#define NRF24L01P_IRQ_EXTI_PRIORITY			0x0F

#define NRF24L01P_CE_LOW					NRF24L01P_CE_PORT->BSRRH = NRF24L01P_CE_PIN
#define NRF24L01P_CE_HIGH					NRF24L01P_CE_PORT->BSRRL = NRF24L01P_CE_PIN
#define NRF24L01P_CSN_LOW					NRF24L01P_CSN_PORT->BSRRH = NRF24L01P_CSN_PIN
#define NRF24L01P_CSN_HIGH					NRF24L01P_CSN_PORT->BSRRL = NRF24L01P_CSN_PIN


void nRF24L01P_InitPin();
void nRF24L01P_SPI_Init();

uint8_t SPI_Send(SPI_TypeDef* SPIx, uint8_t data);
void SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint8_t *dataIn, uint8_t count) ;
void SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t *dataOut, uint8_t count);
void SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t *dataIn, uint8_t dummy, uint8_t count);

#endif /* NRF24L01P_SPI_H_ */
