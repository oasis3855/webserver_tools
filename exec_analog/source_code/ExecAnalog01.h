// ExecAnalog01.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル
#include "GlobalFunc.h"


// CExecAnalog01App:
// このクラスの実装については、ExecAnalog01.cpp を参照してください。
//

class CExecAnalog01App : public CWinApp
{
public:
	CExecAnalog01App();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	struct __sr_init_data srInit;
};

extern CExecAnalog01App theApp;

