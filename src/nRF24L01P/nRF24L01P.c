/*
 * nRF24L01P.c
 *
 *  Created on: 17/dic/2014
 *      Author: Mirko
 */

#include "nRF24L01P.h"

nRF24L01pDevice_Typedef nRF24L01P;

uint8_t RxAddress[MAX_ADDRESS_WIDTH] = {
	0xE7,
	0xE7,
	0xE7,
	0xE7,
	0xE7
};

uint8_t TxAddress[MAX_ADDRESS_WIDTH] = {
	0xC2,
	0xC2,
	0xC2,
	0xC2,
	0xC2
};

void nRF24L01P_Init(AddressWidth_TypeDef addr_width, SpeedMode_TypeDef speed, PowerMode_TypeDef power,
		uint8_t payload_size, uint8_t channel){

	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDInit(LED3);

	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED4);

	uint8_t addr_width_mask = 0;
	uint8_t i = 0;

	if(channel <= MAX_CHANNEL)
		nRF24L01P.channel = channel;
	else
		nRF24L01P.channel = DEFAULT_CHANNEL;

	if(payload_size > MAX_PAYLOAD_SIZE)
		nRF24L01P.payload_size = MAX_PAYLOAD_SIZE;
	else
		nRF24L01P.payload_size = payload_size;

	if(addr_width == ADDR_WIDTH_3)
		addr_width_mask = 0x01;
	else if(addr_width == ADDR_WIDTH_4)
		addr_width_mask = 0x02;
	else
		addr_width_mask = 0x03;

	nRF24L01P.speed = speed;
	nRF24L01P.power = power;
	nRF24L01P.address_width = addr_width;

	for(; i < addr_width; i++){
		nRF24L01P.TxAddress[i] = TxAddress[i];
		nRF24L01P.RxAddressP0[i] = TxAddress[i];
		nRF24L01P.RxAddressP1[i] = RxAddress[i];
	}
	nRF24L01P.RxAddressP2 = 0x05 ;
	nRF24L01P.RxAddressP5 = DEFAULT_BROADCAST_ADDR;
	nRF24L01P_Delay_us(PWRUP_DELAY);

	nRF24L01P_InitPin();

	/* Register Configuration */
	nRF24L01P_WriteRegister(EN_AA, EN_AA_ALLPIPE);
	nRF24L01P_WriteRegister(EN_RX_ADDR, EN_RX_ALLPIPE);
	nRF24L01P_WriteRegister(SETUP_AW, addr_width_mask);
	nRF24L01P_WriteRegister(SETUP_RETR, 0xFF);
	nRF24L01P_WriteRegister(RF_CH, channel);
	nRF24L01P_WriteRegister(RF_SETUP, (speed << 3) | (power << 1));
	nRF24L01P_WriteRegister(FEATURE, FEATURE_EN_DPL);
	nRF24L01P_WriteRegister(DYNPD, DPL_ALLPIPE);

	/* Clear INT Flags */
	nRF24L01P_SetStatus(INT_CLEAN);

	/* Pipe Configuration */
	nRF24L01P_WriteRegister(RX_PW_P0, payload_size);
	nRF24L01P_WriteRegister(RX_PW_P1, payload_size);
	nRF24L01P_WriteRegister(RX_PW_P2, payload_size);
	nRF24L01P_WriteRegister(RX_PW_P3, payload_size);
	nRF24L01P_WriteRegister(RX_PW_P4, payload_size);
	nRF24L01P_WriteRegister(RX_PW_P5, payload_size);

	/* Address Configuration */
	nRF24L01P_SetTxAddress(nRF24L01P.TxAddress);
	nRF24L01P_SetRxAddress(nRF24L01P.RxAddressP1, PIPE1);
	nRF24L01P_SetRxAddress(&nRF24L01P.RxAddressP2, PIPE2);
	nRF24L01P_SetRxAddress(&nRF24L01P.RxAddressP5, PIPE5);

	/* Mask TX_DS and MAX_RT INT (for polling mode), enable CRC of 2 Bytes and PowerUp */
	nRF24L01P_WriteRegister(CONFIG, CONFIG_MASK_TX_DS | CONFIG_MASK_MAX_RT | CONFIG_EN_CRC | CONFIG_PWR_UP);
	nRF24L01P_Delay_us(TPD2STBY_DELAY);

	nRF24L01P_PrintRegister();
	nRF24L01P_PrintAddress();

	nRF24L01P_FlushTx();
	nRF24L01P_FlushRx();

	nRF24L01P_RxMode();
	nRF24L01P.rx_mutex = RX_MUTEX_UNLOCK;

	STM_EVAL_LEDOff(LED5);
	STM_EVAL_LEDOff(LED4);
}

