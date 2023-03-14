#include "process.h"

/********数据采集函数********/
void Collect_Data(void)
{
		uint8_t i=0;									//用于for循坏采集四个传感器数据
		static uint16_t timeCnt=0;	//用于延时计数

		//1.读取4个 DI 的状态
		data_collect.DI1_State = HAL_GPIO_ReadPin(PB4_DI1_GPIO_Port,PB4_DI1_Pin);
		data_collect.DI2_State = HAL_GPIO_ReadPin(PB5_DI2_GPIO_Port,PB5_DI2_Pin);
		data_collect.DI3_State = HAL_GPIO_ReadPin(PB8_DI3_GPIO_Port,PB8_DI3_Pin);
		data_collect.DI4_State = HAL_GPIO_ReadPin(PB9_DI4_GPIO_Port,PB9_DI4_Pin);
		//左右采头选择
		bit_flag.L_R_Picker_Choose_Flag = data_collect.DI4_State;		//0右，1左
		//判断仿形开关是否开启
		bit_flag.Auto_Manual_Flag = data_collect.DI3_State;					//0仿形开，1关
		//手动升降检测
		bit_flag.Manual_Down 	= data_collect.DI1_State ;						//0降标志位有效
		bit_flag.Manual_Up 		= data_collect.DI2_State;							//0上升标志位有效
	
		#if DEBUG 
		printf("DI1_State = %d\r\n 0降标志位有效	 \r\n",data_collect.DI1_State);
		printf("DI2_State = %d\r\n 0上升标志位有效\r\n",data_collect.DI2_State);
		printf("DI3_State = %d\r\n 0仿形开，1关	 \r\n",data_collect.DI3_State);
		printf("DI4_State = %d\r\n 0右，1左			 \r\n",data_collect.DI4_State);
		#endif
	
		//2.读取 ADC 值
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
	
		//3.滑动滤波
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
//		//4.计算输入电压
//		data_result.ADC1_Voltage_1 = data_result.ADC1_after_Filter1*3300*5/3/4096;
//		data_result.ADC1_Voltage_2 = data_result.ADC1_after_Filter2*3300*5/3/4096;
//		data_result.ADC1_Voltage_3 = data_result.ADC1_after_Filter1*3300*5/3/4096;
//		data_result.ADC1_Voltage_4 = data_result.ADC1_after_Filter2*3300*5/3/4096;
		
		//4.传感器故障判断
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
				data_result.ADC1_Sensor_Height[i] = data_storage.PickerPar.SensorK[i]*data_result.ADC1_after_Filter[i] + data_storage.PickerPar.SensorB[i];//精度0.1mm
				if(timeCnt>300)//开机延时3s后，更新最大最小值
				{
//					timeCnt =300;
					data_storage.PickerPar.SensorMin[i] = Min(data_result.ADC1_after_Filter[i],data_storage.PickerPar.SensorMin[i]);
					data_storage.PickerPar.SensorMax[i] = Max(data_result.ADC1_after_Filter[i],data_storage.PickerPar.SensorMax[i]);		
				}					
				
			}
		}
//	printf("data_result.ADC1_Sensor_Height[1] =  %d,\r\n",data_result.ADC1_Sensor_Height[1]);
		
		//5.计算出各角度传感器实际高度，取定每个采头的最小高度为实际高度值
		if(++timeCnt>301)//开机延时3s后，更新更新采头实际高度
		{
				timeCnt =301;
				data_storage.PickerPar.Height = PickerHeight_Get(bit_flag.sersor_state);	//求4个传感器等最小值	和 最大值
		}	
//		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
		#if DEBUG 
		printf("data_storage.PickerPar.Height.Max =  %d,\r\n",data_storage.PickerPar.Height.Max);
		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
		#endif
//		printf("data_storage.PickerPar.Height.Min =  %d,\r\n",data_storage.PickerPar.Height.Min);
}

/********数据处理函数********/
void Process_Data(void)
{	
		float k;   				
		k = data_storage.Rapidity/10.0f;					//线性方程系数

		//1.自动仿形 与 手动仿形切换条件
		if(bit_flag.Auto_Manual_Flag == AUTO)//0自动电子仿形 
		{
			if(data_storage.PickerPar.Height.Min>data_storage.SetHeight+10)//mm
			{//下降
				out_par.DownValve_01 = Drive_On;
				out_par.PWM_Down_Value = (data_storage.PickerPar.Height.Min-data_storage.SetHeight)*k + 250;
				if(out_par.PWM_Down_Value>800)out_par.PWM_Down_Value=800;
				out_par.PWM_Up_Value = 0;					
			}
			else if(data_storage.PickerPar.Height.Min<data_storage.SetHeight-3)//mm
			{//上升
				out_par.DownValve_01 = Drive_Off;
				out_par.PWM_Up_Value = (data_storage.SetHeight - data_storage.PickerPar.Height.Min)*k + 250;	
				if(out_par.PWM_Up_Value>800)out_par.PWM_Up_Value=800;				
				out_par.PWM_Down_Value = 0;				
			}
			else
			{
				out_par.DownValve_01 = Drive_Off;  //电磁阀关闭
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;				
			}	
		}
		else if(bit_flag.Auto_Manual_Flag == MANUAL)//1手动控制
		{
			if(bit_flag.Manual_Down == 0)				//降
			{
				out_par.DownValve_01 = Drive_On;
				out_par.PWM_Down_Value = data_storage.PWM_Value_Down;
				out_par.PWM_Up_Value = 0;		
			}
			else if(bit_flag.Manual_Up == 0)  //升
			{
				out_par.DownValve_01 = Drive_Off;
				out_par.PWM_Up_Value = data_storage.PWM_Value_Up;				
				out_par.PWM_Down_Value = 0;
			}
			else 
			{
				out_par.DownValve_01 = Drive_Off;  //电磁阀关闭
				out_par.PWM_Up_Value = 0;
				out_par.PWM_Down_Value = 0;
			}
		}
}


