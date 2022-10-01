#include "DHT20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "common.h"
#include "stdio.h"

osThreadId_t DHT20TaskHandle;
const osThreadAttr_t DHT20Task_attributes = {
  .name = "DHT20Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

void Delay_N10us(uint32_t t)
{
  uint32_t k;

   while(t--)
  {
    for (k = 0; k < 2; k++);//110
  }
}

void SensorDelay_us(uint32_t t)
{
	for(t = t-2; t>0; t--)
	{
		Delay_N10us(1);
	}
}

void Delay_4us(void)
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}
void Delay_5us(void)
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);

}

void Delay_1ms(uint32_t t)
{
	osDelay(t);
}

static void DHT20_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pins : 
	PB13-----------> DHT20_SCL
	PB14-----------> DHT20_SDA
	*/
  GPIO_InitStruct.Pin = DHT20_SCL_PIN | DHT20_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOB, DHT20_SCL_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, DHT20_SDA_PIN, GPIO_PIN_SET);
}

static void DHT20_SDA_PIN_Output_High(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin   = DHT20_SDA_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, DHT20_SDA_PIN, GPIO_PIN_SET);
}

static void DHT20_SDA_PIN_Output_Low(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin   = DHT20_SDA_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, DHT20_SDA_PIN, GPIO_PIN_RESET);
}

static void DHT20_SDA_PIN_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin   = DHT20_SDA_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void DHT20_SCL_PIN_Output_High(void)
{
	HAL_GPIO_WritePin(GPIOB, DHT20_SCL_PIN, GPIO_PIN_SET);
}

static void DHT20_SCL_PIN_Output_Low(void)
{
	HAL_GPIO_WritePin(GPIOB, DHT20_SCL_PIN, GPIO_PIN_RESET);
}

void I2C_Start(void)
{
	DHT20_SDA_PIN_Output_High();
	SensorDelay_us(8);
	DHT20_SCL_PIN_Output_High();
	SensorDelay_us(8);
	DHT20_SDA_PIN_Output_Low();
	SensorDelay_us(8);
	DHT20_SCL_PIN_Output_Low();
	SensorDelay_us(8);   
}


void DHT20_WR_Byte(uint8_t Byte)
{
	uint8_t Data,N,i;	
	Data=Byte;
	i = 0x80;
	for(N=0;N<8;N++)
	{
		DHT20_SCL_PIN_Output_Low(); 
		Delay_4us();	
		if(i&Data)
		{
			DHT20_SDA_PIN_Output_High();
		}
		else
		{
			DHT20_SDA_PIN_Output_Low();
		}	
			
    DHT20_SCL_PIN_Output_High();
		Delay_4us();
		Data <<= 1;
		 
	}
	DHT20_SCL_PIN_Output_Low();
	SensorDelay_us(8);   
	DHT20_SDA_PIN_Input();
	SensorDelay_us(8);	
}	


uint8_t DHT20_RD_Byte(void)
{
	uint8_t Byte,i,a;
	Byte = 0;
	DHT20_SCL_PIN_Output_Low();
	DHT20_SDA_PIN_Input();
	SensorDelay_us(8);	
	for(i=0;i<8;i++)
	{
    DHT20_SCL_PIN_Output_High();		
		Delay_5us();
		a=0;
		if(HAL_GPIO_ReadPin(GPIOB,DHT20_SDA_PIN)) a=1;
		Byte = (Byte<<1)|a;
		DHT20_SCL_PIN_Output_Low();
		Delay_5us();
	}
  DHT20_SDA_PIN_Input();
	SensorDelay_us(8);	
	return Byte;
}


uint8_t Receive_ACK(void)
{
	uint16_t CNT;
	CNT = 0;
	DHT20_SCL_PIN_Output_Low();	
	DHT20_SDA_PIN_Input();
	SensorDelay_us(8);	
	DHT20_SCL_PIN_Output_High();	
	SensorDelay_us(8);	
	while((HAL_GPIO_ReadPin(GPIOB,DHT20_SDA_PIN))  && CNT < 100) 
	CNT++;
	if(CNT == 100)
	{
		return 0;
	}
 	DHT20_SCL_PIN_Output_Low();	
	SensorDelay_us(8);	
	return 1;
}

