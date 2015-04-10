#include "StdAfx.h"
#include "MainFrameWork.h"
#include "function.h"

#define STR_CLSID _T("{03585792-866A-43A5-8AE0-50C16BC85B24}")
#define STR_LIBID _T("{0AFACED1-E828-11D1-9187-B532F1E9575D}")

DWORD WINAPI  StartInstallThread(LPVOID lpParam)
{
	CMainFrameWork *pThis = (CMainFrameWork *)lpParam;
	pThis->BeginInstall();

	return 0;
}

DWORD WINAPI StartRollBackThread(LPVOID lpParam)
{
	CMainFrameWork *pThis = (CMainFrameWork*)lpParam;
	pThis->BeginRollBack();

	return 0;
}


CMainFrameWork * CMainFrameWork::m_SingleMainFrame = NULL;

CMainFrameWork::CMainFrameWork(void):m_WndInstall(NULL), m_WndQuitInstall(NULL)
{
	m_hInstallThread = NULL;
	m_hRollBackThread = NULL;
}


CMainFrameWork::~CMainFrameWork(void)
{

}

CMainFrameWork * CMainFrameWork::SingleInstance()
{
	if (m_SingleMainFrame == NULL)
	{
		m_SingleMainFrame = new CMainFrameWork;
		static destroy des;
	}

	return m_SingleMainFrame;
}

void CMainFrameWork::ShowWndInstall()
{
	m_WndInstall = new CWndInstall;
	m_WndInstall->Create(NULL, _T("安 装"), WS_VISIBLE | WS_MINIMIZEBOX, 0);
	m_WndInstall->CenterWindow();
	m_WndInstall->ShowWindow();
}

void CMainFrameWork::ShowWndQuitInstall(HWND hParent )
{
	m_WndQuitInstall = new CWndQuitInstall;
	m_WndQuitInstall->Create(hParent, _T("退出安装"), WS_VISIBLE | WS_MINIMIZEBOX, 0);
	m_WndQuitInstall->CenterWindow();
	m_WndQuitInstall->ShowWindow();
}

void CMainFrameWork::StartInstall()
{
	m_hInstallThread = CreateThread(NULL, 0,  StartInstallThread, this	, 0, NULL);
}

void CMainFrameWork::RollBack()
{
	m_hRollBackThread = CreateThread(NULL, 0, StartRollBackThread, this, 0, NULL);
}

void CMainFrameWork::BeginInstall()
{
	FreeResource();
	SetInstallProgress(_T("释放资源..."), 30);
	Sleep(1000);

	WriteInstallRegistry();
	SetInstallProgress(_T("写注册表..."), 50);
	Sleep(1000);

	CreateStartupMenu();
	SetInstallProgress(_T("创建开始菜单..."), 70);
	Sleep(1000);

	CreateVirtualDisk();
	SetInstallProgress(_T("创建虚拟盘..."), 85);
	Sleep(100);

	OpenMainProcess();
	SetInstallProgress(_T("进入主程序..."), 100);

	Sleep(1000);
	SetInstallProgress(_T("进入主程序..."), 1000);
}

void CMainFrameWork::SetInstallProgress( LPCTSTR lpTip, int value )
{
	if (m_WndInstall != NULL)
	{
		m_WndInstall->SetInstallProgress(lpTip, value);
	}
}

void CMainFrameWork::FreeResource()
{
	string_t strInstallPath = m_szInstallPath;
	if (!PathFileExists(strInstallPath.c_str()))
	{
		DWORD nRet  = ::SHCreateDirectoryEx(NULL, strInstallPath.c_str(), NULL);		
	}

	strInstallPath += _T("\\");

	string_t strAesPath = m_szInstallPath;
	strAesPath += _T("\\crypto\\");

	string_t strSplitPaht = m_szInstallPath;
	strSplitPaht += _T("\\split\\");

	CreateDirectory(strAesPath.c_str(), NULL);
	CreateDirectory(strSplitPaht.c_str(), NULL);

	ReleaseResource(IDR_ICON_LOGO, _T("KACPAGE"), (LPTSTR)strInstallPath.c_str(), _T("logo.ico"));
	ReleaseResource(IDR_MAIN_EXE, _T("KACPAGE"), (LPTSTR)strInstallPath.c_str(), _T("凯锐立德安全管家.exe"));
	ReleaseResource(IDR_AES_DLL, _T("KACPAGE"), (LPTSTR)strAesPath.c_str(), _T("AES.dll"));
	ReleaseResource(IDR_BINSTREAD_DLL, _T("KACPAGE"), (LPTSTR)strSplitPaht.c_str(), _T("binstream.dll"));	
}

