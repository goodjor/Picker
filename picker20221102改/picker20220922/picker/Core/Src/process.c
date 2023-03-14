#include "process.h"

/********���ݲɼ�����********/
void Collect_Data(void)
{
		uint8_t i=0;									//����forѭ���ɼ��ĸ�����������
		static uint16_t timeCnt=0;	//������ʱ����

		//1.��ȡ4�� DI ��״̬
		data_collect.DI1_State = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
		data_collect.DI2_State = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
		data_collect.DI3_State = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
		data_collect.DI4_State = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
		//���Ҳ�ͷѡ��
		bit_flag.L_R_Picker_Choose_Flag = data_collect.DI4_State;		//0�ң�1��
		//�жϷ��ο����Ƿ���
		bit_flag.Auto_Manual_Flag = data_collect.DI3_State;					//0���ο���1��
		//�ֶ��������
		bit_flag.Manual_Down 	= data_collect.DI1_State ;						//0����־λ��Ч
		bit_flag.Manual_Up 		= data_collect.DI2_State;							//0������־λ��Ч
	
		#if DEBUG 
		printf("DI1_State = %d\r\n 0����־λ��Ч	 \r\n",data_collect.DI1_State);
		printf("DI2_State = %d\r\n 0������־λ��Ч\r\n",data_collect.DI2_State);
		printf("DI3_State = %d\r\n 0���ο���1��	 \r\n",data_collect.DI3_State);
		printf("DI4_State = %d\r\n 0�ң�1��			 \r\n",data_collect.DI4_State);
		#endif
	
		//2.��ȡ ADC ֵ
		data_result.ADC1_Val_Buffer[0] = data_collect.ADC1_Val_Buffer[0];
		data_result.ADC1_Val_Buffer[1] = data_collect.ADC1_Val_Buffer[1];	
		data_result.ADC1_Val_Buffer[2] = data_collect.ADC1_Val_Buffer[2];
		data_result.ADC1_Val_Buffer[3] = data_collect.ADC1_Val_Buffer[3];
//		printf("ADC1_Val_Buffer[1] =  %d,\r\n",data_collect.ADC1_Val_Buffer[1]);
		#if DEBUG 
		printf("ADC1_Val_Buffer[0] =  %d,\r\n",data_collect.ADC1_Val_Buffer[0]);
		printf("ADC1_Val_Buffer[1] =  %d,\r\n",data_collect.ADC1_Val_Buffer[1]);
		printf("ADC1_Val_Buffer[2] =  %d,\r\n",data_collect.ADC1_Val_Buffer[2]);
		printf("ADC1_Val_Buffer[3] =  %d,\r\n",data_collect.ADC1_Val_Buffer[3]);
		#endif	
	
		//3.�����˲�
		data_result.ADC1_after_Filter[0] = ADC_Filter(data_collect.ADC1_DataBuff1,SENSITIVITY,data_result.ADC1_Val_Buffer[0]);
		data_result.ADC1_after_Filter[1] = ADC_Filter(data_collect.ADC1_DataBuff2,SENSITIVITY,data_result.ADC1_Val_Buffer[1]);
		data_result.ADC1_after_Filter[2] = ADC_Filter(data_collect.ADC1_DataBuff3,SENSITIVITY,data_result.ADC1_Val_Buffer[2]);
		data_result.ADC1_after_Filter[3] = ADC_Filter(data_collect.ADC1_DataBuff4,SENSITIVITY,data_result.ADC1_Val_Buffer[3]);
		#if DEBUG 
		printf("ADC1_after_Filter[0] =  %d,\r\n",data_result.ADC1_after_Filter[0]);
		printf("ADC1_after_Filter[1] =  %d,\r\n",data_result.ADC1_after_Filter[1]);
		printf("ADC1_after_Filter[2] =  %d,\r\n",data_result.ADC1_after_Filter[2]);
		printf("ADC1_after_Filter[3] =  %d,\r\n",data_result.ADC1_after_Filter[3]);	
		#endif
//printf("data_result.ADC1_after_Filter[1] =  %d,\r\n",data_result.ADC1_after_Filter[1]);
//		//4.���������ѹ
//		data_result.ADC1_Voltage_1 = data_result.ADC1_after_Filter1*3300*5/3/4096;
//		data_result.ADC1_Voltage_2 = data_result.ADC1_after_Filter2*3300*5/3/4096;
//		data_result.ADC1_Voltage_3 = data_result.ADC1_after_Filter1*3300*5/3/4096;
//		data_result.ADC1_Voltage_4 = data_result.ADC1_after_Filter2*3300*5/3/4096;
		
		//4.�����������ж�
		for(i=0;i<4;i++)
		{
			if(data_result.ADC1_after_Filter[i]<100)
			{
				bit_flag.sersor_state[i]=0;
				data_result.ADC1_Sensor_Height[i] = 999;//mm
			}
			else
			{
				bit_flag.sersor_state[i]=1;
				data_result.ADC1_Sensor_Height[i] = data_storage.PickerPar.SensorK[i]*data_result.ADC1_after_Filter[i] + data_storage.PickerPar.SensorB[i];//����0.1mm
				if(timeCnt>300)//������ʱ3s�󣬸��������Сֵ
				{
//					timeCnt =300;
					data_storage.PickerPar.SensorMin[i] = Min(data_result.ADC1_after_Filter[i],data_storage.PickerPar.SensorMin[i]);
					data_storage.PickerPar.SensorMax[i] = Max(data_result.ADC1_after_Filter[i],data_storage.PickerPar.SensorMax[i]);		
				}					
				
			}
		}
//	printf("data_result.ADC1_Sensor_Height[1] =  %d,\r\n",data_result.ADC1_Sensor_Height[1]);
		
		//5.��������Ƕȴ�����ʵ�ʸ߶ȣ�ȡ��ÿ����ͷ����С�߶�Ϊʵ�ʸ߶�ֵ
		if(++timeCnt>301)//������ʱ3s�󣬸��¸��²�ͷʵ�ʸ߶�
		{
				timeCnt =301;
				data_storage.PickerPar.Height = PickerHeight_Get(bit_flag.sersor_state);	//��4������������Сֵ	�� ���ֵ
		}	
//		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
		#if DEBUG 
		printf("data_storage.PickerPar.Height.Max =  %d,\r\n",data_storage.PickerPar.Height.Max);
		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
		#endif
//		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
}

