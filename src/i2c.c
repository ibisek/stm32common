/*
 * i2c.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: ibisek
 *
 * @see AN4235 I2C timing configuration tool
 * @see http://www.st.com/content/ccc/resource/technical/document/application_note/de/14/eb/51/75/e3/49/f8/DM00074956.pdf/files/DM00074956.pdf/jcr:content/translations/en.DM00074956.pdf
 */

#include "i2c.h"

/**
 * STM32F042K6: I2C1 mapped to PB6 (SCL) and PB7 (SDA)
 *
 * STM32F303: I2C1 mapped to PB6 (SCL) and PB7 (SDA)
 * STM32F303: I2C2 mapped to PA9 (SCL) and PA10 (SDA)
 *
 * @param ownAddress LEFT SHIFTED ADDRESS; e.g. 0x66 << 1
 * @param enableRxInterrupt
 */
void i2c_init(I2C_TypeDef* I2Cx, uint8_t ownAddress, uint8_t enableRxInterrupt) {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);		// Enable GPIO B bank clock

	// GPIO configuration:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	if(I2Cx == I2C1) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// Alternate function mapping:
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);	// alternate function I2C1_SCL
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);	// alternate function I2C1_SDA

		// Enable I2C clock:
		RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	}
#ifdef STM32F303
	else if(I2Cx == I2C2) {
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// Alternate function mapping:
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_9);	// alternate function I2C2_SCL
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_10);	// alternate function I2C2_SDA

		// Enable I2C clock:
		RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	}
#endif

	I2C_DeInit(I2Cx);

	I2C_InitTypeDef I2C_InitStructure;
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_OwnAddress1 = ownAddress;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_Timing = I2C_TIMING;
	I2C_Init(I2Cx, &I2C_InitStructure);
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	//I2C_StretchClockCmd(I2Cx, ENABLE);
	I2C_Cmd(I2Cx, ENABLE);

	if (enableRxInterrupt) {
		NVIC_InitTypeDef NVIC_InitStructure;

		/* Configure the SPI interrupt priority */
		if(I2Cx == I2C1)
			NVIC_InitStructure.NVIC_IRQChannel = I2C1_IRQn;
#ifdef STM32F303
		else if(I2Cx == I2C2)
			NVIC_InitStructure.NVIC_IRQChannel = I2C2_IRQn;
#endif

		NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* Enable Interrupts */
		I2C_ITConfig(I2Cx, (I2C_IT_ADDRI | I2C_IT_RXI), ENABLE);
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

	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_BUSY) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c BUSY");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	I2C_SendData(I2Cx, registerAddress);

	/* Wait until TXIS (TX Interrupt Status) flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TXIS) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c TIMEOUT 1");
			return ERROR;
		}
	}

	/* Send Register address */
	I2C_SendData(I2Cx, registerAddress);

	/* Wait until TC (transfer Complete) flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TC) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c TIMEOUT 2");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, count, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	/* Loop until all data are received */
	uint8_t i = 0;
	while (count) {
		/* Wait until RXNE flag is set */
		while (I2C_GetFlagStatus(I2Cx, I2C_ISR_RXNE) == RESET) {
			if ((timeout--) == 0) {
				serial_println("## i2c TIMEOUT 3");
				return ERROR;
			}
		}

		/* Read data from RXDR */
		dest[i++] = I2C_ReceiveData(I2Cx);

		/* Decrement the read bytes counter */
		count--;
	}

	/* Wait until STOPF flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_STOPF) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c TIMEOUT 4");
			return ERROR;
		}
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(I2Cx, I2C_ICR_STOPCF);

	return SUCCESS;
}

/**
 * @param deviceAddress
 * @param registerAddress
 * @param data
 */
ErrorStatus i2c_writeByte(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t registerAddress, uint8_t data) {
	uint16_t timeout = I2C_TIMEOUT;

	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_BUSY) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wr BUSY");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS (TX Interrupt Status) flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TXIS) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wr TIMEOUT 1");
			return ERROR;
		}
	}

	/* Send Register address */
	I2C_SendData(I2Cx, (uint8_t) registerAddress);

	/* Wait until TCR flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TCR) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wr TIMEOUT 2");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_AutoEnd_Mode, I2C_No_StartStop);

	/* Wait until TXIS flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TXIS) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wr TIMEOUT 3");
			return ERROR;
		}
	}

	/* Write data to TXDR */
	I2C_SendData(I2Cx, data);

	/* Wait until STOPF flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_STOPF) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c TIMEOUT 4");
			return ERROR;
		}
	}

	//Clear the stop flag for the next potential transfer
	I2C_ClearFlag(I2Cx, I2C_ISR_STOPF);

	return SUCCESS;
}

/**
 * @param deviceAddress
 * @param command
 */
ErrorStatus i2c_writeCommand(I2C_TypeDef* I2Cx, uint8_t deviceAddress, uint8_t command) {
	uint16_t timeout = I2C_TIMEOUT;

	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_BUSY) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wc BUSY");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

	/* Wait until TXIS (TX Interrupt Status) flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TXIS) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wc TIMEOUT 1");
			return ERROR;
		}
	}

	/* Send command */
	I2C_SendData(I2Cx, (uint8_t) command);

	/* Wait until TCR flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TCR) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wc TIMEOUT 2");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 0, I2C_AutoEnd_Mode, I2C_Generate_Stop);

	/* Wait until TXIS flag is set */
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_TXIS) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-wr TIMEOUT 3");
			return ERROR;
		}
	}

	//Clear the stop flag for the next potential transfer
	I2C_ClearFlag(I2Cx, I2C_ISR_STOPF);

	return SUCCESS;
}

uint8_t i2c_readCommandResponse(I2C_TypeDef* I2Cx, uint8_t deviceAddress) {
	uint8_t theByte = 0xFF;

	/* Wait until I2C isn't busy */
	uint16_t timeout = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_BUSY) == SET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-rcr BUSY");
			return ERROR;
		}
	}

	/* Configure slave address, nbytes, reload, end mode and start or stop generation */
	I2C_TransferHandling(I2Cx, deviceAddress, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	/* Wait until RXNE flag is set */
	timeout = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_RXNE) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-rcr TIMEOUT 1");
			return ERROR;
		}
	}

	/* Read data from RXDR */
	theByte = I2C_ReceiveData(I2Cx);

	/* Wait until STOPF flag is set */
	timeout = I2C_TIMEOUT;
	while (I2C_GetFlagStatus(I2Cx, I2C_ISR_STOPF) == RESET) {
		if ((timeout--) == 0) {
			serial_println("## i2c-rcr TIMEOUT 2");
			return ERROR;
		}
	}

	/* Clear STOPF flag */
	I2C_ClearFlag(I2Cx, I2C_ICR_STOPCF);

	return theByte;
}

void i2c_printFlags(I2C_TypeDef* I2Cx) {
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == SET) {
		serial_print("I2C_FLAG_TXIS ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_RXNE) == SET) {
		serial_print("I2C_FLAG_RXNE ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) == SET) {
		serial_print("I2C_FLAG_ADDR ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_NACKF) == SET) {
		serial_print("I2C_FLAG_NACKF ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == SET) {
		serial_print("I2C_FLAG_STOPF ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC) == SET) {
		serial_print("I2C_FLAG_TC ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TCR) == SET) {
		serial_print("I2C_FLAG_TCR ");
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
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_ALERT) == SET) {
		serial_print("I2C_FLAG_ALERT ");
	}
	if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY) == SET) {
		serial_print("I2C_FLAG_BUSY ");
	}
}
