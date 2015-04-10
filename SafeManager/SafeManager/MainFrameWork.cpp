#include "StdAfx.h"
#include "MainFrameWork.h"
#include "function.h"
#include "WndScanQRCodeAndBind.h"
#include "WndScanQRCodeOpen.h"
#include "WndMainFrame.h"
#include "CryptoLayer.h"
#include "SplitLayer.h"
#include "DatabaseManager.h"
#include "FileHandleManager.h"
#include "WndTray.h"

VOID CALLBACK _TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

extern HINSTANCE g_Hinstance;

CMainFrameWork * CMainFrameWork::m_SingleMainFrame = NULL;

CMainFrameWork::CMainFrameWork(void):m_pWndMainFrame(NULL),m_nHotKeyID(-1)
{

}

void CMainFrameWork::Init()
{
	if (Init_CryptoLayer())
	{
		LOG(_T("导入加密算法成功!"));
	}

	if (Init_SplitLayer())
	{
		LOG(_T("导入破碎算法成功!"));
	}	

	m_nValidTime = 0;

	InitConfig();

	CDatabaseManager::GetSingleInstance();

	CFileHandleManager::GetSingleInstance()->InitFileHandleTree();
}

CMainFrameWork::~CMainFrameWork(void)
{
	
}

CMainFrameWork * CMainFrameWork::SingleInstance()
{
	if (m_SingleMainFrame == NULL)
	{
		m_SingleMainFrame = new CMainFrameWork;			
		HWND hwnd = m_SingleMainFrame->Create( NULL, rcDefault, NULL, WS_OVERLAPPEDWINDOW );	
		m_SingleMainFrame->ShowWindow(SW_HIDE);

		WM_TASKBAR_RESTART =  ::RegisterWindowMessage(_T("TaskBarCreated"));

		m_SingleMainFrame->Init();

		static destroy des;
	}

	return m_SingleMainFrame;
}

void CMainFrameWork::Exit_App()
{
	DestroyWindow();
	PostQuitMessage(0);
}

void CMainFrameWork::ShowWndScanQRCodeBind( HWND hParent /*= NULL*/ )
{
	HWND hwnd = ::FindWindow(_T("CWndScanQRCodeAndBind"), _T("扫码绑定"));
	if (hwnd != NULL)
	{
		::BringWindowToTop(hwnd);
	}
	else
	{
		CWndScanQRCodeAndBind *pWnd = new CWndScanQRCodeAndBind;
		pWnd->SetQRCodeData(_T("http://www.baidu.com/"));
		pWnd->Create(hParent, _T("扫码绑定"), WS_VISIBLE | WS_MINIMIZEBOX, 0);
		pWnd->CenterWindow();
		pWnd->ShowWindow();
		::BringWindowToTop(hwnd);
	}
}

void CMainFrameWork::ShowWndScanQRCodeOpen( HWND hParent, int nFlag, LPCTSTR lp)
{
	HWND hwnd = ::FindWindow(_T("CWndScanQRCodeOpen"), _T("扫码打开"));
	if (hwnd != NULL)
	{
		::BringWindowToTop(hwnd);
	}
	else
	{
		CWndScanQRCodeOpen *pWnd = new CWndScanQRCodeOpen;
		pWnd->SetFlag(nFlag);
		pWnd->SetQRCodeData(lp);
		pWnd->Create(hParent, _T("扫码打开"), WS_VISIBLE | WS_MINIMIZEBOX, 0);
		pWnd->CenterWindow();
		pWnd->ShowWindow();
		::BringWindowToTop(pWnd->GetHWND());
	}
}

void CMainFrameWork::ShowWndMainFrame( HWND hParent /*= NULL*/, bool IsVisible/*=TRUE*/ )
{
		if (m_pWndMainFrame == NULL)
		{
			CWndMainFrame *pWnd = new CWndMainFrame;
			pWnd->Create(hParent, _T("安全管家"), UI_WNDSTYLE_FRAME, 0);
			pWnd->CenterWindow();
			pWnd->ShowWindow(IsVisible);
			m_pWndMainFrame = pWnd;
		}	
		else
		{
			if (IsVisible)
			{
				m_pWndMainFrame->ShowWindow(true);
				::BringWindowToTop(m_pWndMainFrame->GetHWND());
			}
			else
			{
				m_pWndMainFrame->ShowWindow(false);
			}
			
		}
}

