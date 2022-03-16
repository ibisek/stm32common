/*
 * watchdog.h
 *
 *  Created on: Feb 9, 2019
 *      Author: ibisek
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#ifdef STM32F10x
#include "stm32f10x_iwdg.h"
#else
#ifdef STM32L1
#include "stm32l1xx_iwdg.h"
#else
#error "Yet unsupported architecture"
#endif
#endif

//--------------------------------------------------
//					WATCHDOG
//--------------------------------------------------

void initWatchdog() {
	// enable access to the IWDG registers:
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	//(PCLK1 (32MHz)/4096)/8 = 977Hz  (~1 ms)
	//(PCLK1 (36MHz)/4096)/8 = 1099Hz (~909 us)
	//(PCLK1 (48MHz)/4096)/8 = 1465Hz (~683 us)
	IWDG_SetPrescaler(IWDG_Prescaler_8);

	// 683us x 15 = ~0.01s
	// 683us x 4095 = ~2.7s
	// 909us x 4095 = ~ 3.7s
	IWDG_SetReload(0xFFF); // <0; 0x0FFF>

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);

	IWDG_Enable();
}

void kickTheDog() {
	IWDG_ReloadCounter();
}

#endif /* WATCHDOG_H_ */
