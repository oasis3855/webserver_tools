// GlobalFunc.cpp : 実装ファイル　（INIファイル読み書き、バッチファイル作成、SQL命令ファイル作成、LogParser実行）
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
#include "Globalfunc.h"

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
	GetPrivateProfileString("ReadEvent", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI が確認できない場合、新規作成
		return 1;
	}


	// 設定値読み込み

	GetPrivateProfileString("ReadEvent", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetName1", "err", srInit->sTarget1, 254, sModulePath);
	if(!strcmp(srInit->sTarget1, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetName2", "err", srInit->sTarget2, 254, sModulePath);
	if(!strcmp(srInit->sTarget2, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetName3", "err", srInit->sTarget3, 254, sModulePath);
	if(!strcmp(srInit->sTarget3, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetName4", "err", srInit->sTarget4, 254, sModulePath);
	if(!strcmp(srInit->sTarget4, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetName5", "err", srInit->sTarget5, 254, sModulePath);
	if(!strcmp(srInit->sTarget5, "err")) return 1;			// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "TargetBackupExecName", "err", srInit->sTargetBackupExec, 254, sModulePath);
	if(!strcmp(srInit->sTargetBackupExec, "err")) return 1;	// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "BackupExecNameString", "err", srInit->sBackupExecName, 254, sModulePath);
	if(!strcmp(srInit->sBackupExecName, "err")) return 1;	// 設定値読み込み不能

	GetPrivateProfileString("ReadEvent", "swErrorOnly", "err", sTemp, 254, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;						// 設定値読み込み不能
	if(!strcmpi(sTemp, "yes")) srInit->bErrorOnly = true;
	else srInit->bErrorOnly = false;

	GetPrivateProfileString("ReadEvent", "swLast7Days", "err", sTemp, 254, sModulePath);
	if(!strcmp(sTemp, "err")) return 1;						// 設定値読み込み不能
	if(!strcmpi(sTemp, "yes")) srInit->b7days = true;
	else srInit->b7days = false;

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

	if(!WritePrivateProfileString("ReadEvent", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName1", "localhost", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName2", "192.168.1.1", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName3", "MYMACHINE", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName4", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetName5", "", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "TargetBackupExecName", "192.168.1.1", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "BackupExecNameString", "BackupExec", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "swErrorOnly", "YES", sModulePath))
		return ;
	if(!WritePrivateProfileString("ReadEvent", "swLast7Days", "YES", sModulePath))
		return ;


	return ;
}