void CMainFrameWork::ShowWndTray( HWND hParent /*= NULL*/ )
{
	DuiLib::CPoint point;
	GetCursorPos(&point);

	CString strx, stry;
	strx.Format(L"%d", point.x);
	stry.Format(L"%d", point.y);

	HWND hWnd = ::FindWindow(_T("CWndTray"), _T("TrayMenu"));
	if (::IsWindow(hWnd))
	{
		::BringWindowToTop(hWnd);
		return ;
	}

	CWndTray *pWnd = new CWndTray;
	pWnd->Create(NULL, _T("TrayMenu"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);	
	pWnd->ShowWindow();

	HDC   hdc=::GetDC(NULL);   //获得屏幕设备描述表句柄   
	int   ScrWidth=GetDeviceCaps(hdc,HORZRES);   //获取屏幕水平分辨率   
	int   ScrHeight=GetDeviceCaps(hdc,VERTRES);     //获取屏幕垂直分辨率  
	::ReleaseDC(NULL,hdc);   //释放屏幕设备描述表

	SetForegroundWindow(pWnd->GetHWND());

	CDuiRect rcWnd;
	::GetWindowRect(pWnd->GetHWND(), &rcWnd);

	if (point.x + rcWnd.GetWidth() > ScrWidth)
	{
		::SetWindowPos(pWnd->GetHWND(), HWND_TOPMOST, point.x - rcWnd.GetWidth(), point.y - rcWnd.GetHeight(), rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_SHOWWINDOW );
	}
	else
	{
		::SetWindowPos(pWnd->GetHWND(), HWND_TOPMOST, point.x, point.y - rcWnd.GetHeight(), rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_SHOWWINDOW );
	}
}

string_t CMainFrameWork::GetFilesAndChipsDirectory()
{
	if (m_FilesAndChipsDirectory.size() != 0)
	{
		return m_FilesAndChipsDirectory;
	}

	TCHAR RetPath[MAX_PATH] = {0};

	TCHAR FilePath[MAX_PATH] = {0};
	int Length = _countof(FilePath);
	GetModuleFileName(NULL, FilePath, Length);

	LPTSTR pos = StrRChr(FilePath, NULL, _T('\\'));

	*(pos + 1) = '\0';

	StringCchCat(FilePath, _countof(FilePath), _T("config.ini"));

	if (!PathFileExists(FilePath))
	{
		::MessageBox(NULL, _T("配置文件不存在!"), _T("提示"), MB_OK);		
	}
	else
	{		
		GetPrivateProfileString(_T("config"), _T("CherileadFilesChips_Dir"), _T(""), RetPath, MAX_PATH, FilePath);
	}

	PathRemoveBackslash(RetPath);

	m_FilesAndChipsDirectory = RetPath;

	return m_FilesAndChipsDirectory;
}

BOOL CMainFrameWork::Init_CryptoLayer()
{
	BOOL bRet = TRUE;

	CCryptoLayer CryptoLayer;

	string_t strPath = GetCurrentModulePath();

	string_t LayerPath = strPath;
	LayerPath += _T("\\crypto\\");

	if (!CryptoLayer.Load((LPTSTR)LayerPath.c_str()))
	{
		LOG(_T("加载加密算法失败！"));

		bRet = FALSE;
	}	

	return bRet;
}

BOOL CMainFrameWork::Init_SplitLayer()
{
	BOOL bRet = TRUE;
	CSplitLayer SplitLayer;

	string_t strPath = GetCurrentModulePath();

	string_t LayerPath = strPath;
	LayerPath += _T("\\split\\");

	if (!SplitLayer.Load((LPTSTR)LayerPath.c_str()))
	{
		LOG(_T("加载加密算法失败！"));
		bRet  = FALSE;
	}

	return bRet;
}

void CMainFrameWork::UnInit_SplitLayer()
{
	CSplitLayer SplitLayer;
	SplitLayer.Destory();
}

void CMainFrameWork::UnInit_CryptoLayer()
{
	CCryptoLayer CryptoLayer;
	CryptoLayer.Destory();
}

CWndMainFrame * CMainFrameWork::GetWndMainFranme()
{
	return m_pWndMainFrame;
}

void CMainFrameWork::InitConfig()
{
	TCHAR szDiskName[MAX_PATH] = {0};
	UINT nShortcutKey = 0;
	UINT nFsModifiers = 0;
	UINT nVirturalKey = 0;
	UINT nShowShortcutIcon = 0;
	UINT nShowTrayIcon = 0;
	UINT nSaveSourceFile = 1;	

	TCHAR FilePath[MAX_PATH] = {0};
	int Length = _countof(FilePath);
	GetModuleFileName(NULL, FilePath, Length);

	LPTSTR pos = StrRChr(FilePath, NULL, _T('\\'));

	*(pos + 1) = '\0';

	StringCchCat(FilePath, _countof(FilePath), _T("config.ini"));

	if (!PathFileExists(FilePath))
	{		
		HANDLE hFile = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CloseHandle(hFile);
		BOOL bRet = WritePrivateProfileString(_T("config"), _T("diskname"), _T(""), FilePath);

		WritePrivateProfileString(_T("config"), _T("have_register_app"), _T("0"), FilePath);
		WritePrivateProfileString(_T("config"), _T("use_shortcut_key"), _T("0"), FilePath);
		WritePrivateProfileString(_T("config"), _T("fsmodifiers"), _T("0"), FilePath);
		WritePrivateProfileString(_T("config"), _T("virtual_key"), _T("0"), FilePath);
		WritePrivateProfileString(_T("config"), _T("show_shortcut_icon"), _T("1"), FilePath);
		WritePrivateProfileString(_T("config"), _T("show_tray_icon"), _T("1"), FilePath);
		WritePrivateProfileString(_T("config"), _T("save_source_file"), _T("1"), FilePath);

		string_t strMaxFreeSpaceDisk = GetMaxFreeSpaceDisk();
		strMaxFreeSpaceDisk += _T("CherileadFilesChips");
		CreateDirectory(strMaxFreeSpaceDisk.c_str(), NULL);
		WritePrivateProfileString(_T("config"), _T("CherileadFilesChips_Dir"), strMaxFreeSpaceDisk.c_str(), FilePath);

		string_t strFragsPath = strMaxFreeSpaceDisk;
		string_t strTempFilePaht =strMaxFreeSpaceDisk;
		strFragsPath += _T("\\frags");
		strTempFilePaht += _T("\\temp");
		CreateDirectory(strFragsPath.c_str(), NULL);
		CreateDirectory(strTempFilePaht.c_str(), NULL);		
	}
	
	GetPrivateProfileString(_T("config"), _T("diskname"), _T(""), szDiskName, MAX_PATH, FilePath);
	nShortcutKey = GetPrivateProfileInt(_T("config"), _T("use_shortcut_key"), 0, FilePath);
	nFsModifiers = GetPrivateProfileInt(_T("config"), _T("fsmodifiers"), 0, FilePath);
	nVirturalKey = GetPrivateProfileInt(_T("config"), _T("virtual_key"), 0, FilePath);
	nShowShortcutIcon = GetPrivateProfileInt(_T("config"), _T("show_shortcut_icon"), 0, FilePath);
	nShowTrayIcon = GetPrivateProfileInt(_T("config"), _T("show_tray_icon"), 0, FilePath);
	nSaveSourceFile = GetPrivateProfileInt(_T("config"), _T("save_source_file"), 1, FilePath);	

	m_cofig.disk_name = szDiskName;
	m_cofig.use_shortcut_key = nShortcutKey;
	m_cofig.fsModifiers = nFsModifiers;
	m_cofig.virtual_key = nVirturalKey;
	m_cofig.show_shortcut_icon = nShowShortcutIcon;
	m_cofig.show_tray_icon = nShowTrayIcon;
	m_cofig.save_source_file = nSaveSourceFile;

	if (m_cofig.show_tray_icon != 0)
	{
		AddNotifyIcon();
	}

	if (m_cofig.use_shortcut_key == 1)
	{
		SelfRegisterHotKey();
	}
}

void CMainFrameWork::SetConfig( LPCTSTR lpDiskName, bool bUseShortcutKey, UINT nModifiers, UINT nVirtualKey, bool bShowShortcutcIcon, bool bShowTrayIcon, bool bSaveSourceFile )
{
	m_cofig.disk_name = lpDiskName;
	m_cofig.use_shortcut_key = bUseShortcutKey ? 1:0;
	m_cofig.fsModifiers = nModifiers;
	m_cofig.virtual_key = nVirtualKey;
	m_cofig.show_shortcut_icon = bShowShortcutcIcon ? 1:0;
	m_cofig.show_tray_icon = bShowTrayIcon ? 1:0;
	m_cofig.save_source_file = bSaveSourceFile ? 1:0;

	if (m_cofig.show_tray_icon == 0)
	{
		DeleteNotifyIcon();
	}
	else
	{
		AddNotifyIcon();
	}

	if (m_cofig.use_shortcut_key == 1)
	{
		SelfUnRegisterHotKey();
		SelfRegisterHotKey();
	}
	else
	{
		SelfUnRegisterHotKey();
	}


	TCHAR FilePath[MAX_PATH] = {0};
	int Length = _countof(FilePath);
	GetModuleFileName(NULL, FilePath, Length);

	LPTSTR pos = StrRChr(FilePath, NULL, _T('\\'));

	*(pos + 1) = '\0';

	StringCchCat(FilePath, _countof(FilePath), _T("config.ini"));

	if (PathFileExists(FilePath))
	{
		TCHAR szTemp[10] = {0};
		
		WritePrivateProfileString(_T("config"), _T("diskname"), m_cofig.disk_name.c_str(), FilePath);

		_itot(m_cofig.use_shortcut_key, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("use_shortcut_key"), szTemp, FilePath);
		_itot(m_cofig.fsModifiers, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("fsmodifiers"), szTemp, FilePath);
		_itot(m_cofig.virtual_key, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("virtual_key"), szTemp, FilePath);
		_itot(m_cofig.show_shortcut_icon, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("show_shortcut_icon"), szTemp, FilePath);
		_itot(m_cofig.show_tray_icon, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("show_tray_icon"), szTemp, FilePath);
		_itot(m_cofig.save_source_file, szTemp, 10);
		WritePrivateProfileString(_T("config"), _T("save_source_file"), szTemp, FilePath);
	}
}

string_t CMainFrameWork::GetConfig_DiskName()
{
	return m_cofig.disk_name;
}

bool CMainFrameWork::GetConfig_UseShortcutKey()
{
	return (m_cofig.use_shortcut_key == 0) ? false:true;
}

UINT CMainFrameWork::GetConfig_fsModifiers()
{
	return m_cofig.fsModifiers;
}

UINT CMainFrameWork::GetConfig_VirturlKey()
{
	return m_cofig.virtual_key;
}

bool CMainFrameWork::GetConfig_ShowShortcutIcon()
{
	return (m_cofig.show_shortcut_icon == 0) ? false:true;
}

bool CMainFrameWork::GetConfig_ShowTrayIcon()
{
	return (m_cofig.show_tray_icon == 0) ? false:true;
}

bool CMainFrameWork::GetConfig_SaveSourceFile()
{
	return (m_cofig.save_source_file == 0) ? false:true;
}

void CMainFrameWork::AddNotifyIcon()
{
	m_TrayNotifyIcon.Init(g_Hinstance, m_hWnd);
	m_TrayNotifyIcon.SetIcon(IDI_SAFEMANAGER);
	bool bRet = m_TrayNotifyIcon.AddNofityIcon();

	int i = 0;
}

void CMainFrameWork::DeleteNotifyIcon()
{
	m_TrayNotifyIcon.DeleteNotifyIcon();
}

LRESULT CMainFrameWork::OnTrayIcon( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle )
{
	if (LOWORD(lParam) == WM_RBUTTONUP)
	{
		ShowWndTray();
	}
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		RunMainAppWnd();
	}

	return 0;
}

