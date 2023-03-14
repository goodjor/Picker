#ifndef __process_H
#define __process_H

#include "main.h"




void Collect_Data(void);
void Process_Data(void);
void Control_Out(void);
void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value);
void Drive_Out(uint8_t Drive_Choose,GPIO_PinState State);
void Default_Data_Init(void);
uint16_t Min(uint16_t value1,uint16_t value2);
uint16_t Max(uint16_t value1,uint16_t value2);
void Hegiht_Calibration(void);
void Real_Height_Init(void);
//HEIGHT PickerHeight_Get(uint8_t sersor_state[]);
uint8_t AT24CXX_Check(void);
void EEPROM_DATA_Init(void);
void EEPROM_Init(void);
void Read_EEPROM(void);

#endif
