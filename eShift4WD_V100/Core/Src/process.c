#include "process.h"


void Data_Collect(void)
{
	//1.读取 DI 值
	shift_geer.data_collect.Low_Speed = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
	shift_geer.data_collect.High_Speed = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
	shift_geer.data_collect.Neutral = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
//	printf("shift_geer.data_collect.Low_Speed = %d \r\n",shift_geer.data_collect.Low_Speed);
//	DI4_Value = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
	
	//2.读取 ADC 值
	shift_geer.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
//	shift_geer.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];	
	
	
	shift_geer.ADC1_after_Filter1 = ADC_Filter(data_collect.ADC1_DataBuff1,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[0]);
//	shift_geer.ADC1_after_Filter2 = ADC_Filter(data_collect.ADC1_DataBuff2,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[1]);	
	
	
	shift_geer.Real_Pressure = shift_geer.ADC1_after_Filter1 *600*3.3/3/4096;//单位0.1MP，0-5V对应0-60Mp
//	printf("a=%d,f=%d,p=%d\r\n",shift_geer.ADC1_Val_Buffer[0],shift_geer.ADC1_after_Filter1,shift_geer.Real_Pressure);

	
}

//void Data_Process(void)
//{
//	//1、判断三个档位的工作状态

//	
//	
////	printf("shift_geer.Low_Speed_Effict = %d \r\n",shift_geer.Low_Speed_Effict);
////	printf("shift_geer.High_Speed_Effict = %d \r\n",shift_geer.High_Speed_Effict);
////	printf("shift_geer.Neutral_Effict = %d \r\n",shift_geer.Neutral_Effict);
////	printf("shift_geer.FWD_Effict = %d \r\n",shift_geer.FWD_Effict);
//	
//	
//	//2、滤波

//	

//}

/*
后驱不作用时，占空比100%
占空比约小，后驱力越大
*/
void ControlLoop(void)
{
	uint16_t setPress,maxPress;
	uint16_t realPress;
	int pwm_value;
	float Kp;
	
	maxPress  = shift_geer.Contol_Pressure;
	setPress  = maxPress-30;//稳定在比设定值小5Mp；如果设定为25Mp，则控制目标为22Mp
	realPress = shift_geer.Real_Pressure;
	pwm_value = shift_geer.PWM_Value;
	Kp = shift_geer.Proportion_Value/10.0;
	
	if(realPress<=maxPress && pwm_value==1000)
	{//
		shift_geer.FWD=0;//前驱	
	}
	else
	{
		shift_geer.FWD=1;//四驱		
	}
	
	if(shift_geer.FWD==1)
	{
		pwm_value+= Kp*(setPress-realPress);//压力单位0.1Mp
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
		case 1:__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, PWM_Value);//比例阀1
		
		break;
		
		case 2:__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, PWM_Value);//比例阀2
		
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
	shift_geer.PWM_Value =1000;	//占空比100% 开机默认关闭后驱
}

