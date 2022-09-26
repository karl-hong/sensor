#include "app_uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "common.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "crc8_16.h"
#include "user_protocol.h"

#define BSP_USART2_RX_MAX_LEN		10

volatile static uint8_t Uart2RxBuffer[BSP_USART2_RX_MAX_LEN] = {0};
volatile static uint32_t uart2_rx_cnt = 0;
volatile static uint32_t cur_index = 0;
volatile static uint32_t next_index = 0;
uint8_t g_uart2_send_data_enable = 0;

osThreadId_t AppUart2RxHandle;
const osThreadAttr_t Uart2RxTask_attributes = {
  .name = "Uart2RxTask",
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t AppUart2TxTaskHandle;
const osThreadAttr_t Uart2TxTask_attributes = {
  .name = "Uart2TxTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

volatile void checkUartBuffer(void)
{
	//cur_index = 0;
	while(1){
		if(Uart2RxBuffer[cur_index] != DUMMY_DATA){
			user_protocol_push_data((uint8_t *)&Uart2RxBuffer[cur_index], 1);
			Uart2RxBuffer[cur_index] = 0xFF;
		}else{
			return;
		}
		cur_index ++;
		if(cur_index >= sizeof(Uart2RxBuffer)){
			cur_index = 0;
			return;
		}
	}	
}

void AppUart2RxTask(void *arg)
{
	/* 使用DMA接收 */
#if USART2_USE_DMA	
	memset((uint8_t *)&Uart2RxBuffer, 0xff, sizeof(Uart2RxBuffer));
	HAL_UART_Receive_DMA(&huart2, (uint8_t *)&Uart2RxBuffer, sizeof(Uart2RxBuffer));
#else	
	/* 使用中断接收 */
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&Uart2RxBuffer, 1);
#endif
	while(1){
#if USART2_USE_DMA		
		checkUartBuffer();
#endif		
		user_protocol_handle();
		if(g_uart2_send_data_enable){
			g_uart2_send_data_enable = 0;
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&Uart2RxBuffer, 1);
		}
		osDelay(10);
	}
}

void AppUart2TxTask(void *arg)
{
	while(1){
		user_protocol_handle();
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&Uart2RxBuffer, 1);
		osDelay(20);
	}
}

void appUart2RxCallBack(void)
{
	user_protocol_push_data((uint8_t *)&Uart2RxBuffer, 1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&Uart2RxBuffer, 1);
}

void AppUart2Init(void)
{
	AppUart2RxHandle = osThreadNew(AppUart2RxTask, NULL, &Uart2RxTask_attributes);
//	AppUart2TxTaskHandle = osThreadNew(AppUart2TxTask, NULL, &Uart2TxTask_attributes);
}


