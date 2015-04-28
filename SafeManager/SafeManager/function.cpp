#include "stdafx.h"
#include "QR_Encode.h"
#include <dlgs.h>
#include "function.h"
#include <winioctl.h>

LONG_PTR g_lOriWndProc = 0;
LRESULT static __stdcall  _WndProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam  );

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

std::wstring ANSI2Unicode( const string &str )
{
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t *pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	ZeroMemory(pUnicode, sizeof(wchar_t) * (unicodeLen + 1));

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (LPWSTR)pUnicode, unicodeLen);

	wstring rt;
	rt = pUnicode;

	delete[] pUnicode;

	return rt;
}

std::string Unicode2Ansi( const wstring &str )
{
	int charLen = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

	char *pChar;
	pChar = new char[charLen + 1];
	ZeroMemory(pChar, sizeof(char) * (charLen + 1));

	::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, pChar, charLen, NULL, NULL);

	string rt;
	rt = pChar;

	delete[] pChar;

	return rt;
}

HBITMAP CreateQRCode(LPCTSTR lpSource, int cx, int cy)
{
	HBITMAP hBmpRet = NULL;

	CBitmap *pWorkBitmap = NULL;
	int nSymbleSize = 0;
	CBitmap *pSymbleBitmap = NULL;
	CDC *pSymbleDC = NULL;

	int i, j;

	CQR_Encode* pQR_Encode = new CQR_Encode;
	string strSource = Unicode2Ansi(lpSource);
	BOOL bResult = pQR_Encode->EncodeData(2, 3, 1, 1, strSource.c_str());

	if (bResult)
	{		
		nSymbleSize = pQR_Encode->m_nSymbleSize;

		pSymbleBitmap = new CBitmap;
		pSymbleBitmap->CreateBitmap(nSymbleSize, nSymbleSize, 1, 1, NULL);

		pSymbleDC = new CDC;
		pSymbleDC->CreateCompatibleDC(NULL);

		pSymbleDC->SelectObject(pSymbleBitmap);

		pSymbleDC->PatBlt(0,0, nSymbleSize, nSymbleSize, WHITENESS);

		for (i = 0; i < pQR_Encode->m_nSymbleSize; ++i)
		{
			for (j = 0; j < pQR_Encode->m_nSymbleSize; ++j)
			{
				if (pQR_Encode->m_byModuleData[i][j])
				{					
					pSymbleDC->SetPixel(i, j, RGB(0, 0, 0));
				}
			}
		}	

		int nSrcSize = nSymbleSize;
		int nDstSize = min(cx, cy);

		CDC *pWorkDC = new CDC;
		pWorkDC->CreateCompatibleDC(NULL);
		
		pWorkBitmap = new CBitmap;
		pWorkBitmap->CreateBitmap(nDstSize, nDstSize, 1, 1, NULL);

		CBitmap *pOldBitmap = pWorkDC->SelectObject(pWorkBitmap);
		pWorkDC->StretchBlt(0,0, nDstSize, nDstSize, pSymbleDC, 0, 0, nSrcSize, nSrcSize, SRCCOPY);		

		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = nDstSize;
		bmi.bmiHeader.biHeight = nDstSize;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = nDstSize * nDstSize * sizeof(DWORD);

		BYTE *pvBits;          // pointer to DIB section
		hBmpRet = ::CreateDIBSection(pWorkDC->m_hDC, &bmi, DIB_PAL_COLORS, (void **)&pvBits, NULL, 0);	
		CDC *pNewDC = new CDC;
		pNewDC->CreateCompatibleDC(NULL);
		HBITMAP pold = (HBITMAP)pNewDC->SelectObject(hBmpRet);
		pNewDC->StretchBlt(0,0, nDstSize, nDstSize, pWorkDC, 0,0, nDstSize, nDstSize, SRCCOPY);

		CImage imgIcon;
		imgIcon.Load(_T("companylogo.ico"));
		int nIconWidth = imgIcon.GetWidth();
		int nIconHeight = imgIcon.GetHeight();
		CDC *pIconDC = new CDC;
		pIconDC->CreateCompatibleDC(NULL);
		HBITMAP pOldIcon = (HBITMAP)pIconDC->SelectObject((HBITMAP)imgIcon);
		pNewDC->StretchBlt((nDstSize - nIconWidth) / 2, (nDstSize - nIconHeight) / 2, nIconWidth, nIconHeight, pIconDC, 0, 0, nIconWidth, nIconHeight, SRCCOPY);

		pIconDC->SelectObject(pOldIcon);
		pNewDC->SelectObject(pold);

		delete pQR_Encode;		
		delete pSymbleBitmap;
		delete pSymbleDC;
		delete pWorkBitmap;
		delete pWorkDC;
		delete pNewDC;
		delete pIconDC;
	}

	return hBmpRet;
}

