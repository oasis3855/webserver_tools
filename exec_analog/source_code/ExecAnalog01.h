// ExecAnalog01.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��
#include "GlobalFunc.h"


// CExecAnalog01App:
// ���̃N���X�̎����ɂ��ẮAExecAnalog01.cpp ���Q�Ƃ��Ă��������B
//

class CExecAnalog01App : public CWinApp
{
public:
	CExecAnalog01App();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	struct __sr_init_data srInit;
};

extern CExecAnalog01App theApp;

