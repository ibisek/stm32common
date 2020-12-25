/*
 * cpuUtils.h
 *
 *  Created on: Dec 10, 2016
 *      Author: ibisek
 */

#ifndef CPUUTILS_H_
#define CPUUTILS_H_

#if defined(STM32F030xC) || defined(STM32F042)
#include <stm32f0xx_flash.h>
#include <stm32f0xx_rcc.h>
#include <system_stm32f0xx.h>
#else
#ifdef STM32F10x
#include <stm32f10x_flash.h>
#include <stm32f10x_rcc.h>
#include <system_stm32f10x.h>
#else
#ifdef STM32L1
#include <stm32l1xx_flash.h>
#include <stm32l1xx_rcc.h>
#include <system_stm32l1xx.h>
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif

void init8MHzClock() {
	FLASH_SetLatency(FLASH_Latency_1);

//	#ifdef STM32F042
//	// Set up 8 MHz Core Clock using HSI (8Mhz)
//	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_1);	// TODO zkontrolovat!!
//	#endif
	#ifdef STM32F10x
	// Set up 8 MHz Core Clock using HSI (8Mhz/2) x 2 = 8MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_2);
	#endif

	RCC_PLLCmd(ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}

void init16MHzClock() {
	FLASH_SetLatency(FLASH_Latency_1);

	#if defined(STM32F030xC) || defined(STM32F042)
	// Set up 8 MHz Core Clock using HSI (8Mhz)
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_2);	// TODO zkontrolovat!!
	#endif
	#ifdef STM32F10x
	// Set up 8 MHz Core Clock using HSI (8Mhz/2) x 4 = 16MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_4);
	#endif

	RCC_PLLCmd(ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}

void init24MHzClock() {
	FLASH_SetLatency(FLASH_Latency_1);

	#if defined(STM32F030xC) || defined(STM32F042)
	// Set up 24 MHz Core Clock using HSI (8Mhz)
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_3);
	#endif
	#ifdef STM32F10x
	// Set up 24 MHz Core Clock using HSI (8Mhz/2) x 6 = 24MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6);
	#endif

	RCC_PLLCmd(ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}

void init32MHzClock() {
	FLASH_SetLatency (FLASH_Latency_1);

	#if defined(STM32F030xC) || defined(STM32F042)
	// Set up 32 MHz Core Clock using HSI (8Mhz) with PLL x 4
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_4);
	#endif
	#ifdef STM32F10x
	// Set up 32 MHz Core Clock using HSI (8Mhz/2) x 8 = 32MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_8);
	#endif
	#ifdef STM32L1
	// Set up 32 MHz Core Clock using HSI (16Mhz/2) x 4 = 32MHz
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_4, RCC_PLLDiv_2);
	#endif

	RCC_PLLCmd (ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}

#ifdef STM32F10x
void init36MHzClock() {
	FLASH_SetLatency (FLASH_Latency_1);

	// Set up 36 MHz Core Clock using HSI (8Mhz/2) x 9 = 36MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);

	RCC_PLLCmd (ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}
#endif

void init48MHzClock() {
	FLASH_SetLatency (FLASH_Latency_1);

	#if defined(STM32F030xC) || defined(STM32F042)
	// Set up 48 MHz Core Clock using HSI (8Mhz) with PLL x 6
	RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_6);
	#endif
	#ifdef STM32F10x
	// Set up 48 MHz Core Clock using HSI (8Mhz/2) x 12 = 48MHz
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
	#endif

	RCC_PLLCmd (ENABLE);

	// Wait for PLLRDY after enabling PLL.
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != SET) {
	}

	RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK);  // Select the PLL as clock source.
	SystemCoreClockUpdate();
}

#endif /* CPUUTILS_H_ */
