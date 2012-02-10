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
#include <string.h>
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

	sTemp.Format("%slogauto.cmd", srInit->sLogDir);

	fo = ::fopen(sTemp, "wt");
	if(fo == NULL)
	{
		return false;
	}

	fprintf(fo, "set path=%%path%%;%s\n"
				"%c:\n"
				"cd %s\n", srInit->sLogParserDir,
				srInit->sLogDir[0], srInit->sLogDir);

	fprintf(fo, "%s file:sql_err.sql -iw:on -i:%s -o:%s\n", srInit->sLogParserProgName, srInit->sLogType, srInit->sOutputType);
	fprintf(fo, "%s file:sql_method.sql -iw:on -i:%s -o:%s\n", srInit->sLogParserProgName, srInit->sLogType, srInit->sOutputType);
	fprintf(fo, "%s file:sql_status.sql -iw:on -i:%s -o:%s\n", srInit->sLogParserProgName, srInit->sLogType, srInit->sOutputType);
	if(stricmp(srInit->sLogType, "NCSA"))
	{	// NCSA(Apache)ではこの集計未対応
		fprintf(fo, "%s file:sql_method2.sql -iw:on -i:%s -o:%s\n", srInit->sLogParserProgName, srInit->sLogType, srInit->sOutputType);
	}


	fprintf(fo, "%c:\ncd %s\n"
				"echo Web Server Log summary on %4d/%02d/%02d %02d:%02d > %s\n",
				srInit->sOutputDir[0], srInit->sOutputDir,
				tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday, tmStart.tm_hour, tmStart.tm_min, srInit->sOutputFilename);
	fprintf(fo, "echo ... >> %s\n"
				"type iislog_status.txt >> %s\n", srInit->sOutputFilename, srInit->sOutputFilename);
	if(stricmp(srInit->sLogType, "NCSA"))
	{	// NCSA(Apache)ではこの集計未対応
		fprintf(fo, "type iislog_method2.txt >> %s\n", srInit->sOutputFilename);
	}
	fprintf(fo, "echo ... >> %s\n"
				"echo ... >> %s\n"
				"echo Error Lines ... >> %s\n"
				"echo ... >> %s\n"
				"type iislog_err.txt >> %s\n"
				"echo ... >> %s\n"
				"echo ... >> %s\n"
				"echo Method (not GET, not POST) Lines ... >> %s\n"
				"echo ... >> %s\n"
				"type iislog_method.txt >> %s\n"
				"echo ... >> %s\n"
				"echo ... >> %s\n"
				"echo EOF >> %s\n",
				srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename,
				srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename,
				srInit->sOutputFilename, srInit->sOutputFilename, srInit->sOutputFilename);


	fputs("del iislog_status.txt\n"
		"del iislog_method2.txt\n"
		"del iislog_err.txt\n"
		"del iislog_method.txt\n", fo);

	fclose(fo);


	if(!stricmp(srInit->sLogType, "IIS"))
	{
		/******** SQL命令ファイルの作成 : error log ********/

		MakeConfigFile_MakeSQL("sql_err.sql", "iislog_err.txt", "select\n\t*\n\n", 
			"\nwhere\n\tStatusCode>400\n\n",
			nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : method log ********/

		MakeConfigFile_MakeSQL("sql_method.sql", "iislog_method.txt", "select\n\t*\n\n", 
			"\nwhere\n\tRequestType<>'GET' and\n\tRequestType<>'POST'\n\n",
			nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : status count up ********/

		MakeConfigFile_MakeSQL("sql_status.sql", "iislog_status.txt", "select\n\tStatusCode, count (*)\n\n", 
			"\ngroup by StatusCode\n\n", nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : method count up ********/

		MakeConfigFile_MakeSQL("sql_method2.sql", "iislog_method2.txt", "select\n\tRequestType, count (*)\n\n", 
			"\ngroup by RequestType\n\n", nSpan, tmStartSec, srInit);

	}
	else if(!stricmp(srInit->sLogType, "IISW3C"))
	{
		/******** SQL命令ファイルの作成 : error log ********/

		MakeConfigFile_MakeSQL("sql_err.sql", "iislog_err.txt", "select\n\t*\n\n", 
			"\nwhere\n\tsc-status>400\n\n",
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

	}
	else if(!stricmp(srInit->sLogType, "NCSA"))
	{
		/******** SQL命令ファイルの作成 : error log ********/

		MakeConfigFile_MakeSQL("sql_err.sql", "iislog_err.txt", "select\n\t*\n\n", 
			"\nwhere\n\tStatusCode>400\n\n",
			nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : method log ********/

		MakeConfigFile_MakeSQL("sql_method.sql", "iislog_method.txt", "select\n\t*\n\n", 
			"\nwhere\n\tRequest not like 'GET %' and\n\tRequest not like 'POST %'\n\n",
			nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : status count up ********/

		MakeConfigFile_MakeSQL("sql_status.sql", "iislog_status.txt", "select\n\tStatusCode, count (*)\n\n", 
			"\ngroup by StatusCode\n\n", nSpan, tmStartSec, srInit);


		/******** SQL命令ファイルの作成 : method count up ********/

		// NCSA形式では、この集計は不可能

	}

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
	BOOL bYear4Digit;
	char sLogFname[256];
	char sTempFname[1024+256];

	// ログファイルの年が4桁か、2桁かを判定 ( log%04d%02d%02d または log%02d%02d%02d の違いを検出）
	sTemp = srInit->sLogName;
	i=0;
	if(sTemp[0] != '%'){ sTemp.Tokenize("%d", i); }		// 文字列先頭が %dで始まらない場合、切り出し1個目は読み飛ばす
	if(atoi(sTemp.Tokenize("%d",i)) >= 3){ bYear4Digit = true; }	// %とdで囲まれた部分を読み出し桁数を判定する
	else{ bYear4Digit = false; }

	// SQL命令ファイルを開く
	sprintf(sTempFname, "%s%s", srInit->sLogDir, sOutputSqlName);

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
		sprintf(sLogFname, srInit->sLogName, (bYear4Digit ? tmStart.tm_year+1900 : tmStart.tm_year+1900-2000), tmStart.tm_mon+1, tmStart.tm_mday);
		sprintf(sTempFname, "%s%s", srInit->sLogDir, sLogFname);
		if(::_access(sTempFname, 0) != 0) continue;	// ログファイルが存在しない場合はスキップ
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
bool RunLogParser(struct __sr_init_data *srInit)
{
	CString sTemp;

	sTemp.Format("%s%s", srInit->sLogParserDir, srInit->sLogParserProgName);
	if(::_access(sTemp, 0) != 0) return false;	// LogParserプログラムが存在しない

	sTemp.Format("%slogauto.cmd", srInit->sLogDir);

	if(::_spawnl(_P_WAIT, (LPCSTR)sTemp, (LPCSTR)sTemp, NULL) != 0)
	{
		return false;
	}
	return true;
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

	GetPrivateProfileString("AutoLogParser", "OutputFilename", "err", srInit->sOutputFilename, 254, sModulePath);
	if(!strcmp(srInit->sOutputFilename, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "OutputType", "err", srInit->sOutputType, 31, sModulePath);
	if(!strcmp(srInit->sOutputType, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogParserDir", "err", srInit->sLogParserDir, 1023, sModulePath);
	if(!strcmp(srInit->sLogParserDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogParserProgName", "err", srInit->sLogParserProgName, 254, sModulePath);
	if(!strcmp(srInit->sLogParserProgName, "err")) return 1;	// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogDir", "err", srInit->sLogDir, 1023, sModulePath);
	if(!strcmp(srInit->sLogDir, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogNameTemplate", "err", srInit->sLogName, 254, sModulePath);
	if(!strcmp(srInit->sLogName, "err")) return 1;		// 設定値読み込み不能

	GetPrivateProfileString("AutoLogParser", "LogType", "err", srInit->sLogType, 31, sModulePath);
	if(!strcmp(srInit->sLogType, "err")) return 1;		// 設定値読み込み不能

	// ログ形式 Apache を NCSA に読み替え
	if(!stricmp(srInit->sLogType, "Apache")){ strcpy(srInit->sLogType, "NCSA"); }

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
	if(!WritePrivateProfileString("AutoLogParser", "OutputDir", "c:\\Inetpub\\wwwroot\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "OutputFilename", "report.txt", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "OutputType", "NAT", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserDir", "c:\\Program Files\\Log Parser 2.2\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogParserProgName", "logparser.exe", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogDir", "c:\\WINDOWS\\System32\\LogFiles\\W3SVC1\\", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogNameTemplate", "ex%02d%02d%02d.log", sModulePath))
		return ;
	if(!WritePrivateProfileString("AutoLogParser", "LogType", "IISW3C", sModulePath))
		return ;

	if(!WritePrivateProfileString("Readme", ";OutputType selection", "NAT/CSV", sModulePath))
		return ;
	if(!WritePrivateProfileString("Readme", ";LogType selection", "IIS/IISW3C/NCSA (Apache=NCSA)", sModulePath))
		return ;


	return ;
}

