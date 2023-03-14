#include "process.h"


void Data_Collect(void)
{
	//1.��ȡ DI ֵ
	shift_geer.data_collect.Low_Speed = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
	shift_geer.data_collect.High_Speed = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
	shift_geer.data_collect.Neutral = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
//	printf("shift_geer.data_collect.Low_Speed = %d \r\n",shift_geer.data_collect.Low_Speed);
//	DI4_Value = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
	
	//2.��ȡ ADC ֵ
	shift_geer.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
//	shift_geer.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];	
	
	
	shift_geer.ADC1_after_Filter1 = ADC_Filter(data_collect.ADC1_DataBuff1,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[0]);
//	shift_geer.ADC1_after_Filter2 = ADC_Filter(data_collect.ADC1_DataBuff2,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[1]);	
	
	
	shift_geer.Real_Pressure = shift_geer.ADC1_after_Filter1 *600*3.3/3/4096;//��λ0.1MP��0-5V��Ӧ0-60Mp
//	printf("a=%d,f=%d,p=%d\r\n",shift_geer.ADC1_Val_Buffer[0],shift_geer.ADC1_after_Filter1,shift_geer.Real_Pressure);

	
}

//void Data_Process(void)
//{
//	//1���ж�������λ�Ĺ���״̬

//	
//	
////	printf("shift_geer.Low_Speed_Effict = %d \r\n",shift_geer.Low_Speed_Effict);
////	printf("shift_geer.High_Speed_Effict = %d \r\n",shift_geer.High_Speed_Effict);
////	printf("shift_geer.Neutral_Effict = %d \r\n",shift_geer.Neutral_Effict);
////	printf("shift_geer.FWD_Effict = %d \r\n",shift_geer.FWD_Effict);
//	
//	
//	//2���˲�

//	

//}

/*
����������ʱ��ռ�ձ�100%
ռ�ձ�ԼС��������Խ��
*/
void ControlLoop(void)
{
	uint16_t setPress,maxPress;
	uint16_t realPress;
	int pwm_value;
	float Kp;
	
	maxPress  = shift_geer.Contol_Pressure;
	setPress  = maxPress-30;//�ȶ��ڱ��趨ֵС5Mp������趨Ϊ25Mp�������Ŀ��Ϊ22Mp
	realPress = shift_geer.Real_Pressure;
	pwm_value = shift_geer.PWM_Value;
	Kp = shift_geer.Proportion_Value/10.0;
	
	if(realPress<=maxPress && pwm_value==1000)
	{//
		shift_geer.FWD=0;//ǰ��	
	}
	else
	{
		shift_geer.FWD=1;//����		
	}
	
	if(shift_geer.FWD==1)
	{
		pwm_value+= Kp*(setPress-realPress);//ѹ����λ0.1Mp
		if(pwm_value>=1000)pwm_value=1000;
		if(pwm_value<0)pwm_value=0;
	} 
	
	shift_geer.PWM_Value = pwm_value;
}


void Contol_Out(void)
{	
	PWM_Out(shift_geer.PWM_Channel,shift_geer.PWM_Value);	
}


void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value)
{
	switch(PWM_Channel_Choose)
	{
		case 1:__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, PWM_Value);//������1
		
		break;
		
		case 2:__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, PWM_Value);//������2
		
		break;
		
		default:
		break;
		
	}
	
}

void Default_Data_Init(void)
{
	shift_geer.PWM_Channel = Channel_1;
	shift_geer.Write_EEPROM_Flag = 0;
	data_collect.Filter_Len = Default_Filter_Len;
	shift_geer.PWM_Value =1000;	//ռ�ձ�100% ����Ĭ�Ϲرպ���
}

