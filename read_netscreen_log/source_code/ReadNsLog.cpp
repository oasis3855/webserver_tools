// ReadNsLog.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
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
#include "ReadNsLog.h"
#include "ReadNsLogDlg.h"
#include ".\readnslog.h"
#include "nslog.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReadNsLogApp

BEGIN_MESSAGE_MAP(CReadNsLogApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CReadNsLogApp �R���X�g���N�V����

CReadNsLogApp::CReadNsLogApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CReadNsLogApp �I�u�W�F�N�g�ł��B

CReadNsLogApp theApp;


// CReadNsLogApp ������

BOOL CReadNsLogApp::InitInstance()
{
	// �A�v���P�[�V�����@�}�j�t�F�X�g���@visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll �o�[�W���� 6�@�ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �Ɂ@InitCommonControls() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	InitCommonControls();

	CWinApp::InitInstance();

	// �W��������
	// �����̋@�\���g�킸�ɁA�ŏI�I�Ȏ��s�\�t�@�C���̃T�C�Y���k���������ꍇ�́A
	// �ȉ�����A�s�v�ȏ��������[�`����
	// �폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ���̕�������A��Ж��܂��͑g�D���Ȃǂ́A
	// �K�؂ȕ�����ɕύX���Ă��������B


	// INI�t�@�C���̓ǂݍ��݁A�����ꍇ�͐V�K�쐬���s��
	if(GetInitData(&srInit) != 0)
	{
		MessageBox(NULL, "INI�t�@�C���ɏ����l��ݒ肵�Ă�������", "Important Message", MB_ICONINFORMATION);
		return FALSE;
	}

	// �v���O�����̈����ɂ��A�������s���_�C�A���O�\�����s��
	if(!strcmp(this->m_lpCmdLine, ""))
	{
		// �_�C�A���O�\��
		CReadNsLogDlg dlg;
		m_pMainWnd = &dlg;
		dlg.srInit = &srInit;
		dlg.DoModal();
	}
	else
	{
		// �������s
		RunAutomatic();
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

// ***********************
// �������s���s�� �i�^�C�}�[�ȂǂŒ���I�Ƀo�b�N�O���E���h�Ŏ��s���邱�Ƃ�z��j
// ***********************
void CReadNsLogApp::RunAutomatic(void)
{
	CNsLog nsLog;
	time_t tmNowSec;
	struct tm *tmNow;

	tmNowSec = ::time(NULL);
	tmNow = ::localtime(&tmNowSec);

	// Set Match List Strings
	strcpy(nsLog.sMatchStrings[0], srInit.sMatch1);
	strcpy(nsLog.sMatchStrings[1], srInit.sMatch2);
	strcpy(nsLog.sMatchStrings[2], srInit.sMatch3);
	strcpy(nsLog.sMatchStrings[3], srInit.sMatch4);
	strcpy(nsLog.sMatchStrings[4], srInit.sMatch5);
	strcpy(nsLog.sMatchStrings[5], srInit.sMatch6);

	// Set Exclude List Strings
	strcpy(nsLog.sExcludeStrings[0], srInit.sExclude1);
	strcpy(nsLog.sExcludeStrings[1], srInit.sExclude2);

	nsLog.MakeReport(tmNow->tm_year+1900, tmNow->tm_mon+1, tmNow->tm_mday, srInit.nDays, &srInit);


}
