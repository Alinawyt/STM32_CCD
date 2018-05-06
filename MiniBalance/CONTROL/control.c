#include "control.h"	


int Encoder_Left,Encoder_Right;    //���ұ��������������
int Target_Left=10,Target_Right=10;      //Ŀ���ٶȣ����ڵ��PID
extern int Moto1,Moto2;            //���PWM���� 	
extern u8 Black_Center;

void TIM5_IRQHandler(void)   //TIM2�ж�
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
        
        Encoder_Left=Read_Encoder(2);                                      //===��ȡ��������ֵ
		Encoder_Right=Read_Encoder(4);                                      //===��ȡ��������ֵ 
        Moto1=Incremental_PI_A(Encoder_Left,Target_Left)-CCD_PID(); //===�ٶȱջ����Ƽ�����A����PWM
		Moto2=Incremental_PI_B(Encoder_Right,Target_Right)+CCD_PID();
        Xianfu_Pwm();                                                       //===PWM�޷�
		Set_Pwm(Moto1,Moto2); //===��ֵ��PWM�Ĵ���
	}
}


//ccd��ȡ���߷��������������PD����
int kerror[2]={0,0};
float CCD_kp=16.8,CCD_kd=5.8;
int CCD_PID(void)
{
	float Sum=0;
	kerror[0]=(Black_Center-(IMAGE_DATA/2));
	Sum = CCD_kp*kerror[0]+CCD_kd*(kerror[0]-kerror[1]);
	kerror[1] = kerror[0];
	return (int)Sum;
}


/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
	if(moto1>=0)	BIN2=0,			BIN1=1;
	else 	    BIN2=1,			BIN1=0;
		PWMB=myabs(moto1);   //����
	if(moto2>=0)	AIN1=0,			AIN2=1;
	else        AIN1=1,			AIN2=0;
		PWMA=myabs(moto2);	 //����
}


/**************************************************************************
�������ܣ�����PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
��������ʽ��ɢPID��ʽ 
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  �Դ����� 
pwm�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
float Velocity_KP=12,Velocity_KI=12;	       //�ٶȿ���PID����
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //����ƫ��
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
	 Last_bias=Bias;	                   //������һ��ƫ�� 
	 return Pwm;                         //�������
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //����ƫ��
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //����ʽPI������
	 Last_bias=Bias;	                   //������һ��ƫ�� 
	 return Pwm;                         //�������
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	int Amplitude=6900;    //===PWM������7200 ������6900
    
    if(Moto1<-Amplitude) 
        Moto1=-Amplitude;	
	if(Moto1>Amplitude)  
        Moto1=Amplitude;	
	if(Moto2<-Amplitude) 
        Moto2=-Amplitude;	
	if(Moto2>Amplitude)  
        Moto2=Amplitude;		
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  
        temp=-a;  
	else 
        temp=a;
	return temp;
}


