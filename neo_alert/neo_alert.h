
// neo_alert.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// Cneo_alertApp:
// �� Ŭ������ ������ ���ؼ��� neo_alert.cpp�� �����Ͻʽÿ�.
//

class Cneo_alertApp : public CWinApp
{
public:
	Cneo_alertApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern Cneo_alertApp theApp;