/********控制输出 控制电磁阀 和 比例阀（PWM）的输出函数********/
void Control_Out(void)
{
	Drive_Out(out_par.Drive_Channel_1,out_par.DownValve_01);
//	Drive_Out(out_par.Drive_Channel_2,out_par.DownValve_02);//备用
	PWM_Out(out_par.PWM_Channel_Up,out_par.PWM_Up_Value);
	PWM_Out(out_par.PWM_Channel_Down,out_par.PWM_Down_Value);	
}


/********PWM输出函数********/
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


/********电磁阀控制输出函数********/
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State)
{
	switch(Drive_Choose)//电磁阀1、2 选择输出
	{
		case 1:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,State);//电磁阀1
			break;
		
		case 2:HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,State);//电磁阀2
			break;
		
		default:
			break;
	}
}


/********默认值初始化函数********/
void Default_Data_Init(void)
{
	bit_flag.Calibration_Flag = 0;					//校准标志位
	bit_flag.Calibration_Success = 0;	   		//校准标志位未校准为0
	bit_flag.Auto_Manual_Flag = MANUAL;  		//默认上电为手动仿形
	Real_Height_Init();											//四个传感器最高最低值设定
}

/********采头高度校准函数********/
void Hegiht_Calibration(void)
{
	//计算线性方程的系数	
	for(int i = 0;i<4;i++)
	{
		data_storage.PickerPar.SensorK[i] = (1400.0f-500.0f)/(data_storage.PickerPar.SensorMax[i]-data_storage.PickerPar.SensorMin[i]);
		data_storage.PickerPar.SensorB[i] = 1400.0f-data_storage.PickerPar.SensorK[i]*data_storage.PickerPar.SensorMax[i];
	}
	
	//把标定之后的各传感器的最大最小值写进EEPROM中																																		
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
	//更新各传感器设定的最大最小值
	data_storage.EEPROM_Init_Par.SensorMin[0] = data_storage.PickerPar.SensorMin[0];
	data_storage.EEPROM_Init_Par.SensorMax[0] = data_storage.PickerPar.SensorMax[0];
	data_storage.EEPROM_Init_Par.SensorMin[1] = data_storage.PickerPar.SensorMin[1];
	data_storage.EEPROM_Init_Par.SensorMax[1] = data_storage.PickerPar.SensorMax[1];
	data_storage.EEPROM_Init_Par.SensorMin[2] = data_storage.PickerPar.SensorMin[2];
	data_storage.EEPROM_Init_Par.SensorMax[2] = data_storage.PickerPar.SensorMax[2];
	data_storage.EEPROM_Init_Par.SensorMin[3] = data_storage.PickerPar.SensorMin[3];
	data_storage.EEPROM_Init_Par.SensorMax[3] = data_storage.PickerPar.SensorMax[3];
	//清楚标定成功标志位
	bit_flag.Calibration_Success = 1;

	#if DEBUG
	printf("bit_flag.Calibration_Success = %d\r\n",bit_flag.Calibration_Success);
	#endif
}


/********采头四个传感器的高度初始化函数********/
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

/********求最小值函数********/
uint16_t Min(uint16_t value1,uint16_t value2)
{
	return (value1 < value2 ? value1:value2);
}


/********求最大值函数********/
uint16_t Max(uint16_t value1,uint16_t value2)
{
	return (value1 > value2 ? value1:value2);
}


/********获取采头高度函数（采头最大值和最小值）********/
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


/********测试AT24CXX是否可以正常读写********/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	uint8_t dat=0x55;
	AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);			   
	if(temp==0x55)
	{
		return 0;	
	}   
	else//排除第一次初始化的情况
	{
		AT24C02_Write(Addr_AT24C02_CheckRW,&dat,1);
		HAL_Delay(10); 		//写一次和读一次之间需要短暂的延时
		AT24C02_Read(Addr_AT24C02_CheckRW,&temp,1);	
		HAL_Delay(10); 	  
		if(temp==0x55) return 0;
	}
	return 1;											  
}

/********EEPROM数据初始化********/
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

/********对EEPROM中的数据初始化********/
void EEPROM_Init(void)
{	
	//依次写入 仿形高度、灵敏度、快速性、手动上升速度、手动下降速度、
	//左传感器最低位、左传感器最高位、右传感器最低位、右传感器最高位 的默认值
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

/********读取EEPROM数据********/
void Read_EEPROM(void)
{
	HAL_Delay(100);
	//依次读取EEPROM中的 仿形高度、灵敏度、快速性、手动上升速度、手动下降速度、
	//左传感器最低位、左传感器最高位、右传感器最低位、右传感器最高位 的值	
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
	//初始化之后，把存在EEPROM中的各传感器的最大最小值取出
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
	//计算线性函数的系数
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


