// AutoLogParserDlg.h : ヘッダー ファイル
//

#pragma once

#include "GlobalFunc.h"

// CAutoLogParserDlg ダイアログ
class CAutoLogParserDlg : public CDialog
{
// コンストラクション
public:
	CAutoLogParserDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_AUTOLOGPARSER_DIALOG };

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
	struct __sr_init_data *srInit;
	afx_msg void OnBnClickedBtnMakecfg();
	afx_msg void OnBnClickedBtnMakecfgAndRun();
};
