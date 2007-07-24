// ReadEvent.cpp : �N���X�����t�@�C���@�i�V�X�e���C�x���g�̓ǂݍ��݁A���oHTML�t�@�C���̍쐬�j
//

// ***********************
// ReadEvent  (Windows�̃V�X�e�� ���O�𒊏o���AHTML������v���O�����j
// 
// Copyright (C) 2007 INOUE. Hirokazu
//
// 
// http://www.opensource.jp/gpl/gpl.ja.html
// ���̃v���O�����̓t���[�\�t�g�E�F�A�ł��B���Ȃ��͂�����A�t���[�\�t�g�E�F
// �A���c�ɂ���Ĕ��s���ꂽ GNU ��ʌ��O���p�����_��(�o�[�W����2���A��
// �]�ɂ���Ă͂���ȍ~�̃o�[�W�����̂����ǂꂩ)�̒�߂�����̉��ōĔЕz
// �܂��͉��ς��邱�Ƃ��ł��܂��B
// 
// ���̃v���O�����͗L�p�ł��邱�Ƃ�����ĔЕz����܂����A*�S���̖��ۏ�* 
// �ł��B���Ɖ\���̕ۏ؂����̖ړI�ւ̓K�����́A���O�Ɏ����ꂽ���̂���
// �ߑS�����݂��܂���B�ڂ�����GNU ��ʌ��O���p�����_�񏑂��������������B
// 
// ���Ȃ��͂��̃v���O�����Ƌ��ɁAGNU ��ʌ��O���p�����_�񏑂̕��������ꕔ
// �󂯎�����͂��ł��B�����󂯎���Ă��Ȃ���΁A�t���[�\�t�g�E�F�A���c��
// �Ő������Ă�������(����� the Free Software Foundation, Inc., 59
// Temple Place, Suite 330, Boston, MA 02111-1307 USA)�B

#include "StdAfx.h"
#include "ReadEvent01.h"
#include ".\readevent.h"
#include "globalfunc.h"

// ***********************
// �N���X�̏�����
// ***********************
CReadEvent::CReadEvent(void)
{
	hEventLog = NULL;		// �C�x���g�n���h���̏�����
	strcpy(sComputerName, "");
	strcpy(sSourceName, "");
	strcpy(sPcName, "");
	strcpy(sEventSourceName, "Application");
}

CReadEvent::~CReadEvent(void)
{
}

// ***********************
// �V�X�e���C�x���g��ǂݍ���
//
// �C�x���g���O �n���h���͂��łɊJ����Ă�������ŁA���̃C�x���g��ǂݍ��݃N���X�ϐ��ɃZ�b�g����
// �C�x���g���O �n���h�����܂��J����Ă��Ȃ��ꍇ�́A�����ŊJ��
// �� �C�x���g���O �n���h���́A�Ăяo�����Ŗ����I�ɕ��邱��
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

	// �C�x���g���O�̃I�[�v��
	if(hEventLog == NULL)
	{
		if(!strcmp(sPcName, ""))
			hEventLog = OpenEventLog(NULL, sEventSourceName);
		else
			hEventLog = OpenEventLog(sPcName, sEventSourceName);
	}

	// �C�x���g���O���J���Ȃ��ꍇ�͏I��
	if(hEventLog == NULL) {
		return FALSE;
	}

	// �ǂݍ��ރC�x���g���O�̑傫���𓾂�
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

	// �C�x���g���O��ǂݍ���
	BufSize = NextSize;
	pBuf = (EVENTLOGRECORD *)GlobalAlloc(GMEM_FIXED, BufSize);	// ���O�̑傫�����̃o�b�t�@���m��

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


	// �ǂݎ�����C�x���g�̕ϐ��ւ̊i�[
	nRecordNumber = pBuf->RecordNumber;				// �C�x���g�ԍ�
	tmGeneratedTime = pBuf->TimeGenerated;			// �C�x���g��������

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

	nEventID = pBuf->EventID & 0xffff;				// �C�x���gID
	nEventType = pBuf->EventType;					// �C�x���g���

	// �C�x���g��ʂ̃e�L�X�g��
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

	strcpy(sSourceName, pSourceName);				// �\�[�X��
	strcpy(sComputerName, pComputerName);			// �R���s���[�^��

	if(pBuf != NULL) GlobalFree(pBuf);
	if(Args != NULL) GlobalFree(Args);

	return TRUE;
}

// ***********************
// �C�x���g���O �n���h�������
// ***********************
void CReadEvent::CloseEventHandle(void)
{
	if(hEventLog != NULL) CloseEventLog(hEventLog);
	hEventLog = NULL;

}

