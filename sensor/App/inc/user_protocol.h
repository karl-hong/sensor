#ifndef __USER_PROTOCOL_H__
#define __USER_PROTOCOL_H__


#include <stdint.h>
#include <stdlib.h>

#define MAX_PACK_LEN			128

#define FRAME_HEADER			0xAA
#define FRAME_TAIL				0x55
#define TRANSLATE_CHAR		0xCC

#define CMD_FIX_LEN				 6
#define MIN_FRAME_LEN			(9)

/**
 *协议格式：帧头（1字节）+ 指令（1字节）+ 操作码（1字节）+ 数据区长度（2字节）+ 数据区（N字节）+ 校验码（2字节）+ 帧尾（1字节）
 */

typedef enum tagEnumPTL_STATUS
{
	PTL_NO_ERROR = 0,		//解包正确
	PTL_CHECKSUM_ERROR,		//校验错误
	PTL_LENGTH_ERROR,		//包长度错误
	PTL_HEAD_ERROR,			//包头错误
	PTL_TAIL_ERROR,			//包尾错误
	PTL_DATA_ERROR			//数据域错误
	
}PTL_STATUS;

//解包数据结构
typedef struct tagUnpack
{
	uint16_t indexOfHead;		//包头索引
	uint16_t indexOfTail;		//包尾索引
	uint8_t  cmdID;				//命令ID，输出参数
	uint8_t  optionID;			//选项
	uint8_t  *pData;			//数据域缓存，输出参数
	uint16_t lenOfData;			//缓冲区数据长度
}UnPack_t;

typedef struct tagComStream
{
	uint8_t* pBuffer;		//数据缓存
	uint16_t nLenOfBuf;		//流长度
}ComStream_t;


typedef struct tagPack
{
	uint8_t cmdID;				//命令ID，输出参数
	uint8_t optionID;				//选项
	uint8_t* pData;				//数据域缓存，输出参数
	uint16_t nLenOfData;			//缓冲区数据长度
}Pack_t;

enum {
	CMD_QUERY = 0x00,
	CMD_ACK,
};

enum {
	OPTION_QUERY_SENSOR_DATA = 1,
	OPT_CODE_SINGLE_MODIFY_BAUDRATE = 0xfe,//单个设备修改波特率
	OPT_CODE_MULTI_MODIFY_BAUDRATE,//批量修改波特率
};


//解包函数
PTL_STATUS ComUnpack(ComStream_t * inStream,UnPack_t *pResult);			
//打包函数
PTL_STATUS ComPack(const Pack_t*  pPack, ComStream_t* outStream);					

void user_protocol_handle(void);

void user_protocol_init(void);

void user_protocol_push_data(uint8_t *data, uint16_t size);

void user_protocol_send_data(uint8_t cmd, uint8_t optID, uint8_t *data, uint16_t size);


#endif
