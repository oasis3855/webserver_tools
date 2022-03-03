// ReadNsLogDlg.cpp : �����t�@�C��
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
#include ".\readnslogdlg.h"
#include "NsLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReadNsLogDlg �_�C�A���O



CReadNsLogDlg::CReadNsLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadNsLogDlg::IDD, pParent)
	, srInit(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReadNsLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_MODE_DAYS, dlgRadioMode);
}

BEGIN_MESSAGE_MAP(CReadNsLogDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_REPORT, OnBnClickedBtnReport)
END_MESSAGE_MAP()


// CReadNsLogDlg ���b�Z�[�W �n���h��

BOOL CReadNsLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
	time_t tmNowSec;
	struct tm *tmNow;
	tmNowSec = ::time(NULL);
	tmNow = ::localtime(&tmNowSec);

	SetDlgItemInt(IDC_EDIT_YEAR, tmNow->tm_year+1900);
	SetDlgItemInt(IDC_EDIT_MONTH, tmNow->tm_mon+1);
	SetDlgItemInt(IDC_EDIT_DAY, tmNow->tm_mday);
	SetDlgItemInt(IDC_EDIT_SPAN, srInit->nDays);

	SetDlgItemText(IDC_EDIT_MATCH1, srInit->sMatch1);
	SetDlgItemText(IDC_EDIT_MATCH2, srInit->sMatch2);
	SetDlgItemText(IDC_EDIT_MATCH3, srInit->sMatch3);
	SetDlgItemText(IDC_EDIT_MATCH4, srInit->sMatch4);
	SetDlgItemText(IDC_EDIT_MATCH5, srInit->sMatch5);
	SetDlgItemText(IDC_EDIT_MATCH6, srInit->sMatch6);

	SetDlgItemText(IDC_EDIT_EXCLUDE1, srInit->sExclude1);
	SetDlgItemText(IDC_EDIT_EXCLUDE2, srInit->sExclude2);

	dlgRadioMode.SetCheck(BST_CHECKED);		// *���ԃ��[�h�Ƀ`�F�b�N


	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CReadNsLogDlg::OnPaint() 
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
HCURSOR CReadNsLogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ***********************
// ���s�{�^�����������Ƃ��̏���
// ***********************
void CReadNsLogDlg::OnBnClickedBtnReport()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	CNsLog nsLog;
	char sTemp[128];
	int i;

	// ��v����������̏�����
	for(i=0; i<6; i++) strcpy(nsLog.sMatchStrings[i], "");

	// ��v������������A�e�L�X�g�{�b�N�X����ǂݍ���
	i=0;
	if(GetDlgItemText(IDC_EDIT_MATCH1, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_MATCH2, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_MATCH3, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_MATCH4, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_MATCH5, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_MATCH6, sTemp, 125))
	{
		strcpy(nsLog.sMatchStrings[i], sTemp);
		i++;
	}

	// ���O����������̏�����
	for(i=0; i<2; i++) strcpy(nsLog.sExcludeStrings[i], "");

	// ���O������������A�e�L�X�g�{�b�N�X����ǂݍ���
	i=0;
	if(GetDlgItemText(IDC_EDIT_EXCLUDE1, sTemp, 125))
	{
		strcpy(nsLog.sExcludeStrings[i], sTemp);
		i++;
	}
	if(GetDlgItemText(IDC_EDIT_EXCLUDE2, sTemp, 125))
	{
		strcpy(nsLog.sExcludeStrings[i], sTemp);
		i++;
	}
 
	// �����ԃ��[�h�A���ԃ��[�h�̐؂�ւ�
	if(IsDlgButtonChecked(IDC_RADIO_MODE_DAYS))
		nsLog.MakeReport(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit);
	else
		nsLog.MakeReportMonth(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), srInit);


}
