#include "process.h"


void Data_Collect(void)
{
	//1.读取 DI 值
	shift_geer.data_collect.Low_Speed = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
	shift_geer.data_collect.High_Speed = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
	shift_geer.data_collect.Neutral = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
	printf("shift_geer.data_collect.Low_Speed = %d \r\n",shift_geer.data_collect.Low_Speed);
//	DI4_Value = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
	
	//2.读取 ADC 值
	shift_geer.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
	shift_geer.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];
	printf("shift_geer.ADC1_Val_Buffer[0] = %d \r\n",shift_geer.ADC1_Val_Buffer[0]);
	printf("shift_geer.ADC1_Val_Buffer[1] = %d \r\n",shift_geer.ADC1_Val_Buffer[1]);
	
}

void Data_Process(void)
{
	//1、判断三个档位的工作状态

	
	
//	printf("shift_geer.Low_Speed_Effict = %d \r\n",shift_geer.Low_Speed_Effict);
//	printf("shift_geer.High_Speed_Effict = %d \r\n",shift_geer.High_Speed_Effict);
//	printf("shift_geer.Neutral_Effict = %d \r\n",shift_geer.Neutral_Effict);
//	printf("shift_geer.FWD_Effict = %d \r\n",shift_geer.FWD_Effict);
	
	
	//2、滤波
		shift_geer.ADC1_after_Filter1 = ADC_Filter(data_collect.ADC1_DataBuff1,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[0]);
		shift_geer.ADC1_after_Filter2 = ADC_Filter(data_collect.ADC1_DataBuff2,data_collect.Filter_Len,shift_geer.ADC1_Val_Buffer[1]);
	

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
	
}

void Geer_Can_Data(void)
{
		//生成 档位 CAN 数据
	shift_geer.Level = (0x01 & shift_geer.data_collect.Neutral) | 
											(0x02 & (shift_geer.data_collect.High_Speed<<1)) | 
											(0x04 & (shift_geer.data_collect.Low_Speed<<2)) |
											(0x08 & (shift_geer.FWD<<3));
	printf("shift_geer.Level = %d \r\n",shift_geer.Level);
	
	
}

