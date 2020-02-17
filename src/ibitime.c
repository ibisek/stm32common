/*
 * ibitime.c
 *
 *  Created on: Dec 6, 2016
 *      Author: ibisek
 */

#include "ibitime.h"

volatile uint32_t _millis = 0;
//volatile uint32_t _micros = 0;
//volatile uint8_t _micros8 = 0;

/**
 * For C++ the following hack is needed:
 * @see http://electronics.stackexchange.com/questions/279524/stm32-interrupts-and-c-dont-go-well-together/279528
 */
#ifdef __cplusplus
extern "C" void SysTick_Handler(void);	// C++ necessity
#endif

// SysTick interrupt handler
void SysTick_Handler() {
//	_micros += 10;
//	_micros8 += 1;
//	if (_micros8 == 100) {
//		_micros8 = 0;
//		_millis++;
//	}

	_millis++;
}

void ibitime_init() {
	SysTick_Config(SystemCoreClock / DELAY_TICK_FREQUENCY_MS);	// !! uS x 10 !! (rychleji to nefunguje)
}

uint32_t ibitime_millis() {
	return _millis;
}

void ibitime_reset() {
//	_micros = 0;
	_millis = 0;
}

void delay_ms(uint16_t delayMs) {
	uint32_t waitUntil = _millis + delayMs;
	while (_millis <= waitUntil)
		;
}

///**
// * @param delayUs in multiples of 10
// */
//void delay_us(uint16_t delayUs) {
//	//_micros = 0;	// bad thing, but overflow can really happen
//	uint32_t waitUntil = _micros + delayUs;
//	while (_micros < waitUntil)
//		;
//}

/**
 * @see pokus15timing
 */
void delay_1us() {
#if SYSTEM_CORE_CLOCK == 32000000
	asm("NOP");	// 2 nops ~ 1.0020us
	asm("NOP");

#elif SYSTEM_CORE_CLOCK == 36000000
	asm("NOP");	// 3 nops ~ ?us	TODO zmerit na osciloskopu
	asm("NOP");
	asm("NOP");

#elif SYSTEM_CORE_CLOCK == 48000000
	asm("NOP");	// 13 nops ~ 1.0020us
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");

#else
#error "SYSTEM_CORE_CLOCK not set"
#endif
}

void delay_50us() {
#if SYSTEM_CORE_CLOCK == 32000000
	for(uint16_t i=0; i<64; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 36000000
	for(uint16_t i=0; i<72; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 48000000
	for(uint16_t i=0; i<95; i++) asm("NOP");

#else
#error "SYSTEM_CORE_CLOCK not set"
#endif
}

void delay_100us() {
#if SYSTEM_CORE_CLOCK == 32000000
	for(uint16_t i=0; i<128; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 36000000
	for(uint16_t i=0; i<144; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 48000000
	for(uint16_t i=0; i<191; i++) asm("NOP");

#else
#error "SYSTEM_CORE_CLOCK not set"
#endif
}

void delay_250us() {
#if SYSTEM_CORE_CLOCK == 16000000
	for(uint16_t i=0; i<156; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 24000000
	for(uint16_t i=0; i<239; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 32000000
	for(uint16_t i=0; i<318; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 36000000
	for(uint16_t i=0; i<358; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 48000000
	for(uint16_t i=0; i<477; i++) asm("NOP");

#else
#error "SYSTEM_CORE_CLOCK not set"
#endif
}

void delay_500us() {
#if SYSTEM_CORE_CLOCK == 32000000
	for(uint16_t i=0; i<636; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 36000000
	for(uint16_t i=0; i<716; i++) asm("NOP");

#elif SYSTEM_CORE_CLOCK == 48000000
	for(uint16_t i=0; i<954; i++) asm("NOP");

#else
#error "SYSTEM_CORE_CLOCK not set"
#endif
}


