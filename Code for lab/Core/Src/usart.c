
#include "usart.h"

UART_HandleTypeDef huart1;



void uartReport(void) {
	char str[40];
	HAL_UART_Transmit(&huart1, (void*)str, sprintf(str, "!Time: %lu - Task: %lu\r\n#", HAL_GetTick(), numTask), 100);
}

void uartRedCount(void) {
	const uint8_t str1[20] = "!MODE: 2#\r\n";
	char str2[20];
	HAL_UART_Transmit(&huart1, str1, strlen((const char *)str1), 100);
	HAL_UART_Transmit(&huart1, (void*)str2, sprintf(str2, "!7SEG: %02d#\r\n", temp_count[0]), 100);
}

void uartGreenCount(void) {
	const uint8_t str1[20] = "!MODE: 4#\r\n";
	char str2[20];
	HAL_UART_Transmit(&huart1, str1, strlen((const char *)str1), 100);
	HAL_UART_Transmit(&huart1, (void*)str2, sprintf(str2, "!7SEG: %02d#\r\n", temp_count[2]), 100);
}

void uartYellowCount(void) {
	const uint8_t str1[20] = "!MODE: 3#\r\n";
	char str2[20];
	HAL_UART_Transmit(&huart1, str1, strlen((const char *)str1), 100);
	HAL_UART_Transmit(&huart1, (void*)str2, sprintf(str2, "!7SEG: %02d#\r\n", temp_count[1]), 100);
}

void uartCounter(void) {
	char str1[20];
	char str2[20];
	HAL_UART_Transmit(&huart1, (void*)str1, sprintf(str1, "!7SEG: %02d#\r\n", led_count[0]), 100);
	HAL_UART_Transmit(&huart1, (void*)str2, sprintf(str2, "!7SEG: %02d#\r\n", led_count[1]), 100);
}
void uartBegin(void){
	const uint8_t str1[20] = "!BEGIN USART#\r\n";
	const uint8_t str2[20] = "!MODE: 1#\r\n";
	HAL_UART_Transmit(&huart1, str1, strlen((const char *)str1), 100);
	HAL_UART_Transmit(&huart1, str2, strlen((const char *)str2), 100);
}

////////////// COMMAND PARSER /////////////////////
#define COMMAND_RST "!RST#"
#define COMMAND_OK "!OK#"
uint8_t temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;
uint32_t ADC_value = 0;
uint8_t command_flag = 0;
char command_data[MAX_BUFFER_SIZE];
//ADC_value = HAL_ADC_GetValue(& hadc1);
//HAL_UART_Transmit(& huart1, (void *) str, sprintf (str, "%d\n", ADC_value ), 1000);
//HAL_Delay(500);
void HAL_UART_RxCpltCallback ( UART_HandleTypeDef * huart ){
	if(huart -> Instance == USART1 ){
//		HAL_UART_Transmit (& huart1 , &temp , 1, 50);
		buffer[index_buffer++] = temp;
		if(index_buffer >= MAX_BUFFER_SIZE){
			index_buffer = 0;
		}
		buffer_flag = 1;
		HAL_UART_Receive_IT (& huart1 , &temp , 1);
	}
}


void command_parser_fsm(){
    // Check if the buffer contains a command
    if(index_buffer > 0){
        // Check for the RST command
        if(strncmp((char *)buffer, COMMAND_RST, strlen(COMMAND_RST)) == 0){
            command_flag = 1;
            strcpy(command_data, COMMAND_RST);
            // Reset buffer after handling command
            memset(buffer, 0, MAX_BUFFER_SIZE);
            index_buffer = 0;
            return;
        }

        // Check for the OK command
        if(strncmp((char *)buffer, COMMAND_OK, strlen(COMMAND_OK)) == 0){
            command_flag = 2; // Indicate OK command received
            strcpy(command_data, COMMAND_OK);
            // Reset buffer after handling command
            memset(buffer, 0, MAX_BUFFER_SIZE);
            index_buffer = 0;
            return;
        }

        // Reset if no valid command found
        memset(buffer, 0, MAX_BUFFER_SIZE);
        index_buffer = 0;
    }
}
void uart_communication_fsm(){
    if(command_flag == 1){
        // Handle RST command
        uint16_t ADC_value = HAL_ADC_GetValue(& hadc1);
        char response[20];
        HAL_UART_Transmit(&huart1, (void *)response, sprintf (response, "!ADC=%d#\r\n", ADC_value), 100);
        command_flag = 0;
    }
    else if(command_flag == 2){
        // Handle OK command
        // End communication or perform cleanup
        command_flag = 0;
    }
}

