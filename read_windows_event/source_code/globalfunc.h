// GlobalFunc.cpp : ヘッダファイル

#pragma once


struct __sr_init_data{
	char sOutputDir[1024];
	char sTarget1[256];
	char sTarget2[256];
	char sTarget3[256];
	char sTarget4[256];
	char sTarget5[256];
	char sTargetBackupExec[256];
	char sBackupExecName[256];
	bool bErrorOnly;
	bool b7days;
};

int GetInitData(__sr_init_data *srInit);
void WriteInitIniFile(void);

