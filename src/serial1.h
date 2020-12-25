/*
 * Serial1.h
 *
 * @author ibisek
 * @version 2017-01-11 STM32
 */

#ifndef SERIAL1_H_
#define SERIAL1_H_

#if defined STM32F0 || STM32F042
#include "stm32f0xx.h"
#else
#ifdef STM32F10x
#include "stm32f10x.h"
#else
#ifdef STM32L1
#include "stm32l1xx.h"
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif

#include "serial.h"

//-----------------------------------------------------
//										PRIVATE
//-----------------------------------------------------

void _serial1_init_f042(uint16_t baudRate, uint8_t enableRxInt);
void _serial1_init_f103(uint16_t baudRate, uint8_t enableRxInt);

//-----------------------------------------------------
//										PUBLIC
//-----------------------------------------------------

void serial1_init(uint16_t baudRate, uint8_t enableRxInt);
void serial1_stop();

void serial1_print_char(const char c);
void serial1_print(const char *msg);
void serial1_println(const char *msg);

void serial1_print_uint(uint8_t i);
void serial1_print_int(int8_t i);
void serial1_print_uint16(uint16_t i);
void serial1_print_int16(int16_t i);
void serial1_print_uint32(uint32_t i);
void serial1_print_int32(int32_t i);
void serial1_print_bin(uint8_t i);
void serial1_print_hex(uint8_t i);
void serial1_print_hex16(uint16_t i);
void serial1_print_hex32(uint32_t i);
void serial1_print_double(double d, uint8_t decimals);

void serial1_println_arr_int(const char *msg, int i);
void serial1_println_arr_int16(const char *msg, int16_t i);
void serial1_println_arr_double(const char *msg, double d, uint8_t decimals);
void serial1_println_arr_hex(const char *msg, int i);
void serial1_println_arr_hex16(const char *msg, int16_t i);
void serial1_print_arr_limited(const char *msg, char*data, int numDataChars);

#endif /* SERIAL1_H_ */
