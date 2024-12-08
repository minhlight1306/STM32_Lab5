
#ifndef INC_USART_H_
#define INC_USART_H_

#include "global.h"

#define MAX_BUFFER_SIZE 30
extern UART_HandleTypeDef huart1;
extern uint8_t temp;
extern uint8_t index_buffer;
extern uint8_t buffer_flag;

void uartReport(void);
void uartRedCount(void);
void uartGreenCount(void);
void uartYellowCount(void);
void uartCounter(void);
void uartBegin(void);
void command_parser_fsm();
void uart_communication_fsm();

#endif /* INC_USART_H_ */
