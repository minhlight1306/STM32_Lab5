
#include "software_timer.h"
//# define MAX_COUNTER 10

int timerCounter[10];
int timerFlag[10];

void setTimer(int index, int counter){
	timerCounter[index] = counter/TICK;
	timerFlag[index] = 0;
}
void timerRun(){
	for(int i = 0; i < 10; i++){
		if(timerCounter[i] >= 0){
			timerCounter[i]--;
		}
		if(timerCounter[i] <= 0){
			timerFlag[i] = 1;
		}
	}
}
int isTimerExpired(int index){
	if(timerFlag[index] == 1){
		timerFlag[index] = 0;
		return 1;
	}
	return 0;
}
