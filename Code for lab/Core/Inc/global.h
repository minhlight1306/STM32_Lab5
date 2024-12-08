/*
 * global.h
 *
 *  Created on: 28 thg 10, 2024
 *      Author: PC
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#define INIT		0


#include "main.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

extern int status;


void blinky();
void blinkyled1();
void blinkyled2();
void blinkyled3();
void blinkyled4();

#endif /* INC_GLOBAL_H_ */
