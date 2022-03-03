// ReadNsLogDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "GlobalFunc.h"

// CReadNsLogDlg ダイアログ
class CReadNsLogDlg : public CDialog
{
// コンストラクション
public:
	CReadNsLogDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_READNSLOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReport();
	CButton dlgRadioMode;
	struct __sr_init_data *srInit;
};
