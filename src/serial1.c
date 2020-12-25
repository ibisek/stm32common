/*
 * serial1.c
 *
 * @version 2017-01-11 STM32
 * @author 	ibisek
 */

#include <serial1.h>

#ifdef STM32F10x
/**
 * Initialises USART1 for STM32F103 on pins PA10 (RX) and PA9 (TX).
 */
void _serial1_init_f10x(uint16_t baudRate, uint8_t enableRxInt) {
	// enable GPIO and USART1 clocks:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// alternate function UART:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);// alternate function UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);// alternate function UART

	// configure USART TX as alternate function push-pull:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// configure USART RX as input floating:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART configuration:
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART1, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART1 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// enable RX interrupt
	}

	USART_Cmd(USART1, ENABLE);	// enable USART
}
#endif

#ifdef STM32F042
/**
 * Initialises USART1 for STM32F042 on pins PA10 (RX) and PA9 (TX).
 */
void _serial1_init_f042(uint16_t baudRate, uint8_t enableRxInt) {
	// Enable clocks:
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	// Enable USART clock:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// alternate function UART:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);// alternate function UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);// alternate function UART

	/* Configure USART1 pins:  Rx and Tx ----------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);	// use default init for 9600
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART1, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART1 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// enable RX interrupt

		//NVIC_EnableIRQ(USART1_IRQn);	// enable USART1 global interrupt
	}

	USART_Cmd(USART1, ENABLE);
}
#endif

#ifdef STM32L1
/**
 * Initialises USART1 for STM32L1xx on pins PA10 (RX) and PA9 (TX).
 */
void _serial1_init_l1xx(uint16_t baudRate, uint8_t enableRxInt) {
	// enable GPIO and USART1 clocks:
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// alternate function UART:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);// alternate function UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);// alternate function UART


	// configure USART TX as alternate function push-pull:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// configure USART RX as input floating:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART configuration:
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART1, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART1 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	// enable RX interrupt
	}

	USART_Cmd(USART1, ENABLE);	// enable USART
}
#endif

/**
 * Initialises USART1 for STM32F303 on pins PA10 (RX) and PA9 (TX).
 * @param baudRate
 * @enableRxInt
 */
void serial1_init(uint16_t baudRate, uint8_t enableRxInt) {
#ifdef STM32F042
	_serial1_init_f042(baudRate, enableRxInt);
#else
#ifdef STM32F10x
	_serial1_init_f10x(baudRate, enableRxInt);
#else
#ifdef STM32L1
	_serial1_init_l1xx(baudRate, enableRxInt);
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
}

void serial1_print_char(char c) {
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET)
		;	// wait until we can transmit next character
	USART_SendData(USART1, c);
}

void serial1_print(const char *msg) {
	uint8_t i = 0;
	while (msg[i] != 0) {
		serial1_print_char(msg[i++]);
	}
}

void serial1_println(const char *msg) {
	serial1_print(msg);
	serial1_print_char('\n');
}

/**
 * Prints uint value in DEC.
 */
void serial1_print_uint(uint8_t i) {
	uint8_t printZeros = 0;
	uint8_t order = CONST_10_2;

	while (1) {
		uint8_t x = i / order;

		if (x > 0 || printZeros) {
			serial1_print_char(0x30 | (x & 0x0F));
			//i -= x * order;
			i = i % order;
			printZeros = 1;
		}

		order /= 10;
		if (order == 1) break;
	}

	serial1_print_char(0x30 | (i & 0x0F));
}

/**
 * Prints int value in DEC.
 */
