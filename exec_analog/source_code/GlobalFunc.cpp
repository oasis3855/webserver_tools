// GlobalFunc.cpp : �����t�@�C���@�iINI�t�@�C���ǂݏ����AAnalog.cfg�쐬�AAnalog���s�A�������s�j
//

// ***********************
// ExecAnalog  �i��������ѐ挎��IIS�f�[�^��Analog�ŉ�͂��鎩�����v���O�����j
// 
// Copyright (C) 2007 INOUE. Hirokazu
//
// 
// http://www.opensource.jp/gpl/gpl.ja.html
// ���̃v���O�����̓t���[�\�t�g�E�F�A�ł��B���Ȃ��͂�����A�t���[�\�t�g�E�F
// �A���c�ɂ���Ĕ��s���ꂽ GNU ��ʌ��O���p�����_��(�o�[�W����2���A��
// �]�ɂ���Ă͂���ȍ~�̃o�[�W�����̂����ǂꂩ)�̒�߂�����̉��ōĔЕz
// �܂��͉��ς��邱�Ƃ��ł��܂��B
// 
// ���̃v���O�����͗L�p�ł��邱�Ƃ�����ĔЕz����܂����A*�S���̖��ۏ�* 
// �ł��B���Ɖ\���̕ۏ؂����̖ړI�ւ̓K�����́A���O�Ɏ����ꂽ���̂���
// �ߑS�����݂��܂���B�ڂ�����GNU ��ʌ��O���p�����_�񏑂��������������B
// 
// ���Ȃ��͂��̃v���O�����Ƌ��ɁAGNU ��ʌ��O���p�����_�񏑂̕��������ꕔ
// �󂯎�����͂��ł��B�����󂯎���Ă��Ȃ���΁A�t���[�\�t�g�E�F�A���c��
// �Ő������Ă�������(����� the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA)�B

#include "stdafx.h"
#include "GlobalFunc.h"
#include "MonthData.h"


