#ifndef __COMMON_H__
#define __COMMON_H__
#include "stdint.h"
#include "stdio.h"

#define DATABASE_START_ADDR         (0x0803F000)
#define DATABASE_MAGIC              (0xaaaaaaaa)

#define DEFAULT_BAUD_RATE_INDEX			(4)

#define USART2_USE_DMA				0

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
}gSensor_Data_t;

typedef struct {
	uint8_t water_sensor_state;
	uint8_t displacement_sensor_state;
	uint8_t smoke_sensor_state;
	uint16_t baudRateIndex;
	uint32_t temperature;
	uint32_t humidity;
	uint32_t vbat_value;
	uint32_t ac_detect_value;
	gSensor_Data_t gSensorData;
	uint32_t uid[3];
	uint8_t gSensorState;
}Sensor_Data_t;

typedef struct {
    uint32_t magic;
    uint32_t baudRateIndex;
}database_t;

extern Sensor_Data_t Sensor;

extern void sc7a20_interrupt_handle(void);

#endif
