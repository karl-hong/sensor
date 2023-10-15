#ifndef __USER_DATA_H__
#define __USER_DATA_H__
#include <stdint.h>

void onCmdQuerySensorData(uint8_t *data, uint16_t length);

void onCmdSingleModifyBaudRate(uint8_t *data, uint16_t length);

void onCmdMultiModifyBaudRate(uint8_t *data, uint16_t length);

void user_database_init(void);

void user_database_save(void);


#endif
