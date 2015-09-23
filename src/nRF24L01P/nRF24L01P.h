/*
 * nRF24L01P.h
 *
 *  Created on: 17/dic/2014
 *      Author: Mirko
 */

#ifndef NRF24L01P_H_
#define NRF24L01P_H_

#include "stm32f4xx.h"
#include "nRF24L01P_SPI.h"
#include "stm32f4_discovery.h"
#include <stdio.h>


/* SPI Command */
#define NRF24L01P_READ_REG				0x00
#define NRF24L01P_WRITE_REG				0x20
#define NRF24L01P_READ_PAYLOAD_LENGTH	0x60
#define NRF24L01P_READ_PAYLOAD			0x61
#define NRF24L01P_WRITE_PAYLOAD			0xA0
#define NRF24L01P_FLUSH_TX				0xE1
#define NRF24L01P_FLUSH_RX				0xE2
#define NRF24L01P_WRITE_TX_NOACK		0xB0
#define NRF24L01P_NOP					0xFF

/* nRF24L01P Registers */
#define CONFIG          				0x00  // Configurate the status of transceiver, mode of CRC and the replay of transceiver status
#define EN_AA           				0x01  // Enable the auto-ack in all channels
#define EN_RX_ADDR       				0x02  // Enable Rx Address
#define SETUP_AW        				0x03  // Configurate the address width
#define SETUP_RETR      				0x04  // Setup the retransmit
#define RF_CH           				0x05  // Configurate the RF frequency
#define RF_SETUP        				0x06  // Setup the rate of data, and transmit power
#define STATUS_nRF	    				0x07  // Status Register
#define OBSERVE_TX      				0x08  // Observe Register
#define CD              				0x09  // Carrier detect
#define FEATURE							0x1D  // Feature Register
#define DYNPD							0x1C  // Dynpd (Dynamic payload) Register

/* nRF24L01P Pipe Registers */
#define RX_ADDR_P0      				0x0A  // Receive address of pipe 0
#define RX_ADDR_P1      				0x0B  // Receive address of pipe 1
#define RX_ADDR_P2      				0x0C  // Receive address of pipe 2
#define RX_ADDR_P3      				0x0D  // Receive address of pipe 3
#define RX_ADDR_P4      				0x0E  // Receive address of pipe 4
#define RX_ADDR_P5      				0x0F  // Receive address of pipe 5
#define TX_ADDR         				0x10  // Transmit address
#define RX_PW_P0        				0x11  // Size of receive data in pipe 0
#define RX_PW_P1        				0x12  // Size of receive data in pipe 1
#define RX_PW_P2        				0x13  // Size of receive data in pipe 2
#define RX_PW_P3        				0x14  // Size of receive data in pipe 3
#define RX_PW_P4        				0x15  // Size of receive data in pipe 4
#define RX_PW_P5        				0x16  // Size of receive data in pipe 5
#define FIFO_STATUS     				0x17  // FIFO Status

/* STATUS REGISTER Options */
#define RX_DR 							0x40
#define TX_DS 							0x20
#define MAX_RT 							0x10
#define INT_CLEAN						0x70
#define RX_P0 							0x00
#define RX_P1 							0x02
#define RX_P2 							0x04
#define RX_P3 							0x05
#define RX_P4 							0x08
#define RX_P5 							0x0A
#define RX_FIFO_EMPTY 					0x0E
#define TX_FULL 						0x01

/* CONFIG REGISTER Options */
#define CONFIG_MASK_RX_DR				0x40
#define CONFIG_MASK_TX_DS				0x20
#define CONFIG_MASK_MAX_RT				0x10
#define CONFIG_EN_CRC					0x08
#define CONFIG_CRCO						0x04
#define CONFIG_PWR_UP					0x02
#define CONFIG_PRIM_RX					0x01

/* ENABLE AUTO-ACK REGISTER Options */
#define EN_AA_P5 						0x20
#define EN_AA_P4 						0x10
#define EN_AA_P3 						0x08
#define EN_AA_P2 						0x04
#define EN_AA_P1 						0x02
#define EN_AA_P0 						0x01
#define EN_AA_ALLPIPE					0x3F

/* ENABLE PIPE REGISTER Options */
#define EN_RX_P5 						0x20
#define EN_RX_P4 						0x10
#define EN_RX_P3						0x08
#define EN_RX_P2 						0x04
#define EN_RX_P1 						0x02
#define EN_RX_P0 						0x01
#define EN_RX_ALLPIPE					0x3F

/* DYNPD REGISTER (Dynamic Payload) Options */
#define DPL_P5	 						0x20
#define DPL_P4 							0x10
#define DPL_P3							0x08
#define DPL_P2 							0x04
#define DPL_P1 							0x02
#define DPL_P0 							0x01
#define DPL_ALLPIPE						0x3F

