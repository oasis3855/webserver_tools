// NsLog.h : ヘッダファイル
//
#pragma once

#include "GlobalFunc.h"

class CNsLog
{
public:
	char sMatchStrings[6][128];
	char sExcludeStrings[2][128];
	CNsLog(void);
	~CNsLog(void);
	bool MakeReport(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit);
	bool MakeReportMonth(int nYear, int nMonth, struct __sr_init_data *srInit);
	bool IsMatchString(char * sBuf);
	bool IsExcludeString(char * sBuf);
};
