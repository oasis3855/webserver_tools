// ReadEvent.cpp : ヘッダファイル
//
#pragma once

#include "globalfunc.h"

class CReadEvent
{
public:
	CReadEvent(void);
	~CReadEvent(void);
	BOOL GetOneEvent(void);
	void CloseEventHandle(void);

	char sPcName[1024];
	char sEventSourceName[125];

	time_t tmGeneratedTime;
	struct tm tmNum;
	int nRecordNumber;
	unsigned int nEventID;
	WORD nEventType;
	char sEventType[32];
	char sSourceName[1024];
	char sComputerName[1024];
	HANDLE hEventLog;
	void WriteToLogFile(BOOL b7days, BOOL bErrorOnly, char * sPcName, __sr_init_data *srInit);
	void WriteBackupExecLog(BOOL b7days, BOOL bErrorOnly, char * _sPcName, __sr_init_data *srInit);
};


