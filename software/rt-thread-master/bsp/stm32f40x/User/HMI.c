/*
 * HMI.c
 *
 *  Created on: 2019��8��7��
 *      Author: zengwangfa
 *      Notes:  ������ͨ��
 */

#include "HMI.h"
#include "DataType.h"
#include <rtthread.h>
#include "uart.h"
#include "DataProcess.h"
/*---------------------- Constant / Macro Definitions -----------------------*/

#define HMI_LEN 5

/*----------------------- Variable Declarations -----------------------------*/

uint8 hmi_data_ok = 0;
int HMI_Status_Flag = 0;//������ �趨״̬��־λ ������ 1��or������2��
int HMI_Page_Number = 0;//���������͵�У׼  ֽ����


/*----------------------- Function Implement --------------------------------*/
uint8 hmi_data[10] = {0};
float FDC2214_Page_Data_Top[100] = {0};
float FDC2214_Page_Data_Bottom[100] = {0};


void FDC2214_Data_Adjust(void)//����У׼
{
		if(1 == HMI_Status_Flag)//��ʼУ׼
		{
				FDC2214_Page_Data_Top   [HMI_Page_Number] = get_top_capacity();   //���� ��Ӧҳ ����ֵ����
				FDC2214_Page_Data_Bottom[HMI_Page_Number] = get_bottom_capacity();//�װ� ��Ӧҳ ����ֵ����
				
				HMI_Status_Flag = 0; //ʹ�������� Flag
		}
		

}



/* ���͸������� �Ƿ� У׼�ɹ�*/
void Uart_Send_HMI(uint8 *data)
{

		rt_device_write(focus_uart_device, 0,data, 1);;

}


/**
  * @brief  HMI_Data_Analysis(�佹����ͷ�������ݽ���)
  * @param  �����ַ����� uint8 Data
  * @retval None
  * @notice �ӵ��ĸ��ֽڿ�ʼΪ�����ַ�
  */
void HMI_Data_Analysis(uint8 Data) //�������ݽ���
{
		static uint8 RxCheck = 0;	  //βУ����
		static uint8 RxCount = 0;	  //���ռ���
		static uint8 i = 0;	   		  //

		hmi_data[RxCount++] = Data;	//���յ������ݴ��뻺������
	
	
		if(RxCount <= (HMI_LEN)){ //�������ݳ���δ������ͷ�Ͱ���3���ֽ�,+4)  
				if(hmi_data[0] == 0xAA){ //���յ���ͷ0xAA
						if(RxCount > 3){
								if(hmi_data[1] == 0x55){ //���յ���ͷ0x55
										if(RxCount >= hmi_data[2]+4){ //���յ����ݰ�����λ����ʼ�ж�ʲôʱ��ʼ����У��
												for(i = 0;i <= (RxCount-2);i++){ //�ۼӺ�У��
														RxCheck += hmi_data[i];
												}
			
												if(RxCheck == hmi_data[RxCount-1]){
														hmi_data_ok = 1; //�������ݰ��ɹ�
												}
												else {hmi_data_ok = 0;}
												
												RxCheck = 0; //�����������
												RxCount = 0;	
										}
								}
								else {hmi_data_ok = 0;RxCount = 0;hmi_data_ok = 0;} //���ղ��ɹ�����
						}
				}
				else {hmi_data_ok = 0;RxCount = 0;hmi_data_ok = 0;} //���ղ��ɹ�����
		}
		else {hmi_data_ok = 0;RxCount = 0;hmi_data_ok = 0;} //���ղ��ɹ�����

		
		if(1 == hmi_data_ok){
				HMI_Status_Flag = hmi_data[3];//��ȡ ����ģʽλ
				HMI_Page_Number = hmi_data[4];//��ȡ У׼ҳ��
				
		}
		else{
		
		
		}
		hmi_data_ok = 0;
}


uint32 get_set_page_number(void)
{
		return HMI_Page_Number;
}

uint32 get_set_status(void)
{
		return HMI_Status_Flag;
}





