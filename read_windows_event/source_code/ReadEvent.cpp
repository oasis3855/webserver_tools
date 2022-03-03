// ReadEvent.cpp : クラス実装ファイル　（システムイベントの読み込み、抽出HTMLファイルの作成）
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

#include "StdAfx.h"
#include "ReadEvent01.h"
#include ".\readevent.h"
#include "globalfunc.h"

// ***********************
// クラスの初期化
// ***********************
CReadEvent::CReadEvent(void)
{
	hEventLog = NULL;		// イベントハンドルの初期化
	strcpy(sComputerName, "");
	strcpy(sSourceName, "");
	strcpy(sPcName, "");
	strcpy(sEventSourceName, "Application");
}

CReadEvent::~CReadEvent(void)
{
}

// ***********************
// システムイベントを読み込む
//
// イベントログ ハンドラはすでに開かれている条件で、次のイベントを読み込みクラス変数にセットする
// イベントログ ハンドラがまだ開かれていない場合は、ここで開く
// ※ イベントログ ハンドラは、呼び出し側で明示的に閉じること
// ***********************
BOOL CReadEvent::GetOneEvent(void)
{
	DWORD BufSize;
	DWORD ReadBytes;
	DWORD NextSize;
	BOOL bResult;
	struct tm *tmLocal;
	char *cp;
	char *pSourceName;
	char *pComputerName;
	EVENTLOGRECORD *pBuf = NULL;
	char **Args = NULL;
	CString strTemp, strMsg;

	// イベントログのオープン
	if(hEventLog == NULL)
	{
		if(!strcmp(sPcName, ""))
			hEventLog = OpenEventLog(NULL, sEventSourceName);
		else
			hEventLog = OpenEventLog(sPcName, sEventSourceName);
	}

	// イベントログが開けない場合は終了
	if(hEventLog == NULL) {
		return FALSE;
	}

	// 読み込むイベントログの大きさを得る
	BufSize = 1;
	pBuf = (EVENTLOGRECORD *)GlobalAlloc(GMEM_FIXED, BufSize);

	bResult = ReadEventLog(hEventLog,
		EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
		0,
		pBuf,
		BufSize,
		&ReadBytes,
		&NextSize);

	if(!bResult && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		if(pBuf != NULL) GlobalFree(pBuf);
		if(Args != NULL) GlobalFree(Args);
		if(hEventLog != NULL) CloseEventLog(hEventLog);

		return FALSE;
	}

	GlobalFree(pBuf);
	pBuf = NULL;

	// イベントログを読み込む
	BufSize = NextSize;
	pBuf = (EVENTLOGRECORD *)GlobalAlloc(GMEM_FIXED, BufSize);	// ログの大きさ分のバッファを確保

	bResult = ReadEventLog(
		hEventLog,
		EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
		0,
		pBuf,
		BufSize,
		&ReadBytes,
		&NextSize);

	if(!bResult)
	{
		if(pBuf != NULL) GlobalFree(pBuf);
		if(Args != NULL) GlobalFree(Args);
		if(hEventLog != NULL) CloseEventLog(hEventLog);

		return FALSE;
	}


	// 読み取ったイベントの変数への格納
	nRecordNumber = pBuf->RecordNumber;				// イベント番号
	tmGeneratedTime = pBuf->TimeGenerated;			// イベント発生時刻

	tmLocal = ::localtime(&tmGeneratedTime);
	tmNum.tm_year = tmLocal->tm_year;
	tmNum.tm_mon = tmLocal->tm_mon;
	tmNum.tm_yday = tmLocal->tm_yday;
	tmNum.tm_wday = tmLocal->tm_wday;
	tmNum.tm_mday = tmLocal->tm_mday;
	tmNum.tm_hour = tmLocal->tm_hour;
	tmNum.tm_min = tmLocal->tm_min;
	tmNum.tm_sec = tmLocal->tm_sec;
	tmNum.tm_isdst = tmLocal->tm_isdst;

	nEventID = pBuf->EventID & 0xffff;				// イベントID
	nEventType = pBuf->EventType;					// イベント種別

	// イベント種別のテキスト化
	switch(pBuf->EventType) {
		case EVENTLOG_SUCCESS: strcpy(sEventType, "Success"); break;
		case EVENTLOG_ERROR_TYPE: strcpy(sEventType, "Error"); break;
		case EVENTLOG_WARNING_TYPE: strcpy(sEventType, "Warning"); break;
		case EVENTLOG_INFORMATION_TYPE: strcpy(sEventType, "Information"); break;
		case EVENTLOG_AUDIT_SUCCESS: strcpy(sEventType, "SuccessAudit"); break;
		case EVENTLOG_AUDIT_FAILURE: strcpy(sEventType, "FailureAudit"); break;
		default: strcpy(sEventType, "UnknownEvent"); break;
	}


	cp = (char *)pBuf;
	cp += sizeof(EVENTLOGRECORD);

	pSourceName = cp;
	cp += strlen(cp)+1;

	pComputerName = cp;
	cp += strlen(cp)+1;

	strcpy(sSourceName, pSourceName);				// ソース名
	strcpy(sComputerName, pComputerName);			// コンピュータ名

	if(pBuf != NULL) GlobalFree(pBuf);
	if(Args != NULL) GlobalFree(Args);

	return TRUE;
}

