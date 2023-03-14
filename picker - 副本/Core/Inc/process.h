#ifndef __process_H
#define __process_H

#include "main.h"




void Collect_Data(void);
void Process_Data(void);
void Control_Out(void);
void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value);
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State);
void Default_Data_Init(void);
void Height_Judgment(uint16_t *hight_value,uint8_t LR);
uint8_t Picker_LR_Choose(uint8_t DI_Value);
uint8_t State_AutoManual_Choose(uint8_t DI_Value);
uint16_t Min(uint16_t value1,uint16_t value2);
uint16_t Max(uint16_t value1,uint16_t value2);
uint16_t Picker_Hieght_Process(void);
void Hegiht_Calibration(void);
void Real_Height_Init(void);



#endif
