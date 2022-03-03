// GlobalFunc.h : �w�b�_�t�@�C��
//
#pragma once

#include "resource.h"		// ���C�� �V���{��

struct __sr_init_data{
	char sOutputDir[1024];
	char sAnalogDir[1024];
	char sAnarogProgName[256];
	char sAnalogCfgTemplate[256];
	char sIISLogDir[1024];
	char sIISLogName[256];
};

int GetInitData(__sr_init_data *srInit);
void WriteInitIniFile(void);

void ExecAnalog(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit);
void _RunAutomatic(struct __sr_init_data *srInit);


bool _CfgFileMaker(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit);