HBITMAP GetIconByFileType(LPCTSTR lpFileType, bool IsDirectory/* = false*/)
{
		HBITMAP hRet = NULL;

		HICON  hLargeIcon = NULL;
		ICONINFO iconInfo;
		int nWidth = 0; 
		int nHeight = 0;

		LONG lRet = 0;

		if (IsDirectory) //为文件夹
		{
			TCHAR strSystemDir[MAX_PATH] = {0};
			GetSystemDirectory(strSystemDir, MAX_PATH);
			lstrcat(strSystemDir, _T("\\shell32.dll"));
			
			ExtractIconEx(strSystemDir, 3, NULL, &hLargeIcon, 1);	
		}
		else			//为文件
		{
			if (lpFileType == NULL)
			{
				TCHAR strSystemDir[MAX_PATH] = {0};
				GetSystemDirectory(strSystemDir, MAX_PATH);
				lstrcat(strSystemDir, _T("\\shell32.dll"));

				ExtractIconEx(strSystemDir, 0, NULL, &hLargeIcon, 1);						
			}
			else
			{
				if (*lpFileType == _T('.'))
				{
					HKEY hkey = HKEY_CLASSES_ROOT;

					CRegKey regRoot;
					lRet = regRoot.Open(hkey, lpFileType);
					if (lRet == ERROR_SUCCESS)
					{
						TCHAR strRootValue[MAX_PATH] = {0};
						ULONG ulSize = MAX_PATH;
						regRoot.QueryStringValue(_T(""), strRootValue, &ulSize);

						if (StrCmp(strRootValue, _T("")) != 0)
						{
							CRegKey regModule;
							lRet = regModule.Open(hkey,  strRootValue);

							if (lRet == ERROR_SUCCESS)
							{
								CRegKey regIcon;
								lRet = regIcon.Open(regModule, _T("DefaultIcon"));

								if (lRet == ERROR_SUCCESS)
								{									
									TCHAR strIcon[MAX_PATH + 10] = {0};
									ULONG ulIcon = MAX_PATH + 10;
									lRet = regIcon.QueryStringValue(_T(""), strIcon, &ulIcon);								

									if (lRet == ERROR_INVALID_DATA)
									{
										int i = 0;
									}

									LPTSTR lpDot = StrRChr(strIcon, NULL, _T(','));
									if (lpDot == NULL)
									{
										TCHAR strSystemDir[MAX_PATH] = {0};
										GetSystemDirectory(strSystemDir, MAX_PATH);
										lstrcat(strSystemDir, _T("\\shell32.dll"));

										ExtractIconEx(strSystemDir, 2, NULL, &hLargeIcon, 1);		
									}
									else
									{
										LPCTSTR lpNum = lpDot + 1;
										*lpDot = _T('\0');
										LPTSTR lpExePath = strIcon;

										if (StrCmp(lpNum, _T("")) != NULL)
										{
											if (*(lpExePath) == _T('"'))
											{
												lpExePath = lpExePath + 1;
											}

											if (*(lpExePath + _tcslen(lpExePath) - 1) == _T('"') )
											{
												*(lpExePath + _tcslen(lpExePath) - 1) = _T('\0');
											}

											TCHAR strExpand[MAX_PATH + 10] = {0};

											ExpandEnvironmentStrings(lpExePath, strExpand, MAX_PATH +10);

											int iconIndex = _tstoi(lpNum);											
											ExtractIconEx(strExpand, iconIndex, NULL, &hLargeIcon, 1);	
											int i = 0;
										}
										else
										{
											TCHAR strSystemDir[MAX_PATH] = {0};
											GetSystemDirectory(strSystemDir, MAX_PATH);
											lstrcat(strSystemDir, _T("\\shell32.dll"));
											
											ExtractIconEx(strSystemDir, 0, NULL, &hLargeIcon, 1);											
										}
										
									}							
								}
							}
						}

					}
				}
			}
		}

		if (hLargeIcon == NULL)
		{
			TCHAR strSystemDir[MAX_PATH] = {0};
			GetSystemDirectory(strSystemDir, MAX_PATH);
			lstrcat(strSystemDir, _T("\\shell32.dll"));
			
			ExtractIconEx(strSystemDir, 0, NULL, &hLargeIcon, 1);
		}

		if (hLargeIcon != NULL)
		{
			GetIconInfo(hLargeIcon, &iconInfo);		

			BITMAP bmp;
			GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bmp);

			nWidth = bmp.bmWidth;
			nHeight = bmp.bmHeight;

			HDC dcMem;
			dcMem = ::CreateCompatibleDC(NULL);			
			HGDIOBJ hObMem = ::SelectObject(dcMem, iconInfo.hbmColor);				

			HDC dcImage;
			dcImage = ::CreateCompatibleDC(NULL);
			::SetBkMode(dcMem, TRANSPARENT);
			HGDIOBJ hObjImage = ::SelectObject(dcImage, iconInfo.hbmMask);				

			BITMAPINFO bmi;
			ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = nWidth;
			bmi.bmiHeader.biHeight = nHeight;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(DWORD);

			BYTE *pvBits;          // pointer to DIB section
			hRet = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);	

			HDC dcRet;
			dcRet = ::CreateCompatibleDC(NULL);
			//::SetBkColor(dcRet, RGB(255,255,255));
			//::SetBkMode(dcRet, TRANSPARENT);
			HGDIOBJ hObjRet = ::SelectObject(dcRet, hRet);			

			::BitBlt(dcRet, 0, 0, nWidth, nHeight, dcImage, 0, 0, SRCCOPY);			
			::BitBlt(dcRet, 0, 0, nWidth, nHeight, dcMem, 0, 0, SRCINVERT);	

			BITMAPINFO MaskBmi;
			ZeroMemory(&MaskBmi, sizeof(BITMAPINFO));
			MaskBmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			MaskBmi.bmiHeader.biWidth = nWidth;
			MaskBmi.bmiHeader.biHeight = nHeight;
			MaskBmi.bmiHeader.biPlanes = 1;
			MaskBmi.bmiHeader.biBitCount = 32;         // four 8-bit components
			MaskBmi.bmiHeader.biCompression = BI_RGB;
			MaskBmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(DWORD);

			BYTE *pvMaskBits = new BYTE[nWidth * nHeight * sizeof(DWORD)];
			::GetDIBits(dcImage, iconInfo.hbmMask, 0, nHeight, pvMaskBits, &MaskBmi, DIB_RGB_COLORS);

			for (int i=0; i<nWidth * nHeight; i++)
			{
				if( *(DWORD*)(&pvMaskBits[i*4]) == 0xFFFFFF )
				{
					pvBits[i*4] = (BYTE)0;
					pvBits[i*4 + 1] = (BYTE)0;
					pvBits[i*4 + 2] = (BYTE)0; 
					pvBits[i*4 + 3] = (BYTE)0;
				}
				else
				{
					//pvBits[i*4] = (BYTE)255;
					//pvBits[i*4 + 1] = (BYTE)255;
					//pvBits[i*4 + 2] = (BYTE)255; 
					pvBits[i*4 + 3] = (BYTE)255;
				}
			}

			delete pvMaskBits;

			::SelectObject(dcImage, hObjImage);
			::SelectObject(dcRet, hObjRet);

			::DeleteDC(dcImage);
			::DeleteDC(dcMem);
			::DeleteDC(dcRet);
		}
		

	return hRet;
}