void nRF24L01P_RxMode(){
	NRF24L01P_CE_LOW;
	uint8_t config_value = nRF24L01P_ReadRegister(CONFIG);

	config_value |= CONFIG_PRIM_RX;
	nRF24L01P_SetStatus(INT_CLEAN);

	nRF24L01P_WriteRegister(CONFIG, config_value);
	NRF24L01P_CE_HIGH;

	nRF24L01P_Delay_us(TSTBY2A_DELAY);
}

TrasmissionResult_TypeDef nRF24L01P_TxPacket(uint8_t* pdata){
	TrasmissionResult_TypeDef result = TX_LOST;
	TxMutex_TypeDef tx_mutex = TX_MUTEX_LOCK;
	uint8_t config_value = nRF24L01P_ReadRegister(CONFIG);
	uint8_t status_value = 0;

	config_value &= ~CONFIG_PRIM_RX;
	nRF24L01P_FlushTx();

	NRF24L01P_CE_LOW;
	nRF24L01P_SetStatus(TX_DS | MAX_RT);
	nRF24L01P_WriteRegister(CONFIG, config_value);

	/* Write payload through SPI */
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_WRITE_PAYLOAD);
	SPI_WriteMulti(NRF24L01P_SPI, pdata, nRF24L01P.payload_size);
	NRF24L01P_CSN_HIGH;

	/* Entering TxMode */
	NRF24L01P_CE_HIGH;

	nRF24L01P_Delay_us(TSTBY2A_DELAY);

	do{
		status_value = nRF24L01P_ReadRegister(STATUS_nRF);

		if((status_value & TX_DS) != 0){
			result = TX_DONE;
			tx_mutex = TX_MUTEX_UNLOCK;
			nRF24L01P_SetStatus(TX_DS);
			STM_EVAL_LEDToggle(LED4);
		}
		if((status_value & MAX_RT) != 0){
			result = TX_LOST;
			tx_mutex = TX_MUTEX_UNLOCK;
			nRF24L01P_SetStatus(MAX_RT);
			STM_EVAL_LEDToggle(LED5);
		}
	}while(tx_mutex == TX_MUTEX_LOCK);

	nRF24L01P_RxMode();

	return result;
}

void nRF24L01P_TxBroadcast(uint8_t* pdata){
	uint8_t tx_broadcast[MAX_ADDRESS_WIDTH];
	uint8_t i = 0;

	for(; i < nRF24L01P.address_width; i++){
		tx_broadcast[i] = nRF24L01P.TxAddress[i];
	}
	tx_broadcast[nRF24L01P.address_width-1] = nRF24L01P.RxAddressP5;

	NRF24L01P_CE_LOW;
	nRF24L01P_SetTxAddress(tx_broadcast);
	nRF24L01P_PrintAddress();
	nRF24L01P_TxPacket(pdata);
	NRF24L01P_CE_LOW;
	nRF24L01P_SetTxAddress(nRF24L01P.TxAddress);
	nRF24L01P_PrintAddress();
	nRF24L01P_RxMode();
}

