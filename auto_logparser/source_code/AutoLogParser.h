// AutoLogParser.h : アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル
#include "GlobalFunc.h"


// CAutoLogParserApp:
// このクラスの実装については、AutoLogParser.cpp を参照してください。
//

class CAutoLogParserApp : public CWinApp
{
public:
	CAutoLogParserApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	bool RunAutomatick(void);
	struct __sr_init_data srInit;
};

extern CAutoLogParserApp theApp;
