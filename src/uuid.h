/**
 * A simple header for reading the STM32 device UUID
 * Tested with STM32F4 and STM32F0 families
 *
 * Version 1.0
 * Written by Uli Koehler
 * Published on https://techoverflow.net/2015/02/03/reading-the-stm32-unique-device-id-in-c/?q=/blog/2015/02/03/reading-the-stm32-unique-device-id-in-c/
 * Licensed under CC0 (public domain):
 * https://creativecommons.org/publicdomain/zero/1.0/
 */
#ifndef __UUID_H
#define __UUID_H

/**
 * The STM32 factory-programmed UUID memory (see ref. manual, search for Unique device ID register)
 * Three values of 32 bits each starting at this address
 * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
 */
#ifdef STM32F042
#define STM32_UUID ((uint32_t *)0x1FFFF7AC)	// address for STM32F0xx
#else
#ifdef STM32F10x
#define STM32_UUID ((uint32_t *)0x1FFFF7E8)	// address for STM32F10x
#else
#ifdef STM32L1
// Base address: 0x1FF80050 for Cat.1 and Cat.2 devices and 0x1FF800D0 for Cat.3, Cat.4, Cat.5 and Cat.6 devices
// For device category see section 1.4 of the reference manual.
#define STM32_UUID ((uint32_t *)0x1FF80050)	// address for STM32L1xx
#else
#error "Yet unsupported architecture"
#endif
#endif
#endif
// F40x 0x1FFF7A10

void printCpuId(void) {
	serial_print("CPU id: ");
	serial_print_hex32(STM32_UUID[0]);
	serial_print_char(':');
	serial_print_hex32(STM32_UUID[1]);
	serial_print_char(':');
	serial_print_hex32(STM32_UUID[2]);
	serial_print_char('\n');
}

/**
 * @return the last section (CCCC) from the UUID = AAAA:BBBB:CCCC
 */
uint32_t getCpuIdSection3(void) {
	return STM32_UUID[2];
}

#endif //__UUID_H
