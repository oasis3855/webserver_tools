// GlobalFunc.cpp : 実装ファイル　（INIファイル読み書き、バッチファイル作成、SQL命令ファイル作成、LogParser実行）
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
#include "GlobalFunc.h"
#include <process.h>
#include <stdlib.h>
#include <io.h>

// ***********************
// コンフィギュレーションファイルの作成
// LogParser実行用のバッチファイル、LogParser用のSQL命令ファイルを作成する
// ***********************
bool MakeConfigFile(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit)
{
	FILE *fo;
	CString sTemp;
	time_t tmStartSec;
	struct tm tmStart;

	// 無効な年月日が渡された場合は終了
	if(nYear < 2007 || nYear > 2020 || nMonth < 1 || nMonth > 12 || nDay < 1 || nDay > 31 || nSpan < 1 || nSpan > 31)
		return false;

	// この関数に渡された年月日を time_t に格納する
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear-1900;
	tmStart.tm_mon = nMonth-1;
	tmStart.tm_mday = nDay;
	tmStartSec = ::mktime(&tmStart);


	/******** バッチファイル（logauto.cmd）の作成 ********/

	sTemp.Format("%slogauto.cmd", srInit->sIISLogDir);

	fo = ::fopen(sTemp, "wt");
	if(fo == NULL)
	{
		return false;
	}

	fprintf(fo, "set path=%%path%%;%s\n", srInit->sLogParserDir);
	fprintf(fo, "%c:\ncd %s\n", srInit->sIISLogDir[0], srInit->sIISLogDir);
	fprintf(fo, "%s file:sql_err.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_method.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_status.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);
	fprintf(fo, "%s file:sql_method2.sql -iw:on -o:NAT\n", srInit->sLogParserProgName);


	fprintf(fo, "%c:\ncd %s\n", srInit->sOutputDir[0], srInit->sOutputDir);
	fprintf(fo, "echo IIS Log summary on %4d/%02d/%02d %02d:%02d > iislog.txt\n",
		tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday, tmStart.tm_hour, tmStart.tm_min);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_status.txt >> iislog.txt\n", fo);
	fputs("type iislog_method2.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo Error Lines ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_err.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo Method (not GET, not POST) Lines ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("type iislog_method.txt >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo ... >> iislog.txt\n", fo);
	fputs("echo EOF >> iislog.txt\n", fo);

	fputs("del iislog_status.txt\n", fo);
	fputs("del iislog_method2.txt\n", fo);
	fputs("del iislog_err.txt\n", fo);
	fputs("del iislog_method.txt\n", fo);

	fclose(fo);


	/******** SQL命令ファイルの作成 : error log ********/

	MakeConfigFile_MakeSQL("sql_err.sql", "iislog_err.txt", "select\n\t*\n\n", 
		"\nwhere\n\tsc-status=500 or\n\tsc-status=501 or\n\tsc-status=502 or\n\tsc-status=403 or\n\tsc-status=404\n\n",
		nSpan, tmStartSec, srInit);


	/******** SQL命令ファイルの作成 : method log ********/

	MakeConfigFile_MakeSQL("sql_method.sql", "iislog_method.txt", "select\n\t*\n\n", 
		"\nwhere\n\tcs-method<>'GET' and\n\tcs-method<>'POST'\n\n",
		nSpan, tmStartSec, srInit);


	/******** SQL命令ファイルの作成 : status count up ********/

	MakeConfigFile_MakeSQL("sql_status.sql", "iislog_status.txt", "select\n\tsc-status, count (*)\n\n", 
		"\ngroup by sc-status\n\n", nSpan, tmStartSec, srInit);


	/******** SQL命令ファイルの作成 : method count up ********/

	MakeConfigFile_MakeSQL("sql_method2.sql", "iislog_method2.txt", "select\n\tcs-method, count (*)\n\n", 
		"\ngroup by cs-method\n\n", nSpan, tmStartSec, srInit);

	return true;
}

// ***********************
// SQL命令ファイルの作成
// ***********************
bool MakeConfigFile_MakeSQL(char *sOutputSqlName, char *sIntoName, char *sSelect, char *sWhereGroupBy, int nSpan, time_t tmStartSec, struct __sr_init_data *srInit)
{
	FILE *fo;
	CString sTemp;
	time_t tmProcSec;
	struct tm tmStart;
	int i;
	char sLogFname[256];
	char sTempFname[1024+256];

	// SQL命令ファイルを開く
	sprintf(sTempFname, "%s%s", srInit->sIISLogDir, sOutputSqlName);

	fo = ::fopen(sTempFname, "wt");
	if(fo == NULL)
	{
		return false;
	}

	// SELECT 文
	fputs(sSelect, fo);

	// INTO 文
	fprintf(fo, "into\n\t%s%s\n\n", srInit->sOutputDir, sIntoName);

	// FROM 文
	fputs("from\n\t", fo);
	for(i=nSpan-1; i>=0; i--)
	{
		tmProcSec = tmStartSec - i * 24 * 60 * 60;
		tmStart = *::localtime(&tmProcSec);
		sprintf(sLogFname, srInit->sIISLogName, tmStart.tm_year+1900-2000, tmStart.tm_mon+1, tmStart.tm_mday);
		sprintf(sTempFname, "%s%s", srInit->sIISLogDir, sLogFname);
		if(::_access(sTempFname, 0) == -1) continue;	// ログファイルが存在しない場合はスキップ
		fprintf(fo, "\n\t%s,", sLogFname);
	}
	fseek(fo, -1, SEEK_CUR);	// 最後のファイル名の後ろのコンマを取る
	fputs("\n", fo);

	// WHERE 文／GROUP BY 文
	fputs(sWhereGroupBy, fo);

	fclose(fo);

	return true;
}

// ***********************
// LoParser実行＆出力ファイルコピー用バッチファイルの実行
// ***********************
void RunLogParser(struct __sr_init_data *srInit)
{
	CString sTemp;

	sTemp.Format("%slogauto.cmd", srInit->sIISLogDir);

	::_spawnl(_P_WAIT, (LPCSTR)sTemp, (LPCSTR)sTemp, NULL);

}

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
	GetPrivateProfileString("AutoLogParser", "install", "err", sTemp, 1023, sModulePath);
	if(!strcmp(sTemp, "err"))
	{
		WriteInitIniFile();		// INI が確認できない場合、新規作成
		return 1;
	}


	// 設定値読み込み

	GetPrivateProfileString("AutoLogParser", "OutputDir", "err", srInit->sOutputDir, 1023, sModulePath);
	if(!strcmp(srInit->sOutputDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogParserDir", "err", srInit->sLogParserDir, 1023, sModulePath);
	if(!strcmp(srInit->sLogParserDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogParserProgName", "err", srInit->sLogParserProgName, 254, sModulePath);
	if(!strcmp(srInit->sLogParserProgName, "err")) return 1;	// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "IISLogDir", "err", srInit->sIISLogDir, 1023, sModulePath);
	if(!strcmp(srInit->sIISLogDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "IISLogName", "err", srInit->sIISLogName, 254, sModulePath);
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

	if(!WritePrivateProfileString("AutoLogParser", "install", "installed (do not delete this line)", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "OutputDir", "d:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserDir", "c:\\Program Files\\Log Parser 2.2\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserProgName", "logparser.exe", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "IISLogDir", "d:\\Inetpub\\Log\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "IISLogName", "ex%02d%02d%02d.log", sModulePath))
		return ;


	return ;
}

