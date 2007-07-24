// GlobalFunc.cpp : 実装ファイル　（INIファイル読み書き）
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
#include "GlobalFunc.h"


// ***********************
// INIファイルから初期値を読み込む
// ***********************
int GetInitData(__sr_init_data *srInit)
{
	char sModulePath[1024];
	char *sPtr;
	char sTemp[1024];
	int i;
	size_t nStrSize;

	// 現在のモジュールのフルパス名を得る
	i = GetModuleFileName(NULL, sTemp, 1023);
	if(i<=0 || i>=1023) return 1;		// 現在のプロセスのパス名が得られなかった


	// モジュールパスの最後の . の位置までをファイル名のボディとする
	sPtr = strrchr(sTemp, '.');
	if(sPtr == NULL)
		nStrSize = strlen(sTemp);		// . が無い場合、パス名全体とする
	else
		nStrSize = strlen(sTemp) - strlen(sPtr);
	strncpy(sModulePath, sTemp, nStrSize);
	sModulePath[nStrSize] = (char)NULL;

	// INI 拡張子をつける
	strcat(sModulePath, ".ini");


	// INIファイルが存在するかの確認
	GetPrivateProfileString("ReadNsLog", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI が確認できない場合、新規作成
		return 1;
	}


	// 設定値読み込み

	GetPrivateProfileString("ReadNsLog", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "OutputFileName", "err", srInit->sOutputFileName, 253, sModulePath);
	if(!strcmp(srInit->sOutputFileName, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "SyslogDir", "err", srInit->sSyslogDir, 1023, sModulePath);
	if(!strcmp(srInit->sSyslogDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "SyslogFileName", "err", srInit->sSyslogFileName, 253, sModulePath);
	if(!strcmp(srInit->sSyslogFileName, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString1", "err", srInit->sMatch1, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString2", "err", srInit->sMatch2, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 2;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString3", "err", srInit->sMatch3, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 3;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString4", "err", srInit->sMatch4, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 4;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString5", "err", srInit->sMatch5, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 5;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "MatchString6", "err", srInit->sMatch6, 125, sModulePath);
	if(!strcmp(srInit->sMatch1, "err")) return 6;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "ExcludeString1", "err", srInit->sExclude1, 125, sModulePath);
	if(!strcmp(srInit->sExclude1, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "ExcludeString2", "err", srInit->sExclude2, 125, sModulePath);
	if(!strcmp(srInit->sExclude2, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ReadNsLog", "Days", "err", sTemp, 125, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;							// 設定値読み込み不能
	srInit->nDays = atoi(sTemp);

	return 0;
}


// ***********************
// 新規のINIファイルを作成する
// ***********************
void WriteInitIniFile(void)
{
	char sModulePath[1024];
	char *sPtr;
	char sTemp[1024];
	int i;
	size_t nStrSize;

	// 現在のモジュールのフルパス名を得る
	i = GetModuleFileName(NULL, sTemp, 1023);
	if(i<=0 || i>=1023) return ;		// 現在のプロセスのパス名が得られなかった


	// モジュールパスの最後の . の位置までをファイル名のボディとする
	sPtr = strrchr(sTemp, '.');
	if(sPtr == NULL)
		nStrSize = strlen(sTemp);		// . が無い場合、パス名全体とする
	else
		nStrSize = strlen(sTemp) - strlen(sPtr);
	strncpy(sModulePath, sTemp, nStrSize);
	sModulePath[nStrSize] = (char)NULL;

	// INI 拡張子をつける
	strcat(sModulePath, ".ini");


	// サンプルのINIファイルを書き込む

	if(!WritePrivateProfileString("ReadNsLog", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "OutputFileName", "NetscreenQuery.txt", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "SyslogDir", "d:\\Syslog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "SyslogFileName", "Syslog-%d-%02d-%02d.log", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString1", "action=Deny", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString2", "emergency", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString3", "critical", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString4", "alert", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString5", "warning", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "MatchString6", "error", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "ExcludeString1", "src_port=80", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "ExcludeString2", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadNsLog", "Days", "7", sModulePath))
		return ;


	return ;
}
