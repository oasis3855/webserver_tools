// NsLog.cpp : クラス実装ファイル
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

#include "StdAfx.h"
#include "ReadNsLog.h"
#include ".\nslog.h"
#include "GlobalFunc.h"

CNsLog::CNsLog(void)
{
}

CNsLog::~CNsLog(void)
{
}

// ***********************
// レポート作成メイン関数
// ***********************
bool CNsLog::MakeReport(int nYear, int nMonth, int nDay, int nSpan, struct __sr_init_data *srInit)
{
	char sFname[1024+256];
	FILE *fi, *fo;
	char sReadBuf[2048], sOutput[2048];
	time_t tmStartSec, tmProcSec;
	struct tm tmStart, *tmProc;
	int i;
	long nLines, nLinesReport;

	// 出力ファイルを開く
	sprintf(sFname, "%s%s", srInit->sOutputDir, srInit->sOutputFileName);
	fo = ::fopen(sFname, "wt");
	if(fo == NULL)
	{
		return false;
	}

	// 出力ファイルにヘッダ行を出力する
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);

	fprintf(fo, "NetScreen Log Query : %d days data before %d/%02d/%02d  ------ this report is created on %d/%02d/%02d %02d:%02d\n\n",
		nSpan, nYear, nMonth, nDay, tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday, tmStart.tm_hour, tmStart.tm_min);

	// Syslogファイル作成用、現在日時
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear-1900;
	tmStart.tm_mon = nMonth-1;
	tmStart.tm_mday = nDay;
	tmStartSec = ::mktime(&tmStart);


	// 出力ファイル用：総処理行数を計算する変数を初期化
	nLines = 0;
	nLinesReport = 0;

	// 指定された日数のSyslogファイルを次々と開き、条件抽出しログファイルに書き込むループ
	for(i=nSpan-1; i>=0; i--)
	{

		// Syslogのファイル名を作成する
		tmProcSec = tmStartSec - i*24*60*60;
		tmProc = ::localtime(&tmProcSec);
		sprintf(sReadBuf, "%s%s", srInit->sSyslogDir, srInit->sSyslogFileName);		// Syslogファイルのパターン
		sprintf(sFname, sReadBuf, tmProc->tm_year+1900, tmProc->tm_mon+1, tmProc->tm_mday);

		// Syslogファイルを開く
		fi = ::fopen(sFname, "rt");
		if(fi == NULL)
		{
			continue;
		}

		for(;;)
		{
			// Syslogファイルから1行読み込む
			fgets(sReadBuf, 2047, fi);
			nLines++;
			if(feof(fi)) break;

			strcpy(sOutput, sReadBuf);		// 検索関数で文字列破壊に備えてコピー

			if(IsMatchString(sReadBuf))
			{
				strcpy(sReadBuf, sOutput);	// 検索関数で破壊された文字列を復活
				if(!IsExcludeString(sReadBuf))
				{
					// ログファイルに書き込む
					fputs(sOutput, fo);
					nLinesReport++;
				}
			}
		}

		// Syslogファイルを閉じる
		if(fi != NULL) fclose(fi);
	}

	fprintf(fo, "\n\nprocessed %ld lines\nreported %ld lines\n", nLines, nLinesReport);


	if(fo != NULL) fclose(fo);

	return true;
}

// ***********************
// レポート作成メイン関数（1ヶ月版）
// ***********************
bool CNsLog::MakeReportMonth(int nYear, int nMonth, struct __sr_init_data *srInit)
{
	time_t tmStartSec;
	struct tm tmStart;
	int i;

	// 関数のパラメータとして渡された年・月をtime_tに変換する（日以下は現在日時を利用）
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear - 1900;
	tmStart.tm_mon = nMonth-1;
	tmStartSec = ::mktime(&tmStart);

	// 31日（があるとして）まで日付を進め、さらにプラス1日する
	tmStartSec += (31-tmStart.tm_mday+1)*24*60*60;

	// 7日戻りながら、月が同じになるまで検索（同一月の最大日を求める）
	for(i=1; i<7; i++)
	{
		tmStart = *::localtime(&tmStartSec);
		if((nMonth - 1) == tmStart.tm_mon) break;
		tmStartSec -= 24*60*60;
	}



	return MakeReport(nYear, nMonth, tmStart.tm_mday, tmStart.tm_mday, srInit);

}

// ***********************
// 一致条件文字列をログ行（1行）に含まれるかチェックする関数
// ※ 渡された文字列は破壊される
// ***********************
bool CNsLog::IsMatchString(char * sBuf)
{
	int i;

	for(i=0; i<6; i++)
	{
		if(strlen(sMatchStrings[i])<=0) break;
		if(strstr(sBuf, sMatchStrings[i]) != NULL) return true;
	}

	return false;
}

// ***********************
// 除外条件文字列をログ行（1行）に含まれるかチェックする関数
// ※ 渡された文字列は破壊される
// ***********************
bool CNsLog::IsExcludeString(char * sBuf)
{
	int i;

	for(i=0; i<2; i++)
	{
		if(strlen(sExcludeStrings[i])<=0) break;
		if(strstr(sBuf, sExcludeStrings[i]) != NULL) return true;
	}


	return false;
}
