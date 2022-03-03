// GlobalFunc.h : ヘッダー ファイルです。
//
#pragma once

#include "resource.h"		// メイン シンボル

struct __sr_init_data{
	char sOutputDir[1024];
	char sOutputFilename[256];
	char sOutputType[32];
	char sLogParserDir[1024];
	char sLogParserProgName[256];
	char sLogDir[1024];
	char sLogName[256];
	char sLogType[32];
};

int GetInitData(__sr_init_data *srInit);
void WriteInitIniFile(void);

bool MakeConfigFile(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit);
bool MakeConfigFile_MakeSQL(char *sOutputSqlName, char *sIntoName, char *sSelect, char *sWhereGroupBy, int nSpan, time_t tmStartSec, struct __sr_init_data *srInit);
bool RunLogParser(struct __sr_init_data *srInit);
