// ReadEvent01.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��
#include "globalfunc.h"


// CReadEvent01App:
// ���̃N���X�̎����ɂ��ẮAReadEvent01.cpp ���Q�Ƃ��Ă��������B
//

class CReadEvent01App : public CWinApp
{
public:
	CReadEvent01App();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	void RunAutomatic(void);
	struct __sr_init_data srInit;
};

extern CReadEvent01App theApp;