/********���ݴ�����********/
void Process_Data(void)
{	
		float k;   				
		k = data_storage.Rapidity/10.0f;					//���Է���ϵ��

		//1.�Զ����� �� �ֶ������л�����
		if(bit_flag.Auto_Manual_Flag == AUTO)//0�Զ����ӷ��� 
		{
			if(data_storage.PickerPar.Height.Min>data_storage.SetHeight+10)//mm
			{//�½�
				out_par.DownValve_01 = Drive_On;
				out_par.PWM_Down_Value = (data_storage.PickerPar.Height.Min-data_storage.SetHeight)*k + 250;
				if(out_par.PWM_Down_Value>800)out_par.PWM_Down_Value=800;
				out_par.PWM_Up_Value = 0;					
			}
			else if(data_storage.PickerPar.Height.Min<data_storage.SetHeight-3)//mm
			{//����
				out_par.DownValve_01 = Drive_Off;
				out_par.PWM_Up_Value = (data_storage.SetHeight - data_storage.PickerPar.Height.Min)*k + 250;	
				if(out_par.PWM_Up_Value>800)out_par.PWM_Up_Value=800;				
				out_par.PWM_Down_Value = 0;				
			}
			else
			{
				out_par.DownValve_01 = Drive_Off;  //��ŷ��ر�
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;				
			}	
		}
		else if(bit_flag.Auto_Manual_Flag == MANUAL)//1�ֶ�����
		{
			if(bit_flag.Manual_Down == 0)				//��
			{
				out_par.DownValve_01 = Drive_On;
				out_par.PWM_Down_Value = data_storage.PWM_Value_Down;
				out_par.PWM_Up_Value = 0;		
			}
			else if(bit_flag.Manual_Up == 0)  //��
			{
				out_par.DownValve_01 = Drive_Off;
				out_par.PWM_Up_Value = data_storage.PWM_Value_Up;				
				out_par.PWM_Down_Value = 0;
			}
			else 
			{
				out_par.DownValve_01 = Drive_Off;  //��ŷ��ر�
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;
			}
		}
}


