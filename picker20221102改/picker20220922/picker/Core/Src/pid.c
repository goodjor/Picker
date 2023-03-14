#include "pid.h"

//int PIDCalc( PID *pp, int NextPoint)
//{
//	int dError,Error;
//	int result=0;
//	
//	Error = pp->SetPoint - NextPoint; // 偏差
//	pp->SumError += Error; // 积分
//	dError = pp->LastError - pp->PrevError; // 当前微分
//	pp->PrevError = pp->LastError;
//	pp->LastError = Error;

//	if(pp->SumError>900)  pp->SumError=900;
//	else if(pp->SumError<-900)	  pp->SumError=-900;

//	result =(pp->Proportion * Error // 比例项
//		+ pp->Integral * pp->SumError // 积分项
//		+ pp->Derivative * dError // 微分项
//		+0.5 //四舍五入
//		);
//	
//	if(result>60)result =60;
//	else if(result <-60) result=-60;
//	return result;
//}
