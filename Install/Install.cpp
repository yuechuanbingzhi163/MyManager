// Install.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Install.h"
#include "MainFrameWork.h"

void Exit_Function();


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	bool bRet = CLog::Init();
	if (bRet)
	{
		LOG(_T("��־ģ���ʼ���ɹ�!"));
	}
	else
	{
		return 0;
	}

	CPaintManagerUI::SetInstance(hInstance);
	HRESULT hr = ::CoInitialize(NULL);
	if (FAILED(hr))
	{
		return 0;
	}

	CMainFrameWork::SingleInstance()->ShowWndInstall();



	CPaintManagerUI::MessageLoop();
	::CoUninitialize();

	return 0;
}

