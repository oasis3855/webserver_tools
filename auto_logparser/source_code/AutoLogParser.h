// AutoLogParser.h : �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��
#include "GlobalFunc.h"


// CAutoLogParserApp:
// ���̃N���X�̎����ɂ��ẮAAutoLogParser.cpp ���Q�Ƃ��Ă��������B
//

class CAutoLogParserApp : public CWinApp
{
public:
	CAutoLogParserApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	bool RunAutomatick(void);
	struct __sr_init_data srInit;
};

extern CAutoLogParserApp theApp;
