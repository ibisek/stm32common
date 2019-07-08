/*
 * ibitime.h
 *
 *  Created on: Dec 6, 2016
 *      Author: ibisek
 */

#ifndef IBITIME_H_
#define IBITIME_H_

#ifdef STM32F030xC
#include "stm32f0xx.h"
#else
#ifdef STM32F042
#include <stm32f0xx_rcc.h>
#include "system_stm32f0xx.h"
#else
#ifdef STM32F10x
#include <stm32f10x_rcc.h>
#include "system_stm32f10x.h"
#else
#ifdef STM32F3
#include "stm32f30x.h"
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
#endif

#include <sys/_stdint.h>

#define SYSTEM_CORE_CLOCK 36000000

#define DELAY_TICK_FREQUENCY_US 1000000   /* = 1MHZ -> microsecond delay */
#define DELAY_TICK_FREQUENCY_MS 1000      /* = 1kHZ -> millisecond delay */

void ibitime_init();
uint32_t ibitime_millis();
void ibitime_reset();

void delay_ms(uint16_t);
//void delay_us(uint16_t);
void delay_1us();
void delay_100us();
void delay_250us();
void delay_500us();

#endif /* IBITIME_H_ */
