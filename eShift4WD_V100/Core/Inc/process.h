#ifndef __process_H
#define __process_H

#include	"main.h"

void Data_Collect(void);

void Data_Process(void);
void ControlLoop(void);

void Contol_Out(void);

void PWM_Out(uint8_t PWM_Channel_Choose,uint16_t PWM_Value);

void Default_Data_Init(void);

#endif
