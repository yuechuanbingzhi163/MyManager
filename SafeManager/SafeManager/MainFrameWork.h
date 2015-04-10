#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include "WndMainFrame.h"
#include "TrayNotifyIcon.h"

static UINT WM_TASKBAR_RESTART;

typedef struct st_config
{
	string_t disk_name;
	UINT use_shortcut_key;
	UINT fsModifiers;
	UINT virtual_key;
	UINT show_shortcut_icon;
	UINT show_tray_icon;
	UINT save_source_file;
}STCONFIG, *LPCONFIG;

class CMainFrameWork : public CWindowImpl<CMainFrameWork>
{
public:	
	virtual ~CMainFrameWork(void);

	void Init();

	void Exit_App();

	BOOL IsHaveRegisterApp();
	void RegisterApp();

	void RunMainAppWnd( );

	void ShowWndScanQRCodeBind(HWND hParent = NULL);
	void ShowWndScanQRCodeOpen(HWND hParent = NULL, int nFlag = 0, LPCTSTR lp = NULL);
	void ShowWndMainFrame(HWND hParent = NULL, bool IsVisible=TRUE);
	void ShowWndTray(HWND hParent = NULL);
	CWndMainFrame *GetWndMainFranme();

	BOOL Init_SplitLayer();
	BOOL Init_CryptoLayer();
	void	UnInit_SplitLayer();
	void	UnInit_CryptoLayer();

	void InitConfig();
	void SetConfig(LPCTSTR lpDiskName, bool bUseShortcutKey, UINT nModifiers, UINT nVirtualKey, bool bShowShortcutcIcon, bool bShowTrayIcon, bool bSaveSourceFile);
	string_t GetConfig_DiskName();
	bool GetConfig_UseShortcutKey();
	UINT GetConfig_fsModifiers();
	UINT GetConfig_VirturlKey( );
	bool GetConfig_ShowShortcutIcon();
	bool GetConfig_ShowTrayIcon();
	bool GetConfig_SaveSourceFile();

	void AddNotifyIcon();
	void DeleteNotifyIcon();

	void SelfRegisterHotKey();
	void SelfUnRegisterHotKey();

	void SetValidTime(UINT nTime);
	UINT GetValidTime();

	void UpdateValidTime();


	//读取配置文件获得文件和碎片存放根目录
	static string_t GetFilesAndChipsDirectory();

public:
	static CMainFrameWork *SingleInstance();

	DECLARE_WND_CLASS_EX(_T("MainFrame"), 0, -1)

	BEGIN_MSG_MAP(CMainFrameWork)
		MESSAGE_HANDLER(WM_NIDTIP, OnTrayIcon)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		MESSAGE_HANDLER(WM_TASKBAR_RESTART, OnTaskBarRestart)
	END_MSG_MAP()

	LRESULT OnTrayIcon(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);
	LRESULT OnHotKey(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);
	LRESULT OnTaskBarRestart(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle);

	void OnFinalMessage(HWND hWnd);


protected:
	CMainFrameWork(void);

	LRESULT OnExitApp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	static CMainFrameWork *m_SingleMainFrame;

	CWndMainFrame  *m_pWndMainFrame;

	static string_t m_FilesAndChipsDirectory;

	STCONFIG m_cofig;

	CTrayNotifyIcon m_TrayNotifyIcon;

	int m_nHotKeyID;

	UINT m_nValidTime;

	UINT m_TimerID;

private:
	class destroy
	{
	public:
		destroy();
		~destroy();
	};
};

