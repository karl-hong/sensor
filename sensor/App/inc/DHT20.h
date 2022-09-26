#ifndef __DHT20_H__
#define __DHT20_H__
#include "stm32l4xx_hal.h"

#define DHT20_ADDR				0x38
#define DHT20_SCL_PIN			GPIO_PIN_13
#define DHT20_SDA_PIN			GPIO_PIN_14


void DHT20_Task_Init(void);

#endif