void CMainFrameWork::OnFinalMessage( HWND hWnd )
{
	if (m_nHotKeyID != -1)
	{
		UnregisterHotKey(m_hWnd, m_nHotKeyID);
	}
}

LRESULT CMainFrameWork::OnHotKey( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle )
{
	if ((long)wParam == m_nHotKeyID)
	{
		if (m_pWndMainFrame == NULL)
		{
			RunMainAppWnd();
		}
		else
		{
			m_pWndMainFrame->ShowWindow();
		}
	}

	return 0;
}

void CMainFrameWork::SelfRegisterHotKey()
{
	ATOM atom = GlobalAddAtom(_T("SafeManagerHotKey"));
	m_nHotKeyID = atom - 0xC000;

	RegisterHotKey(m_hWnd, m_nHotKeyID, m_cofig.fsModifiers, m_cofig.virtual_key);
}

void CMainFrameWork::SelfUnRegisterHotKey()
{
	UnregisterHotKey(m_hWnd, m_nHotKeyID);
	m_nHotKeyID = -1;
}

LRESULT CMainFrameWork::OnTaskBarRestart( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandle )
{
	if (m_cofig.show_tray_icon == 1)
	{
		 AddNotifyIcon();
	}

	return 0;
}

BOOL CMainFrameWork::IsHaveRegisterApp()
{
	BOOL bRet = FALSE;

	TCHAR FilePath[MAX_PATH] = {0};
	int Length = _countof(FilePath);
	GetModuleFileName(NULL, FilePath, Length);

	LPTSTR pos = StrRChr(FilePath, NULL, _T('\\'));

	*(pos + 1) = '\0';

	StringCchCat(FilePath, _countof(FilePath), _T("config.ini"));

	int nRet = GetPrivateProfileInt(_T("config"), _T("have_register_app"), 0, FilePath);

	if (nRet == 0)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}

	return bRet;
}

