// Line.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CLineApp:
// �� Ŭ������ ������ ���ؼ��� Line.cpp�� �����Ͻʽÿ�.
//

class CLineApp : public CWinApp
{
public:
	CLineApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CLineApp theApp;