/*
 * Serial.h
 *
 * @author ibisek
 * @version 2017-07-26
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#ifdef STM32F042
#include "stm32f0xx.h"
#else
#ifdef STM32F10x
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#if defined(STM32F10x) && defined(SERIAL2_WITH_DMA)
#include "serialDma.h"
#endif
#else
#ifdef STM32F030xC
#include "stm32f0xx.h"
#else
#ifdef STM32L1
#include "stm32l1xx.h"
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
#endif


//-----------------------------------------------------
//										PRIVATE
//-----------------------------------------------------

#define CONST_10_2 100
#define CONST_10_5 10000
#define CONST_10_9 1000000000

double _pow(uint8_t x, uint8_t y);
void _serial_print_uint32(uint32_t i, uint8_t zeroPadding, uint32_t startOrder);
void _serial_init_f042(uint32_t baudRate, uint8_t enableRxInt);
void _serial_init_f103(uint32_t baudRate, uint8_t enableRxInt);

//-----------------------------------------------------
//										PUBLIC
//-----------------------------------------------------

void serial_init(uint32_t baudRate, uint8_t enableRxInt);
void serial_stop();

//#if defined(STM32F10x) && defined(SERIAL2_WITH_DMA)
void serial_initDmaRx(char* rxBuffer, uint8_t rxBufferSize);
void serial_initDmaTx();
void serial_printDma(char* buffer, uint8_t dataLen);
void serial_printfDma(const char *fmt, ...);
//#endif

void serial_print_char(const char c);
void serial_print(const char *msg);
void serial_println(const char *msg);

void serial_print_uint(uint8_t i);
void serial_print_int(int8_t i);
void serial_print_uint16(uint16_t i);
void serial_print_int16(int16_t i);
void serial_print_uint32(uint32_t i);
void serial_print_int32(int32_t i);
void serial_print_bin(uint8_t i);
void serial_print_hex(uint8_t i);
void serial_print_hex16(uint16_t i);
void serial_print_hex32(uint32_t i);
void serial_print_double(double d, uint8_t decimals);

void serial_println_arr_int(const char *msg, int i);
void serial_println_arr_int16(const char *msg, int16_t i);
void serial_println_arr_double(const char *msg, double d, uint8_t decimals);
void serial_println_arr_hex(const char *msg, int i);
void serial_println_arr_hex16(const char *msg, int16_t i);
void serial_print_arr_limited(const char *msg, char*data, int numDataChars);
void serial_print_buf_limited(char* buf, uint16_t length);

void serial_print_test();

#endif /* SERIAL_H_ */
