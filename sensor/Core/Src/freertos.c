/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "common.h"
#include "sc7a20.h"
#include "stdio.h"
#include "DHT20.h"
#include "app_uart.h"
#include "user_protocol.h"
#include "user_data.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint32_t adc_value[2];
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	static uint16_t cnt = 0;
	
	user_protocol_init();
	Sensor.uid[0] = HAL_GetUIDw0();
	Sensor.uid[1] = HAL_GetUIDw1();
	Sensor.uid[2] = HAL_GetUIDw2();
	
	DHT20_Task_Init();
	AppUart2Init();
	
	sc7a20_init(&sc7a20_misc_data);
  /* Infinite loop */
  for(;;)
  {
		sc7a20_get_acceleration_data(&sc7a20_misc_data, (int16_t *)&Sensor.gSensorData);
		cnt ++;
		if(cnt >= 500){
			cnt = 0;
			printf("\r\n===================================\r\n");
			printf("move sensor state: %d\r\n", Sensor.displacement_sensor_state);
			printf("smoke sensor state: %d\r\n", Sensor.smoke_sensor_state);
			printf("water sensor state: %d\r\n", Sensor.water_sensor_state);
			printf("temperature: %d\r\n", Sensor.temperature);
			printf("humidity: %d\r\n", Sensor.humidity);
			printf("bat value: %d\r\n", Sensor.vbat_value);
			printf("ac detect: %d\r\n", Sensor.ac_detect_value);
			printf("gsensor x: %d\r\n", Sensor.gSensorData.x);
			printf("gsensor y: %d\r\n", Sensor.gSensorData.y);
			printf("gsensor z: %d\r\n", Sensor.gSensorData.z);
			printf("baudRateIndex: %d\r\n", Sensor.baudRateIndex);
			printf("uid: 0x%x%x%x\r\n", Sensor.uid[0], Sensor.uid[1], Sensor.uid[2]);
			printf("===================================\r\n");
		}
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

