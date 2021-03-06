/*
 * drv_MS5837.c
 *
 *  Created on: 2019年3月30日
 *      Author: zengwangfa
 *       Notes: 水深传感器设备驱动
 *   Attention: SCL - E10 (黑色)   
 *							SDA - E12 (黄色)   
 */

#include "init.h"
#include "drv_MS5837.h"
#include "drv_i2c.h"
#include <math.h>
#include <rtthread.h>
#include "filter.h"


 
/*
C1 压力灵敏度 SENS|T1
C2  压力补偿  OFF|T1
C3	温度压力灵敏度系数 TCS
C4	温度系数的压力补偿 TCO
C5	参考温度 T|REF
C6 	温度系数的温度 TEMPSENS
*/


double OFF_;
float Aux;
/*
dT 实际和参考温度之间的差异
MS_TEMP 实际温度	
*/

int32 Cal_C[7];	        //用于存放PROM中的6组数据1-6
int64 dT;
float MS_TEMP,MS5837_Temperature;
int64 D1_Pres,D2_Temp;	// 数字压力值,数字温度值


/*
OFF 实际温度补偿
SENS 实际温度灵敏度
*/
uint64 SENS;
uint32 MS5837_Pressure;				//大气压
uint32 TEMP2,T2,OFF2,SENS2;	//温度校验值



/**
  * @brief  MS583703BA 复位
  * @param  None
  * @retval None
  */
void MS583703BA_RESET(void)
{
		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress);//CSB接地，主机地址：0XEE，否则 0X77
	  IIC_Wait_Ack();
    IIC_Send_Byte(MS583703BA_RST);//发送复位命令
	  IIC_Wait_Ack();
    IIC_Stop();
	
}
/**
  * @brief  MS5837_CRC4校验(4bit校验)
  * @param  MS5837 PROM标定参数数组
  * @retval 返回CRC校验码
  */
