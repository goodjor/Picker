#include "process.h"

void Collect_Data(void)
{

		//1.��ȡ4�� DI ��״̬
		data_collect.DI1_State = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
		data_collect.DI2_State = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
		data_collect.DI3_State = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
		data_collect.DI4_State = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
	
		//2.��ȡ ADC ֵ
		data_result.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
		data_result.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];	

		#if Model_Switch
		data_result.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[2];
		data_result.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[3];
		#endif	
	
		//3.�����˲�
		data_result.ADC1_after_Filter1 = ADC_Filter(data_collect.ADC1_DataBuff1,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[0]);
		data_result.ADC1_after_Filter2 = ADC_Filter(data_collect.ADC1_DataBuff2,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[1]);

		#if Model_Switch
		data_result.ADC1_after_Filter3 = ADC_Filter(data_collect.ADC1_DataBuff3,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[2]);
		data_result.ADC1_after_Filter4 = ADC_Filter(data_collect.ADC1_DataBuff4,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[3]);
		#endif	
		
		//4.���������ѹ
		data_result.ADC1_Voltage_1 = data_result.ADC1_after_Filter1*3300*5/3/4096;
		data_result.ADC1_Voltage_2 = data_result.ADC1_after_Filter2*3300*5/3/4096;

		#if Model_Switch
		data_result.ADC1_Voltage_3 = data_result.ADC1_after_Filter1*3300*5/3/4096;
		data_result.ADC1_Voltage_4 = data_result.ADC1_after_Filter2*3300*5/3/4096;
		#endif
		
		//5.�Ƕȴ����������СֵУ׼
		if((bit_flag.Calibration_Flag ==0) && bit_flag.Calibration_Delay_Falg)
		{
			//һֱ���������Сֵ
				data_storage.Picker_Real.Left1_Min = Min(data_result.ADC1_after_Filter1,data_storage.Picker_Real.Left1_Min);
				data_storage.Picker_Real.Left1_Max = Max(data_result.ADC1_after_Filter1,data_storage.Picker_Real.Left1_Max);
				data_storage.Picker_Real.Right1_Min = Min(data_result.ADC1_after_Filter2,data_storage.Picker_Real.Right1_Min);
				data_storage.Picker_Real.Right1_Max = Max(data_result.ADC1_after_Filter2,data_storage.Picker_Real.Right1_Max);
		#if Model_Switch
				data_storage.Picker_Real.Left2_Min = Min(data_result.ADC1_Val_Buffer[0],data_storage.Picker_Real.Left2_Min);
				data_storage.Picker_Real.Left2_Max = Max(data_result.ADC1_Val_Buffer[0],data_storage.Picker_Real.Left2_Max);
				data_storage.Picker_Real.Right2_Min = Min(data_result.ADC1_Val_Buffer[1],data_storage.Picker_Real.Right2_Min);
				data_storage.Picker_Real.Right2_Max = Max(data_result.ADC1_Val_Buffer[1],data_storage.Picker_Real.Right2_Max);			
		#endif 

		}
		//6.�趨Ŀ��߶�
		data_storage.Pid_Par.SetPoint = data_storage.Set_Height;
		
		//7.��������Ƕȴ�����ʵ�ʸ߶ȣ�ȡ��ÿ����ͷ����С�߶�Ϊʵ�ʸ߶�ֵ

		data_result.ADC1_Height = Picker_Hieght_Process();
	
		//8.PID ����ƫ��  �ж� ƫ������,��ȷ����ͷ����
		data_result.PWM_Value = data_result.PWM_Value + PIDCalc(&data_storage.Pid_Par , data_result.ADC1_Height);
		//1)�����ľ���ֵ ��|data_storage.Pid_Par.Error| ��Χ�ڣ�����PWM1��PWM2 ռ�ձ�Ϊ0���������ڱ�����
		if((data_storage.Pid_Par.LastError < data_storage.Pid_Par.Error) && (data_storage.Pid_Par.LastError > -data_storage.Pid_Par.Error))
		{
			out_par.PWM_Up_Value = 0;		
			out_par.PWM_Down_Value = 0;			
			out_par.Drive1_State = Drive_Off;
			
		}
		else
		{
			//2)�����ľ���ֵ ��|data_storage.Pid_Par.Error| ��Χ�⣬��Ϊ����������½�����������֮�����������������������Ӧ�� PWMռ�ձ�
			bit_flag.Up_Or_Down_Flag = (data_storage.Pid_Par.LastError > 0) ? 1 : 0;
			switch(bit_flag.Up_Or_Down_Flag)
			{
				//��
				case 0 : out_par.PWM_Up_Value = (out_par.PWM_Up_Value - data_result.PWM_Value) + 300;//����ϵ��k��δ����
			
								 out_par.PWM_Down_Value = 0;
								 
				
				break;
				//��
				case 1 : out_par.PWM_Down_Value = (out_par.PWM_Down_Value + data_result.PWM_Value) + 200;
			
								 out_par.PWM_Up_Value = 0;	
								 
							 
				break;
				default:
				printf("��ͷ���Ƴ��� \r\n");
				break;	
			}
		}
	
}