void CMainFrameWork::RegisterApp()
{
	TCHAR FilePath[MAX_PATH] = {0};
	int Length = _countof(FilePath);
	GetModuleFileName(NULL, FilePath, Length);

	LPTSTR pos = StrRChr(FilePath, NULL, _T('\\'));

	*(pos + 1) = '\0';

	StringCchCat(FilePath, _countof(FilePath), _T("config.ini"));

	WritePrivateProfileString(_T("config"), _T("have_register_app"), _T("1"), FilePath);
}

void CMainFrameWork::SetValidTime( UINT nTime )
{
	m_nValidTime = nTime;

	//::SetTimer(m_hWnd, m_TimerID, 1000, _TimerProc);
}

UINT CMainFrameWork::GetValidTime()
{
	return m_nValidTime;
}

void CMainFrameWork::RunMainAppWnd()
{
	ShowWndMainFrame();
}

void CMainFrameWork::UpdateValidTime()
{
	m_nValidTime -= 1000;

	if (m_nValidTime <= 0)
	{	
		ShowWndMainFrame(NULL, false);
		ShowWndScanQRCodeOpen(NULL, 1, _T("http:\\www.baidu.com"));
		::KillTimer(m_hWnd, m_TimerID);
	}
}


string_t CMainFrameWork::m_FilesAndChipsDirectory;

CMainFrameWork::destroy::destroy()
{

}

CMainFrameWork::destroy::~destroy()
{
	if (CMainFrameWork::m_SingleMainFrame != NULL)
	{
		delete CMainFrameWork::m_SingleMainFrame;
	}
}

VOID CALLBACK _TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	CMainFrameWork::SingleInstance()->UpdateValidTime();
}









