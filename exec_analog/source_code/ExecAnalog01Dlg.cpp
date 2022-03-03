// ExecAnalog01Dlg.cpp : 実装ファイル
//

// ***********************
// ExecAnalog  （今月および先月のIISデータをAnalogで解析する自動化プログラム）
// 
// Copyright (C) 2007 INOUE. Hirokazu
//
// 
// http://www.opensource.jp/gpl/gpl.ja.html
// このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
// ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン2か、希
// 望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
// または改変することができます。
// 
// このプログラムは有用であることを願って頒布されますが、*全くの無保証* 
// です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
// め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
// 
// あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
// 受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
// で請求してください(宛先は the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA)。

#include "stdafx.h"
#include "ExecAnalog01.h"
#include "ExecAnalog01Dlg.h"
#include ".\execanalog01dlg.h"
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExecAnalog01Dlg ダイアログ



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


// CExecAnalog01Dlg メッセージ ハンドラ

BOOL CExecAnalog01Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// 現在の年月、前月の年月をセットする
	SetDlgItemInt(IDC_TXT_MONTH_NOW, MonthData.GetMonthNow());
	SetDlgItemInt(IDC_TXT_YEAR_NOW, MonthData.GetYearNow());
	SetDlgItemInt(IDC_TXT_MONTH_PREV, MonthData.GetMonthPrev());
	SetDlgItemInt(IDC_TXT_YEAR_PREV, MonthData.GetYearPrev());
	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CExecAnalog01Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CExecAnalog01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ***********************
// Analog.cfg 作成と Analog の実行
// ***********************
void CExecAnalog01Dlg::OnBnClickedBtnExecnow()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。

	// 今月のログファイル用
	ExecAnalog(GetDlgItemInt(IDC_TXT_YEAR_NOW), GetDlgItemInt(IDC_TXT_MONTH_NOW), "log", srInit);

	// 先月のログファイル用
	ExecAnalog(GetDlgItemInt(IDC_TXT_YEAR_PREV), GetDlgItemInt(IDC_TXT_MONTH_PREV), "logprev", srInit);

}