void serial1_print_int(int8_t i) {
	if ((i & 0x80) != 0) { 	// top bit
		serial1_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	serial1_print_uint(i);
}

/**
 * A helper function to print uint8,16,32 and decimals for double padded with zeros
 * @param i integer to print
 * @param zeroPadding pad with leading zeros?
 * @param startOrder - 100 for uint8, 10000 for uint16, 10^10 for uint32
 */
void _serial1_print_uint32(uint32_t i, uint8_t zeroPadding, uint32_t startOrder) {
	uint8_t printZeros = zeroPadding;
	uint32_t order = startOrder;

	while (1) {
		if (order == 1) break;

		uint32_t x = i / order;

		if (x > 0 || printZeros) {
			serial1_print_char(0x30 | (x & 0x0F));
			i = i % order;
			printZeros = 1;
		}

		order /= 10;
	}

	serial1_print_char(0x30 | (i & 0x0F));
}

/**
 * Prints uint16 value in DEC.
 */
void serial1_print_uint16(uint16_t i) {
	_serial1_print_uint32(i, 0, CONST_10_5);
}

/**
 * Prints int16 value in DEC.
 */
void serial1_print_int16(int16_t i) {
	if ((i & 0x8000) != 0) {	// top bit
		serial1_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	serial1_print_uint16(i);
}

void serial1_print_uint32(uint32_t i) {
	_serial1_print_uint32((uint32_t) i, 0, CONST_10_9);
}

void serial1_print_int32(int32_t i) {
	if ((i & 0x80000000) != 0) {	// top bit
		serial1_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	_serial1_print_uint32((uint32_t) i, 0, CONST_10_9);
}

void serial1_print_double(double d, uint8_t decimals) {
	if (d < 0) {
		serial1_print("-");
		d *= -1;
	}

	// integer part:
	uint32_t i = (uint32_t) d;
	d -= i;
	serial1_print_int32(i);

	if (decimals == 0) return;

	serial1_print(".");

	if (decimals > 4) decimals = 4; // more will not fit into uint16

	// decimal part:
	uint16_t order = _pow(10, decimals);
	d = d * order;
	i = (uint16_t) d;

	// This is a helper to print double values correctly.
	// (there was a problem with printing value of 1.0123 where the zero was omitted and the number was printed as 1.123)
	_serial1_print_uint32(i, 1, order / 10); // we want n-decimal places, but order -1
}

/**
 * Prints int8 value in binary form.
 */
void serial1_print_bin(uint8_t value) {
	uint8_t checkBit = 0x80;
	for (uint8_t i = 0; i < 8; i++) {

		if (value & checkBit)
			serial1_print("1");
		else
			serial1_print("0");

		checkBit = checkBit >> 1;
	}
}

/**
 * Prints 0-F
 * @param val 0-15
 */
void _serial1_print_hexHalfValue(uint8_t val) {
	if (val >= 10) {
		serial1_print_char('A' - 10 + val);
	} else {
		serial1_print_char(0x30 | val);
	}
}

/**
 * Prints int8 value in HEX.
 */
void serial1_print_hex(uint8_t i) {
	uint8_t val = i >> 4;
	_serial1_print_hexHalfValue(val);

	val = i & 0x0F;
	_serial1_print_hexHalfValue(val);
}

/**
 * Prints uint16 value in HEX.
 */
void serial1_print_hex16(uint16_t i) {
	serial1_print_hex((uint8_t) ((i >> 8) & 0xFF));
	serial1_print_hex((uint8_t) (i & 0xFF));
}

/**
 * Prints uint32 value in HEX.
 */
void serial1_print_hex32(uint32_t i) {
	serial1_print_hex((uint8_t) ((i >> 24) & 0xFF));
	serial1_print_hex((uint8_t) ((i >> 16) & 0xFF));
	serial1_print_hex((uint8_t) ((i >> 8) & 0xFF));
	serial1_print_hex((uint8_t) (i & 0xFF));
}

void serial1_println_arr_int(const char *msg, int i) {
	serial1_print(msg);
	serial1_print_int(i);
	serial1_print_char('\n');
}

void serial1_println_arr_int16(const char *msg, int16_t i) {
	serial1_print(msg);
	serial1_print_int16(i);
	serial1_print_char('\n');
}

void serial1_println_arr_double(const char *msg, double d, uint8_t decimals) {
	serial1_print(msg);
	serial1_print_double(d, decimals);
	serial1_print_char('\n');
}

void serial1_println_arr_hex(const char *msg, int i) {
	serial1_print(msg);
	serial1_print_hex(i);
	serial1_print_char('\n');
}

void serial1_println_arr_hex16(const char *msg, int16_t i) {
	serial1_print(msg);
	serial1_print_hex16(i);
	serial1_print_char('\n');
}

void serial1_print_arr_limited(const char *msg, char*data, int numDataChars) {
	serial1_print(msg);
	for (uint8_t i = 0; i < numDataChars; i++) {
		if (data[i] != 0) serial1_print_char(data[i]);
	}
}
