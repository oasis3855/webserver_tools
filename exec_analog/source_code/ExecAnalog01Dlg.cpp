// ExecAnalog01Dlg.cpp : �����t�@�C��
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
#include "ExecAnalog01.h"
#include "ExecAnalog01Dlg.h"
#include ".\execanalog01dlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExecAnalog01Dlg �_�C�A���O



CExecAnalog01Dlg::CExecAnalog01Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExecAnalog01Dlg::IDD, pParent)
	, srInit(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExecAnalog01Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExecAnalog01Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EXECNOW, OnBnClickedBtnExecnow)
END_MESSAGE_MAP()


// CExecAnalog01Dlg ���b�Z�[�W �n���h��

BOOL CExecAnalog01Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	// ���݂̔N���A�O���̔N�����Z�b�g����
	SetDlgItemInt(IDC_TXT_MONTH_NOW, MonthData.GetMonthNow());
	SetDlgItemInt(IDC_TXT_YEAR_NOW, MonthData.GetYearNow());
	SetDlgItemInt(IDC_TXT_MONTH_PREV, MonthData.GetMonthPrev());
	SetDlgItemInt(IDC_TXT_YEAR_PREV, MonthData.GetYearPrev());
	
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CExecAnalog01Dlg::OnPaint() 
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
HCURSOR CExecAnalog01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ***********************
// Analog.cfg �쐬�� Analog �̎��s
// ***********************
void CExecAnalog01Dlg::OnBnClickedBtnExecnow()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	// �����̃��O�t�@�C���p
	ExecAnalog(GetDlgItemInt(IDC_TXT_YEAR_NOW), GetDlgItemInt(IDC_TXT_MONTH_NOW), "log", srInit);

	// �挎�̃��O�t�@�C���p
	ExecAnalog(GetDlgItemInt(IDC_TXT_YEAR_PREV), GetDlgItemInt(IDC_TXT_MONTH_PREV), "logprev", srInit);

}