// ***********************
// イベントログ ハンドラを閉じる
// ***********************
void CReadEvent::CloseEventHandle(void)
{
	if(hEventLog != NULL) CloseEventLog(hEventLog);
	hEventLog = NULL;

}

// ***********************
// イベントログ を読み込んで、HTMLに書き出す
// ***********************
void CReadEvent::WriteToLogFile(BOOL b7days, BOOL bErrorOnly, char * _sPcName, __sr_init_data *srInit)
{
	
	time_t tmNow;
	struct tm *tmLocaltime;
	CString sTemp;
	char strOutputFileName[1024];

	FILE *fo;


	tmNow = time(NULL);
	tmLocaltime = localtime(&tmNow);

	strcpy(sPcName, _sPcName);
	if(!strcmpi(sPcName, "localhost")) strcpy(sPcName, "");		// "localhost" は NULL 文字列にする

	// 出力HTMLファイル名 （先頭はevtで始まる）
	sprintf(strOutputFileName, "%sevt%s.html", srInit->sOutputDir, sPcName);

	// 出力HTMLファイルを開く
	fo = ::fopen(strOutputFileName, "wt");
	if(fo == NULL)
	{
		fclose(fo);
		return ;
	}

	// HTMLヘッダー、ファイルの概要を書き込む
	fprintf(fo, "<html>\n<body bgcolor=#d0d0d0>\n<p>Event Log &nbsp;&nbsp; - on &nbsp;&nbsp;%d/%02d/%02d %02d:%02d</p>\n",
		tmLocaltime->tm_year+1900, tmLocaltime->tm_mon+1, tmLocaltime->tm_mday, tmLocaltime->tm_hour, tmLocaltime->tm_min);

	fputs("<table cellspacing=0 cellpadding=0 border=0 width=90%>\n", fo);


	// アプリケーション イベント ログを読み込んで、HTML化
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>Application Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Application");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// 出力データ（1行）
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// エラー、警告の種類に応じてセルの着色
		if(nEventType != EVENTLOG_INFORMATION_TYPE && nEventType != EVENTLOG_AUDIT_SUCCESS)
		{
			if(nEventType == EVENTLOG_WARNING_TYPE) fputs("<tr bgcolor=#eff2b5>\n", fo);
			if(nEventType == EVENTLOG_ERROR_TYPE) fputs("<tr bgcolor=#f7c1c1>\n", fo);
			if(nEventType == EVENTLOG_AUDIT_FAILURE) fputs("<tr bgcolor=#c6f4c6>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
		else if(!bErrorOnly)
		{
			fputs("<tr>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
	}
	CloseEventHandle();


	// セキュリティ イベント ログを読み込んで、HTML化
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>Security Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Security");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// 出力データ（1行）
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// エラー、警告の種類に応じてセルの着色
		if(nEventType != EVENTLOG_INFORMATION_TYPE && nEventType != EVENTLOG_AUDIT_SUCCESS)
		{
			if(nEventType == EVENTLOG_WARNING_TYPE) fputs("<tr bgcolor=#eff2b5>\n", fo);
			if(nEventType == EVENTLOG_ERROR_TYPE) fputs("<tr bgcolor=#f7c1c1>\n", fo);
			if(nEventType == EVENTLOG_AUDIT_FAILURE) fputs("<tr bgcolor=#c6f4c6>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
		else if(!bErrorOnly)
		{
			fputs("<tr>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
	}
	CloseEventHandle();


	// システム イベント ログを読み込んで、HTML化
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>System Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "System");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// 出力データ（1行）
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// エラー、警告の種類に応じてセルの着色
		if(nEventType != EVENTLOG_INFORMATION_TYPE && nEventType != EVENTLOG_AUDIT_SUCCESS)
		{
			if(nEventType == EVENTLOG_WARNING_TYPE) fputs("<tr bgcolor=#eff2b5>\n", fo);
			if(nEventType == EVENTLOG_ERROR_TYPE) fputs("<tr bgcolor=#f7c1c1>\n", fo);
			if(nEventType == EVENTLOG_AUDIT_FAILURE) fputs("<tr bgcolor=#c6f4c6>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
		else if(!bErrorOnly)
		{
			fputs("<tr>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
	}
	CloseEventHandle();


	// HTMLの終末を書き込む
	fputs("</table>\n", fo);
	fputs("</body>\n</html>\n", fo);

	// 出力HTMLファイルを閉じる
	if(fo != NULL) ::fclose(fo);


}

// ***********************
// イベントログ（バックアップ プログラムのみ） を読み込んで、HTMLに書き出す
// （アプリケーション ログのみしか対象としない）
// ***********************
void CReadEvent::WriteBackupExecLog(BOOL b7days, BOOL bErrorOnly, char * _sPcName, __sr_init_data *srInit)
{
	
	time_t tmNow;
	struct tm *tmLocaltime;
	CString sTemp;
	char strOutputFileName[1024];

	FILE *fo;


	tmNow = time(NULL);
	tmLocaltime = localtime(&tmNow);

	strcpy(sPcName, _sPcName);
	if(!strcmpi(sPcName, "localhost")) strcpy(sPcName, "");		// "localhost" は NULL 文字列にする

	// 出力HTMLファイル名 （先頭はevtで始まる）
	sprintf(strOutputFileName, "%sevt%s%s.html", srInit->sOutputDir, sPcName, srInit->sBackupExecName);

	// 出力HTMLファイルを開く
	fo = ::fopen(strOutputFileName, "wt");
	if(fo == NULL)
	{
		fclose(fo);
		return ;
	}

	// HTMLヘッダー、ファイルの概要を書き込む
	fprintf(fo, "<html>\n<body bgcolor=#d0d0d0>\n<p>Event Log (%s)&nbsp;&nbsp; - on &nbsp;&nbsp;%d/%02d/%02d %02d:%02d</p>\n",
		srInit->sBackupExecName, tmLocaltime->tm_year+1900, tmLocaltime->tm_mon+1, tmLocaltime->tm_mday, tmLocaltime->tm_hour, tmLocaltime->tm_min);


	fputs("<table cellspacing=0 cellpadding=0 border=0 width=90%>\n", fo);


	// アプリケーション イベント ログを読み込んで、HTML化
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>BackupExec Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Application");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		if(strcmp(sSourceName, "Backup Exec")) continue;

		// 出力データ（1行）
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// エラー、警告の種類に応じてセルの着色
		if(nEventType != EVENTLOG_INFORMATION_TYPE && nEventType != EVENTLOG_AUDIT_SUCCESS)
		{
			if(nEventType == EVENTLOG_WARNING_TYPE) fputs("<tr bgcolor=#eff2b5>\n", fo);
			if(nEventType == EVENTLOG_ERROR_TYPE) fputs("<tr bgcolor=#f7c1c1>\n", fo);
			if(nEventType == EVENTLOG_AUDIT_FAILURE) fputs("<tr bgcolor=#c6f4c6>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
		else if(!bErrorOnly)
		{
			fputs("<tr>\n", fo);
			if(::fputs(sTemp, fo) == EOF) break;
			fputs("</tr>\n", fo);
		}
	}
	CloseEventHandle();


	// HTMLの終末を書き込む
	fputs("</table>\n", fo);
	fputs("</body>\n</html>\n", fo);


	// 出力HTMLファイルを閉じる
	if(fo != NULL) ::fclose(fo);


}

