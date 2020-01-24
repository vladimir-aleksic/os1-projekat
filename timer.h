#ifndef __TIMER_H__
#define __TIMER_H__

#include "const.h"

void interrupt timer(...);
void tick();


class Timer {
public:
    static void init();
    static void restore();

private:
    static bool routineSet;
    static interruptRoutine oldTimer;
};

#endif
