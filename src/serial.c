/*
 * serial.c
 *
 * @version 2017-07-26
 * @author 	ibisek
 */

#include "serial.h"

#ifdef SERIAL2_WITH_DMA
#include "serialDma.h"
#include <stdarg.h>
#include <stdio.h>
#endif

#ifdef STM32F10x

/**
 * Initialises USART2 for STM32F103 on pins PA3 (RX) and PA2 (TX).
 */
void _serial_init_f10x(uint32_t baudRate, uint8_t enableRxInt) {
	USART_Cmd(USART2, DISABLE);	// disable USART

	// enable GPIO and USART2 clocks:
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// configure USART TX as alternate function push-pull:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// configure USART RX as input floating:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART configuration:
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART2, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART2 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	// enable RX interrupt
	}

	USART_Cmd(USART2, ENABLE);	// enable USART
}
#endif

#if defined(SERIAL2_WITH_DMA)
/**
 * Initializes RX interrupt and DMA controller. Requires DMA1_Channel6_IRQHandler to be implemented.
 * @param rxBuffer
 * @param rxBufferSize
 */
void serial_initDmaRx(char* rxBuffer, uint8_t rxBufferSize) {
	serialDma::init_serial2DmaInterrupt(1, 0);
	serialDma::init_serial2DmaRx(rxBuffer, rxBufferSize);
}

/**
 * Initializes TX interrupt and DMA controller. Requires DMA1_Channel7_IRQHandler to be implemented.
 */
void serial_initDmaTx() {
	serialDma::init_serial2DmaInterrupt(0, 1);
	serialDma::init_serial2DmaTx();
}

/**
 * Initiates DMA transfer from given buffer to UART2. Requires DMA1_Channel7_IRQHandler to be implemented.
 * @param buffer
 * @param dataLen
 */
void serial_printDma(char* buffer, uint8_t dataLen) {
	serialDma::serial2StartDmaTx(buffer, dataLen);
}

void serial_printfDma(const char *fmt, ...) {
	char buf[256];
	memset(buf, 0, sizeof(buf));
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	serial_printDma(buf, strlen(buf));
	va_end(args);
}
#endif

#ifdef STM32F042
/**
 * Initialises USART2 for STM32F042 on pins PA3 (RX) and PA2 (TX).
 */
void _serial_init_f042(uint32_t baudRate, uint8_t enableRxInt) {
	// Enable clocks:
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	// Enable USART clock:
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// alternate function UART:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);// alternate function UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);// alternate function UART

	/* Configure USART2 pins:  Rx and Tx ----------------------------*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);	// use default init for 9600
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART2, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART2 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	// enable RX interrupt

		//NVIC_EnableIRQ(USART2_IRQn);	// enable USART2 global interrupt
	}

	USART_Cmd(USART2, ENABLE);
}
#endif

#ifdef STM32L1
/**
 * Initialises USART2 for STM32L1xx on pins PA3 (RX) and PA2 (TX).
 */
void _serial_init_l1xx(uint32_t baudRate, uint8_t enableRxInt) {
	// enable GPIO and USART2 clocks:
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// alternate function UART:
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);// alternate function UART
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);// alternate function UART

	// configure USART TX as alternate function push-pull:
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// configure USART RX as input floating:
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART configuration:
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	if (baudRate != 9600) USART_InitStructure.USART_BaudRate = baudRate;
	USART_Init(USART2, &USART_InitStructure);

	if (enableRxInt == 1) { /* Enable USART2 IRQ */
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	// enable RX interrupt
	}

	USART_Cmd(USART2, ENABLE);	// enable USART
}
#endif

/**
 * Initialises USART2 for STM32F303 on pins PA3 (RX) and PA2 (TX).
 * @param baudRate
 * @enableRxInt
 */
void serial_init(uint32_t baudRate, uint8_t enableRxInt) {
#ifdef STM32F042
	_serial_init_f042(baudRate, enableRxInt);
#else
#ifdef STM32F10x
	_serial_init_f10x(baudRate, enableRxInt);
#else
#ifdef STM32L1
	_serial_init_l1xx(baudRate, enableRxInt);
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
}

void serial_print_char(char c) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET)
		;	// wait until we can transmit next character
	USART_SendData(USART2, c);
}

void serial_print(const char *msg) {
	uint8_t i = 0;
	while (msg[i] != 0) {
		serial_print_char(msg[i++]);
	}
}

void serial_println(const char *msg) {
	serial_print(msg);
	serial_print_char('\n');
}

/**
 * Prints uint value in DEC.
 */
void serial_print_uint(uint8_t i) {
	uint8_t printZeros = 0;
	uint8_t order = CONST_10_2;

	while (1) {
		uint8_t x = i / order;

		if (x > 0 || printZeros) {
			serial_print_char(0x30 | (x & 0x0F));
			//i -= x * order;
			i = i % order;
			printZeros = 1;
		}

		order /= 10;
		if (order == 1) break;
	}

	serial_print_char(0x30 | (i & 0x0F));
}

/**
 * Prints int value in DEC.
 */
