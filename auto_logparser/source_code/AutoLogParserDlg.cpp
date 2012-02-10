// AutoLogParserDlg.cpp : 実装ファイル
//

// ***********************
// AutoLogParser  (Microsoft Log ParserでIISログファイルを処理するためのバッチファイル作成ソフト）
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
//


#include "stdafx.h"
#include "AutoLogParser.h"
#include "AutoLogParserDlg.h"
#include ".\autologparserdlg.h"
#include "GlobalFunc.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoLogParserDlg ダイアログ



CAutoLogParserDlg::CAutoLogParserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLogParserDlg::IDD, pParent)
	, srInit(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoLogParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAutoLogParserDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_BTN_MAKECFG, OnBnClickedBtnMakecfg)
ON_BN_CLICKED(IDC_BTN_MAKECFG_AND_RUN, OnBnClickedBtnMakecfgAndRun)
END_MESSAGE_MAP()


// CAutoLogParserDlg メッセージ ハンドラ

BOOL CAutoLogParserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	time_t tmNowSec;
	struct tm *tmNow;

	// 現在の日時を得る
	tmNowSec = ::time(NULL);
	tmNow = ::localtime(&tmNowSec);

	// ダイアログの初期設定値 （現在の年月日をセット）
	SetDlgItemInt(IDC_EDIT_YEAR, tmNow->tm_year+1900);
	SetDlgItemInt(IDC_EDIT_MONTH, tmNow->tm_mon+1);
	SetDlgItemInt(IDC_EDIT_DAY, tmNow->tm_mday);
	SetDlgItemInt(IDC_EDIT_SPAN, 7);

	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CAutoLogParserDlg::OnPaint() 
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
HCURSOR CAutoLogParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ***********************
// コンフィギュレーションファイルの作成
// LogParser実行用のバッチファイル、LogParser用のSQL命令ファイルを作成する
// ***********************
void CAutoLogParserDlg::OnBnClickedBtnMakecfg()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。

	// バッチファイル、SQL命令ファイルを作成
	if(!::MakeConfigFile(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit))
	{
		this->MessageBox("自動実行用バッチファイル、SQLファイルの作成に失敗しました");
		return;
	}
}

// ***********************
// コンフィギュレーションファイルの作成 ＆ LogParser 実行
// LogParser実行用のバッチファイル、LogParser用のSQL命令ファイルを作成する
// 作成したLogparser実行・ファイル結合用バッチファイルを実行する
// ***********************
void CAutoLogParserDlg::OnBnClickedBtnMakecfgAndRun()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。

	// Log Parser 実行ファイルが存在するか確認する
	CString sTemp;
	sTemp.Format("%s%s", srInit->sLogParserDir, srInit->sLogParserProgName);
	if(::_access(sTemp, 0) != 0)
	{
		this->MessageBox("Microsoft Log Parser実行ファイルが見つかりません");
		return;
	}

	// バッチファイル、SQL命令ファイルを作成
	if(!::MakeConfigFile(GetDlgItemInt(IDC_EDIT_YEAR), GetDlgItemInt(IDC_EDIT_MONTH), GetDlgItemInt(IDC_EDIT_DAY), GetDlgItemInt(IDC_EDIT_SPAN), srInit))
	{
		this->MessageBox("自動実行用バッチファイル、SQLファイルの作成に失敗しました");
		return;
	}

	// LogParserを実行
	if(!::RunLogParser(srInit))
	{
		this->MessageBox("自動実行用バッチファイルが実行できませんでした");
		return;
	}
}