// ***********************
// �C�x���g���O ��ǂݍ���ŁAHTML�ɏ����o��
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
	if(!strcmpi(sPcName, "localhost")) strcpy(sPcName, "");		// "localhost" �� NULL ������ɂ���

	// �o��HTML�t�@�C���� �i�擪��evt�Ŏn�܂�j
	sprintf(strOutputFileName, "%sevt%s.html", srInit->sOutputDir, sPcName);

	// �o��HTML�t�@�C�����J��
	fo = ::fopen(strOutputFileName, "wt");
	if(fo == NULL)
	{
		fclose(fo);
		return ;
	}

	// HTML�w�b�_�[�A�t�@�C���̊T�v����������
	fprintf(fo, "<html>\n<body bgcolor=#d0d0d0>\n<p>Event Log &nbsp;&nbsp; - on &nbsp;&nbsp;%d/%02d/%02d %02d:%02d</p>\n",
		tmLocaltime->tm_year+1900, tmLocaltime->tm_mon+1, tmLocaltime->tm_mday, tmLocaltime->tm_hour, tmLocaltime->tm_min);

	fputs("<table cellspacing=0 cellpadding=0 border=0 width=90%>\n", fo);


	// �A�v���P�[�V���� �C�x���g ���O��ǂݍ���ŁAHTML��
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>Application Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Application");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// �o�̓f�[�^�i1�s�j
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// �G���[�A�x���̎�ނɉ����ăZ���̒��F
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


	// �Z�L�����e�B �C�x���g ���O��ǂݍ���ŁAHTML��
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>Security Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Security");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// �o�̓f�[�^�i1�s�j
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// �G���[�A�x���̎�ނɉ����ăZ���̒��F
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


	// �V�X�e�� �C�x���g ���O��ǂݍ���ŁAHTML��
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>System Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "System");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		// �o�̓f�[�^�i1�s�j
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// �G���[�A�x���̎�ނɉ����ăZ���̒��F
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


	// HTML�̏I������������
	fputs("</table>\n", fo);
	fputs("</body>\n</html>\n", fo);

	// �o��HTML�t�@�C�������
	if(fo != NULL) ::fclose(fo);


}

// ***********************
// �C�x���g���O�i�o�b�N�A�b�v �v���O�����̂݁j ��ǂݍ���ŁAHTML�ɏ����o��
// �i�A�v���P�[�V���� ���O�݂̂����ΏۂƂ��Ȃ��j
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
	if(!strcmpi(sPcName, "localhost")) strcpy(sPcName, "");		// "localhost" �� NULL ������ɂ���

	// �o��HTML�t�@�C���� �i�擪��evt�Ŏn�܂�j
	sprintf(strOutputFileName, "%sevt%s%s.html", srInit->sOutputDir, sPcName, srInit->sBackupExecName);

	// �o��HTML�t�@�C�����J��
	fo = ::fopen(strOutputFileName, "wt");
	if(fo == NULL)
	{
		fclose(fo);
		return ;
	}

	// HTML�w�b�_�[�A�t�@�C���̊T�v����������
	fprintf(fo, "<html>\n<body bgcolor=#d0d0d0>\n<p>Event Log (%s)&nbsp;&nbsp; - on &nbsp;&nbsp;%d/%02d/%02d %02d:%02d</p>\n",
		srInit->sBackupExecName, tmLocaltime->tm_year+1900, tmLocaltime->tm_mon+1, tmLocaltime->tm_mday, tmLocaltime->tm_hour, tmLocaltime->tm_min);


	fputs("<table cellspacing=0 cellpadding=0 border=0 width=90%>\n", fo);


	// �A�v���P�[�V���� �C�x���g ���O��ǂݍ���ŁAHTML��
	fputs("<tr bgcolor=#a0a0ff>\n<td colspan=6>BackupExec Event</td></tr>\n", fo);
	strcpy(sEventSourceName, "Application");
	for(;;)
	{
		if(!GetOneEvent()) break;

		if(b7days && tmNow - tmGeneratedTime > 7*24*60*60) break;

		if(strcmp(sSourceName, "Backup Exec")) continue;

		// �o�̓f�[�^�i1�s�j
		sTemp.Format("<td>%d</td><td>%04d/%02d/%02d %02d:%02d:%02d</td><td>%u</td><td>%s</td><td>%s</td><td>%s</td>\n", nRecordNumber, tmNum.tm_year+1900, tmNum.tm_mon+1,
			tmNum.tm_mday, tmNum.tm_hour, tmNum.tm_min, tmNum.tm_sec,
			nEventID, sEventType, sSourceName, sComputerName);

		// �G���[�A�x���̎�ނɉ����ăZ���̒��F
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


	// HTML�̏I������������
	fputs("</table>\n", fo);
	fputs("</body>\n</html>\n", fo);


	// �o��HTML�t�@�C�������
	if(fo != NULL) ::fclose(fo);


}

