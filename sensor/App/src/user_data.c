#include <stdio.h>
#include "user_data.h"
#include "common.h"
#include "user_protocol.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_flash_ex.h"  

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

void onReportSingleModifyBaudRate(void)
{
    uint8_t buffer[23];
    uint8_t cnt = 0;
    /* addr */
    buffer[cnt++] = 0;//Sensor.address;
    /* lock baudRateIndex */
    buffer[cnt++] = Sensor.baudRateIndex;
    /* UID */
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

    user_protocol_send_data(CMD_ACK, OPT_CODE_SINGLE_MODIFY_BAUDRATE, buffer, cnt); 

    while(1);//wait for watchdog reset 
}


void onCmdSingleModifyBaudRate(uint8_t *data, uint16_t length)
{
    uint32_t uid0;
    uint32_t uid1;
    uint32_t uid2;
    uint16_t pos = 0;
    uint8_t baudRateIndex = 0;

    baudRateIndex = data[pos++];

    uid0 = (data[pos++] << 24);
    uid0 += (data[pos++] << 16);
    uid0 += (data[pos++] << 8);
    uid0 += data[pos++];

    uid1 = (data[pos++] << 24);
    uid1 += (data[pos++] << 16);
    uid1 += (data[pos++] << 8);
    uid1 += data[pos++];

    uid2 = (data[pos++] << 24);
    uid2 += (data[pos++] << 16);
    uid2 += (data[pos++] << 8);
    uid2 += data[pos++]; 

    if(Sensor.uid[0] != uid0 || Sensor.uid[1] != uid1 || Sensor.uid[2] != uid2){
        printf("[%s]UID is not matched!\r\n", __FUNCTION__);
        return;
    }  
out: 
    Sensor.baudRateIndex = baudRateIndex;

    user_database_save();
	/* send msg and wait for reset */
	onReportSingleModifyBaudRate();
}

void onCmdMultiModifyBaudRate(uint8_t *data, uint16_t length)
{
    uint16_t pos = 0;
    uint8_t baudRateIndex = 0;

    baudRateIndex = data[pos++];

    Sensor.baudRateIndex = baudRateIndex;

    user_database_save();

  	//wait for watchdog reset
  	while(1);
}

void onCmdQuerySensorData(uint8_t *data, uint16_t length)
{
    (void) data;
    (void) length;

    ReportSensorData();
		static uint32_t recCnt = 0;
		recCnt += 8;
}

static void user_read_flash(uint32_t address, uint32_t *ptr, uint16_t nWord)
{
	for(uint16_t i=0;i<nWord;i++){
		ptr[i] = *(__IO uint32_t *)address;
		address += 4;
	}
}

static uint8_t user_write_flash(uint32_t address, uint64_t *ptr, uint16_t nWord)
{
	uint8_t page;
	uint32_t pageError = 0;
	FLASH_EraseInitTypeDef flashEraseInitType;
	HAL_StatusTypeDef status;
	uint32_t addr = address;

	if(nWord > 512){
		return 1;
	}

	/* unlock flash */
	HAL_FLASH_Unlock();

	/* clear all error flag */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	/* calc page index */
	page = (addr - FLASH_BASE) / FLASH_PAGE_SIZE;

	flashEraseInitType.TypeErase = FLASH_TYPEERASE_PAGES;
	flashEraseInitType.Page 	 = page;
	flashEraseInitType.Banks	 = FLASH_BANK_1;
	flashEraseInitType.NbPages	 = 1;
	
	/* erase page */
	status = HAL_FLASHEx_Erase(&flashEraseInitType, &pageError);
	if(HAL_OK != status){
        HAL_FLASH_Lock();
       // printf("Flash erase error: %d\r\n", status);
        return 1;
    }

	/* write data to flash */
	for(uint16_t i=0;i<nWord;i++){
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, ptr[i]);
		if(HAL_OK != status){
		//	printf("[%s]write data[%d] fail!\r\n", __FUNCTION__, i);
		}
		addr += 8;
	}

	HAL_FLASH_Lock();

	return 0;
}

void user_database_init(void)
{
    uint16_t i;
    uint16_t lenOfDataBase = sizeof(database_t) / sizeof(uint32_t);
    database_t readDataBase;
    uint32_t *pData = (uint32_t *)&readDataBase;

    memset((uint8_t *)&Sensor, 0x00, sizeof(Sensor));

		user_read_flash(DATABASE_START_ADDR, pData, lenOfDataBase);

    if(DATABASE_MAGIC != readDataBase.magic){
        Sensor.baudRateIndex = DEFAULT_BAUD_RATE_INDEX;
        user_database_save();
    }else{
        Sensor.baudRateIndex = (readDataBase.baudRateIndex == 0xffffffff) ? DEFAULT_BAUD_RATE_INDEX : readDataBase.baudRateIndex;
    }
}

void user_database_save(void)
{
    uint16_t i;
    uint16_t lenOfDataBase = sizeof(database_t) / sizeof(uint64_t);
    database_t writeDataBase;
    uint64_t *pData = (uint64_t *)&writeDataBase;

    writeDataBase.magic = DATABASE_MAGIC;
    writeDataBase.baudRateIndex = Sensor.baudRateIndex;

		user_write_flash(DATABASE_START_ADDR, pData, lenOfDataBase);
}