void serial_print_int(int8_t i) {
	if ((i & 0x80) != 0) { 	// top bit
		serial_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	serial_print_uint(i);
}

/**
 * A helper function to print uint8,16,32 and decimals for double padded with zeros
 * @param i integer to print
 * @param zeroPadding pad with leading zeros?
 * @param startOrder - 100 for uint8, 10000 for uint16, 10^10 for uint32
 */
void _serial_print_uint32(uint32_t i, uint8_t zeroPadding, uint32_t startOrder) {
	uint8_t printZeros = zeroPadding;
	uint32_t order = startOrder;

	while (1) {
		if (order == 1) break;

		uint32_t x = i / order;

		if (x > 0 || printZeros) {
			serial_print_char(0x30 | (x & 0x0F));
			i = i % order;
			printZeros = 1;
		}

		order /= 10;
	}

	serial_print_char(0x30 | (i & 0x0F));
}

/**
 * Prints uint16 value in DEC.
 */
void serial_print_uint16(uint16_t i) {
	_serial_print_uint32(i, 0, CONST_10_5);
}

/**
 * Prints int16 value in DEC.
 */
void serial_print_int16(int16_t i) {
	if ((i & 0x8000) != 0) {	// top bit
		serial_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	serial_print_uint16(i);
}

void serial_print_uint32(uint32_t i) {
	_serial_print_uint32((uint32_t) i, 0, CONST_10_9);
}

void serial_print_int32(int32_t i) {
	if ((i & 0x80000000) != 0) {	// top bit
		serial_print_char('-');
		i = ~(i - 1);	// two's complement
	}

	_serial_print_uint32((uint32_t) i, 0, CONST_10_9);
}

double _pow(uint8_t x, uint8_t y) {
	if (y == 1) return x;

	double val = x;
	for (uint8_t i = 1; i < y; i++) {
		val *= x;
	}

	return val;
}

void serial_print_double(double d, uint8_t decimals) {
	if (d < 0) {
		serial_print("-");
		d *= -1;
	}

	// integer part:
	uint32_t i = (uint32_t) d;
	d -= i;
	serial_print_int32(i);

	if (decimals == 0) return;

	serial_print(".");

	if (decimals > 4) decimals = 4; // more will not fit into uint16

	// decimal part:
	uint16_t order = _pow(10, decimals);
	d = d * order;
	i = (uint16_t) d;

	// This is a helper to print double values correctly.
	// (there was a problem with printing value of 1.0123 where the zero was omitted and the number was printed as 1.123)
	_serial_print_uint32(i, 1, order / 10); // we want n-decimal places, but order -1
}

/**
 * Prints int8 value in binary form.
 */
void serial_print_bin(uint8_t value) {
	uint8_t checkBit = 0x80;
	for (uint8_t i = 0; i < 8; i++) {

		if (value & checkBit)
			serial_print("1");
		else
			serial_print("0");

		checkBit = checkBit >> 1;
	}
}

/**
 * Prints 0-F
 * @param val 0-15
 */
void _serial_print_hexHalfValue(uint8_t val) {
	if (val >= 10) {
		serial_print_char('A' - 10 + val);
	} else {
		serial_print_char(0x30 | val);
	}
}

/**
 * Prints int8 value in HEX.
 */
void serial_print_hex(uint8_t i) {
	uint8_t val = i >> 4;
	_serial_print_hexHalfValue(val);

	val = i & 0x0F;
	_serial_print_hexHalfValue(val);
}

/**
 * Prints uint16 value in HEX.
 */
void serial_print_hex16(uint16_t i) {
	serial_print_hex((uint8_t) ((i >> 8) & 0xFF));
	serial_print_hex((uint8_t) (i & 0xFF));
}

/**
 * Prints uint32 value in HEX.
 */
void serial_print_hex32(uint32_t i) {
	serial_print_hex((uint8_t) ((i >> 24) & 0xFF));
	serial_print_hex((uint8_t) ((i >> 16) & 0xFF));
	serial_print_hex((uint8_t) ((i >> 8) & 0xFF));
	serial_print_hex((uint8_t) (i & 0xFF));
}

void serial_println_arr_int(const char *msg, int i) {
	serial_print(msg);
	serial_print_int(i);
	serial_print_char('\n');
}

void serial_println_arr_int16(const char *msg, int16_t i) {
	serial_print(msg);
	serial_print_int16(i);
	serial_print_char('\n');
}

void serial_println_arr_double(const char *msg, double d, uint8_t decimals) {
	serial_print(msg);
	serial_print_double(d, decimals);
	serial_print_char('\n');
}

void serial_println_arr_hex(const char *msg, int i) {
	serial_print(msg);
	serial_print_hex(i);
	serial_print_char('\n');
}

void serial_println_arr_hex16(const char *msg, int16_t i) {
	serial_print(msg);
	serial_print_hex16(i);
	serial_print_char('\n');
}

void serial_print_arr_limited(const char *msg, char*data, int numDataChars) {
	serial_print(msg);
	serial_print_buf_limited(data, numDataChars);
}

void serial_print_buf_limited(char* buf, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		if (buf[i] != 0) serial_print_char(buf[i]);
	}
}

void serial_print_test() {
	serial_print("Serial test:\n -128=");
	serial_print_int(0x80);
	serial_print("\n -32123=");
	serial_print_int16(-32123);
	serial_print("\n 4294967295=");		// max uint32
	//serial_print_uint32(4294967295);
	serial_print("\n -2147483647=");
	serial_print_int32(-2147483647);	// max int32
	serial_print("\n -12.3456=");
	serial_print_double(-12.34567, 4);
	serial_print_char('\n');
}
