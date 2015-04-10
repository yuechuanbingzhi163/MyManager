#include "stdafx.h"

BOOL CreateShortcut( LPCTSTR lpFileName, LPCTSTR lpLnkName )
{
	HRESULT					hr		= -1;
	IShellLink				*pLink	= NULL;		//IShellLink对象指针

	IPersistFile			*ppf	= NULL;		//IPersisFil对象指针

	CoInitialize(NULL);
	do 
	{
		if(!lpFileName || !lpLnkName)
			break;

		//创建IShellLink对象
		hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
		if (FAILED(hr))
			break;

		//从IShellLink对象中获取IPersistFile接口
		hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
		if (FAILED(hr))
		{
			pLink->Release();
			break;
		}

		hr = pLink->SetPath(lpFileName);
		if(FAILED(hr))
		{
			ppf->Release();
			pLink->Release();
			break;
		}

		hr = pLink->SetShowCmd(SW_SHOWNORMAL);
		if(FAILED(hr))
		{
			ppf->Release();
			pLink->Release();
			break;
		}

		hr = pLink->Resolve(NULL, SLR_UPDATE);
		if(FAILED(hr))
		{
			ppf->Release();
			pLink->Release();
			break;
		}

		hr = ppf->Save(lpLnkName, TRUE);
		if(FAILED(hr))
		{
			ppf->Release();
			pLink->Release();
			break;
		}

		ppf->Release();
		pLink->Release();

	} while (FALSE);

	CoUninitialize();
	return SUCCEEDED(hr);
}

BOOL ReleaseResource( IN ULONG ResID, IN LPCTSTR lpResType, IN LPTSTR lpPath, IN LPTSTR lpFileName )
{
	BOOL		bRet				= FALSE;
	HMODULE		hModule				= NULL;
	HRSRC		hSrc				= NULL;
	DWORD		dwSize				= 0;
	HGLOBAL		hGlobal				= NULL;
	LPVOID		lpBuf				= NULL;
	TCHAR		FileName[MAX_PATH]	= {0};
	HANDLE		hFile				= INVALID_HANDLE_VALUE;

	do 
	{
		hModule = ::GetModuleHandle(NULL);
		if (!hModule)
			break;

		hSrc = ::FindResource(hModule, MAKEINTRESOURCE(ResID), lpResType);
		if (!hSrc)
			break;

		dwSize  = ::SizeofResource(hModule, hSrc);
		hGlobal = ::LoadResource(hModule, hSrc);
		if (!hGlobal)
			break;

		lpBuf = ::LockResource(hGlobal);
		if (!lpBuf)
			break;

		::StringCbPrintf(FileName, MAX_PATH * sizeof(TCHAR), _T("%s%s"), lpPath, lpFileName);
		hFile = ::CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			break;

		if(!::WriteFile(hFile, lpBuf, dwSize, &dwSize, NULL))
			break;
		bRet = TRUE;
	} while (FALSE);

	if (INVALID_HANDLE_VALUE != hFile)
		::CloseHandle(hFile);

	return bRet;
}