void CMainFrameWork::WriteInstallRegistry()
{

}

void CMainFrameWork::CreateStartupMenu()
{
	TCHAR chDir[512] = {0};

	LPMALLOC ShellMalloc;
	if (FAILED(SHGetMalloc(&ShellMalloc)))
	{
		return;
	}	

	LPITEMIDLIST DesktopPidl;
	if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_STARTMENU , &DesktopPidl)))
	{
		return;
	}

	if (FAILED(SHGetPathFromIDList(DesktopPidl, chDir)))
	{
		ShellMalloc->Free(DesktopPidl);
		ShellMalloc->Release();

		return;
	}

	TCHAR szDirectory[MAX_PATH] = {0};
	StringCchPrintf(szDirectory, _countof(szDirectory), _T("%s"), chDir);
	StringCchCat(szDirectory, _countof(szDirectory), _T("\\CheriLeadSafeManager"));
	if (!PathFileExists(szDirectory))
	{
		CreateDirectory(szDirectory, NULL);
	}

	TCHAR szLnk[MAX_PATH] = {0};
	StringCchPrintf(szLnk, _countof(szLnk), _T("%s"), szDirectory);
	StringCchCat(szLnk, _countof(szLnk), _T("\\凯锐立德安全管家.lnk"));

	TCHAR szMainExe[MAX_PATH] = {0};
	StringCchPrintf(szMainExe, _countof(szMainExe), m_szInstallPath.c_str());
	StringCchCat(szMainExe, _countof(szMainExe), _T("\\凯锐立德安全管家.exe"));

	CreateShortcut(szMainExe, szLnk);
}

