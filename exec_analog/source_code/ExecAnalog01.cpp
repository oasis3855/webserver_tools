// ExecAnalog01.cpp : アプリケーションのクラス動作を定義します。
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
#include "GlobalFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExecAnalog01App

BEGIN_MESSAGE_MAP(CExecAnalog01App, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CExecAnalog01App コンストラクション

CExecAnalog01App::CExecAnalog01App()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CExecAnalog01App オブジェクトです。

CExecAnalog01App theApp;


// CExecAnalog01App 初期化

BOOL CExecAnalog01App::InitInstance()
{
	// アプリケーション　マニフェストが　visual スタイルを有効にするために、
	// ComCtl32.dll バージョン 6　以降の使用を指定する場合は、
	// Windows XP に　InitCommonControls() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	InitCommonControls();

	CWinApp::InitInstance();

	// 標準初期化
	// これらの機能を使わずに、最終的な実行可能ファイルのサイズを縮小したい場合は、
	// 以下から、不要な初期化ルーチンを
	// 削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: この文字列を、会社名または組織名などの、
	// 適切な文字列に変更してください。
//	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	// INIファイルの読み込み、無い場合は新規作成を行う
	if(GetInitData(&srInit) != 0)
	{
		MessageBox(NULL, "INIファイルに初期値を設定してください", "Important Message", MB_ICONINFORMATION);
		return FALSE;
	}

	// プログラムの引数により、自動実行かダイアログ表示を行う
	if(!strcmp(this->m_lpCmdLine, ""))
	{
		// ダイアログ表示
		CExecAnalog01Dlg dlg;
		m_pMainWnd = &dlg;
		dlg.srInit = &srInit;
		dlg.DoModal();
	}
	else
	{
		// 自動実行
		_RunAutomatic(&srInit);
	}


	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}


