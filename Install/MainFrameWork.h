#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include "WndInstall.h"
#include "WndQuitInstall.h"

class CMainFrameWork : public CWindowImpl<CMainFrameWork>
{
public:	
	virtual ~CMainFrameWork(void);

	void Exit_App();

	void ShowWndInstall();
	void ShowWndQuitInstall(HWND hParent = NULL);

	void StartInstall();
	void BeginInstall();   //1.释放资源；2.写注册表（配置文件可以在住进程中新建改写); 3.创建开始菜单；4.创建虚拟盘； 5.打开主进程

	void RollBack();		//程序回滚
	void BeginRollBack();

	void SetInstallPath(LPCTSTR lpPath);
	string_t GetInstallPath();
	
	HANDLE GetThreadInstallHandle();

public:
	static CMainFrameWork *SingleInstance();

	DECLARE_WND_CLASS_EX(_T("MainFrame"), 0, -1)

	BEGIN_MSG_MAP(CMainFrameWork)
		MESSAGE_HANDLER(WM_EXIT_APP, OnExitApp)
	END_MSG_MAP()


protected:
	CMainFrameWork(void);

	void FreeResource();
	void WriteInstallRegistry();
	void CreateStartupMenu();
	void CreateVirtualDisk();
	void OpenMainProcess();

	void SetInstallProgress(LPCTSTR lpTip, int value);

	void WriteRegister(HKEY hKey);

	LRESULT OnExitApp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	static CMainFrameWork *m_SingleMainFrame;

	CWndInstall *m_WndInstall;
	CWndQuitInstall *m_WndQuitInstall;

	HANDLE  m_hInstallThread;

	HANDLE m_hRollBackThread;

	string_t  m_szInstallPath;

private:
	class destroy
	{
	public:
		destroy();
		~destroy();
	};
};