UINT_PTR CALLBACK OpenFileDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (hWnd != NULL && GetParent(hWnd) != NULL)
	{
		TCHAR wcsClassNameParent[MAX_PATH] = {0};
		TCHAR wcsClassName[MAX_PATH] = {0};
		HWND hParent = GetParent(hWnd);
		GetClassName(hParent, wcsClassNameParent, MAX_PATH);
		GetClassName(hWnd, wcsClassName, MAX_PATH);

		if (StrCmp(wcsClassNameParent, wcsClassName) == 0)
		{
			LONG_PTR lUserData = GetWindowLongPtr(hWnd, GWL_USERDATA);

			if (lUserData != 100)
			{
				g_lOriWndProc = SetWindowLongPtr(hParent, GWLP_WNDPROC, (LONG_PTR)_WndProc);
				SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)100);				
			}
		}

		int i = 0;
	}


	if (uMsg == WM_NOTIFY)
	{
		HWND hParent = GetParent(hWnd);		

		LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;

		SetWindowLongPtr(hParent, GWL_USERDATA, (LONG_PTR)(lpOfNotify->lpOFN));

		if (lpOfNotify->hdr.code == CDN_SELCHANGE)
		{
			HWND hChild = GetDlgItem(GetParent(hWnd), cmb13);

			TCHAR strFilePath[MAX_PATH] = {0};
			SendMessage(GetParent(hWnd), (UINT)CDM_GETFILEPATH, (WPARAM)MAX_PATH, (LPARAM)strFilePath);

			LPTSTR lpTemp = StrRChr(strFilePath, NULL, _T('\\'));

			if (lpTemp == NULL)
			{
				SetWindowText(hChild, strFilePath);
			}
			else if (lpTemp != NULL && (StrCmp(lpTemp, _T("\\")) == 0))
			{
				SetWindowText(hChild, strFilePath);
			}
			else
			{
				SetWindowText(hChild, lpTemp + 1);
			}
		}
		else if (lpOfNotify->hdr.code == CDN_INITDONE)
		{
			HWND hChild = GetDlgItem(GetParent(hWnd), IDOK);
			SetWindowText(hChild, _T("存入安全管家"));			
		}
		else if (lpOfNotify->hdr.code == CDN_FILEOK)
		{
			int i = 0;
		}
	}


	return 0;
}

