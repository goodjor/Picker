#include "process.h"

void Collect_Data(void)
{

		//1.读取4个 DI 的状态
		data_collect.DI1_State = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
		data_collect.DI2_State = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
		data_collect.DI3_State = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
		data_collect.DI4_State = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
	
		//2.读取 ADC 值
		data_result.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
		data_result.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];	

		#if Model_Switch
		data_result.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[2];
		data_result.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[3];
		#endif	
	
		//3.滑动滤波
		data_result.ADC1_after_Filter1 = ADC_Filter(data_collect.ADC1_DataBuff1,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[0]);
		data_result.ADC1_after_Filter2 = ADC_Filter(data_collect.ADC1_DataBuff2,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[1]);

		#if Model_Switch
		data_result.ADC1_after_Filter3 = ADC_Filter(data_collect.ADC1_DataBuff3,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[2]);
		data_result.ADC1_after_Filter4 = ADC_Filter(data_collect.ADC1_DataBuff4,101-data_storage.Sensitivity,data_result.ADC1_Val_Buffer[3]);
		#endif	
		
		//4.计算输入电压
		data_result.ADC1_Voltage_1 = data_result.ADC1_after_Filter1*3300*5/3/4096;
		data_result.ADC1_Voltage_2 = data_result.ADC1_after_Filter2*3300*5/3/4096;

		#if Model_Switch
		data_result.ADC1_Voltage_3 = data_result.ADC1_after_Filter1*3300*5/3/4096;
		data_result.ADC1_Voltage_4 = data_result.ADC1_after_Filter2*3300*5/3/4096;
		#endif
		
		//5.角度传感器最大最小值校准
		if((bit_flag.Calibration_Flag ==0) && bit_flag.Calibration_Delay_Falg)
		{
			//一直计算最大最小值
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
		//6.设定目标高度
		data_storage.Pid_Par.SetPoint = data_storage.Set_Height;
		
		//7.计算出各角度传感器实际高度，取定每个采头的最小高度为实际高度值

		data_result.ADC1_Height = Picker_Hieght_Process();
	
		//8.PID 计算偏差  判断 偏差正负,来确定采头上下
		data_result.PWM_Value = data_result.PWM_Value + PIDCalc(&data_storage.Pid_Par , data_result.ADC1_Height);
		//1)当误差的绝对值 在|data_storage.Pid_Par.Error| 误差范围内，设置PWM1和PWM2 占空比为0，即不调节比例阀
		if((data_storage.Pid_Par.LastError < data_storage.Pid_Par.Error) && (data_storage.Pid_Par.LastError > -data_storage.Pid_Par.Error))
		{
			out_par.PWM_Up_Value = 0;		
			out_par.PWM_Down_Value = 0;			
			out_par.Drive1_State = Drive_Off;
			
		}
		else
		{
			//2)当误差的绝对值 在|data_storage.Pid_Par.Error| 误差范围外，且为正，则控制下降比例阀；反之则控制上升比例阀，设置相应的 PWM占空比
			bit_flag.Up_Or_Down_Flag = (data_storage.Pid_Par.LastError > 0) ? 1 : 0;
			switch(bit_flag.Up_Or_Down_Flag)
			{
				//升
				case 0 : out_par.PWM_Up_Value = (out_par.PWM_Up_Value - data_result.PWM_Value) + 300;//比例系数k还未设置
			
								 out_par.PWM_Down_Value = 0;
								 
				
				break;
				//降
				case 1 : out_par.PWM_Down_Value = (out_par.PWM_Down_Value + data_result.PWM_Value) + 200;
			
								 out_par.PWM_Up_Value = 0;	
								 
							 
				break;
				default:
				printf("采头控制出错 \r\n");
				break;	
			}
		}
	
}


