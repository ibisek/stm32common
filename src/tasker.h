/**
 * Tasker - a cooperative task scheduler for STM32
 *
 * Port from original Tasker for Arduino from Petr Stehlik  petr@pstehlik.cz
 * Distributed under the GNU LGPL http://www.gnu.org/licenses/lgpl.txt
 *
 * @author ibisek
 * @version 2017-01-10
 */

#ifndef TASKER_H_
#define TASKER_H_

#include <sys/_stdint.h>
#include <string.h>
#include "ibitime.h"

typedef void (*TaskCallback)(uint8_t);

#ifndef TASKER_MAX_TASKS
#define TASKER_MAX_TASKS	32	/* max 254 entries, one occupies 14 bytes of RAM */
#endif

void tasker_init(uint8_t prioritized);
int8_t tasker_setRepeated(TaskCallback func, int32_t interval, uint8_t repeat, uint8_t param);
int8_t tasker_setTimeout(TaskCallback func, int32_t interval, uint8_t param);
int8_t tasker_setInterval(TaskCallback func, int32_t interval, uint8_t param);
void tasker_loop(void);
void run(void);

void tasker_changeInterval(TaskCallback func, int32_t interval);
void tasker_resetCountdown(TaskCallback func);
void tasker_cancel(TaskCallback func);
uint8_t tasker_isScheduled(TaskCallback func);

#endif
