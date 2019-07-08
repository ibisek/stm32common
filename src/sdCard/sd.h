/*
 * sd.h
 *
 *  Created on: Sep 4, 2018
 *      Author: ibisek
 */

#ifndef SDCARD_SD_H_
#define SDCARD_SD_H_

//#include <stddef.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

#include "ff.h"
#include "diskio.h"

#include "serial.h"
#include "../../src/watchdog.h"

//--------------------------------------------------
//					TIMER stuff
//--------------------------------------------------

extern "C" void TIM2_IRQHandler(void);	// C++ stuff necessity

void TIM2_IRQHandler() {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	// clear the flag!!

//		GPIO_WriteBit(GPIOA, GPIO_Pin_15, !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15));

		disk_timerproc();
	}
}

static void sd__init_timer() {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	// 32MHz / 10k = 3.2kHz / 32 = 10ms interval

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 10000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 32;	// = counts up to this value and raises an interrupt
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

//--------------------------------------------------
//					SD CARD OPERATIONS
//--------------------------------------------------

FATFS fs;


DSTATUS sd_init() {
//	spi_initF103parametric(SPI1, GPIOB, GPIO_Pin_3);	//OGN2 SD CS is at PB3 .. toto zpusobuje na OGN2 nejake divne blikani na PA15 (SPI1_SS pin)
	init_pin_out(GPIOB, GPIO_Pin_3);	//OGN2 SD CS is at PB3
	sd__init_timer();
	return disk_initialize(0);
}

void sd_deinit() {	// stops the timer and associated interrupt code
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);	// not needed, right?
}

/**
 * @return true if given file exists
 */
bool sd_fileExists(const char *filename) {
	FIL file;
	FRESULT res = f_open(&file, filename, FA_READ);
	if (res != FR_OK)	// file does not exist
		return false;

	f_close(&file);

	return true;
}

/**
 * Appends string to a file. If such file does not exists, it is created.
 */
FRESULT sd_appendToFile(const char *filename, char* str, uint16_t strLen) {
	kickTheDog();

	FRESULT res;
	FIL file;
	UINT len;

	res = f_open(&file, filename, FA_WRITE);
	if (res == FR_NO_FILE)	// file does not exist
		res = f_open(&file, filename, FA_WRITE | FA_CREATE_NEW);
	else if(res == FR_NOT_READY)
		return res;

	if(file.fsize > 0) f_lseek(&file, file.fsize);

	res = f_write(&file, str, strLen, &len);
	if (res)
		return res;

	return f_close(&file);
}

//FRESULT sd_dumpFileHex(const char *filename) {
//	FRESULT res;
//	FIL file;
//
//	res = f_open(&file, filename, FA_READ);
//	if (res == FR_NO_FILE || res == FR_NOT_READY)	// file does not exist or .. some other error
//		return res;
//
//	const UINT BUF_SIZE = 2048;
//	char buf[BUF_SIZE];
//	UINT numRead = 0;
//
//	while(true) {
//		FRESULT res = f_read(&file, (void*)buf, BUF_SIZE, &numRead);
//		if (res == FR_OK) {
//			if(numRead == 0) break;
//
//			for(UINT i=0; i< numRead; i++) serial_print_hex(buf[i]);
//
//		} else {
//			return res;
//		}
//	}
//
//	serial_print_char('\n');
//
//	return f_close(&file);
//}

/* 15-9: Year(0-127 org.1980), 8-5: Month(1-12), 4-0: Day(1-31) */
void sd__printDate(WORD date) {
	uint8_t year = ((date >> 9) & 0b01111111) - 20;
	uint8_t month = (date >> 5) & 0b00011111;
	uint8_t day = date & 0b00011111;

	serial_print("20");
	serial_print_uint(year);
	serial_print_char('-');
	serial_print_uint(month);
	serial_print_char('-');
	serial_print_uint(day);
}

/* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
void sd__printTime(WORD time) {
	uint8_t hour = (time >> 11) & 0b00011111;
	uint8_t min = (time >> 5) & 0b00111111;
	uint8_t sec = time & 0b00011111;

	serial_print_uint(hour);
	serial_print_char(':');
	serial_print_uint(min);
	serial_print_char(':');
	serial_print_uint(sec);
}

FRESULT sd_listDir(const char *path) {
	kickTheDog();

	DIR dir;
	FILINFO info;
	FRESULT res;

	res = f_opendir(&dir, path);
	if(res) return res;

	serial_print("Files:\n");
	res = f_readdir(&dir, &info);
	while(res == FR_OK) {
		serial_print(" ");
		serial_print_uint32(info.fsize);
		serial_print("B ");
		serial_print(info.fname);
		serial_print_char(' ');
		sd__printDate(info.fdate);
		serial_print_char(' ');
		sd__printTime(info.ftime);
		serial_print_char('\n');

		res = f_readdir(&dir, &info);
		if(info.fname[0] == 0) break;
	}

	return FR_OK;
}

/**
 * @param path "0:"
 * @return list of files in form "file1.txt;512\nfile2.txt;1024\n.."
 */
FRESULT sd_listDirCsv(const char *path) {
	kickTheDog();

	DIR dir;
	FILINFO info;
	FRESULT res;

	res = f_opendir(&dir, path);
	if(res) return res;

	res = f_readdir(&dir, &info);
	while(res == FR_OK) {
		serial_print(info.fname);
		serial_print(";");
		serial_print_uint32(info.fsize);
		serial_print_char('\n');

		res = f_readdir(&dir, &info);
		if(info.fname[0] == 0) break;
	}

	return FR_OK;
}

/**
 * 15-9: Year(0-127 org.1980), 8-5: Month(1-12), 4-0: Day(1-31)
 * @param year current - 1980
 * @param mont
 * @param day
 */
WORD sd__convertDate(uint8_t year, uint8_t month, uint8_t day) {
	WORD retVal = ((WORD)(year & 0b01111111)) << 9;
	retVal |= ((WORD)(month & 0b00011111)) << 5;
	retVal |= day & 0b00011111;

	return retVal;
}

/**
 * 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2)
 * @param hour
 * @param min
 * @param sec
 */
WORD sd__convertTime(uint8_t hour, uint8_t min, uint8_t sec) {
	WORD retVal = ((WORD)(hour & 0b00011111)) << 11;
	retVal |= ((WORD)(min & 0b00011111)) << 5;
	retVal |= (sec/2U) & 0b00011111;	// just five bits must also accommodate seconds > 32 (!) (just 2 seconds "resolution")

	return retVal;
}

/**
 * @param filename
 * @year current - 1980
 * @month
 * @day
 * @hour
 * @min
 * @sec
 */
FRESULT sd_setDateTime(const char* filename, uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	FILINFO info;
	FRESULT res = f_stat(filename, &info);

	if(res == FR_OK) {
		info.fdate = sd__convertDate(year, month, day);
		info.ftime = sd__convertTime(hour, min, sec);
		res = f_utime(filename, &info);
	}

	return res;
}

FRESULT sd_mount() {
	return f_mount(0, &fs);
}

FRESULT sd_umount() {
	return f_mount(0, NULL);
}

extern DSTATUS cardStatus;
uint8_t sd_cardPresent() {
	return disk_sdCardPresent();
}

FRESULT sd_getFreeSpace(uint32_t *freeKB) {

	DWORD fre_clust, fre_sect; //, tot_sect;

	FATFS* ffss = &fs;
	/* Get volume information and free clusters of drive 1 */
	FRESULT res = f_getfree("0:", &fre_clust, &ffss);
	if(res) return res;

	/* Get total sectors and free sectors */
	//tot_sect = (fs.sects_fat - 2) * fs.csize;
	fre_sect = fre_clust * fs.csize;

	*freeKB = fre_sect / 2;	// [kB]	(assuming 512 bytes/sector)

	/* Print the free space (assuming 512 bytes/sector) */
	//char buf[128];
	//sprintf(buf, "%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
	//serial_print_buf_limited(buf, strlen(buf));

	return res;
}

#endif /* SDCARD_SD_H_ */
