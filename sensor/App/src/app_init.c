#include "app_init.h"

osThreadId_t AppInitTaskHandle;
const osThreadAttr_t appInitTask_attributes = {
  .name = "AppInitTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

void AppInitTask(void *argument);

//extern void start_adc(void);

void App_Init(void) {
  AppInitTaskHandle = osThreadNew(AppInitTask, NULL, &appInitTask_attributes);
}


void AppInitTask(void *argument)
{
	osDelay(500);
//	start_adc();
	while(1){
		printf("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\r\n");
		osDelay(1000);
	}
}

