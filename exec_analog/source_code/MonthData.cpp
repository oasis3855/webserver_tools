// MonthData.cpp : �N���X�����t�@�C���@�i�����A�挎�̌v�Z������N���X�j
//

// ***********************
// ExecAnalog  �i��������ѐ挎��IIS�f�[�^��Analog�ŉ�͂��鎩�����v���O�����j
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
#include ".\monthdata.h"

CMonthData::CMonthData(void)
{
	// ���ݎ��Ԃ� time_t�\���̂Ɋi�[
	::time(&tmNowUx);
}

CMonthData::~CMonthData(void)
{
}

int CMonthData::GetMonthNow(void)
{
	struct tm *tmNow;

	// ���ݎ��Ԃ� t �\���̂ɑ��
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... �̕␳�����āA�����̐���Ԃ�
	return tmNow->tm_mon + 1;
}

int CMonthData::GetYearNow(void)
{
	struct tm *tmNow;

	// ���ݎ��Ԃ� t �\���̂ɑ��
	tmNow = ::localtime(&tmNowUx);

	// 1900�N��_�̕␳�����āA���N�̐���Ԃ�
	return 1900 + tmNow->tm_year;
}

int CMonthData::GetMonthPrev(void)
{
	struct tm *tmNow;
	int nCurMonth;

	// ���ݎ��Ԃ� t �\���̂ɑ��
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... �̕␳�����āA�����̐�����
	nCurMonth = tmNow->tm_mon + 1;

	if(nCurMonth > 1) return nCurMonth - 1;
	else return 12;
}

int CMonthData::GetYearPrev(void)
{
	struct tm *tmNow;
	int nCurMonth;

	// ���ݎ��Ԃ� t �\���̂ɑ��
	tmNow = ::localtime(&tmNowUx);

	// 0,1,2... �̕␳�����āA�����̐�����
	nCurMonth = tmNow->tm_mon + 1;

	if(nCurMonth > 1) return 1900 + tmNow->tm_year;
	else return 1900 + tmNow->tm_year - 1;
}
