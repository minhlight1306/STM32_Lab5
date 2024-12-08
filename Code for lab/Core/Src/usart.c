
#include "usart.h"

UART_HandleTypeDef huart1;
ADC_HandleTypeDef hadc1;

void uartBegin(void){
	const uint8_t str1[20] = "!BEGIN USART#\r\n";
	HAL_UART_Transmit(&huart1, str1, strlen((const char *)str1), 100);
}

////////////// COMMAND PARSER /////////////////////
#define COMMAND_RST "!RST#"
#define COMMAND_OK "!OK#"
#define TIMEOUT_PERIOD 3000
uint8_t temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;
uint32_t ADC_value = 0;
uint8_t command_flag = 0;
char command_data[MAX_BUFFER_SIZE];
uint32_t last_response_time = 0;

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
	if(huart -> Instance == USART1 ){
//		HAL_UART_Transmit (& huart1 , &temp , 1, 50);
		buffer[index_buffer++] = temp;
		if(index_buffer >= MAX_BUFFER_SIZE){
			index_buffer = 0;
		}
		buffer_flag = 1;
		HAL_UART_Receive_IT (& huart1, &temp, 1);
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
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

            ADC_value = HAL_ADC_GetValue(& hadc1); // Function to get the ADC value
            last_response_time = HAL_GetTick();
            return;
        }

        // Check for the OK command
        if(strncmp((char *)buffer, COMMAND_OK, strlen(COMMAND_OK)) == 0){
            command_flag = 2; // Indicate OK command received
            strcpy(command_data, COMMAND_OK);
            // Reset buffer after handling command
            memset(buffer, 0, MAX_BUFFER_SIZE);
            index_buffer = 0;

            last_response_time = 0;
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

        char response[20];
        HAL_UART_Transmit(&huart1, (void *)response, sprintf (response, "!ADC=%lu#\r\n", ADC_value), 100);
        command_flag = 0;
    }
    else if(command_flag == 2){
        // Handle OK command
        // End communication or perform cleanup
        command_flag = 0;
        return;
    }

    if(HAL_GetTick() - last_response_time > TIMEOUT_PERIOD && last_response_time != 0){
            // Timeout occurred, resend the last ADC response
            char response[20];
            HAL_UART_Transmit(&huart1, (void *)response, sprintf (response, "!ADC=%lu#\r\n", ADC_value), 100);
            last_response_time = HAL_GetTick(); // Reset the last response time
    }
}

