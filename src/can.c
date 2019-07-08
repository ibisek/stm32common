/*
 * can.c
 *
 *  Created on: 08. 12. 2016
 *      Author: ibisek
 */

#include <can.h>
#include <ibitime.h>
#include <stm32f0xx.h>
#include <stm32f0xx_gpio.h>
#include <stm32f0xx_misc.h>
#include <stm32f0xx_rcc.h>
#include <sys/_stdint.h>

uint8_t _canBitrate;

/**
 * @param *CAN_InitStructure
 * @param bitrate CAN_BITRATE_125k (1), CAN_BITRATE_250k (2), CAN_BITRATE_500k (3)
 */
void can_setCanBitrate(CAN_InitTypeDef *CAN_InitStructure, uint8_t bitrate) {
	// CAN clock 24/48MHz; http://www.bittiming.can-wiki.info/

	_canBitrate = bitrate;

	/* 8 MHz */

	if (SystemCoreClock == 8000000) {
		switch (bitrate) {
		case CAN_BITRATE_125k:
			CAN_InitStructure->CAN_Prescaler = 4; // number of time quanta = 8M/4/125k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_250k:
			CAN_InitStructure->CAN_Prescaler = 2;	// number of time quanta = 8M/2/250k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_500k:
			CAN_InitStructure->CAN_Prescaler = 1;	// number of time quanta = 8M/1/500k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_1M:
			CAN_InitStructure->CAN_Prescaler = 1;	// number of time quanta = 8M/1/1M = 8
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_6tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_1tq;
			break;

		default:
			serial_print("Error: Unsupported CAN bitrate!!");
			while (1)
				;
		}
	}

	/* 24 MHz */

	if (SystemCoreClock == 24000000) {
		switch (bitrate) {
		case CAN_BITRATE_125k:
			CAN_InitStructure->CAN_Prescaler = 12; // number of time quanta = 24M/12/125k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_250k:
			CAN_InitStructure->CAN_Prescaler = 6;	// number of time quanta = 24M/6/250k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_500k:
			CAN_InitStructure->CAN_Prescaler = 3;	// number of time quanta = 24M/3/500k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_1M:
			CAN_InitStructure->CAN_Prescaler = 2;	// number of time quanta = 24M/2/1M = 12
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_10tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_1tq;
			break;

		default:
			serial_print("Error: Unsupported CAN bitrate!!");
			while (1)
				;
		}
	}

	/* 48 MHz */

	if (SystemCoreClock == 48000000) {
		switch (bitrate) {
		case CAN_BITRATE_125k:
			CAN_InitStructure->CAN_Prescaler = 24; // number of time quanta = 48M/24/125k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_250k:
			CAN_InitStructure->CAN_Prescaler = 12;	// number of time quanta = 48M/12/250k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_500k:
			CAN_InitStructure->CAN_Prescaler = 6;	// number of time quanta = 48M/6/500k = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		case CAN_BITRATE_1M:
			CAN_InitStructure->CAN_Prescaler = 3;	// number of time quanta = 48M/3/1M = 16
			CAN_InitStructure->CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure->CAN_BS1 = CAN_BS1_13tq;
			CAN_InitStructure->CAN_BS2 = CAN_BS2_2tq;
			break;

		default:
			serial_print("Error: Unsupported CAN bitrate!!");
			while (1)
				;
		}
	}
}

/**
 * Needs the following to be already enabled:
 * RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	// Enable GPIO A bank clock
 * @param bitrate CAN_BITRATE_125k (1), CAN_BITRATE_250k (2), CAN_BITRATE_500k (3)
 */
void can_init(uint8_t bitrate) {
	CAN_DeInit(CAN);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);		// Enable GPIO A bank clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);	// Enable CAN1 clock

	// I/O pins mode configuration:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	// GPIO_PuPd_NOPULL | GPIO_PuPd_UP ; If you alter to GPIO_PuPd_NOPULL then 1MBaud will not work
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CAN_GPIO, &GPIO_InitStructure);

	// pin function remapping
	GPIO_PinAFConfig(CAN_GPIO, CAN_RX_PIN_SOURCE, GPIO_AF_4);	// alternate function CAN (@see datasheet page 38)
	GPIO_PinAFConfig(CAN_GPIO, CAN_TX_PIN_SOURCE, GPIO_AF_4);	// alternate function CAN (@see datasheet page 38)

	CAN_InitTypeDef CAN_InitStructure;
	CAN_StructInit(&CAN_InitStructure);	// load default settings
	CAN_InitStructure.CAN_TTCM = DISABLE;	// time-triggered communication mode
	CAN_InitStructure.CAN_ABOM = DISABLE;	// automatic bus-off management mode
	CAN_InitStructure.CAN_AWUM = DISABLE;	// automatic wake-up mode
	CAN_InitStructure.CAN_NART = DISABLE;	// non-automatic retransmission mode
	CAN_InitStructure.CAN_RFLM = DISABLE;	// receive FIFO locked mode
	CAN_InitStructure.CAN_TXFP = DISABLE;	// transmit FIFO priority
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // CAN_Mode_Normal | CAN_Mode_LoopBack;

	can_setCanBitrate(&CAN_InitStructure, bitrate);

	CAN_Init(CAN, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	if (CAN_ENABLE_RX_INT == 1) {
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);	// FIFO 0 message pending interrupt
		CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);	// FIFO 1 message pending interrupt

		CAN_ITConfig(CAN, CAN_IT_ERR, ENABLE);		// CAN error interrupt
	}
}

