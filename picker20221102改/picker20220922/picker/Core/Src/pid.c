#include "pid.h"

//int PIDCalc( PID *pp, int NextPoint)
//{
//	int dError,Error;
//	int result=0;
//	
//	Error = pp->SetPoint - NextPoint; // ƫ��
//	pp->SumError += Error; // ����
//	dError = pp->LastError - pp->PrevError; // ��ǰ΢��
//	pp->PrevError = pp->LastError;
//	pp->LastError = Error;

//	if(pp->SumError>900)  pp->SumError=900;
//	else if(pp->SumError<-900)	  pp->SumError=-900;

//	result =(pp->Proportion * Error // ������
//		+ pp->Integral * pp->SumError // ������
//		+ pp->Derivative * dError // ΢����
//		+0.5 //��������
//		);
//	
//	if(result>60)result =60;
//	else if(result <-60) result=-60;
//	return result;
//}
