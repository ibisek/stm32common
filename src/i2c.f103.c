/*
 * i2c.f103.c
 *
 *  Created on: Dec 6, 2016
 *      Author: ibisek
 *
 * @see AN4235 I2C timing configuration tool
 * @see http://www.st.com/content/ccc/resource/technical/document/application_note/de/14/eb/51/75/e3/49/f8/DM00074956.pdf/files/DM00074956.pdf/jcr:content/translations/en.DM00074956.pdf
 */

#include "i2c.f103.h"

/**
 *
 * STM32F103: I2Cx mapped to PB6 (SCL) and PB7 (SDA)
 * STM32F103: I2C2 mapped to PB10 (SCL) and PB11 (SDA)
 *
 * @param ownAddress
 * @param enableRxInterrupt
 */
void i2c_init(I2C_TypeDef* I2Cx, uint8_t ownAddress, uint8_t enableRxInterrupt) {
	I2C_DeInit(I2Cx);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Enable GPIO B bank clock

	// GPIO configuration:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;

	if (I2Cx == I2C1) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// Enable I2C clock:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	} else if (I2Cx == I2C2) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// Enable I2C clock:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	}

	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_OwnAddress1 = ownAddress;
	I2C_InitStructure.I2C_ClockSpeed = 100000;	// 100kHz
	I2C_Init(I2Cx, &I2C_InitStructure);

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	I2C_Cmd(I2Cx, ENABLE);

	if (enableRxInterrupt) {
		NVIC_InitTypeDef NVIC_InitStructure;

		/* Configure the SPI interrupt priority */
		if (I2Cx == I2C1)
			NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
		else if (I2Cx == I2C2)
			NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;

		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Enable Interrupts */
		I2C_ITConfig(I2Cx, I2C_IT_EVT, ENABLE);
	}
}

/**
 * @param deviceAddress
 * @param registerAddress
 */
uint8_t i2c_readByte(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress) {
	uint8_t theByte = 0xFF;

	i2c_readBytes(I2Cx, deviceAddress, registerAddress, 1, &theByte);

	return theByte;
}

/**
 * @param deviceAddress
 * @param registerAddress
 * @param count	number of bytes to read
 * @param *dest preallocated buffer
 */

ErrorStatus i2c_readBytes(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t count, uint8_t *dest) {
	uint16_t timeout = I2C_TIMEOUT;

	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c BUSY");
			return ERROR;
		}
	}

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// ----[ address the device and register to read from ]----
	I2C_GenerateSTART(I2Cx, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB));
	/* Send device address for write */
	I2C_Send7bitAddress(I2Cx, deviceAddress, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/* Send the device's internal address to write to */
	I2C_SendData(I2Cx,registerAddress);
	/* Test on TXE FLag (data sent) */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_TXE));

	// ----[ read out the data ]----
	/* Send START condition a second time (Re-Start) */
	I2C_GenerateSTART(I2Cx, ENABLE);
	/* Test start flag */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB));
	/* Send address for read */
	I2C_Send7bitAddress(I2Cx, deviceAddress, I2C_Direction_Receiver);
	/* Test Receive mode Flag */
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	/* Loop until all data are received */
	uint8_t i = 0;
	while (count) {
		//while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
		while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE));

		/* Read data from RXDR */
		dest[i++] = I2C_ReceiveData(I2Cx);

		/* Decrement the read bytes counter */
		count--;
	}

	/* enable NACK bit */ // (the communication with BMP280 is ended by NACK bit (see datasheet p.30)
	I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2Cx, DISABLE);

	/* Send STOP condition */
	I2C_GenerateSTOP(I2Cx, ENABLE);

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)); // stop bit flag

	//--

//	i2c__sendStartCondition(I2Cx, deviceAddress);
//
//	/* Send Register address */
//	I2C_SendData(I2Cx, (uint8_t) registerAddress);
//
//	/* Wait until byte is transmitted */
//	while (!I2C_CheckEvent( I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-rd TIMEOUT 2");
//			return ERROR;
//		}
//	}
//
//	/* Loop until all data are received */
//	uint8_t i = 0;
//	while (count) {
//		/* Wait until RXNE flag is set */
//		while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
//			if ((timeout--) == 0) {
//				serial_println("## i2c-rd TIMEOUT 3");
//				return ERROR;
//			}
//		}
//
//		/* Read data from RXDR */
//		dest[i++] = I2C_ReceiveData(I2Cx);
//
//		/* Decrement the read bytes counter */
//		count--;
//	}
//
//	i2c__sendStopCondition(I2Cx);
//
//	/* Wait until STOPF flag is set */
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-rd TIMEOUT 4");
//			return ERROR;
//		}
//	}
//
//	/* Clear STOPF flag */
//	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);

	return SUCCESS;
}

