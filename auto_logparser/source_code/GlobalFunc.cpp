// GlobalFunc.cpp : �����t�@�C���@�iINI�t�@�C���ǂݏ����A�o�b�`�t�@�C���쐬�ASQL���߃t�@�C���쐬�ALogParser���s�j
//

// ***********************
// AutoLogParser  (Microsoft Log Parser��IIS���O�t�@�C�����������邽�߂̃o�b�`�t�@�C���쐬�\�t�g�j
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
//

#include "stdafx.h"
#include "AutoLogParser.h"
#include "GlobalFunc.h"
#include <process.h>
#include <stdlib.h>
#include <io.h>

// ***********************
// �R���t�B�M�����[�V�����t�@�C���̍쐬
// LogParser���s�p�̃o�b�`�t�@�C���ALogParser�p��SQL���߃t�@�C�����쐬����
// ***********************
bool MakeConfigFile(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit)
{
	FILE *fo;
	CString sTemp;
	time_t tmStartSec;
	struct tm tmStart;

	// �����ȔN�������n���ꂽ�ꍇ�͏I��
	if(nYear < 2007 || nYear > 2020 || nMonth < 1 || nMonth > 12 || nDay < 1 || nDay > 31 || nSpan < 1 || nSpan > 31)
		return false;

	// ���̊֐��ɓn���ꂽ�N������ time_t �Ɋi�[����
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear-1900;
	tmStart.tm_mon = nMonth-1;
	tmStart.tm_mday = nDay;
	tmStartSec = ::mktime(&tmStart);


	/******** �o�b�`�t�@�C���ilogauto.cmd�j�̍쐬 ********/

	sTemp.Format("%slogauto.cmd", srInit->sIISLogDir);

	fo = ::fopen(sTemp, "wt");
	if(fo == NULL)
	{
		return false;
	}

	fprintf(fo, "set path=%%path%%;%s\n", srInit->sLogParserDir);
	fprintf(fo, "%c:\ncd %s\n", srInit->sIISLogDir[0], srInit->sIISLogDir);
	fprintf(fo, "%s file:sql_err.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_method.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_status.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_method2.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);


	fprintf(fo, "%c:\ncd %s\n", srInit->sOutputDir[0], srInit->sOutputDir);
	fprintf(fo, "echo IIS Log summary on %4d/%02d/%02d %02d:%02d > iislog.txt\n",
		tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday, tmStart.tm_hour, tmStart.tm_min);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_status.txt >> iislog.txt\n", fo);
	fputs("type iislog_method2.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo Error Lines ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_err.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo Method (not GET, not POST) Lines ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_method.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo EOF >> iislog.txt\n", fo);

	fputs("del iislog_status.txt\n", fo);
	fputs("del iislog_method2.txt\n", fo);
	fputs("del iislog_err.txt\n", fo);
	fputs("del iislog_method.txt\n", fo);

	fclose(fo);


	/******** SQL���߃t�@�C���̍쐬 : error log ********/

	MakeConfigFile_MakeSQL("sql_err.sql", "iislog_err.txt", "select\n\t*\n\n", 
		"\nwhere\n\tsc-status=500 or\n\tsc-status=501 or\n\tsc-status=502 or\n\tsc-status=403 or\n\tsc-status=404\n\n",
		nSpan, tmStartSec, srInit);


	/******** SQL���߃t�@�C���̍쐬 : method log ********/

	MakeConfigFile_MakeSQL("sql_method.sql", "iislog_method.txt", "select\n\t*\n\n", 
		"\nwhere\n\tcs-method<>'GET' and\n\tcs-method<>'POST'\n\n",
		nSpan, tmStartSec, srInit);


	/******** SQL���߃t�@�C���̍쐬 : status count up ********/

	MakeConfigFile_MakeSQL("sql_status.sql", "iislog_status.txt", "select\n\tsc-status, count (*)\n\n", 
		"\ngroup by sc-status\n\n", nSpan, tmStartSec, srInit);


	/******** SQL���߃t�@�C���̍쐬 : method count up ********/

	MakeConfigFile_MakeSQL("sql_method2.sql", "iislog_method2.txt", "select\n\tcs-method, count (*)\n\n", 
		"\ngroup by cs-method\n\n", nSpan, tmStartSec, srInit);

	return true;
}

// ***********************
// SQL���߃t�@�C���̍쐬
// ***********************
bool MakeConfigFile_MakeSQL(char *sOutputSqlName, char *sIntoName, char *sSelect, char *sWhereGroupBy, int nSpan, time_t tmStartSec, struct __sr_init_data *srInit)
{
	FILE *fo;
	CString sTemp;
	time_t tmProcSec;
	struct tm tmStart;
	int i;
	char sLogFname[256];
	char sTempFname[1024+256];

	// SQL���߃t�@�C�����J��
	sprintf(sTempFname, "%s%s", srInit->sIISLogDir, sOutputSqlName);

	fo = ::fopen(sTempFname, "wt");
	if(fo == NULL)
	{
		return false;
	}

	// SELECT ��
	fputs(sSelect, fo);

	// INTO ��
	fprintf(fo, "into\n\t%s%s\n\n", srInit->sOutputDir, sIntoName);

	// FROM ��
	fputs("from\n\t", fo);
	for(i=nSpan-1; i>=0; i--)
	{
		tmProcSec = tmStartSec - i * 24 * 60 * 60;
		tmStart = *::localtime(&tmProcSec);
		sprintf(sLogFname, srInit->sIISLogName, tmStart.tm_year+1900-2000, tmStart.tm_mon+1, tmStart.tm_mday);
		sprintf(sTempFname, "%s%s", srInit->sIISLogDir, sLogFname);
		if(::_access(sTempFname, 0) == -1) continue;	// ���O�t�@�C�������݂��Ȃ��ꍇ�̓X�L�b�v
		fprintf(fo, "\n\t%s,", sLogFname);
	}
	fseek(fo, -1, SEEK_CUR);	// �Ō�̃t�@�C�����̌��̃R���}�����
	fputs("\n", fo);

	// WHERE ���^GROUP BY ��
	fputs(sWhereGroupBy, fo);

	fclose(fo);

	return true;
}

// ***********************
// LoParser���s���o�̓t�@�C���R�s�[�p�o�b�`�t�@�C���̎��s
// ***********************
void RunLogParser(struct __sr_init_data *srInit)
{
	CString sTemp;

	sTemp.Format("%slogauto.cmd", srInit->sIISLogDir);

	::_spawnl(_P_WAIT, (LPCSTR)sTemp, (LPCSTR)sTemp, NULL);

}

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
	GetPrivateProfileString("AutoLogParser", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI ���m�F�ł��Ȃ��ꍇ�A�V�K�쐬
		return 1;
	}


	// �ݒ�l�ǂݍ���

	GetPrivateProfileString("AutoLogParser", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("AutoLogParser", "LogParserDir", "err", srInit->sLogParserDir, 1023, sModulePath);
	if(!strcmp(srInit->sLogParserDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("AutoLogParser", "LogParserProgName", "err", srInit->sLogParserProgName, 254, sModulePath);
	if(!strcmp(srInit->sLogParserProgName, "err")) return 1;	// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("AutoLogParser", "IISLogDir", "err", srInit->sIISLogDir, 1023, sModulePath);
	if(!strcmp(srInit->sIISLogDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("AutoLogParser", "IISLogName", "err", srInit->sIISLogName, 254, sModulePath);
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

	if(!WritePrivateProfileString("AutoLogParser", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserDir", "c:\\Program Files\\Log Parser 2.2\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserProgName", "logparser.exe", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "IISLogDir", "d:\\Inetpub\\Log\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "IISLogName", "ex%02d%02d%02d.log", sModulePath))
		return ;


	return ;
}