void Send_ACK(void)
{
	DHT20_SCL_PIN_Output_Low();	
	SensorDelay_us(8);	
	DHT20_SDA_PIN_Output_Low();
	SensorDelay_us(8);	
	DHT20_SCL_PIN_Output_High();	
	SensorDelay_us(8);
	DHT20_SCL_PIN_Output_Low();	
	SensorDelay_us(8);
	DHT20_SDA_PIN_Input();
	SensorDelay_us(8);
}

void Send_NOT_ACK(void)
{
	DHT20_SCL_PIN_Output_Low();	
	SensorDelay_us(8);
	DHT20_SDA_PIN_Output_High();
	SensorDelay_us(8);
	DHT20_SCL_PIN_Output_High();	
	SensorDelay_us(8);		
	DHT20_SCL_PIN_Output_Low();	
	SensorDelay_us(8);
  DHT20_SDA_PIN_Output_Low();
	SensorDelay_us(8);
}

void Stop_I2C(void)
{
	DHT20_SDA_PIN_Output_Low();
	SensorDelay_us(8);
	DHT20_SCL_PIN_Output_High();	
	SensorDelay_us(8);
	DHT20_SDA_PIN_Output_High();
	SensorDelay_us(8);
}

uint8_t DHT20_Read_Status(void)
{
	uint8_t Byte_first;	
	I2C_Start();
	DHT20_WR_Byte(0x71);
	Receive_ACK();
	Byte_first = DHT20_RD_Byte();
	Send_NOT_ACK();
	Stop_I2C();
	return Byte_first;
}

uint8_t DHT20_Read_Cal_Enable(void)
{
	uint8_t val = 0;//ret = 0,
  val = DHT20_Read_Status();
	 if((val & 0x68)==0x08)
		 return 1;
   else  return 0;
 }

void DHT20_SendAC(void)
{

	I2C_Start();
	DHT20_WR_Byte(0x70);
	Receive_ACK();
	DHT20_WR_Byte(0xac);//0xAC????
	Receive_ACK();
	DHT20_WR_Byte(0x33);
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

}

//CRC????:CRC8/MAXIM
//???:X8+X5+X4+1
//Poly:0011 0001  0x31
//????????? 1000 1100 0x8c
//C????:
uint8_t Calc_CRC8(uint8_t *message,uint8_t Num)
{
        uint8_t i;
        uint8_t byte;
        uint8_t crc=0xFF;
  for(byte=0; byte<Num; byte++)
  {
    crc^=(message[byte]);
    for(i=8;i>0;--i)
    {
      if(crc&0x80) crc=(crc<<1)^0x31;
      else crc=(crc<<1);
    }
  }
        return crc;
}

void DHT20_Read_CTdata(uint32_t *ct)
{
	volatile uint8_t  Byte_1th=0;
	volatile uint8_t  Byte_2th=0;
	volatile uint8_t  Byte_3th=0;
	volatile uint8_t  Byte_4th=0;
	volatile uint8_t  Byte_5th=0;
	volatile uint8_t  Byte_6th=0;
	 uint32_t RetuData = 0;
	DHT20_SendAC();
	Delay_1ms(80);

	while(((DHT20_Read_Status()&0x80)==0x80))
	{
		Delay_1ms(1);
	}
	I2C_Start();
	DHT20_WR_Byte(0x71);
	Receive_ACK();
	Byte_1th = DHT20_RD_Byte();
	Send_ACK();
	Byte_2th = DHT20_RD_Byte();//??
	Send_ACK();
	Byte_3th = DHT20_RD_Byte();//??
	Send_ACK();
	Byte_4th = DHT20_RD_Byte();//??/??
	Send_ACK();
	Byte_5th = DHT20_RD_Byte();//??
	Send_ACK();
	Byte_6th = DHT20_RD_Byte();//??
	Send_NOT_ACK();
	Stop_I2C();

	RetuData = (RetuData|Byte_2th)<<8;
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	RetuData =RetuData >>4;
	ct[0] = RetuData;//??
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	RetuData = RetuData&0xfffff;
	ct[1] =RetuData; //??

}