// ***********************
// INI�t�@�C�����珉���l��ǂݍ���
// ***********************
int GetInitData(__sr_init_data *srInit)
{
	char sModulePath[1024];
	char *sPtr;
	char sTemp[1024];
	int i;
	size_t nStrSize;

	// ���݂̃��W���[���̃t���p�X���𓾂�
	i = GetModuleFileName(NULL, sTemp, 1023);
	if(i<=0 || i>=1023) return 1;		// ���݂̃v���Z�X�̃p�X���������Ȃ�����


	// ���W���[���p�X�̍Ō�� . �̈ʒu�܂ł��t�@�C�����̃{�f�B�Ƃ���
	sPtr = strrchr(sTemp, '.');
	if(sPtr == NULL)
		nStrSize = strlen(sTemp);		// . �������ꍇ�A�p�X���S�̂Ƃ���
	else
		nStrSize = strlen(sTemp) - strlen(sPtr);
	strncpy(sModulePath, sTemp, nStrSize);
	sModulePath[nStrSize] = (char)NULL;

	// INI �g���q������
	strcat(sModulePath, ".ini");


	// INI�t�@�C�������݂��邩�̊m�F
	GetPrivateProfileString("ExecAnalog", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI ���m�F�ł��Ȃ��ꍇ�A�V�K�쐬
		return 1;
	}


	// �ݒ�l�ǂݍ���

	GetPrivateProfileString("ExecAnalog", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ExecAnalog", "AnalogDir", "err", srInit->sAnalogDir, 1023, sModulePath);
	if(!strcmp(srInit->sAnalogDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ExecAnalog", "AnalogProgName", "err", srInit->sAnarogProgName, 254, sModulePath);
	if(!strcmp(srInit->sAnarogProgName, "err")) return 1;	// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ExecAnalog", "AnalogCfgTemplate", "err", srInit->sAnalogCfgTemplate, 254, sModulePath);
	if(!strcmp(srInit->sAnalogCfgTemplate, "err")) return 1;// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ExecAnalog", "IISLogDir", "err", srInit->sIISLogDir, 1023, sModulePath);
	if(!strcmp(srInit->sIISLogDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ExecAnalog", "IISLogName", "err", srInit->sIISLogName, 254, sModulePath);
	if(!strcmp(srInit->sIISLogName, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	return 0;
}


// ***********************
// �V�K��INI�t�@�C�����쐬����
// ***********************
void WriteInitIniFile(void)
{
	char sModulePath[1024];
	char *sPtr;
	char sTemp[1024];
	int i;
	size_t nStrSize;

	// ���݂̃��W���[���̃t���p�X���𓾂�
	i = GetModuleFileName(NULL, sTemp, 1023);
	if(i<=0 || i>=1023) return ;		// ���݂̃v���Z�X�̃p�X���������Ȃ�����


	// ���W���[���p�X�̍Ō�� . �̈ʒu�܂ł��t�@�C�����̃{�f�B�Ƃ���
	sPtr = strrchr(sTemp, '.');
	if(sPtr == NULL)
		nStrSize = strlen(sTemp);		// . �������ꍇ�A�p�X���S�̂Ƃ���
	else
		nStrSize = strlen(sTemp) - strlen(sPtr);
	strncpy(sModulePath, sTemp, nStrSize);
	sModulePath[nStrSize] = (char)NULL;

	// INI �g���q������
	strcat(sModulePath, ".ini");


	// �T���v����INI�t�@�C������������

	if(!WritePrivateProfileString("ExecAnalog", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "OutputDir", "d:\\Inetpub\\wwwroot\\analog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogDir", "c:\\Program Files\\analog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogProgName", "analog.exe", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogCfgTemplate", "analog.cfg.tmpl", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "IISLogDir", "d:\\Inetpub\\Log\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "IISLogName", "ex%02d%02d*.log", sModulePath))
		return ;


	return ;
}

void ExecAnalog(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char sAnalogProgFullPath[1024+256];

	// analog.cfg�t�@�C���̍쐬
	::_CfgFileMaker(nYear, nMonth, sOutputSubDir, srInit);

	// Analog.exe �̃t���p�X�쐬
	sprintf(sAnalogProgFullPath, "%s%s", srInit->sAnalogDir, srInit->sAnarogProgName);

	// Analog�v���Z�X�̋N��
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);	// �����w�肵�Ȃ��ꍇ�ł��A si �͕K�v
	if(!::CreateProcess(NULL, sAnalogProgFullPath, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, srInit->sAnalogDir, &si, &pi))
	{
		// �v���Z�X�쐬���s
	}
	else
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);
	}

}

// ***********************
// �������s���s�� �i�^�C�}�[�ȂǂŒ���I�Ƀo�b�N�O���E���h�Ŏ��s���邱�Ƃ�z��j
//
// �i�����A�挎��IIS���O��ΏۂƂ���j
// ***********************
void _RunAutomatic(struct __sr_init_data *srInit)
{
	CMonthData MonthData;

	// �����̃��O�t�@�C���p
	ExecAnalog(MonthData.GetYearNow(), MonthData.GetMonthNow(), "log", srInit);

	// �挎�̃��O�t�@�C���p
	ExecAnalog(MonthData.GetYearPrev(), MonthData.GetMonthPrev(), "logprev", srInit);

}


// ***********************
// analog.cfg �̍쐬
// ***********************
bool _CfgFileMaker(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit)
{

	FILE *fi, *fo;
	char sTemplateFile[1024+256];
	char sCfgFile[1024+256];
	char sBuffer[2048];
	char sLogFilename[300];



	// �e���v���[�g �t�@�C�����J��
	sprintf(sTemplateFile, "%s%s", srInit->sAnalogDir, srInit->sAnalogCfgTemplate);
	fi = ::fopen(sTemplateFile, "rt");
	if(fi == NULL) return false;

	// analog.cfg �t�@�C�����J��
	sprintf(sCfgFile, "%sanalog.cfg", srInit->sAnalogDir);
	fo = ::fopen(sCfgFile, "wt");
	if(fo == NULL)
	{
		fclose(fi);
		return false;
	}

	// ����̃L�[���[�h�i"$$LOGFILE$$", "$$OUTFILE$$"�j�̒u�����s���Ȃ���A�t�@�C���R�s�[
	for(;;)
	{
		if(fgets(sBuffer, 2046, fi) == NULL) break;
		if(!strcmpi(sBuffer, "$$LOGFILE$$\n"))
		{
			nYear %=100;
			sprintf(sLogFilename, srInit->sIISLogName, nYear, nMonth);		// "ex%02d%02d*.log" -> "ex0704*.log"
			sprintf(sBuffer, "LOGFILE %s%s\n", srInit->sIISLogDir, sLogFilename);
		}
		if(!strcmpi(sBuffer, "$$OUTFILE$$\n"))
		{
			sprintf(sBuffer, "OUTFILE %s%s\\log.html\n", srInit->sOutputDir, sOutputSubDir);
		}
		if(fputs(sBuffer, fo) == EOF) break;

	}

	// �S�Ẵt�@�C�������
	fclose(fo);
	fclose(fi);

	return true;
}

