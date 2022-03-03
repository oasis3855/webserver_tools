// ExecAnalog01Dlg.h : ヘッダー ファイル
//

#pragma once
#include "monthdata.h"


// CExecAnalog01Dlg ダイアログ
class CExecAnalog01Dlg : public CDialog
{
// コンストラクション
public:
	CExecAnalog01Dlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_EXECANALOG01_DIALOG };

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
	CMonthData MonthData;
	afx_msg void OnBnClickedBtnExecnow();
	struct __sr_init_data *srInit;
};
