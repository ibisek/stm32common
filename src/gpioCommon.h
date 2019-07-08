/*
 * gpioCommon.h
 *
 *  Created on: Dec 5, 2016
 *      Author: ibisek
 */

#ifndef GPIOCOMMON_H_
#define GPIOCOMMON_H_

#ifdef STM32F030xC
#include "stm32f0xx.h"
#else
#ifdef STM32F042
#include "stm32f0xx.h"
#else
#ifdef STM32F10x
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif

#include <sys/_stdint.h>

void init_pin_out(GPIO_TypeDef* GPIOx, uint32_t gpioPin);
void init_pin_in(GPIO_TypeDef* GPIOx, uint32_t gpioPin);
void init_pin_analog(GPIO_TypeDef* GPIOx, uint32_t gpioPin);
void disableJTAG();

#ifdef STM32F042
void init_pin_in_pushPull(GPIO_TypeDef* GPIOx, uint32_t gpioPin, GPIOPuPd_TypeDef upDown);
#endif

#ifdef STM32F10x
void init_pin_in_pushPull(GPIO_TypeDef* GPIOx, uint32_t gpioPin, GPIOMode_TypeDef upDown);
#endif

#endif /* GPIOCOMMON_H_ */