void nRF24L01P_SetTxAddress(uint8_t *adr) {
	nRF24L01P_WriteRegisterMulti(RX_ADDR_P0, adr, nRF24L01P.address_width);
	nRF24L01P_Delay_us(TPECE2CSN_DELAY);
	nRF24L01P_WriteRegisterMulti(TX_ADDR, adr, nRF24L01P.address_width);
	nRF24L01P_Delay_us(TPECE2CSN_DELAY);
}

void nRF24L01P_SetRxAddress(uint8_t* addr, PipeNumber_TypeDef pipe) {
	uint8_t val;

	if(pipe == PIPE0){
		val = RX_ADDR_P0;
		nRF24L01P_WriteRegisterMulti(val, addr, nRF24L01P.address_width);
	}
	else if(pipe == PIPE1){
		val = RX_ADDR_P1;
		nRF24L01P_WriteRegisterMulti(val, addr, nRF24L01P.address_width);
	}
	else{
		if(pipe == PIPE2)
			val = RX_ADDR_P2;
		else if(pipe == PIPE3)
			val = RX_ADDR_P3;
		else if(pipe == PIPE4)
			val = RX_ADDR_P4;
		else
			val = RX_ADDR_P5;
		nRF24L01P_WriteRegister(val, addr[0]);
	}
}

/*
----------------------------------------------------------------------
---   Almost Private Functions  --------------------------------------
----------------------------------------------------------------------
*/
void nRF24L01P_ReadPayloadLength(uint8_t* pdata){
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_READ_PAYLOAD_LENGTH);
	SPI_ReadMulti(NRF24L01P_SPI, pdata, NRF24L01P_NOP, 1);
	NRF24L01P_CSN_HIGH;
}

void nRF24L01P_GetPayload(uint8_t* pdata, uint8_t payload_length){
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_READ_PAYLOAD);
	SPI_ReadMulti(NRF24L01P_SPI, pdata, NRF24L01P_NOP, payload_length);
	NRF24L01P_CSN_HIGH;
}

void nRF24L01P_SetStatus(uint8_t value){
	nRF24L01P_WriteRegister(STATUS_nRF, value);
}

void nRF24L01P_FlushTx(){
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_FLUSH_TX);
	NRF24L01P_CSN_HIGH;
}

void nRF24L01P_FlushRx(){
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_FLUSH_RX);
	NRF24L01P_CSN_HIGH;
}

/*
----------------------------------------------------------------------
---   ISR RX Packet   ------------------------------------------------
----------------------------------------------------------------------
*/
void EXTI9_5_IRQHandler(void) {
	if (EXTI_GetITStatus(NRF24L01P_IRQ_EXTI_LINE) != RESET) {
		EXTI_ClearITPendingBit(NRF24L01P_IRQ_EXTI_LINE);

		uint8_t status_value;
		uint8_t payload_length;

		STM_EVAL_LEDToggle(LED4);

		status_value = nRF24L01P_ReadRegister(STATUS_nRF);
		if ((status_value & RX_DR) != 0) {
			NRF24L01P_CE_LOW;
			//TODO discriminare su quale pipe è arrivato il dato
			//uint8_t fifo_status;
			//nRF24L01P_GetFifoStatus(&fifo_status);

			//uint8_t buff[MAX_PAYLOAD_SIZE];

			nRF24L01P_ReadPayloadLength(&payload_length);

			if (payload_length > MAX_PAYLOAD_SIZE || payload_length <= 0) {
				nRF24L01P_FlushRx();
			} else {
				nRF24L01P_GetPayload(nRF24L01P.rx_buffer, payload_length);
				nRF24L01P_WriteRegister(STATUS_nRF, RX_DR);
				nRF24L01P.rx_mutex = RX_MUTEX_UNLOCK;
			}
			NRF24L01P_CE_HIGH;
		}
	}
}

/*
----------------------------------------------------------------------
---   Private Functions   --------------------------------------------
----------------------------------------------------------------------
*/
uint8_t nRF24L01P_ReadRegister(uint8_t reg) {
	uint8_t reg_value;

	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_READ_REG | reg);
	reg_value = SPI_Send(NRF24L01P_SPI, NRF24L01P_NOP);
	NRF24L01P_CSN_HIGH;

	return reg_value;
}

