// ReadEvent01Dlg.cpp : �����t�@�C��
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
#include "ReadEvent01.h"
#include "ReadEvent01Dlg.h"
#include ".\readevent01dlg.h"
#include "ReadEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReadEvent01Dlg �_�C�A���O



CReadEvent01Dlg::CReadEvent01Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReadEvent01Dlg::IDD, pParent)
	, srInit(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReadEvent01Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_PC, LstPcName);
	DDX_Control(pDX, IDC_CHK_7DAYS, btn7days);
	DDX_Control(pDX, IDC_CHK_ERROR_ONLY, btnErrorOnly);
}

BEGIN_MESSAGE_MAP(CReadEvent01Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_BTN_READEVENT, OnBnClickedBtnReadevent)
ON_BN_CLICKED(IDC_BTN_READEVENTALL, OnBnClickedBtnReadeventall)
END_MESSAGE_MAP()


// CReadEvent01Dlg ���b�Z�[�W �n���h��

BOOL CReadEvent01Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	CString sTemp;

	// �^�[�Q�b�g�̃R���s���[�^�������X�g�{�b�N�X�ɃZ�b�g
	if(strlen(srInit->sTarget1)) LstPcName.AddString(srInit->sTarget1);
	if(strlen(srInit->sTarget2)) LstPcName.AddString(srInit->sTarget2);
	if(strlen(srInit->sTarget3)) LstPcName.AddString(srInit->sTarget3);
	if(strlen(srInit->sTarget4)) LstPcName.AddString(srInit->sTarget4);
	if(strlen(srInit->sTarget5)) LstPcName.AddString(srInit->sTarget5);
	if(strlen(srInit->sTargetBackupExec))
	{
		sTemp.Format("*%s:%s", srInit->sTargetBackupExec, srInit->sBackupExecName);
		LstPcName.AddString(sTemp);
	}

	LstPcName.SetCurSel(0);		// ���X�g�{�b�N�X�擪��I��

	// �ꊇ���s�̑ΏۃR���s���[�^�����e�L�X�g�{�b�N�X�ɃZ�b�g
	sTemp = "";
	if(strlen(srInit->sTarget1)) sTemp = sTemp + srInit->sTarget1 + "\n";
	if(strlen(srInit->sTarget2)) sTemp = sTemp + srInit->sTarget2 + "\n";
	if(strlen(srInit->sTarget3)) sTemp = sTemp + srInit->sTarget3 + "\n";
	if(strlen(srInit->sTarget4)) sTemp = sTemp + srInit->sTarget4 + "\n";
	if(strlen(srInit->sTarget5)) sTemp = sTemp + srInit->sTarget5 + "\n";

	SetDlgItemText(IDC_TEXT_ALL_SERVER, sTemp);

	// �`�F�b�N�{�b�N�X�̏����l���Z�b�g
	if(srInit->b7days) btn7days.SetCheck(BST_CHECKED);
	if(srInit->bErrorOnly) btnErrorOnly.SetCheck(BST_CHECKED);
	
	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CReadEvent01Dlg::OnPaint() 
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
HCURSOR CReadEvent01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ***********************
// �C�x���g��HTML���{�^���������ꂽ�Ƃ�
//
// ���X�g�{�b�N�X�őI������Ă���R���s���[�^�̃C�x���g��ǂݎ��HTML������
// ***********************
void CReadEvent01Dlg::OnBnClickedBtnReadevent()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	BOOL b7days;
	BOOL bErrorOnly;
	char sPcName[1024];
	CReadEvent evt;

	if(IsDlgButtonChecked(IDC_CHK_7DAYS)) b7days = TRUE;
	else b7days = FALSE;

	if(IsDlgButtonChecked(IDC_CHK_ERROR_ONLY)) bErrorOnly = TRUE;
	else bErrorOnly = FALSE;


	LstPcName.GetText(LstPcName.GetCurSel(), sPcName);


	if(sPcName[0] != '*')
	{
		// �ʏ�̃C�x���g���o
		evt.WriteToLogFile(b7days, bErrorOnly, sPcName, srInit);
	}
	else
	{
		// �o�b�N�A�b�v �v���O�����p�̃C�x���g���o
		evt.WriteBackupExecLog(b7days, false, srInit->sTargetBackupExec, srInit);
	}

}

// ***********************
// �C�x���g��HTML���{�^���i�S�}�V���j�������ꂽ�Ƃ�
//
// �S�ẴR���s���[�^�iINI�t�@�C���ɐݒ肳�ꂽ���́j�̃C�x���g��ǂݎ��HTML������
// ***********************
void CReadEvent01Dlg::OnBnClickedBtnReadeventall()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	BOOL b7days;
	BOOL bErrorOnly;
	CReadEvent evt;

	if(IsDlgButtonChecked(IDC_CHK_7DAYS)) b7days = TRUE;
	else b7days = FALSE;

	if(IsDlgButtonChecked(IDC_CHK_ERROR_ONLY)) bErrorOnly = TRUE;
	else bErrorOnly = FALSE;


	// INI����ǂݏo�����ΏۃR���s���[�^�S�Ăɑ΂��ăC�x���g�𒊏o��HTML������
	if(strlen(srInit->sTarget1)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget1, srInit);
	if(strlen(srInit->sTarget2)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget2, srInit);
	if(strlen(srInit->sTarget3)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget3, srInit);
	if(strlen(srInit->sTarget4)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget4, srInit);
	if(strlen(srInit->sTarget5)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget5, srInit);

}



