#include "usart.h"

UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc1;

void uartBegin(void){
	const uint8_t str1[20] = "BEGIN UART\r\n";
	HAL_UART_Transmit(&huart2, str1, strlen((const char *)str1), 100);
}

////////////// COMMAND PARSER /////////////////////
#define COMMAND_RST "RST"
#define COMMAND_OK "OK"
#define TIMEOUT_PERIOD 300
enum status_parser{Idle1, CHECK, RECHECK};
enum status_uart{Idle11, SEND, WAIT, RESEND, FAILURE};
int status_parser1 = Idle1;
int status_uart1 = Idle1;
uint8_t temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint32_t len_buffer;
uint8_t buffer_flag = 0;
uint32_t ADC_value = 0;
uint8_t command_flag = 0;
char command_data[MAX_BUFFER_SIZE];
uint32_t last_response_time = 0;

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
	if(huart -> Instance == USART2 ){
		HAL_UART_Transmit (& huart2 , &temp , 1, 50);
		buffer[index_buffer++] = temp;
		if(index_buffer >= MAX_BUFFER_SIZE){
			index_buffer = 0;
		}
		buffer_flag = 1;
		HAL_UART_Receive_IT (& huart2, &temp, 1);
//		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
	}
}

void command_parser_fsm(){
    // Check if the buffer contains a command
    switch(status_parser1){
    case Idle1:
        if(buffer[index_buffer - 1] == '!'){
        	status_parser1 = CHECK;

        }
        break;
    case CHECK:
    	if(buffer[index_buffer - 1] == '#'){
    		status_parser1 = RECHECK;
    		buffer_flag = 1;
    		break;
    	}
    	if(strlen(command_data) < 20){
    		command_data[strlen(command_data)] = buffer[index_buffer - 1];
    		break;
    	}
		memset(command_data, 0, MAX_BUFFER_SIZE);
    	break;
    case RECHECK:
    	// Check for the RST command
    	if (strncmp(command_data, "RST", strlen(command_data)) == 0){
    		command_flag = 1;
    		status_parser1 = Idle1;
    		memset(command_data, 0, MAX_BUFFER_SIZE);
    		break;
    	}
    	// Check for the OK command
    	if(strncmp(command_data, "OK", strlen(command_data)) == 0){
    		if(status_uart1 == RESEND){
    			status_uart1 = Idle1;
    		}
    		memset(command_data, 0, MAX_BUFFER_SIZE);
    		status_parser1 = Idle1;
    		break;
    	}
    default:
    	break;
    }
}
void uart_communication_fsm(){
	char response[20];
	const uint8_t str[20] = "Failure\r\n";
    switch(status_uart1){
    case Idle1:
    	if(command_flag == 1){
    		status_uart1 = SEND;
    		command_flag = 0;
    		HAL_ADC_Start(&hadc1);
    		ADC_value = HAL_ADC_GetValue(& hadc1);
    		HAL_ADC_Stop(&hadc1);
    	}
    	break;
    case SEND:
        // Handle RST command
        HAL_UART_Transmit(&huart2, (void *)response, sprintf (response, "!ADC=%lu#\r\n", ADC_value), 100);
        status_uart1 = WAIT;
        break;
    case WAIT:
        setTimer(0, 3000);
        status_uart1 = RESEND;
    	break;
    case RESEND:
        if(isTimerExpired(0)){
        	setTimer(0, 3000);
            // Timeout occurred, resend the last ADC response
            HAL_UART_Transmit(&huart2, (void *)response, sprintf (response, "!ADC=%lu#\r\n", ADC_value), 100);
    	}
        break;
    case FAILURE:
    	HAL_UART_Transmit(&huart2, str, strlen((const char *)str), 100);
    	break;
    default:
    	break;
    }
}
