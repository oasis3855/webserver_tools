// ExecAnalog01Dlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "monthdata.h"


// CExecAnalog01Dlg �_�C�A���O
class CExecAnalog01Dlg : public CDialog
{
// �R���X�g���N�V����
public:
	CExecAnalog01Dlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_EXECANALOG01_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMonthData MonthData;
	afx_msg void OnBnClickedBtnExecnow();
	struct __sr_init_data *srInit;
};
