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

uint8 him_uart_cmd[11] = {0x66,0x64,0x2E,0x76,0x61,0x6C,0x3D,0x31,0xff,0xff,0xff};    // 01 д��ɹ�  02 д��ʧ��   03 ����д��... 04 ��ʾhmi����
uint8 him_ret_status = 0;
uint8 hmi_data[10] = {0};
float FDC2214_Page_Data_Top[100]    = {0};
float FDC2214_Page_Data_Bottom[100] = {0};

/*----------------------- Function Implement --------------------------------*/

/* ���͸������� д���״̬
01:д��ɹ�
02:д��ʧ��
03:����д��
04:��ʾhmi����
*/
void Uart_Send_HMI(uint8 *cmd)
{

		him_uart_cmd[7] = *cmd; //�����Ӧд��
		rt_device_write(focus_uart_device, 0,him_uart_cmd	, 11);
		*cmd = 0; //��������
}

void FDC2214_Data_Adjust(void)//����У׼ �洢
{
		him_ret_status = 3;
		Uart_Send_HMI(&him_ret_status);//����д��
		rt_thread_mdelay(1000);
		FDC2214_Page_Data_Top   [HMI_Page_Number] = get_top_capacity();   //���� ��Ӧҳ ����ֵ����
		FDC2214_Page_Data_Bottom[HMI_Page_Number] = get_bottom_capacity();//�װ� ��Ӧҳ ����ֵ����
		rt_thread_mdelay(1000);
		him_ret_status = 1;
		//Uart_Send_HMI(&him_ret_status);//����д��


}




uint8 RxCheck = 0;	  //βУ����
uint8 RxCount = 0;	  //���ռ���

/**
  * @brief  HMI_Data_Analysis(�������������ݽ���)
  * @param  �����ַ����� uint8 Data
  * @retval None
  * @notice �ӵ��ĸ��ֽڿ�ʼΪ�����ֽ�
  */
void HMI_Data_Analysis(uint8 Data) //�������ݽ���
{

		static uint8 i = 0;	   		  //

		hmi_data[RxCount++] = Data;	//���յ������ݴ��뻺������
	
	
		if(RxCount <= (HMI_LEN+4)){ //�������ݳ���δ������ͷ�Ͱ���3���ֽ�,+4)  
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
				//rt_kprintf("HMI_Status_Flag:%d\n",HMI_Status_Flag);
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





