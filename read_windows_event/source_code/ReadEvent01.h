// ReadEvent01.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル
#include "globalfunc.h"


// CReadEvent01App:
// このクラスの実装については、ReadEvent01.cpp を参照してください。
//

class CReadEvent01App : public CWinApp
{
public:
	CReadEvent01App();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
	void RunAutomatic(void);
	struct __sr_init_data srInit;
};

extern CReadEvent01App theApp;
