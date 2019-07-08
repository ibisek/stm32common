/*
 * gpioCommon.c
 *
 *  Created on: Nov 2, 2016
 *      Author: ibisek
 */

#include <gpioCommon.h>


void init_pin_out(GPIO_TypeDef* GPIOx, uint32_t gpioPin) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if defined(STM32F030xC) || defined(STM32F042)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	// push-pull (PP) or open-drain (OD)
#endif
#ifdef STM32F10x
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
#endif

	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void init_pin_in(GPIO_TypeDef* GPIOx, uint32_t gpioPin) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if defined(STM32F030xC) || defined(STM32F042)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
#ifdef STM32F10x
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif

	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

#ifdef STM32F042
/**
 * @param GPIOx
 * @param gpioPin
 * @param upDown GPIO_PuPd_NOPULL | GPIO_PuPd_UP | GPIO_PuPd_DOWN
 */
void init_pin_in_pushPull(GPIO_TypeDef* GPIOx, uint32_t gpioPin, GPIOPuPd_TypeDef upDown) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if defined(STM32F030xC) || defined(STM32F042)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = upDown;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	// push-pull (PP) or open-drain (OD)
#endif

	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
#endif

#ifdef STM32F10x
/**
 * @param GPIOx
 * @param gpioPin
 * @param upDown GPIO_Mode_IN_FLOATING | GPIO_Mode_IPD | GPIO_Mode_IPU
 */
void init_pin_in_pushPull(GPIO_TypeDef* GPIOx, uint32_t gpioPin, GPIOMode_TypeDef upDown) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Mode = upDown;

	GPIO_Init(GPIOx, &GPIO_InitStructure);
}
#endif


void init_pin_analog(GPIO_TypeDef* GPIOx, uint32_t gpioPin) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = gpioPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if defined(STM32F030xC) || defined(STM32F042)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
#ifdef STM32F10x
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
#endif

	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

#ifdef STM32F10x
/**
 * Disables JTAG in order PB3, PB4 and PA15 could be used as GPIO.
 * (for STM32F103)
 */
void disableJTAG() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	// JTAG-DP Disabled and SW-DP Enabled
}
#endif
