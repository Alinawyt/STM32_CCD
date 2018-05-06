#include "stm32f10x.h"
#include "sys.h"
#include "control.h"
#include "calculation.h"
#include "Landzo_CCD.h"

u8 Pixel[128];
u8 Image_Data1[128]={0};
u8 Image_Data2[128]={0};
u8 Image_Data3[128]={0};
u8 Black_Left=0,Black_Right=127,Black_Center=IMAGE_DATA/2;

int Moto1,Moto2;                      //���PWM����	

extern u8 TIME1flag_1ms;
extern u8 TIME1flag_20ms;

int main(void)
{
    u8 count=0;  
    
    MY_NVIC_PriorityGroupConfig(2);	//=====�жϷ���
	delay_init();	    	        //=====��ʱ������ʼ��	
	uart_init(115200);	            //=====���ڳ�ʼ��Ϊ  

	LED_Init();						//=====LED��ʼ��
	
    MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ������������� �����ʼ������ӿ�  A8 A11   B12-B15
	Encoder_Init_TIM2();            //=====��ʼ��������1    A0 A1
	Encoder_Init_TIM4();            //=====��ʼ��������2    B6 B7
	TIM5_Int_Init(49,7199);         //72M/7200=10Khz�ļ���Ƶ�ʣ�������50Ϊ5ms,����ִ�л������PID������
    
    TIM3_Int_Init(999,72);//1ms��ʱ������CCD�ع�ʱ�����
    LandzoCCD_init();
    
    //OLED_Init();                    //=====OLED��ʼ��
    
    while(1)
    {     	
		//**********���ݲɼ�**********//
		if(TIME1flag_20ms==1)
		{
			TIME1flag_20ms=0;
			if(count==0)
			{			
				ImageCapture(Image_Data1);
				CalculateIntegrationTime(Image_Data1);
			}
			else if(count==1)
			{
				ImageCapture(Image_Data2);
				CalculateIntegrationTime(Image_Data2);
			}
			else if(count==2)
			{
				ImageCapture(Image_Data3);
				CalculateIntegrationTime(Image_Data3);
			}
			count++;
		}
		if(count==3)
		{
            LED0=~LED0;   //LED��˸��ʾ������������
			count=0;
			//**********3��ƽ��ֵ�˲�**********//
			for(int j=0;j<IMAGE_DATA;j++)
			{
				Pixel[j]=(Image_Data1[j]+Image_Data2[j]+Image_Data3[j])/3;
			}
//			//**********���ڷ���ͼ������**********//
//			SendImageData(Pixel);   //���ڷ������βɼ��˲����Ч��ͼ
			//**********ͼ�����ݴ���**********//
			Image_Data_Search_Line(IMAGE_DATA,Pixel);
        }
//        SendImageData(Pixel);   //���ڷ���ͼ����ֵ��������ͼ
//	    oled_show();              //===��ʾ���� 	  	 
    }	 
}