/* FEATURE REGISTER Options */
#define FEATURE_EN_DPL					0x04
#define FEATURE_EN_ACK_PL				0x02
#define FEATURE_EN_DY_ACK				0x01

/* nRF24L01P Tempification */
#define TPECE2CSN_DELAY					4		// 4 da datasheet, funziona anche con 0
#define TSTBY2A_DELAY					130		// Tempo da attendere per il passaggio da STANDBY1 a RXMODE o TXMODE dovuto al settaggio del PLL
#define TPD2STBY_DELAY					15000	// Tempo da attendere per il passaggio dallo stato PowerDown a Standby1
#define PWRUP_DELAY						100000

/* nRF24L01P max parameters */
#define MAX_PAYLOAD_SIZE				32
#define MAX_ADDRESS_WIDTH				5
#define MAX_CHANNEL						0x7A

/* nRF24L01P default parametres */
#define DEFAULT_CHANNEL					0x7A
#define DEFAULT_BROADCAST_ADDR			0xFF

/* Entry Mode Set Typedef */
typedef enum {
	PIPE0 = 0,
	PIPE1 = 1,
	PIPE2 = 2,
	PIPE3 = 3,
	PIPE4 = 4,
	PIPE5 = 5
} PipeNumber_TypeDef;

/* Address Width Typedef */
typedef enum {
	ADDR_WIDTH_3 = 3,
	ADDR_WIDTH_4 = 4,
	ADDR_WIDTH_5 = 5
} AddressWidth_TypeDef;

/* Speed Mode Typedef */
typedef enum {
	SPEED_250Kbps = 2,
	SPEED_1Mbps = 0,
	SPEED_2Mbps = 1
} SpeedMode_TypeDef;

/* Power Mode Typedef, all values are negatives */
typedef enum {
	POWER_0dB = 3,
	POWER_6dB = 2,
	POWER_12dB = 1,
	POWER_18dB = 0
} PowerMode_TypeDef;

/* TxPacket Return Typedef */
typedef enum {
	TX_DONE = 1,
	TX_LOST = 0
} TrasmissionResult_TypeDef;

/* TxMutex Typedef */
typedef enum {
	TX_MUTEX_LOCK = 1,
	TX_MUTEX_UNLOCK = 0
} TxMutex_TypeDef;

typedef enum {
	RX_MUTEX_LOCK = 1,
	RX_MUTEX_UNLOCK = 0
} RxMutex_TypeDef;

/* nRF24L01P Control Struct */
typedef struct{
	char* nRF24L01_DeviceName;

	uint8_t power;
	uint8_t speed;
	uint8_t channel;
	uint8_t payload_size;
	uint8_t address_width;

	uint8_t  TxAddress[MAX_ADDRESS_WIDTH];
	uint8_t  RxAddressP0[MAX_ADDRESS_WIDTH];
	uint8_t  RxAddressP1[MAX_ADDRESS_WIDTH];
	uint8_t  RxAddressP2;
	uint8_t  RxAddressP3;
	uint8_t  RxAddressP4;
	uint8_t  RxAddressP5;

	uint8_t rx_buffer[MAX_PAYLOAD_SIZE];
	RxMutex_TypeDef rx_mutex;
} nRF24L01pDevice_Typedef;


void nRF24L01P_Init(AddressWidth_TypeDef addr_width, SpeedMode_TypeDef speed, PowerMode_TypeDef power,
			uint8_t payload_size, uint8_t channel);
void nRF24L01P_RxMode();
TrasmissionResult_TypeDef nRF24L01P_TxPacket(uint8_t* pdata);
void nRF24L01P_TxBroadcast(uint8_t* pdata);
void nRF24L01P_SetTxAddress(uint8_t *adr);
void nRF24L01P_SetRxAddress(uint8_t *adr, PipeNumber_TypeDef pipe);

void nRF24L01P_GetPayload(uint8_t* pdata, uint8_t payload_length);
void nRF24L01P_ReadPayloadLength(uint8_t* pdata);
void nRF24L01P_SetStatus(uint8_t value);
void nRF24L01P_FlushTx();
void nRF24L01P_FlushRx();

/*
----------------------------------------------------------------------
---   Private Functions  ---------------------------------------------
----------------------------------------------------------------------
*/
uint8_t nRF24L01P_WriteRegister(uint8_t reg, uint8_t value);
void nRF24L01P_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count);
uint8_t nRF24L01P_ReadRegister(uint8_t reg);
void nRF24L01P_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count);
void nRF24L01P_Delay_us(unsigned long n);
void nRF24L01P_PrintRegister();
void nRF24L01P_PrintAddress();
#endif /* NRF24L01P_H_ */