void Process_Data(void)
{	
		
		//1.ѡ������Ĳ�ͷ
		bit_flag.L_R_Picker_Choose_Flag = Picker_LR_Choose(data_collect.DI4_State);

		//2.�жϷ��ο����Ƿ���
		bit_flag.Auto_Manual_Flag = State_AutoManual_Choose(data_collect.DI3_State);
	

		//3.�Զ����� �� �ֶ������л�����
		if(bit_flag.Auto_Manual_Flag == Auto_On)
		{
		//�жϵ�ŷ��Ŀ��ϣ���ͷ�½�PWMֵΪ0ʱ����ŷ��أ���֮����
				if( out_par.PWM_Up_Value != 0 )//����ռ�ձȲ�Ϊ0����ر��½���ŷ�
				{
					out_par.Drive1_State = Drive_Off;
			
				}
				else if( out_par.PWM_Down_Value != 0 )//�½�ռ�ձȲ�Ϊ0�������½���ŷ�
				{
					out_par.Drive1_State = Drive_On;
			
				}
			
		}
		else if(bit_flag.Auto_Manual_Flag == Manual_On)//�ֶ�����
		{
			if(data_collect.DI1_State == 0)				//��
			{
				out_par.Drive1_State = Drive_On;
				out_par.PWM_Down_Value = data_storage.PWM_Value_Down;
				out_par.PWM_Up_Value = 0;
				bit_flag.Manual_Down = 0;//����־λ��Ч
				bit_flag.Manual_Up = 1;
				
			}
			else if(data_collect.DI2_State == 0)  //��
			{
				out_par.Drive1_State = Drive_Off;
				out_par.PWM_Up_Value = data_storage.PWM_Value_Up;				
				out_par.PWM_Down_Value = 0;
				bit_flag.Manual_Down = 1;
				bit_flag.Manual_Up = 0;//����־λ��Ч
			}
			else 
			{
				out_par.Drive1_State = Drive_Off;  //��ŷ��ر�
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;
				bit_flag.Manual_Down = 0;
				bit_flag.Manual_Up = 0;//����־λ��Ч
			}
			
			
		}


	
}


//������� ���Ƶ�ŷ��ͱ������� PWM�������
void Control_Out(void)
{
	Drive_Out(out_par.Drive_Channel_1,out_par.Drive1_State);
	Drive_Out(out_par.Drive_Channel_2,out_par.Drive2_State);
	PWM_Out(out_par.PWM_Channel_Up,out_par.PWM_Up_Value);
	PWM_Out(out_par.PWM_Channel_Down,out_par.PWM_Down_Value);
	
}


//PWM���
void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value)
{
	switch(PWM_Channel_Choose)
	{
		case 1:__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, PWM_Value);//����������
		
		break;
		
		case 2:__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, PWM_Value);//�½�������
		
		break;
		
		default:
		break;
		
	}
	
}


//��ŷ�1��2 ѡ�����
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State)
{
	switch(Drive_Choose)
	{
		case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,State);//��ŷ�1
		
		break;
		
		case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,State);//��ŷ�2
		
		break;
		
		default:
		break;
		
	}
}


//Ĭ��ֵ��ʼ��
void Default_Data_Init(void)
{
	data_storage.Set_Height = 100; //
	data_storage.Sensitivity = 10;
	data_storage.Pid_Par.Proportion = 10;
	data_storage.Pid_Par.Integral = 1;
	data_storage.Pid_Par.Derivative = 1;
	data_storage.Pid_Par.PrevError = 0;
	data_storage.Pid_Par.LastError = 0;
	data_storage.Pid_Par.SumError =0;
	data_storage.Picker_Set.K1 = 0.045;
	data_storage.Picker_Set.K2 = 0.045;
	data_storage.Picker_Set.K3 = 0.045;
	data_storage.Picker_Set.K4 = 0.045;
	data_storage.Picker_Set.B = 50;
	data_storage.PWM_Value_Up = Default_PWM_Value_Up;			//�ֶ�����ռ�ձ�
	data_storage.PWM_Value_Down = Default_PWM_Value_Down;		//�ֶ��½�ռ�ձ�
	bit_flag.Manual_Down = 1;
	bit_flag.Manual_Up = 1;//���ͽ���־λ��Ч
	bit_flag.Calibration_Flag = 0;
	bit_flag.Calibration_Delay_Falg = 0;
	Real_Height_Init();
}


//�޶���ͷ�߶ȷ�Χ  ���趨�����ֵ�� ��Сֵ֮��
void Height_Judgment(uint16_t *hight_value,uint8_t LR)
{

						if(*hight_value <= data_storage.Picker_Set.Min)
						{
							*hight_value = data_storage.Picker_Set.Min;
							
						}
						else if(*hight_value >= data_storage.Picker_Set.Max)
						{
							*hight_value = data_storage.Picker_Set.Max;
							
						}

	
}