void DHT20_Read_CTdata_crc(uint32_t *ct) //CRC???,??AHT20????????
{
	volatile uint8_t  Byte_1th=0;
	volatile uint8_t  Byte_2th=0;
	volatile uint8_t  Byte_3th=0;
	volatile uint8_t  Byte_4th=0;
	volatile uint8_t  Byte_5th=0;
	volatile uint8_t  Byte_6th=0;
	volatile uint8_t  Byte_7th=0;
	 uint32_t RetuData = 0;
	 uint8_t  CTDATA[6]={0};
	
	DHT20_SendAC();
	Delay_1ms(80);	

	while(((DHT20_Read_Status()&0x80)==0x80))//????bit[7]?0,???????,??1,?????
	{
		Delay_1ms(1);
	}
	
	I2C_Start();

	DHT20_WR_Byte(0x71);
	Receive_ACK();
	CTDATA[0]=Byte_1th = DHT20_RD_Byte();//???,??????0x98,??????,bit[7]?1;???0x1C,??0x0C,??0x08???????,bit[7]?0
	Send_ACK();
	CTDATA[1]=Byte_2th = DHT20_RD_Byte();//??
	Send_ACK();
	CTDATA[2]=Byte_3th = DHT20_RD_Byte();//??
	Send_ACK();
	CTDATA[3]=Byte_4th = DHT20_RD_Byte();//??/??
	Send_ACK();
	CTDATA[4]=Byte_5th = DHT20_RD_Byte();//??
	Send_ACK();
	CTDATA[5]=Byte_6th = DHT20_RD_Byte();//??
	Send_ACK();
	Byte_7th = DHT20_RD_Byte();//CRC??
	Send_NOT_ACK();                           //??: ?????NAK
	Stop_I2C();
	
	if(Calc_CRC8(CTDATA,6)==Byte_7th)
	{
	RetuData = (RetuData|Byte_2th)<<8;
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	RetuData =RetuData >>4;
	ct[0] = RetuData;//??
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	RetuData = RetuData&0xfffff;
	ct[1] =RetuData; //??
		
	}
	else
	{
		ct[0]=0x00;
		ct[1]=0x00;//???????,????????????
	}//CRC??
}


void DHT20_Init(void)   //???AHT20
{	
	I2C_Start();
	DHT20_WR_Byte(0x70);
	Receive_ACK();
	DHT20_WR_Byte(0xa8);//0xA8??NOR????
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

	Delay_1ms(10);//??10ms??

	I2C_Start();
	DHT20_WR_Byte(0x70);
	Receive_ACK();
	DHT20_WR_Byte(0xbe);//0xBE?????,AHT20???????0xBE,   AHT10???????0xE1
	Receive_ACK();
	DHT20_WR_Byte(0x08);//?????bit[3]?1,?????
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();
	Delay_1ms(10);//??10ms??
}
void DHT20_Reset_REG(uint8_t addr)
{
	
	uint8_t Byte_first,Byte_second,Byte_third,Byte_fourth;
	UNUSED(Byte_first);
	UNUSED(Byte_second);
	UNUSED(Byte_third);
	UNUSED(Byte_fourth);
	
	I2C_Start();
	DHT20_WR_Byte(0x70);//???0x70
	Receive_ACK();
	DHT20_WR_Byte(addr);
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	DHT20_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();

	Delay_1ms(5);//??5ms??
	I2C_Start();
	DHT20_WR_Byte(0x71);//
	Receive_ACK();
	Byte_first = DHT20_RD_Byte();
	Send_ACK();
	Byte_second = DHT20_RD_Byte();
	Send_ACK();
	Byte_third = DHT20_RD_Byte();
	Send_NOT_ACK();
	Stop_I2C();
	
  Delay_1ms(10);//??10ms??
	I2C_Start();
	DHT20_WR_Byte(0x70);///
	Receive_ACK();
	DHT20_WR_Byte(0xB0|addr);////?????
	Receive_ACK();
	DHT20_WR_Byte(Byte_second);
	Receive_ACK();
	DHT20_WR_Byte(Byte_third);
	Receive_ACK();
	Stop_I2C();
	
	Byte_second=0x00;
	Byte_third =0x00;
}

