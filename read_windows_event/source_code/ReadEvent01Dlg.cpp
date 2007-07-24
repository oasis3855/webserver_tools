// ReadEvent01Dlg.cpp : 実装ファイル
//

// ***********************
// ReadEvent  (Windowsのシステム ログを抽出し、HTML化するプログラム）
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
#include "ReadEvent01.h"
#include "ReadEvent01Dlg.h"
#include ".\readevent01dlg.h"
#include "ReadEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CReadEvent01Dlg ダイアログ



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


// CReadEvent01Dlg メッセージ ハンドラ

BOOL CReadEvent01Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	CString sTemp;

	// ターゲットのコンピュータ名をリストボックスにセット
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

	LstPcName.SetCurSel(0);		// リストボックス先頭を選択

	// 一括実行の対象コンピュータ名をテキストボックスにセット
	sTemp = "";
	if(strlen(srInit->sTarget1)) sTemp = sTemp + srInit->sTarget1 + "\n";
	if(strlen(srInit->sTarget2)) sTemp = sTemp + srInit->sTarget2 + "\n";
	if(strlen(srInit->sTarget3)) sTemp = sTemp + srInit->sTarget3 + "\n";
	if(strlen(srInit->sTarget4)) sTemp = sTemp + srInit->sTarget4 + "\n";
	if(strlen(srInit->sTarget5)) sTemp = sTemp + srInit->sTarget5 + "\n";

	SetDlgItemText(IDC_TEXT_ALL_SERVER, sTemp);

	// チェックボックスの初期値をセット
	if(srInit->b7days) btn7days.SetCheck(BST_CHECKED);
	if(srInit->bErrorOnly) btnErrorOnly.SetCheck(BST_CHECKED);
	
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CReadEvent01Dlg::OnPaint() 
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
HCURSOR CReadEvent01Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ***********************
// イベントのHTML化ボタンを押されたとき
//
// リストボックスで選択されているコンピュータのイベントを読み取りHTML化する
// ***********************
void CReadEvent01Dlg::OnBnClickedBtnReadevent()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。

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
		// 通常のイベント抽出
		evt.WriteToLogFile(b7days, bErrorOnly, sPcName, srInit);
	}
	else
	{
		// バックアップ プログラム用のイベント抽出
		evt.WriteBackupExecLog(b7days, false, srInit->sTargetBackupExec, srInit);
	}

}

// ***********************
// イベントのHTML化ボタン（全マシン）を押されたとき
//
// 全てのコンピュータ（INIファイルに設定されたもの）のイベントを読み取りHTML化する
// ***********************
void CReadEvent01Dlg::OnBnClickedBtnReadeventall()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。

	BOOL b7days;
	BOOL bErrorOnly;
	CReadEvent evt;

	if(IsDlgButtonChecked(IDC_CHK_7DAYS)) b7days = TRUE;
	else b7days = FALSE;

	if(IsDlgButtonChecked(IDC_CHK_ERROR_ONLY)) bErrorOnly = TRUE;
	else bErrorOnly = FALSE;


	// INIから読み出した対象コンピュータ全てに対してイベントを抽出しHTML化する
	if(strlen(srInit->sTarget1)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget1, srInit);
	if(strlen(srInit->sTarget2)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget2, srInit);
	if(strlen(srInit->sTarget3)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget3, srInit);
	if(strlen(srInit->sTarget4)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget4, srInit);
	if(strlen(srInit->sTarget5)) evt.WriteToLogFile(b7days, bErrorOnly, srInit->sTarget5, srInit);

}