void nRF24L01P_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count) {
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_READ_REG | reg);
	SPI_ReadMulti(NRF24L01P_SPI, data, NRF24L01P_NOP, count);
	NRF24L01P_CSN_HIGH;
}


uint8_t nRF24L01P_WriteRegister(uint8_t reg, uint8_t value) {
	uint8_t status_value;

	NRF24L01P_CSN_LOW;
	status_value = SPI_Send(NRF24L01P_SPI, NRF24L01P_WRITE_REG | reg);
	SPI_Send(NRF24L01P_SPI, value);
	NRF24L01P_CSN_HIGH;

	return status_value;
}

void nRF24L01P_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count) {
	NRF24L01P_CSN_LOW;
	SPI_Send(NRF24L01P_SPI, NRF24L01P_WRITE_REG | reg);
	SPI_WriteMulti(NRF24L01P_SPI, data, count);
	NRF24L01P_CSN_HIGH;
}

void nRF24L01P_PrintRegister(){
	uint8_t en_aa = nRF24L01P_ReadRegister(EN_AA);
	uint8_t en_rx = nRF24L01P_ReadRegister(EN_RX_ADDR);
	uint8_t setup_aw = nRF24L01P_ReadRegister(SETUP_AW);
	uint8_t setup_retr = nRF24L01P_ReadRegister(SETUP_RETR);
	uint8_t channel = nRF24L01P_ReadRegister(RF_CH);
	uint8_t rf_setup = nRF24L01P_ReadRegister(RF_SETUP);
	uint8_t dynpd = nRF24L01P_ReadRegister(DYNPD);
	uint8_t feature = nRF24L01P_ReadRegister(FEATURE);
	uint8_t config = nRF24L01P_ReadRegister(CONFIG);
	uint8_t status = nRF24L01P_ReadRegister(STATUS_nRF);

	printf("\n\n STAMPA REGISTRI:");
	printf("\nAA: %x", en_aa);
	printf("\nADDR: %x", en_rx);
	printf("\nAW: %x", setup_aw);
	printf("\nCH: %x", channel);
	printf("\nRETR: %x", setup_retr);
	printf("\nRFS: %x", rf_setup);
	printf("\nDYNPD: %x", dynpd);
	printf("\nFEATURE: %x", feature);
	printf("\nCONFIG: %x", config);
	printf("\nSTATUS: %x", status);
	nRF24L01P_Delay_us(4000000);
}

void nRF24L01P_PrintAddress(){
	uint8_t tx_addr[MAX_ADDRESS_WIDTH];
	uint8_t rx_addr_pipe0[MAX_ADDRESS_WIDTH];
	uint8_t rx_addr_pipe1[MAX_ADDRESS_WIDTH];
	uint8_t rx_addr_pipe2, rx_addr_pipe5;

	NRF24L01P_CE_LOW;

	nRF24L01P_ReadRegisterMulti(TX_ADDR, tx_addr, nRF24L01P.address_width);
	nRF24L01P_ReadRegisterMulti(RX_ADDR_P0, rx_addr_pipe0, nRF24L01P.address_width);
	nRF24L01P_ReadRegisterMulti(RX_ADDR_P1, rx_addr_pipe1, nRF24L01P.address_width);
	rx_addr_pipe2 = nRF24L01P_ReadRegister(RX_ADDR_P2);
	rx_addr_pipe5 = nRF24L01P_ReadRegister(RX_ADDR_P5);

	printf("\nTX: %x", tx_addr);
	printf("\nP0: %x", rx_addr_pipe0);
	printf("\nP1: %x", rx_addr_pipe1);

	nRF24L01P_Delay_us(4000000);
}

void nRF24L01P_Delay_us(unsigned long n) {
	unsigned long i;

	while (n--)  // delay n us
	{
		i = 25;
		while (i--)
			; // delay 1 us
	}
}

