// GlobalFunc.cpp : �����t�@�C���@�iINI�t�@�C���ǂݏ����j
//

// ***********************
// ReadNsLog  �iNwtScreen��Syslog����������o����v���O�����j
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
	GetPrivateProfileString("ReadNsLog", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI ���m�F�ł��Ȃ��ꍇ�A�V�K�쐬
		return 1;
	}


	// �ݒ�l�ǂݍ���

	GetPrivateProfileString("ReadNsLog", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "OutputFileName", "err", srInit->sOutputFileName, 253, sModulePath);
	if(!strcmp(srInit->sOutputFileName, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "SyslogDir", "err", srInit->sSyslogDir, 1023, sModulePath);
	if(!strcmp(srInit->sSyslogDir, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "SyslogFileName", "err", srInit->sSyslogFileName, 253, sModulePath);
	if(!strcmp(srInit->sSyslogFileName, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString1", "err", srInit->sMatch1, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString2", "err", srInit->sMatch2, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 2;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString3", "err", srInit->sMatch3, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 3;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString4", "err", srInit->sMatch4, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 4;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString5", "err", srInit->sMatch5, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 5;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "MatchString6", "err", srInit->sMatch6, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 6;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "ExcludeString1", "err", srInit->sExclude1, 125, sModulePath);
	if(!strcmp(srInit->sExclude1, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "ExcludeString2", "err", srInit->sExclude2, 125, sModulePath);
	if(!strcmp(srInit->sExclude2, "err")) return 1;		// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadNsLog", "Days", "err", sTemp, 125, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;							// �ݒ�l�ǂݍ��ݕs�\
	srInit->nDays = atoi(sTemp);

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

	if(!WritePrivateProfileString("ReadNsLog", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "OutputFileName", "NetscreenQuery.txt", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "SyslogDir", "d:\\Syslog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "SyslogFileName", "Syslog-%d-%02d-%02d.log", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString1", "action=Deny", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString2", "emergency", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString3", "critical", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString4", "alert", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString5", "warning", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString6", "error", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "ExcludeString1", "src_port=80", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "ExcludeString2", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "Days", "7", sModulePath))
		return ;


	return ;
}
