// ReadNsLogDlg.cpp : 実装ファイル
//

// ***********************
// ReadNsLog  （NwtScreenのSyslogから条件抽出するプログラム）
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
#include "ReadNsLog.h"
#include "ReadNsLogDlg.h"
#include ".\readnslogdlg.h"
#include "NsLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReadNsLogDlg ダイアログ



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


// CReadNsLogDlg メッセージ ハンドラ

BOOL CReadNsLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
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

	dlgRadioMode.SetCheck(BST_CHECKED);		// *日間モードにチェック


	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CReadNsLogDlg::OnPaint() 
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
HCURSOR CReadNsLogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ***********************
// 実行ボタンを押したときの処理
// ***********************
void CReadNsLogDlg::OnBnClickedBtnReport()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	CNsLog nsLog;
	char sTemp[128];
	int i;

	// 一致条件文字列の初期化
	for(i=0; i<6; i++) strcpy(nsLog.sMatchStrings[i], "");

	// 一致条件文字列を、テキストボックスから読み込む
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

	// 除外条件文字列の初期化
	for(i=0; i<2; i++) strcpy(nsLog.sExcludeStrings[i], "");

	// 除外条件文字列を、テキストボックスから読み込む
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
 
	// ｎ日間モード、月間モードの切り替え
	if(IsDlgButtonChecked(IDC_RADIO_MODE_DAYS))
		nsLog.MakeReport(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit);
	else
		nsLog.MakeReportMonth(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), srInit);


}
