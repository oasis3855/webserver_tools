// ReadEvent01Dlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"


// CReadEvent01Dlg ダイアログ
class CReadEvent01Dlg : public CDialog
{
// コンストラクション
public:
	CReadEvent01Dlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_READEVENT01_DIALOG };

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
	afx_msg void OnBnClickedBtnReadevent();
	CListBox LstPcName;
	CButton btn7days;
	CButton btnErrorOnly;
	afx_msg void OnBnClickedBtnReadeventall();
	afx_msg void OnBnClickedBtnAbout();
	__sr_init_data *srInit;
};
