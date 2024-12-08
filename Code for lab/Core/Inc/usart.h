
#ifndef INC_USART_H_
#define INC_USART_H_

#include "global.h"

#define MAX_BUFFER_SIZE 30
extern uint8_t buffer_flag;

void uartBegin(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void command_parser_fsm();
void uart_communication_fsm();

#endif /* INC_USART_H_ */
