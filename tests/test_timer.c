#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/timer.h"
#include "../src/pomodoro.h"

static int tests_run = 0;

#define TEST_ASSERT(expr, msg)						\
    do {								\
	tests_run++;							\
	if(!(expr)) {							\
	    fprintf(stderr, "X [FAIL] %s:%d: %s\n,", __FILE__,__LINE__, msg); \
	    return false;						\
	} else {							\
	    printf("V [PASS] %s\n", msg);				\
	}								\
    } while(0)

bool test_timer_digits_to_seconds(void){
    int digits1[6] = {0, 1, 3, 0, 0, 0};
    TEST_ASSERT(timer_digits_to_seconds(digits1) == 5400,
		"01:30:00 should equal 5400s");

    int digits2[6] = {0, 0, 0, 5, 2, 5};
    TEST_ASSERT(timer_digits_to_seconds(digits2) == 325,
		"00:05:23 should equal 325");

    return true;
}

bool test_timer_seconds_to_digits(){
    int digits[6] = {0};
    timer_seconds_to_digits(3665, digits);
    TEST_ASSERT(digits[0] == 0 && digits[1] == 1 &&
		digits[2] == 0 && digits[3] == 1 &&
		digits[4] == 0 && digits[5] == 5,
	"3665s should convert to digits [0,1, 0,1, 0,5]");
    return true;
}

bool test_pomodoro_cycle_transitions(void){
    Pomodoro pomo;
    pomodoro_init(&pomo);

    TEST_ASSERT(pomo.phase == POMO_PHASE_FOCUS && pomo.cycle == 1,
		"Pomodoro initial state is Focus cycle 1");
    TEST_ASSERT(pomodoro_get_current_duration(&pomo) == 1500.0,
		"Initial Focus duration is 25m (1500s)");

    pomodoro_next_phase(&pomo);
    TEST_ASSERT(pomo.phase == POMO_PHASE_SHORT_BREAK && pomo.cycle == 1,
		"Cycle 1 moves to Short Break");
    pomodoro_next_phase(&pomo);
    TEST_ASSERT(pomo.phase == POMO_PHASE_FOCUS && pomo.cycle == 2,
		"Moves to Cycle 2 Focus");

    pomodoro_next_phase(&pomo);
    pomodoro_next_phase(&pomo);
    pomodoro_next_phase(&pomo);
    pomodoro_next_phase(&pomo);
    TEST_ASSERT(pomo.phase == POMO_PHASE_FOCUS && pomo.cycle == 4,
		"Reached Cycle 4 Focus");

    pomodoro_next_phase(&pomo);
    TEST_ASSERT(pomo.phase == POMO_PHASE_LONG_BREAK, "Cycle 4 moves to long Break (15m)");
    TEST_ASSERT(pomodoro_get_current_duration(&pomo) == 900.0,
		"Long break duration is 15m (900s)");

    pomodoro_next_phase(&pomo);
    TEST_ASSERT(pomo.phase == POMO_PHASE_FOCUS && pomo.cycle == 1,
		"Long Break returns to cycle 1 focus");

    return true;
}

int main(void){
    printf("--- Running unit test ---\n");
    bool all_passed = true;

    all_passed &= test_timer_digits_to_seconds();
    all_passed &= test_timer_seconds_to_digits();
    all_passed &= test_pomodoro_cycle_transitions();

    if (all_passed) {
	printf("\n all %d assertion successfully!\n", tests_run);
	return 0;
    }else {
	printf("\n Some test failed");
	return 1;
    }
}