//void i2c__sendStartCondition(I2C_TypeDef* I2Cx, uint8_t deviceAddress) {
//	I2C_GenerateSTART(I2Cx, ENABLE);
//	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
//		;
//	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//		;
//}
//
//void i2c__sendStopCondition(I2C_TypeDef* I2Cx) {
//	I2C_GenerateSTOP(I2Cx, ENABLE);
//}

/**
 * @param deviceAddress
 * @param registerAddress
 * @param data
 */
ErrorStatus i2c_writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddress,	uint8_t registerAddress, uint8_t data) {
	ErrorStatus retVal = SUCCESS;
	uint16_t timeout = I2C_TIMEOUT;

	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr BUSY");
			retVal = ERROR;
		}
	}

	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	// ----[ address the device and register to read from ]----
	I2C_GenerateSTART(I2Cx, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB)) {
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr TIMEOUT 1");
			retVal = ERROR;
		}
	}

	/* Send device address for write */
	I2C_Send7bitAddress(I2Cx, deviceAddress, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr TIMEOUT 2");
			retVal = ERROR;
			break;
		}
	}

	/* Send the device's internal address to write to */
	I2C_SendData(I2Cx,registerAddress);
	/* Test on TXE FLag (data sent) */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_TXE)) {
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr TIMEOUT 3");
			retVal = ERROR;
			break;
		}
	}

	I2C_SendData(I2Cx,data);
	/* Test on TXE FLag (data sent) */
	while (!I2C_GetFlagStatus(I2Cx,I2C_FLAG_TXE)) {
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr TIMEOUT 4");
			retVal = ERROR;
			break;
		}
	}

	/* Send STOP condition */
	I2C_GenerateSTOP(I2Cx, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) {	// stop bit flag
		if ((timeout--) == 0) {
			//serial_println("## i2c-wr TIMEOUT 5");
			retVal = ERROR;
			break;
		}
	}

	//--

	//Clear the stop flag for the next potential transfer
	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);

	return retVal;
}

///**
// * @param deviceAddress
// * @param command
// */
//ErrorStatus i2c_writeCommand(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t command) {
//	uint16_t timeout = I2C_TIMEOUT;
//
//	/* Test on BUSY Flag */
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-wc BUSY");
//			return ERROR;
//		}
//	}
//
//	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
//	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);
//
//	/* Wait until TXIS (TX Interrupt Status) flag is set */
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-wc TIMEOUT 1");
//			return ERROR;
//		}
//	}
//
//	/* Send command */
//	I2C_SendData(I2Cx, (uint8_t) command);
//
//	/* Wait until TCR flag is set */
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-wc TIMEOUT 2");
//			return ERROR;
//		}
//	}
//
//	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
//	I2C_TransferHandling(I2Cx, deviceAddress, 0, I2C_AutoEnd_Mode, I2C_Generate_Stop);
//
//	/* Wait until TXIS flag is set */
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-wr TIMEOUT 3");
//			return ERROR;
//		}
//	}
//
//	//Clear the stop flag for the next potential transfer
//	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
//
//	return SUCCESS;
//}
//
//uint8_t i2c_readCommandResponse(I2C_TypeDef* I2Cx, uint8_t deviceAddress) {
//	uint8_t theByte = 0xFF;
//
//	/* Wait until I2C isn't busy */
//	uint16_t timeout = I2C_TIMEOUT;
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-rcr BUSY");
//			return ERROR;
//		}
//	}
//
//	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
//	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
//
//	/* Wait until RXNE flag is set */
//	timeout = I2C_TIMEOUT;
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == RESET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-rcr TIMEOUT 1");
//			return ERROR;
//		}
//	}
//
//	/* Read data from RXDR */
//	theByte = I2C_ReceiveData(I2Cx);
//
//	/* Wait until STOPF flag is set */
//	timeout = I2C_TIMEOUT;
//	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == RESET) {
//		if ((timeout--) == 0) {
//			serial_println("## i2c-rcr TIMEOUT 2");
//			return ERROR;
//		}
//	}
//
//	/* Clear STOPF flag */
//	I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
//
//	return theByte;
//}

void i2c_printFlags(I2C_TypeDef* I2Cx) {
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) == SET) {
		serial_print("I2C_FLAG_TXE ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == SET) {
		serial_print("I2C_FLAG_RXNE ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
		serial_print("I2C_FLAG_ADDR ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_AF) == SET) {
		serial_print("I2C_FLAG_AF ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == SET) {
		serial_print("I2C_FLAG_STOPF ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == SET) {
		serial_print("I2C_FLAG_BTF ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BERR) == SET) {
		serial_print("I2C_FLAG_BERR ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ARLO) == SET) {
		serial_print("I2C_FLAG_ARLO ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_OVR) == SET) {
		serial_print("I2C_FLAG_OVR ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_PECERR) == SET) {
		serial_print("I2C_FLAG_PECERR ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TIMEOUT) == SET) {
		serial_print("I2C_FLAG_TIMEOUT ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_SMBALERT) == SET) {
		serial_print("I2C_FLAG_SMBALERT ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		serial_print("I2C_FLAG_BUSY ");
	}
}