uint8 MS5837_CRC4(int32 *n_prom) // n_prom defined as 8x unsigned int (n_prom[8])
{
		int32  cnt; // simple counter
		uint32 n_rem=0; // crc remainder
		uint8  n_bit;
		n_prom[0]=((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
		n_prom[7]=0; // Subsidiary value, set to 0
		for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
		{ 			// choose LSB or MSB
				if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
				else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
				for (n_bit = 8; n_bit > 0; n_bit--)
				{
						if (n_rem & (0x8000)) n_rem = (n_rem << 1) ^ 0x3000;
						else n_rem = (n_rem << 1);
				}
		}
		n_rem= ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
		return (n_rem ^ 0x00);
}
/**
  * @brief  MS5837_Get_PROM
  * @param  None
  * @retval 返回MS5837_Get_PROM(出厂标定参数)是否成功标志：1成功，0失败
  */
uint8 MS5837_Get_PROM(void)
{	 
		uint8  inth,intl,i;
		uint8  CRC_Check = 0;
		for (i=0;i<=6;i++) 
		{
				IIC_Start();
				IIC_Send_Byte(MS583703BA_SlaveAddress);
				IIC_Wait_Ack();
				IIC_Send_Byte(0xA0 + (i*2));
				IIC_Wait_Ack();
				IIC_Stop();

				rt_thread_mdelay(5);
				IIC_Start();
				IIC_Send_Byte(MS583703BA_SlaveAddress+0x01);  //进入接收模式

				rt_thread_mdelay(1);
				IIC_Wait_Ack();
				inth = IIC_Read_Byte(1);  		//带ACK的读数据 ，带ACK就表示还需要读取
				
				rt_thread_mdelay(1);
				intl = IIC_Read_Byte(0); 			//最后一个字节NACK
				IIC_Stop();
				Cal_C[i] = (((uint16_t)inth << 8) | intl);
		}
		CRC_Check = (uint8)((Cal_C[0]&0xF000)>>12);
		if(CRC_Check == MS5837_CRC4(Cal_C)){
					return 1;
		}
		else {return 0;}
	
}
/**
  * @brief  MS5837_Init
  * @param  None
  * @retval 初始化是否成功标志：1成功，0失败
  */
uint8 MS5837_Init(void){
		
		IIC_Init();	
		rt_thread_mdelay(100);
		MS583703BA_RESET();	     
		rt_thread_mdelay(100);  
	
		if(1 == MS5837_Get_PROM()){
				return 1;
		}
		else {
				return 0;
		}
		
}


/**
  * @brief  MS583703BA转换结果
  * @param  命令值(温度、气压)
  * @retval 返回MS5837初始化是否成功标志：1成功，0失败
  */
uint64 MS583703BA_getConversion(uint8_t command)
{
 
			uint64 conversion = 0;
			uint8 temp[3];
	
	    IIC_Start();
			IIC_Send_Byte(MS583703BA_SlaveAddress); 		//写地址
			IIC_Wait_Ack();
			IIC_Send_Byte(command); //写转换命令
			IIC_Wait_Ack();
			IIC_Stop();

	    rt_thread_mdelay(20);  //读取8196转换值得关键，必须大于PDF-2页中的18.08毫秒
			IIC_Start();
			IIC_Send_Byte(MS583703BA_SlaveAddress); 		//写地址
			IIC_Wait_Ack();
			IIC_Send_Byte(0);				// start read sequence
			IIC_Wait_Ack();
			IIC_Stop();
		 
			IIC_Start();
			IIC_Send_Byte(MS583703BA_SlaveAddress+0x01);  //进入接收模式
			IIC_Wait_Ack();
			temp[0] = IIC_Read_Byte(1);  //带ACK的读数据  bit 23-16
			temp[1] = IIC_Read_Byte(1);  //带ACK的读数据  bit 8-15
			temp[2] = IIC_Read_Byte(0);  //带NACK的读数据 bit 0-7
			IIC_Stop();
			
			conversion = (uint64)temp[0] <<16 | (uint64)temp[1] <<8 | (uint64)temp[2];
			return conversion;
 
}


/**
  * @brief  MS583703BA转换温度结果
  * @param  None
  * @retval None
  */
void MS583703BA_getTemperature(void)
{
	
	D2_Temp = MS583703BA_getConversion(MS583703BA_D2_OSR_2048);

	dT=D2_Temp - (((uint32_t)Cal_C[5])*256);
	MS_TEMP=2000+dT*((uint32_t)Cal_C[6])/8388608;  //问题在于此处没有出现负号
}

/**
  * @brief  MS583703BA转换气压结果
  * @param  None
  * @retval None
  */
void MS583703BA_getPressure(void)
{
		D1_Pres= MS583703BA_getConversion(MS583703BA_D1_OSR_8192);

		
		OFF_=(uint32_t)Cal_C[2]*65536+((uint32_t)Cal_C[4]*dT)/128;
		SENS=(uint32_t)Cal_C[1]*32768+((uint32_t)Cal_C[3]*dT)/256;

		if(MS_TEMP<2000)  // 低于20℃时
		{
				Aux = (2000-MS_TEMP)*(2000-MS_TEMP);
				T2 = 3*(dT*dT) /0x80000000; 
				OFF2 = (uint32)1.5*Aux;
				SENS2 = 5*Aux/8;
				
				OFF_ = OFF_ - OFF2;
				SENS = SENS - SENS2;	
		}
		else{
				T2=2*(dT*dT)/137438953472;
				OFF2 = 1*Aux/16;
				SENS2 = 0;
				OFF_ = OFF_ - OFF2;
				SENS = SENS - SENS2;			 
		}
		MS5837_Pressure= ((D1_Pres*SENS/2097152-OFF_)/4096)/10;
		MS5837_Temperature=(MS_TEMP-T2)/100;
}

uint32 res_value[10] = {0};

uint32 get_ms5837_pressure(void)
{		
		uint32 ms5837_value = 0;
	
		for(char i = 0;i < 10;i++){  //先行获取 10次数据以防数据出错
				MS583703BA_getTemperature();//获取外部温度
				MS583703BA_getPressure();   //获取水压
				rt_thread_mdelay(5); //50ms
				
				res_value[i] = MS5837_Pressure; 
		}
		ms5837_value = Bubble_Filter(res_value);
		//rt_kprintf("ms5837_init_value: %d",ms5837_init_value);
		return ms5837_value;
}

float get_ms5837_temperature(void)
{
		return MS5837_Temperature;
}


