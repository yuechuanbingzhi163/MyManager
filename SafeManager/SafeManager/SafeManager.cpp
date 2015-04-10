// SafeManager.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SafeManager.h"
#include "MainFrameWork.h"
#include "HttpsRequest.h"
#include "SplitLayer.h"
#include "FileHandleManager.h"
#include "WndMessageBox.h"

void Exit_Function();

HINSTANCE g_Hinstance = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	::OleInitialize(NULL);
	::CoInitialize(NULL);

	g_Hinstance = hInstance;

	bool bRet = CLog::Init();
	if (bRet)
	{
		LOG(_T("日志模块初始化成功!"));
	}
	else
	{
		return 0;
	}

	CPaintManagerUI::SetInstance(hInstance);

	 CMainFrameWork::SingleInstance()->RunMainAppWnd();

	CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	::OleUninitialize();

	return 0;
}

void Exit_Function()
{

}