void can_printRxMessage(CanRxMsg *msg) {
	// print standard/extended id:
	if (msg->IDE == CAN_Id_Standard) {
		serial_print("STD ");
		serial_print_hex32(msg->StdId);
	} else if (msg->IDE == CAN_Id_Extended) {
		serial_print("EXT ");
		serial_print_hex32(msg->ExtId);
	} else {
		serial_print("???");
	}

	// print data length:
	serial_print(" [");
	serial_print_hex(msg->DLC);
	serial_print("]");

	// print data:
	for (uint8_t i = 0; i < msg->DLC; i++) {
		serial_print(" ");
		serial_print_hex(msg->Data[i]);
	}

	serial_print("\n");
}

/**
 * @return 1/true if a message can be sent
 */
uint8_t can_messageTxPossible() {
	uint8_t canTxStatusMB0 = CAN_TransmitStatus(CAN, 0);
	uint8_t canTxStatusMB1 = CAN_TransmitStatus(CAN, 1);

	if ((canTxStatusMB0 == CAN_TxStatus_Pending || canTxStatusMB0 == CAN_TxStatus_NoMailBox)
			&& (canTxStatusMB1 == CAN_TxStatus_Pending || canTxStatusMB0 == CAN_TxStatus_NoMailBox)) {
		return 0;

	} else {
		return 1;
	}
}

/**
 * Sends a message over configured CAN.
 *
 * @param txMsg
 * @return CanStatus OK/FAIL/MB_FULL
 */
CanStatus can_sendMessage(CanTxMsg *txMsg) {
	// firstly, check if there is an usable mailbox:
	if (!can_messageTxPossible()) return NO_MB;

	uint8_t canTxMailbox = CAN_Transmit(CAN, txMsg);

	if (canTxMailbox != CAN_TxStatus_NoMailBox) {
#if CAN_DEBUG
		serial_print("mailbox:");
		serial_print_int(canTxMailbox);
#endif
	} else {
#if CAN_DEBUG
		serial_print("mb full");
		serial_print(" .. relaxing on full (1s) ..\n");
#endif
		//delay_ms(1000);

		return MB_FULL;
	}

	// wait for some time for the message to be send:
	uint8_t timeout = 11;
	while (--timeout > 0 && CAN_TransmitStatus(CAN, canTxMailbox) == CAN_TxStatus_Pending) {
		delay_ms(1);	//wait and check again
	}

	if (CAN_TransmitStatus(CAN, canTxMailbox) == CAN_TxStatus_Ok) {
#if CAN_DEBUG
		serial_print(" TX:OK\n");
#endif

		return OK;

	} else if (CAN_TransmitStatus(CAN, canTxMailbox) == CAN_TxStatus_Failed) {
#if CAN_DEBUG
		serial_print(" TX:FAIL\n");
		serial_print(" .. relaxing on fail (4s) ..\n");
#endif
		//delay_ms(4000);

#if CAN_DEBUG
		serial_print("CAN re-init\n");
#endif
		can_init(_canBitrate);

		return FAIL;

	} else if (CAN_TransmitStatus(CAN, canTxMailbox) == CAN_TxStatus_Pending) {
		return PENDING;

	} else if (CAN_TransmitStatus(CAN, canTxMailbox) == CAN_TxStatus_NoMailBox) {
		return NO_MB;
	}

	return UNKNOWN;
}

/**
 * @param filterNumber 0-x
 * @param filterFifo 0/1 or CAN_Filter_FIFO0 / CAN_Filter_FIFO1
 * @param canAddr STD address (currently cannot do EXT)
 */
void can_setRxFilter(uint8_t filterNumber, uint8_t filterFifo, uint16_t canAddr) {
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = filterNumber;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

	canAddr = canAddr << 5;	// needs to be shifted by 5 bits -  see manual "Filter bank scale configuration - register organization"
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = canAddr & 0x0000FFFF;

	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = filterFifo;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

/*
 * @param filterNumber 0-x
 * @param filterFifo 0/1 or CAN_Filter_FIFO0 / CAN_Filter_FIFO1
 */
void can_disableRxFilter(uint8_t filterNumber, uint8_t filterFifo) {
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = filterNumber;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = filterFifo;
	CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}
