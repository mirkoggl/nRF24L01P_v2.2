#include "stm32f4xx.h"
#include "nRF24L01P/nRF24L01P.h"

extern nRF24L01pDevice_Typedef nRF24L01P;

static Led_TypeDef LEDs[4] = {LED3, LED4, LED5, LED6};
uint8_t counter = 0;

void setup();
void loopTx();
void loopRx();

int main(void) {
	setup();
	while(1){
		loopRx();
	}
}

void setup(){
	uint8_t i;
	uint8_t data[32];
	TrasmissionResult_TypeDef result;

	for(i = 0; i<MAX_PAYLOAD_SIZE; i++){
		data[i] = i+1;
	}

	nRF24L01P_Init(ADDR_WIDTH_5, SPEED_1Mbps, POWER_0dB, 32, 0x12);
	nRF24L01P_RxMode();
	STM_EVAL_LEDToggle(LED3);

	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

//	result = nRF24L01P_TxPacket(data);
//
//	if(result == TX_LOST)
//		printf("Pacchetto perso!");
//	if(result == TX_DONE)
//		printf("Pacchetto inviato!");
//	nRF24L01P_Delay_us(40000);
//
//	nRF24L01P_TxBroadcast(data);
//	printf("Pacchetto Broadcast inviato!");

}

void loopRx(){
	nRF24L01P.rx_mutex = RX_MUTEX_LOCK;
	while(nRF24L01P.rx_mutex == RX_MUTEX_LOCK);
	STM_EVAL_LEDToggle(LEDs[nRF24L01P.rx_buffer[0]]);
}

void loopTx(){
	if(STM_EVAL_PBGetState(BUTTON_USER) == 1){
		STM_EVAL_LEDToggle(LED6);

		nRF24L01P_TxPacket(&counter);
		counter = (counter + 1) % 4;
		nRF24L01P_Delay_us(40000);
	}
}