void Process_Data(void)
{	
		
		//1.选择操作的采头
		bit_flag.L_R_Picker_Choose_Flag = Picker_LR_Choose(data_collect.DI4_State);

		//2.判断仿形开关是否开启
		bit_flag.Auto_Manual_Flag = State_AutoManual_Choose(data_collect.DI3_State);
	

		//3.自动仿形 与 手动仿形切换条件
		if(bit_flag.Auto_Manual_Flag == Auto_On)
		{
		//判断电磁阀的开合，采头下降PWM值为0时，电磁阀关，反之，则开
				if( out_par.PWM_Up_Value != 0 )//上升占空比不为0，则关闭下降电磁阀
				{
					out_par.Drive1_State = Drive_Off;
			
				}
				else if( out_par.PWM_Down_Value != 0 )//下降占空比不为0，则开启下降电磁阀
				{
					out_par.Drive1_State = Drive_On;
			
				}
			
		}
		else if(bit_flag.Auto_Manual_Flag == Manual_On)//手动控制
		{
			if(data_collect.DI1_State == 0)				//降
			{
				out_par.Drive1_State = Drive_On;
				out_par.PWM_Down_Value = data_storage.PWM_Value_Down;
				out_par.PWM_Up_Value = 0;
				bit_flag.Manual_Down = 0;//降标志位有效
				bit_flag.Manual_Up = 1;
				
			}
			else if(data_collect.DI2_State == 0)  //升
			{
				out_par.Drive1_State = Drive_Off;
				out_par.PWM_Up_Value = data_storage.PWM_Value_Up;				
				out_par.PWM_Down_Value = 0;
				bit_flag.Manual_Down = 1;
				bit_flag.Manual_Up = 0;//升标志位有效
			}
			else 
			{
				out_par.Drive1_State = Drive_Off;  //电磁阀关闭
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;
				bit_flag.Manual_Down = 0;
				bit_flag.Manual_Up = 0;//升标志位有效
			}
			
			
		}


	
}


//控制输出 控制电磁阀和比例阀（ PWM）的输出
void Control_Out(void)
{
	Drive_Out(out_par.Drive_Channel_1,out_par.Drive1_State);
	Drive_Out(out_par.Drive_Channel_2,out_par.Drive2_State);
	PWM_Out(out_par.PWM_Channel_Up,out_par.PWM_Up_Value);
	PWM_Out(out_par.PWM_Channel_Down,out_par.PWM_Down_Value);
	
}


//PWM输出
void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value)
{
	switch(PWM_Channel_Choose)
	{
		case 1:__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, PWM_Value);//上升比例阀
		
		break;
		
		case 2:__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, PWM_Value);//下降比例阀
		
		break;
		
		default:
		break;
		
	}
	
}


//电磁阀1、2 选择输出
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State)
{
	switch(Drive_Choose)
	{
		case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,State);//电磁阀1
		
		break;
		
		case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,State);//电磁阀2
		
		break;
		
		default:
		break;
		
	}
}


//默认值初始化
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
	data_storage.PWM_Value_Up = Default_PWM_Value_Up;			//手动上升占空比
	data_storage.PWM_Value_Down = Default_PWM_Value_Down;		//手动下降占空比
	bit_flag.Manual_Down = 1;
	bit_flag.Manual_Up = 1;//升和降标志位无效
	bit_flag.Calibration_Flag = 0;
	bit_flag.Calibration_Delay_Falg = 0;
	Real_Height_Init();
}


//限定采头高度范围  在设定的最大值与 最小值之间
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


//根据DI的输入状态，选择左右采头
uint8_t Picker_LR_Choose(uint8_t DI_Value)
{
	
	int value;

		if(DI_Value == 0)
		{
			value = 1;  //右采头有效
					
		}
		else 
		{
			value = 0;	//左采头有效
		}
	
	return value;
}

//根据DI的输入状态，确定仿形的工作模式：自动仿形 和 手动仿形。
uint8_t State_AutoManual_Choose(uint8_t DI_Value)
{
	
	int value;

	if(DI_Value == 0)
	{
		value = Auto_On;  //自动仿形
					
	}
	else 
	{
		value = Manual_On;	//手动模式
	}
		

	return value;
}


//求最小值
uint16_t Min(uint16_t value1,uint16_t value2)
{


	return (value1 < value2 ? value1:value2);

}


//求最大值
uint16_t Max(uint16_t value1,uint16_t value2)
{


	return (value1 > value2 ? value1:value2);

}


//左右采头高度计算
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

//采头四个传感器的高度初始化
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