/********������� ���Ƶ�ŷ� �� ��������PWM�����������********/
void Control_Out(void)
{
	Drive_Out(out_par.Drive_Channel_1,out_par.DownValve_01);
//	Drive_Out(out_par.Drive_Channel_2,out_par.DownValve_02);//����
	PWM_Out(out_par.PWM_Channel_Up,out_par.PWM_Up_Value);
	PWM_Out(out_par.PWM_Channel_Down,out_par.PWM_Down_Value);	
}


/********PWM�������********/
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


/********��ŷ������������********/
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State)
{
	switch(Drive_Choose)//��ŷ�1��2 ѡ�����
	{
		case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,State);//��ŷ�1
			break;
		
		case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,State);//��ŷ�2
			break;
		
		default:
			break;
	}
}


/********Ĭ��ֵ��ʼ������********/
void Default_Data_Init(void)
{
	bit_flag.Calibration_Flag = 0;					//У׼��־λ
	bit_flag.Calibration_Success = 0;	   		//У׼��־λδУ׼Ϊ0
	bit_flag.Auto_Manual_Flag = MANUAL;  		//Ĭ���ϵ�Ϊ�ֶ�����
	Real_Height_Init();											//�ĸ�������������ֵ�趨
}

/********��ͷ�߶�У׼����********/
void Hegiht_Calibration(void)
{
	//�������Է��̵�ϵ��	
	for(int i = 0;i<4;i++)
	{
		data_storage.PickerPar.SensorK[i] = (1400.0f-500.0f)/(data_storage.PickerPar.SensorMax[i]-data_storage.PickerPar.SensorMin[i]);
		data_storage.PickerPar.SensorB[i] = 1400.0f-data_storage.PickerPar.SensorK[i]*data_storage.PickerPar.SensorMax[i];
	}
	
	//�ѱ궨֮��ĸ��������������Сֵд��EEPROM��																																		
	AT24C02_WriteLenByte(Addr_Sensor1_Min,data_storage.PickerPar.SensorMin[0],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor1_Max,data_storage.PickerPar.SensorMax[0],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor2_Min,data_storage.PickerPar.SensorMin[1],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor2_Max,data_storage.PickerPar.SensorMax[1],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor3_Min,data_storage.PickerPar.SensorMin[2],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor3_Max,data_storage.PickerPar.SensorMax[2],2);
	HAL_Delay(10);                                                          
	AT24C02_WriteLenByte(Addr_Sensor4_Min,data_storage.PickerPar.SensorMin[3],2);
	HAL_Delay(10);                                                         
	AT24C02_WriteLenByte(Addr_Sensor4_Max,data_storage.PickerPar.SensorMax[3],2);
	HAL_Delay(10);		       
	//���¸��������趨�������Сֵ
	data_storage.EEPROM_Init_Par.SensorMin[0] = data_storage.PickerPar.SensorMin[0];
	data_storage.EEPROM_Init_Par.SensorMax[0] = data_storage.PickerPar.SensorMax[0];
	data_storage.EEPROM_Init_Par.SensorMin[1] = data_storage.PickerPar.SensorMin[1];
	data_storage.EEPROM_Init_Par.SensorMax[1] = data_storage.PickerPar.SensorMax[1];
	data_storage.EEPROM_Init_Par.SensorMin[2] = data_storage.PickerPar.SensorMin[2];
	data_storage.EEPROM_Init_Par.SensorMax[2] = data_storage.PickerPar.SensorMax[2];
	data_storage.EEPROM_Init_Par.SensorMin[3] = data_storage.PickerPar.SensorMin[3];
	data_storage.EEPROM_Init_Par.SensorMax[3] = data_storage.PickerPar.SensorMax[3];
	//����궨�ɹ���־λ
	bit_flag.Calibration_Success = 1;

	#if DEBUG
	printf("bit_flag.Calibration_Success = %d\r\n",bit_flag.Calibration_Success);
	#endif
}


/********��ͷ�ĸ��������ĸ߶ȳ�ʼ������********/
void Real_Height_Init(void)
{
	data_storage.PickerPar.SensorMin[0] = 5000;
	data_storage.PickerPar.SensorMax[0] = 0;
	data_storage.PickerPar.SensorMin[1] = 5000;
	data_storage.PickerPar.SensorMax[1] = 0;
	data_storage.PickerPar.SensorMin[2] = 5000;
	data_storage.PickerPar.SensorMax[2] = 0;
	data_storage.PickerPar.SensorMin[3] = 5000;
	data_storage.PickerPar.SensorMax[3] = 0;	
	
}

