// GlobalFunc.cpp : �����t�@�C���@�iINI�t�@�C���ǂݏ����A�o�b�`�t�@�C���쐬�ASQL���߃t�@�C���쐬�ALogParser���s�j
//

// ***********************
// ReadEvent  (Windows�̃V�X�e�� ���O�𒊏o���AHTML������v���O�����j
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
#include "Globalfunc.h"

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
	GetPrivateProfileString("ReadEvent", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI ���m�F�ł��Ȃ��ꍇ�A�V�K�쐬
		return 1;
	}


	// �ݒ�l�ǂݍ���

	GetPrivateProfileString("ReadEvent", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetName1", "err", srInit->sTarget1, 254, sModulePath);
	if(!strcmp(srInit->sTarget1, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetName2", "err", srInit->sTarget2, 254, sModulePath);
	if(!strcmp(srInit->sTarget2, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetName3", "err", srInit->sTarget3, 254, sModulePath);
	if(!strcmp(srInit->sTarget3, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetName4", "err", srInit->sTarget4, 254, sModulePath);
	if(!strcmp(srInit->sTarget4, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetName5", "err", srInit->sTarget5, 254, sModulePath);
	if(!strcmp(srInit->sTarget5, "err")) return 1;			// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "TargetBackupExecName", "err", srInit->sTargetBackupExec, 254, sModulePath);
	if(!strcmp(srInit->sTargetBackupExec, "err")) return 1;	// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "BackupExecNameString", "err", srInit->sBackupExecName, 254, sModulePath);
	if(!strcmp(srInit->sBackupExecName, "err")) return 1;	// �ݒ�l�ǂݍ��ݕs�\

	GetPrivateProfileString("ReadEvent", "swErrorOnly", "err", sTemp, 254, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;						// �ݒ�l�ǂݍ��ݕs�\
	if(!strcmpi(sTemp, "yes")) srInit->bErrorOnly = true;
	else srInit->bErrorOnly = false;

	GetPrivateProfileString("ReadEvent", "swLast7Days", "err", sTemp, 254, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;						// �ݒ�l�ǂݍ��ݕs�\
	if(!strcmpi(sTemp, "yes")) srInit->b7days = true;
	else srInit->b7days = false;

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

	if(!WritePrivateProfileString("ReadEvent", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName1", "localhost", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName2", "192.168.1.1", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName3", "MYMACHINE", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName4", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName5", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetBackupExecName", "192.168.1.1", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "BackupExecNameString", "BackupExec", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "swErrorOnly", "YES", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "swLast7Days", "YES", sModulePath))
		return ;


	return ;
}

