// GlobalFunc.h : ヘッダー ファイルです。
//
#pragma once

#include "resource.h"		// メイン シンボル

struct __sr_init_data{
	char sOutputDir[1024];
	char sLogParserDir[1024];
	char sLogParserProgName[256];
	char sIISLogDir[1024];
	char sIISLogName[256];
};

int GetInitData(__sr_init_data *srInit);
void WriteInitIniFile(void);

bool MakeConfigFile(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit);
bool MakeConfigFile_MakeSQL(char *sOutputSqlName, char *sIntoName, char *sSelect, char *sWhereGroupBy, int nSpan, time_t tmStartSec, struct __sr_init_data *srInit);
void RunLogParser(struct __sr_init_data *srInit);