/********����Сֵ����********/
uint16_t Min(uint16_t value1,uint16_t value2)
{
	return (value1 < value2 ? value1:value2);
}


/********�����ֵ����********/
uint16_t Max(uint16_t value1,uint16_t value2)
{
	return (value1 > value2 ? value1:value2);
}


/********��ȡ��ͷ�߶Ⱥ�������ͷ���ֵ����Сֵ��********/
HEIGHT PickerHeight_Get(uint8_t sersorstate[])
{
	int i=0;
	HEIGHT PickerHeight;
	PickerHeight.Min=4000;
	PickerHeight.Max=0;
	for(i=0;i<4;i++)
	{
		if(sersorstate[i] ==1)
		{
			PickerHeight.Min = Min(PickerHeight.Min,data_storage.PickerPar.SensorMin[i]);
			PickerHeight.Max = Max(PickerHeight.Max,data_storage.PickerPar.SensorMax[i]);			
		}
	}
	return PickerHeight;
}


/********����AT24CXX�Ƿ����������д********/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t dat=0x55;
	AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);			   
	if(temp==0x55)
	{
		return 0;	
	}   
	else//�ų���һ�γ�ʼ�������
	{
		AT24C02_Write(Addr_AT24C02_CheckRW,&dat,1);
		HAL_Delay(10); 		//дһ�κͶ�һ��֮����Ҫ���ݵ���ʱ
		AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);	
		HAL_Delay(10); 	  
		if(temp==0x55) return 0;
	}
	return 1;											  
}

/********EEPROM���ݳ�ʼ��********/
void EEPROM_DATA_Init(void)
{
	uint8_t temp;
	uint8_t dat = 0x56;

	uint8_t ret;
	ret = AT24CXX_Check();
	if (ret == 0)
		printf("AT24CXX_Check  success!\r\n");
	else
		printf("AT24CXX_Check  fail!\r\n");
	AT24C02_Read( Addr_AT24C02_CheckNO,&temp,1);
	if (temp != dat)
	{
		printf("New board, Init EEPROM!\r\n");
		AT24C02_Write(Addr_AT24C02_CheckNO,&dat,1);
		EEPROM_Init();
	}

	Read_EEPROM();
}

/********��EEPROM�е����ݳ�ʼ��********/
void EEPROM_Init(void)
{	
	//����д�� ���θ߶ȡ������ȡ������ԡ��ֶ������ٶȡ��ֶ��½��ٶȡ�
	//�󴫸������λ���󴫸������λ���Ҵ��������λ���Ҵ��������λ ��Ĭ��ֵ
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_SetHeight,DEFAULT_SETHEIGHT,1);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Sensitivity,DEFAULT_SENSITUVITY,1);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Rapidity,DEFAULT_RAPIDITY,1);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_PWM_Value_Up,DEFAULT_PWM_UP_VALUE,2);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_PWM_Value_Down,DEFAULT_PWM_DOWN_VALUE,2);
	HAL_Delay(10); 
	
	AT24C02_WriteLenByte(Addr_Sensor1_Min,DEFAULT_HEIGHT_MIN ,2);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Sensor1_Max,DEFAULT_HEIGHT_MAX,2);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Sensor2_Min,DEFAULT_HEIGHT_MIN ,2);
	HAL_Delay(10); 	
	AT24C02_WriteLenByte(Addr_Sensor2_Max,DEFAULT_HEIGHT_MAX,2);
	HAL_Delay(10); 	
	AT24C02_WriteLenByte(Addr_Sensor3_Min,DEFAULT_HEIGHT_MIN ,2);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Sensor3_Max,DEFAULT_HEIGHT_MAX,2);
	HAL_Delay(10); 
	AT24C02_WriteLenByte(Addr_Sensor4_Min,DEFAULT_HEIGHT_MIN ,2);
	HAL_Delay(10); 	
	AT24C02_WriteLenByte(Addr_Sensor4_Max,DEFAULT_HEIGHT_MAX,2);
	HAL_Delay(10); 	
	AT24C02_WriteLenByte(Addr_SensorB,DEFAULT_SENSOR_B,2);
	HAL_Delay(10); 		
	
}

