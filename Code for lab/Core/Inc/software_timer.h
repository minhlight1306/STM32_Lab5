
#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "global.h"
void setTimer(int index, int counter);
int isTimerExpired(int index);

void timerRun();

#endif /* INC_SOFTWARE_TIMER_H_ */