void DHT20_Start_Init(void)
{
	DHT20_Reset_REG(0x1b);
	DHT20_Reset_REG(0x1c);
	DHT20_Reset_REG(0x1e);
}

void DHT20_Task(void *argument)
{
	volatile int  c1,t1;
	uint8_t state = 0;
	volatile uint8_t  Byte_1th=0;
	volatile uint8_t  Byte_2th=0;
	volatile uint8_t  Byte_3th=0;
	volatile uint8_t  Byte_4th=0;
	volatile uint8_t  Byte_5th=0;
	volatile uint8_t  Byte_6th=0;
	volatile uint8_t  Byte_7th=0;
	uint32_t RetuData = 0;
	uint8_t  CTDATA[6]={0};
	
	while(1)
	{
			switch(state){
				case 1:{
					if((DHT20_Read_Status()&0x18)!=0x18){
						DHT20_Start_Init();
						osDelay(10);
					}
					state ++;
					break;
				}
				
				case 2:{
					DHT20_SendAC();
					state ++;
					osDelay(80);
					break;
				}
				
				case 3:{
					static uint16_t busyCnt = 0;
					if(((DHT20_Read_Status()&0x80)==0x80)){
						busyCnt ++;
						if(busyCnt >= 1000){
							busyCnt = 0;
							state = 1;
						}else{
							osDelay(1);
						}
						break;
					}
					
					state ++;
					break;
				}
				
				case 4:{
					I2C_Start();
					DHT20_WR_Byte(0x71);
					Receive_ACK();
					
					CTDATA[0]=Byte_1th = DHT20_RD_Byte();//???,??????0x98,??????,bit[7]?1;???0x1C,??0x0C,??0x08???????,bit[7]?0
					Send_ACK();
					CTDATA[1]=Byte_2th = DHT20_RD_Byte();//??
					Send_ACK();
					CTDATA[2]=Byte_3th = DHT20_RD_Byte();//??
					Send_ACK();
					CTDATA[3]=Byte_4th = DHT20_RD_Byte();//??/??
					Send_ACK();
					CTDATA[4]=Byte_5th = DHT20_RD_Byte();//??
					Send_ACK();
					CTDATA[5]=Byte_6th = DHT20_RD_Byte();//??
					Send_ACK();
					Byte_7th = DHT20_RD_Byte();//CRC??
					Send_NOT_ACK();                           //??: ?????NAK
					Stop_I2C();
					
					if(Calc_CRC8(CTDATA,6)==Byte_7th){
						RetuData = 0;
						RetuData = (RetuData|Byte_2th)<<8;
						RetuData = (RetuData|Byte_3th)<<8;
						RetuData = (RetuData|Byte_4th);
						RetuData =RetuData >>4;
						c1 = RetuData;//??
						RetuData = 0;
						RetuData = (RetuData|Byte_4th)<<8;
						RetuData = (RetuData|Byte_5th)<<8;
						RetuData = (RetuData|Byte_6th);
						RetuData = RetuData&0xfffff;
						t1 =RetuData; //??
						/* get humidity and temperatrue */
						Sensor.temperature = t1;
						Sensor.humidity = c1;
					}else{
						c1 = 0x00;
						t1 = 0x00;//???????,????????????
					}//CRC??
					
					state =2;
					osDelay(100);
					break;
				}
				
				case 0:
				default:{
					DHT20_GPIO_Init();
					state = 1;
					osDelay(500);
					break;
				}
			}
	 }
}

void DHT20_Task_Init(void)
{
	 DHT20TaskHandle = osThreadNew(DHT20_Task, NULL, &DHT20Task_attributes);
}

