// AutoLogParserDlg.cpp : �����t�@�C��
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
#include ".\autologparserdlg.h"
#include "GlobalFunc.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoLogParserDlg �_�C�A���O



CAutoLogParserDlg::CAutoLogParserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLogParserDlg::IDD, pParent)
	, srInit(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoLogParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoLogParserDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_BTN_MAKECFG, OnBnClickedBtnMakecfg)
ON_BN_CLICKED(IDC_BTN_MAKECFG_AND_RUN, OnBnClickedBtnMakecfgAndRun)
END_MESSAGE_MAP()


// CAutoLogParserDlg ���b�Z�[�W �n���h��

BOOL CAutoLogParserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	time_t tmNowSec;
	struct tm *tmNow;

	// ���݂̓����𓾂�
	tmNowSec = ::time(NULL);
	tmNow = ::localtime(&tmNowSec);

	// �_�C�A���O�̏����ݒ�l �i���݂̔N�������Z�b�g�j
	SetDlgItemInt(IDC_EDIT_YEAR, tmNow->tm_year+1900);
	SetDlgItemInt(IDC_EDIT_MONTH, tmNow->tm_mon+1);
	SetDlgItemInt(IDC_EDIT_DAY, tmNow->tm_mday);
	SetDlgItemInt(IDC_EDIT_SPAN, 7);

	
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CAutoLogParserDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CAutoLogParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ***********************
// �R���t�B�M�����[�V�����t�@�C���̍쐬
// LogParser���s�p�̃o�b�`�t�@�C���ALogParser�p��SQL���߃t�@�C�����쐬����
// ***********************
void CAutoLogParserDlg::OnBnClickedBtnMakecfg()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	// �o�b�`�t�@�C���ASQL���߃t�@�C�����쐬
	if(!::MakeConfigFile(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit))
	{
		this->MessageBox("�������s�p�o�b�`�t�@�C���ASQL�t�@�C���̍쐬�Ɏ��s���܂���");
		return;
	}
}

// ***********************
// �R���t�B�M�����[�V�����t�@�C���̍쐬 �� LogParser ���s
// LogParser���s�p�̃o�b�`�t�@�C���ALogParser�p��SQL���߃t�@�C�����쐬����
// �쐬����Logparser���s�E�t�@�C�������p�o�b�`�t�@�C�������s����
// ***********************
void CAutoLogParserDlg::OnBnClickedBtnMakecfgAndRun()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	// Log Parser ���s�t�@�C�������݂��邩�m�F����
	CString sTemp;
	sTemp.Format("%s%s", srInit->sLogParserDir, srInit->sLogParserProgName);
	if(::_access(sTemp, 0) != 0)
	{
		this->MessageBox("Microsoft Log Parser���s�t�@�C����������܂���");
		return;
	}

	// �o�b�`�t�@�C���ASQL���߃t�@�C�����쐬
	if(!::MakeConfigFile(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit))
	{
		this->MessageBox("�������s�p�o�b�`�t�@�C���ASQL�t�@�C���̍쐬�Ɏ��s���܂���");
		return;
	}

	// LogParser�����s
	if(!::RunLogParser(srInit))
	{
		this->MessageBox("�������s�p�o�b�`�t�@�C�������s�ł��܂���ł���");
		return;
	}
}