/********��ȡEEPROM����********/
void Read_EEPROM(void)
{
	HAL_Delay(100);
	//���ζ�ȡEEPROM�е� ���θ߶ȡ������ȡ������ԡ��ֶ������ٶȡ��ֶ��½��ٶȡ�
	//�󴫸������λ���󴫸������λ���Ҵ��������λ���Ҵ��������λ ��ֵ	
	data_storage.SetHeight = AT24C02_ReadLenByte(Addr_SetHeight,1);
	HAL_Delay(10); 
	data_storage.Sensitivity = AT24C02_ReadLenByte(Addr_Sensitivity,1);
	HAL_Delay(10); 
	data_storage.Rapidity = AT24C02_ReadLenByte(Addr_Rapidity,1);
	HAL_Delay(10); 
	data_storage.PWM_Value_Up = AT24C02_ReadLenByte(Addr_PWM_Value_Up,2);
	HAL_Delay(10); 
	data_storage.PWM_Value_Down = AT24C02_ReadLenByte(Addr_PWM_Value_Down,2);
	HAL_Delay(10); 
	//��ʼ��֮�󣬰Ѵ���EEPROM�еĸ��������������Сֵȡ��
	data_storage.EEPROM_Init_Par.SensorMin[0] = AT24C02_ReadLenByte(Addr_Sensor1_Min,2);
	HAL_Delay(10);          
	data_storage.EEPROM_Init_Par.SensorMax[0] = AT24C02_ReadLenByte(Addr_Sensor1_Max,2);
	HAL_Delay(10);        
	data_storage.EEPROM_Init_Par.SensorMin[1] = AT24C02_ReadLenByte(Addr_Sensor2_Min,2);
	HAL_Delay(10);         
	data_storage.EEPROM_Init_Par.SensorMax[1] = AT24C02_ReadLenByte(Addr_Sensor2_Max,2);
	HAL_Delay(10);         
	data_storage.EEPROM_Init_Par.SensorMin[2] = AT24C02_ReadLenByte(Addr_Sensor3_Min,2);
	HAL_Delay(10);          
	data_storage.EEPROM_Init_Par.SensorMax[2] = AT24C02_ReadLenByte(Addr_Sensor3_Max,2);
	HAL_Delay(10);          
	data_storage.EEPROM_Init_Par.SensorMin[3] = AT24C02_ReadLenByte(Addr_Sensor4_Min,2);
	HAL_Delay(10);         
	data_storage.EEPROM_Init_Par.SensorMax[3] = AT24C02_ReadLenByte(Addr_Sensor4_Max,2);
	HAL_Delay(10); 
	//�������Ժ�����ϵ��
	for(int i = 0;i<4;i++)
	{
		data_storage.PickerPar.SensorK[i] = (1400-400)/((float)(data_storage.EEPROM_Init_Par.SensorMax[i]-data_storage.EEPROM_Init_Par.SensorMin[i]));
		data_storage.PickerPar.SensorB[i] = 1400.0f-data_storage.PickerPar.SensorK[i]*data_storage.EEPROM_Init_Par.SensorMax[i];
	}

	
	#if DEBUG
	printf("SensorMax[1] =  %d,\r\n",data_storage.EEPROM_Init_Par.SensorMax[1]);
	printf("SensorMin[i] =  %d,\r\n",data_storage.EEPROM_Init_Par.SensorMin[1]);
	printf("SensorK[1] =  %.6f,\r\n",data_storage.PickerPar.SensorK[1]);
	printf("SensorB[1] =  %f,\r\n",data_storage.PickerPar.SensorB[1]);
	printf("data_storage.PickerPar.SensorK[0] =  %f,\r\n",data_storage.PickerPar.SensorK[0]);
	printf("data_storage.PickerPar.SensorK[1] =  %f,\r\n",data_storage.PickerPar.SensorK[1]);
	printf("data_storage.PickerPar.SensorK[2] =  %f,\r\n",data_storage.PickerPar.SensorK[2]);
	printf("data_storage.PickerPar.SensorK[3] =  %f,\r\n",data_storage.PickerPar.SensorK[3]);
	#endif

}