void CMainFrameWork::CreateVirtualDisk()
{
	HKEY keyRoots[] = { HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE };

	for (int i=0; i<sizeof(keyRoots) / sizeof(HKEY); i++) 
	{
		WriteRegister(keyRoots[i]);
	}

	CRegKey keyCurUser;
	LONG nRet;
	nRet = keyCurUser.Open(keyRoots[1], _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace"));
	CRegKey keyCurUserNamespace;
	nRet = keyCurUserNamespace.Create(keyCurUser, STR_CLSID, NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS);
	if (nRet == ERROR_SUCCESS)
	{
		keyCurUserNamespace.SetStringValue(_T(""), _T("凯锐立德安全管家"));
	}

	keyCurUser.Close();
	keyCurUserNamespace.Close();

	CRegKey keyLocalMachine;
	nRet = keyLocalMachine.Open(keyRoots[2], _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace"));
	CRegKey keykeyLocalMachineNamespace;
	nRet = keykeyLocalMachineNamespace.Create(keyLocalMachine, STR_CLSID, NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS);
	if (nRet == ERROR_SUCCESS)
	{
		keykeyLocalMachineNamespace.SetStringValue(_T(""), _T("凯锐立德安全管家"));
	}

	keyLocalMachine.Close();
	keykeyLocalMachineNamespace.Close();

}

void CMainFrameWork::OpenMainProcess()
{

}

void CMainFrameWork::SetInstallPath( LPCTSTR lpPath )
{
	m_szInstallPath = lpPath;
}

string_t CMainFrameWork::GetInstallPath()
{
	return m_szInstallPath;
}

void CMainFrameWork::WriteRegister( HKEY hKey )
{	
	BOOL bHCRResult = FALSE;

	LONG nRet;

	TCHAR strDefaultIconPath[MAX_PATH] = {0};
	StringCchPrintf(strDefaultIconPath, _countof(strDefaultIconPath), m_szInstallPath.c_str());
	StringCchCat(strDefaultIconPath, _countof(strDefaultIconPath), _T("\\logo.ico"));

	CRegKey keyCLSID;

	if (hKey == HKEY_CLASSES_ROOT)
	{
		nRet = keyCLSID.Open(hKey, _T("CLSID"), WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS);
	}
	else if (hKey == HKEY_CURRENT_USER)
	{
		nRet = keyCLSID.Open(hKey, _T("SOFTWARE\\Classes\\CLSID"), WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS);
	}
	else if (hKey == HKEY_LOCAL_MACHINE)
	{
		nRet = keyCLSID.Open(hKey, _T("SOFTWARE\\Classes\\CLSID"), WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS);
	}	

	CRegKey keyClsidRoot;
	nRet = keyClsidRoot.Open(keyCLSID, STR_CLSID);

	if (nRet == ERROR_FILE_NOT_FOUND)
	{
		nRet = keyClsidRoot.Create(keyCLSID, STR_CLSID, NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS );
	}

	if (nRet == ERROR_SUCCESS)
	{
		keyClsidRoot.SetStringValue(_T(""), _T("安全管家"));
		keyClsidRoot.SetStringValue(_T("LocalizedString"), _T("安全管家"));
		keyClsidRoot.SetStringValue(_T("InfoTip"), _T("方便好用的安全管家"));
		keyClsidRoot.SetStringValue(_T("TileInfo"), _T("prop:System.ItemAuthors"));
		keyClsidRoot.SetStringValue(_T("System.ItemAuthors"), _T("方便好用的安全管家"));
	}

	CRegKey keyDefaultIcon;
	if (ERROR_SUCCESS == keyDefaultIcon.Create(keyClsidRoot, _T("DefaultIcon"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		keyDefaultIcon.SetStringValue(_T(""), strDefaultIconPath, REG_EXPAND_SZ);
	}

	CRegKey keyInprocServer32;
	if (ERROR_SUCCESS == keyInprocServer32.Create(keyClsidRoot, _T("InprocServer32"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		keyInprocServer32.SetStringValue(_T(""), _T("%SystemRoot%\\system32\\shdocvw.dll"), REG_EXPAND_SZ);
		keyInprocServer32.SetStringValue( _T("ThreadingModel"), _T("Apartment" ) );
	}

	CRegKey keyInstance;
	if (ERROR_SUCCESS == keyInstance.Create(keyClsidRoot, _T("Instance"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		keyInstance.SetStringValue(_T(""), _T(""));
		keyInstance.SetStringValue( _T("CLSID"), STR_LIBID);
	}

	CRegKey keyInstance_InitPropertyBag;
	if (ERROR_SUCCESS == keyInstance_InitPropertyBag.Create(keyInstance, _T("InitPropertyBag"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		//keyInstance_InitPropertyBag.SetStringValue(_T(""), _T(""));
		keyInstance_InitPropertyBag.SetStringValue( _T("Target"), _T("C:\\"));
	}

	CRegKey keyShellFolder;
	if (ERROR_SUCCESS == keyShellFolder.Create(keyClsidRoot, _T("ShellFolder"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		keyShellFolder.SetDWORDValue( _T("Attributes"), 0X48800108 ) ;
		keyShellFolder.SetStringValue( _T("PinToNameSpaceTree"), _T("") ) ;
		keyShellFolder.SetStringValue( _T("QueryForOverlay"), _T("") ) ;
		keyShellFolder.SetStringValue( _T("WantsFORPARSING"), _T("") ) ;
	}

	CRegKey keyShell;
	if (ERROR_SUCCESS == keyShell.Create(keyClsidRoot, _T("Shell"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		//keyShell.SetStringValue(_T(""), _T(""));
	}

	CRegKey keyShellOpen;
	if (ERROR_SUCCESS == keyShellOpen.Create(keyShell, _T("Open"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		//keyShellOpen.SetStringValue(_T(""), _T(""));
	}

	CRegKey keyShellOpenCommand;
	if (ERROR_SUCCESS == keyShellOpenCommand.Create(keyShellOpen, _T("Command"), NULL, NULL, WRITE_OWNER|KEY_WOW64_64KEY|KEY_ALL_ACCESS))
	{
		TCHAR szMainExe[MAX_PATH] = {0};
		StringCchPrintf(szMainExe, _countof(szMainExe), m_szInstallPath.c_str());
		StringCchCat(szMainExe, _countof(szMainExe), _T("\\凯锐立德安全管家.exe -diskopen"));
		keyShellOpenCommand.SetStringValue(_T(""), szMainExe);
	}

	keyCLSID.Close();
	keyClsidRoot.Close();
	keyDefaultIcon.Close();
	keyInprocServer32.Close();
	keyInstance.Close();
	keyInstance_InitPropertyBag.Close();
	keyShellFolder.Close();
	keyShell.Close();
	keyShellOpen.Close();
	keyShellOpenCommand.Close();
}

void CMainFrameWork::Exit_App()
{
	//DestroyWindow();
	PostQuitMessage(0);
}

HANDLE CMainFrameWork::GetThreadInstallHandle()
{
	return m_hInstallThread;
}

void CMainFrameWork::BeginRollBack()
{

}

LRESULT CMainFrameWork::OnExitApp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (uMsg == WM_EXIT_APP)
	{
		PostQuitMessage(0);
		exit(0);
	}


	return 0;
}

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
