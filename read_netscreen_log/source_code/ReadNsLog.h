// ReadNsLog.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��
#include "GlobalFunc.h"


// CReadNsLogApp:
// ���̃N���X�̎����ɂ��ẮAReadNsLog.cpp ���Q�Ƃ��Ă��������B
//

class CReadNsLogApp : public CWinApp
{
public:
	CReadNsLogApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	void RunAutomatic(void);
	struct __sr_init_data srInit;
};

extern CReadNsLogApp theApp;