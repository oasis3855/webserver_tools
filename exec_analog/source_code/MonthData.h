// MonthData.h : �w�b�_�t�@�C��
//
#pragma once

class CMonthData
{
public:
	CMonthData(void);
	~CMonthData(void);

	time_t tmNowUx;
	int GetMonthNow(void);
	int GetYearNow(void);
	int GetMonthPrev(void);
	int GetYearPrev(void);
};
