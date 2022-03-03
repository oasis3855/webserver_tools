// NsLog.cpp : �N���X�����t�@�C��
//

// ***********************
// ReadNsLog  �iNwtScreen��Syslog����������o����v���O�����j
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
// ���|�[�g�쐬���C���֐�
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

	// �o�̓t�@�C�����J��
	sprintf(sFname, "%s%s", srInit->sOutputDir, srInit->sOutputFileName);
	fo = ::fopen(sFname, "wt");
	if(fo == NULL)
	{
		return false;
	}

	// �o�̓t�@�C���Ƀw�b�_�s���o�͂���
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);

	fprintf(fo, "NetScreen Log Query : %d days data before %d/%02d/%02d  ------ this report is created on %d/%02d/%02d %02d:%02d\n\n",
		nSpan, nYear, nMonth, nDay, tmStart.tm_year+1900, tmStart.tm_mon+1, tmStart.tm_mday, tmStart.tm_hour, tmStart.tm_min);

	// Syslog�t�@�C���쐬�p�A���ݓ���
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear-1900;
	tmStart.tm_mon = nMonth-1;
	tmStart.tm_mday = nDay;
	tmStartSec = ::mktime(&tmStart);


	// �o�̓t�@�C���p�F�������s�����v�Z����ϐ���������
	nLines = 0;
	nLinesReport = 0;

	// �w�肳�ꂽ������Syslog�t�@�C�������X�ƊJ���A�������o�����O�t�@�C���ɏ������ރ��[�v
	for(i=nSpan-1; i>=0; i--)
	{

		// Syslog�̃t�@�C�������쐬����
		tmProcSec = tmStartSec - i*24*60*60;
		tmProc = ::localtime(&tmProcSec);
		sprintf(sReadBuf, "%s%s", srInit->sSyslogDir, srInit->sSyslogFileName);		// Syslog�t�@�C���̃p�^�[��
		sprintf(sFname, sReadBuf, tmProc->tm_year+1900, tmProc->tm_mon+1, tmProc->tm_mday);

		// Syslog�t�@�C�����J��
		fi = ::fopen(sFname, "rt");
		if(fi == NULL)
		{
			continue;
		}

		for(;;)
		{
			// Syslog�t�@�C������1�s�ǂݍ���
			fgets(sReadBuf, 2047, fi);
			nLines++;
			if(feof(fi)) break;

			strcpy(sOutput, sReadBuf);		// �����֐��ŕ�����j��ɔ����ăR�s�[

			if(IsMatchString(sReadBuf))
			{
				strcpy(sReadBuf, sOutput);	// �����֐��Ŕj�󂳂ꂽ������𕜊�
				if(!IsExcludeString(sReadBuf))
				{
					// ���O�t�@�C���ɏ�������
					fputs(sOutput, fo);
					nLinesReport++;
				}
			}
		}

		// Syslog�t�@�C�������
		if(fi != NULL) fclose(fi);
	}

	fprintf(fo, "\n\nprocessed %ld lines\nreported %ld lines\n", nLines, nLinesReport);


	if(fo != NULL) fclose(fo);

	return true;
}

// ***********************
// ���|�[�g�쐬���C���֐��i1�����Łj
// ***********************
bool CNsLog::MakeReportMonth(int nYear, int nMonth, struct __sr_init_data *srInit)
{
	time_t tmStartSec;
	struct tm tmStart;
	int i;

	// �֐��̃p�����[�^�Ƃ��ēn���ꂽ�N�E����time_t�ɕϊ�����i���ȉ��͌��ݓ����𗘗p�j
	::time(&tmStartSec);
	tmStart = *::localtime(&tmStartSec);
	tmStart.tm_year = nYear - 1900;
	tmStart.tm_mon = nMonth-1;
	tmStartSec = ::mktime(&tmStart);

	// 31���i������Ƃ��āj�܂œ��t��i�߁A����Ƀv���X1������
	tmStartSec += (31-tmStart.tm_mday+1)*24*60*60;

	// 7���߂�Ȃ���A���������ɂȂ�܂Ō����i���ꌎ�̍ő�������߂�j
	for(i=1; i<7; i++)
	{
		tmStart = *::localtime(&tmStartSec);
		if((nMonth - 1) == tmStart.tm_mon) break;
		tmStartSec -= 24*60*60;
	}



	return MakeReport(nYear, nMonth, tmStart.tm_mday, tmStart.tm_mday, srInit);

}

// ***********************
// ��v��������������O�s�i1�s�j�Ɋ܂܂�邩�`�F�b�N����֐�
// �� �n���ꂽ������͔j�󂳂��
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
// ���O��������������O�s�i1�s�j�Ɋ܂܂�邩�`�F�b�N����֐�
// �� �n���ꂽ������͔j�󂳂��
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
