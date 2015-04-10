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
	void BeginInstall();   //1.�ͷ���Դ��2.дע��������ļ�������ס�������½���д); 3.������ʼ�˵���4.���������̣� 5.��������

	void RollBack();		//����ع�
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

