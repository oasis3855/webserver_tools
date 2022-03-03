// GlobalFunc.h : ヘッダファイル
//
#pragma once

#include "resource.h"		// メイン シンボル

struct __sr_init_data{
	char sOutputDir[1024];
	char sOutputFileName[256];
	char sSyslogDir[1024];
	char sSyslogFileName[256];
	char sMatch1[128];
	char sMatch2[128];
	char sMatch3[128];
	char sMatch4[128];
	char sMatch5[128];
	char sMatch6[128];
	char sExclude1[128];
	char sExclude2[128];
	int nDays;
};

int GetInitData(__sr_init_data *srInit);
void WriteInitIniFile(void);

