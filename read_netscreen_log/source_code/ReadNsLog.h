// ReadNsLog.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル
#include "GlobalFunc.h"


// CReadNsLogApp:
// このクラスの実装については、ReadNsLog.cpp を参照してください。
//

class CReadNsLogApp : public CWinApp
{
public:
	CReadNsLogApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	void RunAutomatic(void);
	struct __sr_init_data srInit;
};

extern CReadNsLogApp theApp;