//����DI������״̬��ѡ�����Ҳ�ͷ
uint8_t Picker_LR_Choose(uint8_t DI_Value)
{
	
	int value;

		if(DI_Value == 0)
		{
			value = 1;  //�Ҳ�ͷ��Ч
					
		}
		else 
		{
			value = 0;	//���ͷ��Ч
		}
	
	return value;
}

//����DI������״̬��ȷ�����εĹ���ģʽ���Զ����� �� �ֶ����Ρ�
uint8_t State_AutoManual_Choose(uint8_t DI_Value)
{
	
	int value;

	if(DI_Value == 0)
	{
		value = Auto_On;  //�Զ�����
					
	}
	else 
	{
		value = Manual_On;	//�ֶ�ģʽ
	}
		

	return value;
}


//����Сֵ
uint16_t Min(uint16_t value1,uint16_t value2)
{


	return (value1 < value2 ? value1:value2);

}


//�����ֵ
uint16_t Max(uint16_t value1,uint16_t value2)
{


	return (value1 > value2 ? value1:value2);

}


//���Ҳ�ͷ�߶ȼ���
uint16_t Picker_Hieght_Process(void)
{
	uint16_t heigh;
	data_result.ADC1_Picker_L1_Height = data_storage.Picker_Set.K1*data_result.ADC1_after_Filter1 + data_storage.Picker_Set.B;
	data_result.ADC1_Picker_R1_Height = data_storage.Picker_Set.K2*data_result.ADC1_after_Filter2 + data_storage.Picker_Set.B;
	heigh = Min(data_result.ADC1_Picker_L1_Height,data_result.ADC1_Picker_R1_Height);
	#if Model_Switch
	data_result.ADC1_Picker_L2_Height = data_storage.Picker_Set.K3*data_result.ADC1_after_Filter3 + data_storage.Picker_Set.B;
	data_result.ADC1_Picker_R2_Height = data_storage.Picker_Set.K4*data_result.ADC1_after_Filter4 + data_storage.Picker_Set.B;
	heigh = Min(Min(data_result.ADC1_Picker_L2_Height,data_result.ADC1_Picker_R2_Height),heigh);
	#endif

	return heigh;
	
}


void Hegiht_Calibration(void)
{

	if(bit_flag.Calibration_Flag == 1)
	{

				data_storage.Picker_Set.Left1_Min = data_storage.Picker_Real.Left1_Min;
				data_storage.Picker_Set.Left1_Max = data_storage.Picker_Real.Left1_Max;
				data_storage.Picker_Set.Right1_Min = data_storage.Picker_Real.Right1_Min;
				data_storage.Picker_Set.Right1_Max = data_storage.Picker_Real.Right1_Max;
		
				data_storage.Picker_Set.K1 = (140-50)/(data_storage.Picker_Set.Left1_Max-data_storage.Picker_Set.Left1_Min );
				data_storage.Picker_Set.K2 = (140-50)/(data_storage.Picker_Set.Right1_Max-data_storage.Picker_Set.Right1_Min );
		
		#if Model_Switch
				data_storage.Picker_Set.Left2_Min = data_storage.Picker_Real.Left2_Min;
				data_storage.Picker_Set.Left2_Max = data_storage.Picker_Real.Left2_Max;
				data_storage.Picker_Set.Right2_Min = data_storage.Picker_Real.Right2_Min;
				data_storage.Picker_Set.Right2_Max = data_storage.Picker_Real.Right2_Max;
				data_storage.Picker_Set.K3 = (140-50)/(data_storage.Picker_Set.Left2_Max-data_storage.Picker_Set.Left2_Min );
				data_storage.Picker_Set.K4 = (140-50)/(data_storage.Picker_Set.Right2_Max-data_storage.Picker_Set.Right2_Min );		
		#endif

				bit_flag.Calibration_Success = 1;
				bit_flag.Calibration_Flag = 0;
				bit_flag.Write_EEPROM_Picker_Height_Flag = 1;
				printf("bit_flag.Calibration_Success = %d\r\n",bit_flag.Calibration_Success);
				if(bit_flag.Calibration_Success == 1)
				{
		
					CAN_Transmit_Calibration();
					HAL_Delay(10);
					bit_flag.Calibration_Success = 0;
				}
		
	}
	
}

//��ͷ�ĸ��������ĸ߶ȳ�ʼ��
void Real_Height_Init(void)
{
	data_storage.Picker_Real.Left1_Min = 4000;
	data_storage.Picker_Real.Left1_Max = 0;
	data_storage.Picker_Real.Right1_Min = 4000;
	data_storage.Picker_Real.Right1_Max = 0;
	#if Model_Switch
	data_storage.Picker_Real.Left2_Min = 4000;
	data_storage.Picker_Real.Left2_Max = 0;
	data_storage.Picker_Real.Right2_Min = 4000;
	data_storage.Picker_Real.Right2_Max = 0;	
	#endif

	
	
}
