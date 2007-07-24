// AutoLogParser.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
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
#include "AutoLogParserDlg.h"
#include "GlobalFunc.h"
#include ".\autologparser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoLogParserApp

BEGIN_MESSAGE_MAP(CAutoLogParserApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoLogParserApp �R���X�g���N�V����

CAutoLogParserApp::CAutoLogParserApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CAutoLogParserApp �I�u�W�F�N�g�ł��B

CAutoLogParserApp theApp;


// CAutoLogParserApp ������

BOOL CAutoLogParserApp::InitInstance()
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
		CAutoLogParserDlg dlg;
		m_pMainWnd = &dlg;
		dlg.srInit = &srInit;
		dlg.DoModal();
	}
	else
	{
		// �������s
		RunAutomatick();
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

// ***********************
// �������s���s�� �i�^�C�}�[�ȂǂŒ���I�Ƀo�b�N�O���E���h�Ŏ��s���邱�Ƃ�z��j
// ***********************
void CAutoLogParserApp::RunAutomatick(void)
{
	time_t tmNowSec;
	struct tm *tmNow;
	// ���݂̓����𓾂�
	tmNowSec = ::time(NULL);
	tmNow = ::localtime(&tmNowSec);

	// �o�b�`�t�@�C���ASQL���߃t�@�C�����쐬
	::MakeConfigFile(tmNow->tm_year+1900, tmNow->tm_mon+1, tmNow->tm_mday, 7, &srInit);

	// LogParser�����s
	::RunLogParser(&srInit);
}
