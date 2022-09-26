#include <stdio.h>
#include "user_data.h"
#include "common.h"
#include "user_protocol.h"

void ReportSensorData(void)
{
	uint16_t cnt = 0;
	uint8_t buffer[128];

	/* move sensor state */
	buffer[cnt ++] = Sensor.displacement_sensor_state;
	/* smoke sensor state */
	buffer[cnt ++] = Sensor.smoke_sensor_state;
	/* water sensor state */
	buffer[cnt ++] = Sensor.water_sensor_state;
	/* temperature */
	buffer[cnt ++] = (Sensor.temperature >> 24) & 0x00ff;
	buffer[cnt ++] = (Sensor.temperature >> 16) & 0x00ff;
	buffer[cnt ++] = (Sensor.temperature >> 8) & 0x00ff;
	buffer[cnt ++] = Sensor.temperature & 0x00ff;
	/* humidity */
	buffer[cnt ++] = (Sensor.humidity >> 24) & 0x00ff;
	buffer[cnt ++] = (Sensor.humidity >> 16) & 0x00ff;
	buffer[cnt ++] = (Sensor.humidity >> 8) & 0x00ff;
	buffer[cnt ++] = Sensor.humidity & 0x00ff;
	/* gSensor data */
	buffer[cnt ++] = (Sensor.gSensorData.x >> 8) & 0xff;
	buffer[cnt ++] = Sensor.gSensorData.x & 0xff;
	buffer[cnt ++] = (Sensor.gSensorData.y >> 8) & 0xff;
	buffer[cnt ++] = Sensor.gSensorData.y & 0xff;
	buffer[cnt ++] = (Sensor.gSensorData.z >> 8) & 0xff;
	buffer[cnt ++] = Sensor.gSensorData.z & 0xff;
	/* adc data */
	buffer[cnt ++] = (Sensor.vbat_value >> 8) & 0xff;
	buffer[cnt ++] = Sensor.vbat_value & 0xff;
	buffer[cnt ++] = (Sensor.ac_detect_value >> 8) & 0xff;
	buffer[cnt ++] = Sensor.ac_detect_value & 0xff;
	/* uid data */
	buffer[cnt ++] = (Sensor.uid[0]  >> 24) & 0xff;
	buffer[cnt ++] = (Sensor.uid[0]  >> 16) & 0xff;
	buffer[cnt ++] = (Sensor.uid[0]  >> 8) & 0xff;
	buffer[cnt ++] = Sensor.uid[0]  & 0xff;
	buffer[cnt ++] = (Sensor.uid[1]  >> 24) & 0xff;
	buffer[cnt ++] = (Sensor.uid[1]  >> 16) & 0xff;
	buffer[cnt ++] = (Sensor.uid[1]  >> 8) & 0xff;
	buffer[cnt ++] = Sensor.uid[1]  & 0xff;
	buffer[cnt ++] = (Sensor.uid[2]  >> 24) & 0xff;
	buffer[cnt ++] = (Sensor.uid[2]  >> 16) & 0xff;
	buffer[cnt ++] = (Sensor.uid[2]  >> 8) & 0xff;
	buffer[cnt ++] = Sensor.uid[2]  & 0xff;

	user_protocol_send_data(CMD_ACK, OPTION_QUERY_SENSOR_DATA, (uint8_t *)&buffer, cnt);
}

void onCmdQuerySensorData(uint8_t *data, uint16_t length)
{
    (void) data;
    (void) length;

    ReportSensorData();
		static uint32_t recCnt = 0;
	recCnt += 8;
}


