/**
 * Tasker - a cooperative task scheduler for STM32
 *
 * Port from original Tasker for Arduino from Petr Stehlik  petr@pstehlik.cz
 * Distributed under the GNU LGPL http://www.gnu.org/licenses/lgpl.txt
 *
 * @author ibisek
 * @version 2017-01-10
 */

#include <tasker.h>

typedef struct {
	TaskCallback call;
	uint8_t param;

	// !! musi byt int a ne uint, protoze pak by v loopu nevysel zaporny rozdil a delalo by to blbiny !!
	// (nicmene takto vychazi maximalni rozsah jen na 2,147,483,647ms ~ 24dni)
	int32_t interval;
	int32_t lastRun;

	uint8_t repeat;
} TASK;

TASK tasks[TASKER_MAX_TASKS];

uint8_t t_count;
uint8_t t_prioritized = 1;

void tasker_init(uint8_t prioritized) {
	t_count = 0;
	t_prioritized = prioritized;

	ibitime_init();
}

int8_t tasker_setRepeated(TaskCallback func, int32_t interval, uint8_t repeat, uint8_t param) {
	if (t_count >= TASKER_MAX_TASKS || interval == 0) return 0;

	TASK *t = &tasks[t_count];
	t->call = func;
	t->interval = interval;
	t->param = param;
	t->lastRun = ibitime_millis();
	t->repeat = repeat;
	t_count++;

	return 1;
}

/**
 * @param interval must be >= 1
 */
int8_t tasker_setTimeout(TaskCallback func, int32_t interval, uint8_t param) {
	return tasker_setRepeated(func, interval, 1, param);
}

int8_t tasker_setInterval(TaskCallback func, int32_t interval, uint8_t param) {
	return tasker_setRepeated(func, interval, 0, param);
}

void tasker_loop(void) {
	int8_t t_idx = 0;
	int32_t now = ibitime_millis();
	while (t_idx < t_count) {
		int8_t inc = 1;
		TASK *t = &tasks[t_idx];
		if (now - t->lastRun >= t->interval) {
			t->lastRun += t->interval;
			(*(t->call))(t->param);
			if (t->repeat > 0 && --t->repeat == 0) {
				// drop the finished task by removing its slot
				memmove(tasks + t_idx, tasks + t_idx + 1, sizeof(TASK) * (t_count - t_idx - 1));
				t_count--;
				inc = 0;
			}
			if (t_prioritized) break;
			now = ibitime_millis();
		}
		if (inc) t_idx++;
	}
}

void run(void) {
	while (1)
		tasker_loop();
}

void tasker_changeInterval(TaskCallback func, int32_t interval) {
	for (uint8_t i = 0; i < t_count; i++) {
		TASK *t = &tasks[i];
		if (t->call == func) {
			t->interval = interval;
			return;
		}
	}
}

void tasker_resetCountdown(TaskCallback func) {
	for (uint8_t i = 0; i < t_count; i++) {
		TASK *t = &tasks[i];
		if (t->call == func) {
			t->lastRun = ibitime_millis();
			return;
		}
	}
}

void tasker_cancel(TaskCallback func) {
	for (uint8_t i = 0; i < t_count; i++) {
		TASK *t = &tasks[i];

		if (t->call == func) {
			// drop the this task by removing its slot
			memmove(tasks + i, tasks + i + 1, sizeof(TASK) * (t_count - i - 1));
			t_count--;

			return;
		}
	}
}

uint8_t tasker_isScheduled(TaskCallback func) {
	for (uint8_t i = 0; i < t_count; i++) {
			TASK *t = &tasks[i];

			if (t->call == func) {
				return 1;
			}
		}

	return 0;
}