LRESULT  __stdcall _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (uMsg == WM_COMMAND)
	{
		if (wParam == IDOK)
		{
			TCHAR strFilePath[MAX_PATH] = {0};
			SendMessage(hwnd, (UINT)CDM_GETFILEPATH, (WPARAM)MAX_PATH, (LPARAM)strFilePath);
			
			if (PathFileExists(strFilePath))
			{
				OPENFILENAME *pOfn = (OPENFILENAME *)GetWindowLong(hwnd, GWL_USERDATA);
				lstrcpy(pOfn->lpstrFile, strFilePath);

				SendMessage(hwnd, WM_CLOSE, NULL, NULL);

				int i = 0;
			}
		}
	}

	return CallWindowProc((WNDPROC)g_lOriWndProc, hwnd, uMsg, wParam, lParam);
}

string_t CreateGUID()
{
	TCHAR strGUID[MAX_PATH] = {0};
	GUID guid;

	if (S_OK == ::CoCreateGuid(&guid))
	{
		_sntprintf(strGUID, _countof(strGUID), _T("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"), guid.Data1, guid.Data2,	guid.Data3,	guid.Data4[0], guid.Data4[1],guid.Data4[2], guid.Data4[3],guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}

	return string_t(strGUID);
}

std::wstring UTF8ToUnicode( const char* lpcszString )
{
	int len = strlen(lpcszString);
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset((void*)pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	wstring wstrReturn(pUnicode);
	delete [] pUnicode;
	return wstrReturn;
}

std::string UnicodeToUTF8( const wchar_t* lpwcszWString )
{
	char* pElementText;
	int iTextLen =WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete [] pElementText;
	return strReturn;
}

string_t GetCurrentModulePath()
{
	TCHAR strModuleName[MAX_PATH] = {0};
	GetModuleFileName(NULL, strModuleName, MAX_PATH);

	LPTSTR lpTemp = StrRChr(strModuleName, NULL, _T('\\'));

	if (lpTemp != NULL)
	{
		*lpTemp = _T('\0');
		return string_t(strModuleName);
	}
	else
	{
		return string_t(_T(""));
	}
}

void CheckFileExistAndRename( LPTSTR lpFileName )
{
	if (!PathFileExists(lpFileName))
	{
		return;
	}

	LPTSTR lpExtension = StrRChr(lpFileName, NULL, _T('.'));

	TCHAR strName[MAX_PATH] = {0};
	TCHAR strExtension[MAX_PATH] = {0};

	if (lpExtension != NULL)		//有后缀名
	{
		CopyMemory(strName, lpFileName, (lpExtension - lpFileName) * sizeof(TCHAR));
		CopyMemory(strExtension, lpExtension, (_tcslen(lpFileName) - (lpExtension - lpFileName)) * sizeof(TCHAR));
	}
	else
	{
		CopyMemory(strName, lpExtension + 1, _tcslen(lpFileName) * sizeof(TCHAR));
	}		

	LPTSTR leftBrackets = StrRChr(strName, NULL, _T('('));
	LPTSTR rightBracket = StrRChr(strName, NULL, _T(')'));

	if ((leftBrackets != NULL) && (rightBracket != NULL) && (leftBrackets < rightBracket))		//说明有( )
	{
		TCHAR strIndex[MAX_PATH] = {0};
		CopyMemory(strIndex, leftBrackets + 1, rightBracket - leftBrackets - 1);
		int index = _tstoi(strIndex);

		if (index > 0)		//说明括号中间是数字
		{
			TCHAR strTempName[MAX_PATH] = {0};
			CopyMemory(strTempName, strName, (leftBrackets - strName ) * sizeof(TCHAR));

			StringCchCat(strTempName, _countof(strTempName), strExtension);

			if (!PathFileExists(strTempName))
			{
				ZeroMemory(lpFileName, _tcslen(lpFileName) * sizeof(TCHAR));
				CopyMemory(lpFileName, strTempName, _tcslen(strTempName) * sizeof(TCHAR));
				return;
			}
			else
			{
				for (int i=1; i < 256; i++)
				{			
					ZeroMemory(strTempName, _countof(strTempName));
					CopyMemory(strTempName, strName, leftBrackets - strName);

					TCHAR temp[MAX_PATH] = {0};
					StringCchPrintf(temp, _countof(temp), _T("(%d)%s"), i, strExtension);
					StringCchCat(strTempName, _countof(strTempName), temp);
					if (!PathFileExists(strTempName))
					{
						ZeroMemory(lpFileName, _tcslen(lpFileName) * sizeof(TCHAR));
						CopyMemory(lpFileName, strTempName, _tcslen(strTempName) * sizeof(TCHAR));
						return;
					}						
				}
			}
		}
		else				//说明中间不是数字
		{
			TCHAR strTempName[MAX_PATH] = {0};				

			for (int i=1; i < 256; i++)
			{			
				ZeroMemory(strTempName, _countof(strTempName));
				CopyMemory(strTempName, strName, _tcslen(strName) * sizeof(TCHAR));

				TCHAR temp[MAX_PATH] = {0};
				StringCchPrintf(temp, _countof(temp), _T("(%d)%s"), i, strExtension);
				StringCchCat(strTempName, _countof(strTempName), temp);
				if (!PathFileExists(strTempName))
				{
					ZeroMemory(lpFileName, _tcslen(lpFileName) * sizeof(TCHAR));
					CopyMemory(lpFileName, strTempName, _tcslen(strTempName) * sizeof(TCHAR));
					return;
				}						
			}
		}
	}				//说明没（）
	else
	{
		TCHAR strTempName[MAX_PATH] = {0};				

		for (int i=1; i < 256; i++)
		{			
			ZeroMemory(strTempName, _countof(strTempName));
			CopyMemory(strTempName, strName, _tcslen(strName) * sizeof(TCHAR));

			TCHAR temp[MAX_PATH] = {0};
			StringCchPrintf(temp, _countof(temp), _T("(%d)%s"), i, strExtension);
			StringCchCat(strTempName, _countof(strTempName), temp);
			if (!PathFileExists(strTempName))
			{
				ZeroMemory(lpFileName, _tcslen(lpFileName) * sizeof(TCHAR));
				CopyMemory(lpFileName, strTempName, _tcslen(strTempName) * sizeof(TCHAR));
				return;
			}						
		}
	}
}

BOOL ModifyFileSizeFormat( LPTSTR lpText, ULONG ulLen, UINT Size )
{
	BOOL bRet = FALSE;

	DOUBLE NewSize = 0;


	do 
	{
		if (!lpText)
			break;

		NewSize = Size;

		if (NewSize < 1024)
		{
			// b
			StringCbPrintf(lpText, ulLen, _T("%2d B"), NewSize);
			bRet = TRUE;
			break;
		}

		NewSize = NewSize / 1024;
		if (NewSize < 1024)
		{
			// kb
			StringCbPrintf(lpText, ulLen, _T("%2.1f KB"), NewSize);
			bRet = TRUE;
			break;
		}

		NewSize = NewSize / 1024;
		if (NewSize < 1024)
		{
			// mb
			StringCbPrintf(lpText, ulLen, _T("%2.1f MB"), NewSize);
			bRet = TRUE;
			break;
		}

		NewSize = NewSize / 1024;
		if (NewSize < 1024)
		{
			// gb
			StringCbPrintf(lpText, ulLen, _T("%2.1f GB"), NewSize);
			bRet = TRUE;
			break;
		}

		NewSize = NewSize / 1024;
		if (NewSize < 1024)
		{
			// tb
			StringCbPrintf(lpText, ulLen, _T("%2.1f TB"), NewSize);
			bRet = TRUE;
			break;
		}
	} while (FALSE);

	return bRet;
}

void SplitString( string_t str, string_t pattern, vector<string_t> &vecStrs )
{
	string_t::size_type pos;

	str += pattern;
	
	int size = str.size();

	for (int i=0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos <size)
		{
			string_t temp = str.substr(i, pos - i);
			i = pos + pattern.size() - 1;
		}
	}
}

std::string String_tToString( string_t &str )
{
	string strRet;

#ifdef UNICODE
	strRet = Unicode2Ansi(str);
#else
	strRet = str;
#endif

	return strRet;
}

std::wstring String_tToUnicode( string_t &str )
{
	wstring strRet;

#ifdef UNICODE
	strRet = str;
#else
	strRet = ANSI2Unicode(str);
#endif

	return strRet;
}

string_t ANSIToString_t( string &str )
{
	string_t strRet;

#ifdef UNICODE
	strRet = ANSI2Unicode(str);
#else
	strRet = str;
#endif

	return strRet;
}

string_t UnicodeToString_t( wstring &str )
{
	string_t strRet;

#ifdef UNICODE
	strRet = str;
#else
	strRet = Unicode2Ansi(str);
#endif

	return strRet;
}

std::string String_tToUtf8( string_t str )
{
	wstring temp = String_tToUnicode(str);

	return string(UnicodeToUTF8(temp.c_str()));
}

string_t Utf8ToString_t( string str )
{
	wstring temp = UTF8ToUnicode(str.c_str());

	return string_t(UnicodeToString_t(temp));
}

INT64 SystemTimeToINT64(const SYSTEMTIME &systime )
{
	struct tm gm = {systime.wSecond, systime.wMinute, systime.wHour, systime.wDay, systime.wMonth-1, systime.wYear-1900, systime.wDayOfWeek, 0, 0};

	return _mktime64(&gm);
}

string_t INT64TimeToString( INT64 nTime )
{
	__time64_t t = nTime;

	tm temptm = *localtime(&t);

	SYSTEMTIME sysModifyTime = {1900 + temptm.tm_year, 
		1 + temptm.tm_mon, 
		temptm.tm_wday, 
		temptm.tm_mday, 
		temptm.tm_hour, 
		temptm.tm_min, 
		temptm.tm_sec, 
		0};
	
	TCHAR strModifyTime[MAX_PATH] = {0};	
	StringCchPrintf(strModifyTime, _countof(strModifyTime), _T("%d/%02d/%02d %02d:%02d:%02d"), sysModifyTime.wYear, sysModifyTime.wMonth, sysModifyTime.wDay, sysModifyTime.wHour, sysModifyTime.wMinute, sysModifyTime.wSecond);

	return strModifyTime;
}

string_t GetMaxFreeSpaceDisk()
{
	string_t strRet;

	TCHAR szLogicalDriveStrings[1024] = {0};
	TCHAR *pDrive = NULL;

	UINT  nDiskType = 0;
	ULARGE_INTEGER TempUln;
	TempUln.QuadPart = 0;

	ULARGE_INTEGER ulnFreeBytesAvailable;
	ULARGE_INTEGER ulnTotalNumberofBytes;
	ULARGE_INTEGER ulnTotalNumberofFreeBytes;

	GetLogicalDriveStrings(1024 - 1, szLogicalDriveStrings);
	pDrive = szLogicalDriveStrings;

	do 
	{
		nDiskType = GetDriveType(pDrive);

		GetDiskFreeSpaceEx(pDrive, &ulnFreeBytesAvailable, &ulnTotalNumberofBytes, &ulnTotalNumberofFreeBytes);

		if ((nDiskType == DRIVE_FIXED) && (ulnFreeBytesAvailable.QuadPart > TempUln.QuadPart ) && (!IsUsbDev(pDrive)))
		{
			strRet = pDrive;
			TempUln = ulnFreeBytesAvailable;
		}

		pDrive += (_tcslen(pDrive) + 1);
		int i = 0;
	} while (*pDrive != _T('\x00'));


	return strRet;
}

BOOL IsExtDev(LPTSTR lpFileName)
{
	BOOL			bResult		= TRUE;
	TCHAR			Volume[5]	= {0};


	do 
	{
		if(!lpFileName)
		{			
			break;
		}

		if(lpFileName[0] == _T('\\') || lpFileName[0] == _T('/'))
			break;

		if(IsUsbDev(lpFileName))
			break;

		CopyMemory(Volume, lpFileName, 3 * sizeof(TCHAR));
		if(GetDriveType(Volume) != DRIVE_FIXED)
			break;

		bResult = FALSE;

	} while (FALSE);

	return bResult;
}

BOOL IsUsbDev(LPTSTR lpFileName)
{
	BOOL						bRet					= FALSE;


	TCHAR						FileSymName[MAX_PATH * 2]	= {0};
	TCHAR						VolumeSymName[10]		= {0};
	HANDLE						hVolume					= INVALID_HANDLE_VALUE;
	STORAGE_PROPERTY_QUERY		Query;
	STORAGE_DEVICE_DESCRIPTOR	devDescriptor			= {0};
	BOOL						bResult					= FALSE;
	DWORD						dwBytesReturned			= 0;


	__try
	{
		if (!lpFileName)
			__leave;

		wcscat_s(FileSymName, MAX_PATH * 2, _T("\\\\.\\"));
		wcscat_s(FileSymName, MAX_PATH * 2, lpFileName);

		CopyMemory(VolumeSymName, FileSymName, 6 * sizeof(TCHAR));

		hVolume = CreateFile(
			VolumeSymName,
			SYNCHRONIZE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);
		if (hVolume == INVALID_HANDLE_VALUE)
			__leave;

		Query.PropertyId = StorageDeviceProperty;
		Query.QueryType = PropertyStandardQuery;

		devDescriptor.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

		bResult = DeviceIoControl(
			hVolume,
			IOCTL_STORAGE_QUERY_PROPERTY,
			&Query,
			sizeof(STORAGE_PROPERTY_QUERY),
			&devDescriptor,
			devDescriptor.Size,
			&dwBytesReturned,
			NULL
			);
		if (!bResult)
			__leave;

		if (devDescriptor.BusType == BusTypeUsb)
			bRet = TRUE;
	}
	__finally
	{
		if (hVolume != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hVolume);
			hVolume = INVALID_HANDLE_VALUE;
		}
	}

	return bRet;
}

RECT CalTextRect( LPCTSTR lpText, HFONT hFont)
{
	RECT rect = {0};
	HDC hDc = ::CreateCompatibleDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDc, hFont);
	::DrawText(hDc, lpText, -1, &rect, DT_CALCRECT);
	::SelectObject(hDc, hOldFont);
	::DeleteDC(hDc);	

	return rect;
}




