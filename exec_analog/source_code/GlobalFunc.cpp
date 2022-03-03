// GlobalFunc.cpp : 実装ファイル　（INIファイル読み書き、Analog.cfg作成、Analog実行、自動実行）
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
#include "GlobalFunc.h"
#include "MonthData.h"


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
	GetPrivateProfileString("ExecAnalog", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI が確認できない場合、新規作成
		return 1;
	}


	// 設定値読み込み

	GetPrivateProfileString("ExecAnalog", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ExecAnalog", "AnalogDir", "err", srInit->sAnalogDir, 1023, sModulePath);
	if(!strcmp(srInit->sAnalogDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ExecAnalog", "AnalogProgName", "err", srInit->sAnarogProgName, 254, sModulePath);
	if(!strcmp(srInit->sAnarogProgName, "err")) return 1;	// 設定値読み込み不能

	GetPrivateProfileString("ExecAnalog", "AnalogCfgTemplate", "err", srInit->sAnalogCfgTemplate, 254, sModulePath);
	if(!strcmp(srInit->sAnalogCfgTemplate, "err")) return 1;// 設定値読み込み不能

	GetPrivateProfileString("ExecAnalog", "IISLogDir", "err", srInit->sIISLogDir, 1023, sModulePath);
	if(!strcmp(srInit->sIISLogDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("ExecAnalog", "IISLogName", "err", srInit->sIISLogName, 254, sModulePath);
	if(!strcmp(srInit->sIISLogName, "err")) return 1;		// 設定値読み込み不能

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

	if(!WritePrivateProfileString("ExecAnalog", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "OutputDir", "d:\\Inetpub\\wwwroot\\analog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogDir", "c:\\Program Files\\analog\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogProgName", "analog.exe", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "AnalogCfgTemplate", "analog.cfg.tmpl", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "IISLogDir", "d:\\Inetpub\\Log\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("ExecAnalog", "IISLogName", "ex%02d%02d*.log", sModulePath))
		return ;


	return ;
}

void ExecAnalog(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char sAnalogProgFullPath[1024+256];

	// analog.cfgファイルの作成
	::_CfgFileMaker(nYear, nMonth, sOutputSubDir, srInit);

	// Analog.exe のフルパス作成
	sprintf(sAnalogProgFullPath, "%s%s", srInit->sAnalogDir, srInit->sAnarogProgName);

	// Analogプロセスの起動
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);	// 何も指定しない場合でも、 si は必要
	if(!::CreateProcess(NULL, sAnalogProgFullPath, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, srInit->sAnalogDir, &si, &pi))
	{
		// プロセス作成失敗
	}
	else
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);
	}

}

// ***********************
// 自動実行を行う （タイマーなどで定期的にバックグラウンドで実行することを想定）
//
// （今月、先月のIISログを対象とする）
// ***********************
void _RunAutomatic(struct __sr_init_data *srInit)
{
	CMonthData MonthData;

	// 今月のログファイル用
	ExecAnalog(MonthData.GetYearNow(), MonthData.GetMonthNow(), "log", srInit);

	// 先月のログファイル用
	ExecAnalog(MonthData.GetYearPrev(), MonthData.GetMonthPrev(), "logprev", srInit);

}


// ***********************
// analog.cfg の作成
// ***********************
bool _CfgFileMaker(int nYear, int nMonth, char *sOutputSubDir, struct __sr_init_data *srInit)
{

	FILE *fi, *fo;
	char sTemplateFile[1024+256];
	char sCfgFile[1024+256];
	char sBuffer[2048];
	char sLogFilename[300];



	// テンプレート ファイルを開く
	sprintf(sTemplateFile, "%s%s", srInit->sAnalogDir, srInit->sAnalogCfgTemplate);
	fi = ::fopen(sTemplateFile, "rt");
	if(fi == NULL) return false;

	// analog.cfg ファイルを開く
	sprintf(sCfgFile, "%sanalog.cfg", srInit->sAnalogDir);
	fo = ::fopen(sCfgFile, "wt");
	if(fo == NULL)
	{
		fclose(fi);
		return false;
	}

	// 特定のキーワード（"$$LOGFILE$$", "$$OUTFILE$$"）の置換を行いながら、ファイルコピー
	for(;;)
	{
		if(fgets(sBuffer, 2046, fi) == NULL) break;
		if(!strcmpi(sBuffer, "$$LOGFILE$$\n"))
		{
			nYear %=100;
			sprintf(sLogFilename, srInit->sIISLogName, nYear, nMonth);		// "ex%02d%02d*.log" -> "ex0704*.log"
			sprintf(sBuffer, "LOGFILE %s%s\n", srInit->sIISLogDir, sLogFilename);
		}
		if(!strcmpi(sBuffer, "$$OUTFILE$$\n"))
		{
			sprintf(sBuffer, "OUTFILE %s%s\\log.html\n", srInit->sOutputDir, sOutputSubDir);
		}
		if(fputs(sBuffer, fo) == EOF) break;

	}

	// 全てのファイルを閉じる
	fclose(fo);
	fclose(fi);

	return true;
}

