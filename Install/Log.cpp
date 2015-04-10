#include "StdAfx.h"
#include "Log.h"


namespace MyLog
{
	HANDLE CLog::m_hLogFile = NULL;

	CRITICAL_SECTION CLog::m_CriticalSection;


	CLog::CLog(void)
	{

	}


	CLog::~CLog(void)
	{

	}

	bool CLog::Init()
	{
		bool bRet = false;	

		do 
		{
			TCHAR strModulePath[MAX_PATH] = {0};
			GetModuleFileName(NULL, strModulePath, sizeof(strModulePath));

			LPTSTR lpPath = StrRChr(strModulePath, NULL, _T('\\'));
			if (lpPath == NULL)
			{
				break;
			}

			TCHAR strModuleName[MAX_PATH] = {0};
			StringCchPrintf(strModuleName, _countof(strModuleName), _T("%s"), lpPath + 1);
			LPTSTR lpModuleName = StrRChr(strModuleName, NULL, _T('.'));
			if (lpModuleName != NULL)
			{
				*lpModuleName = _T('\0');
			}

			*(lpPath+1) = _T('\0');
			StringCchCat(strModulePath, _countof(strModulePath), _T("log"));
			if (!PathFileExists(strModulePath))
			{
				SHCreateDirectory(NULL, strModulePath);
			}

			TCHAR strLogFile[MAX_PATH] = {0};
			StringCchPrintf(strLogFile, _countof(strLogFile), _T("%s\\%s.log"), strModulePath, strModuleName);

			m_hLogFile = CreateFile(strLogFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (m_hLogFile == INVALID_HANDLE_VALUE)
			{
				break;
			}
	#ifdef UNICODE
			WORD unicode = 0xFEFF;
			DWORD dwWriteLength = 0;
			WriteFile(m_hLogFile, &unicode, sizeof(unicode), &dwWriteLength, NULL);
	#endif

			bRet = true;

			InitializeCriticalSection(&m_CriticalSection);

		} while (false);

		return bRet;
	}

	bool CLog::UnInit()
	{
		bool bRet = false;

		//貌似无用
		if (m_hLogFile != NULL || m_hLogFile != INVALID_HANDLE_VALUE)
		{		
			FlushFileBuffers(m_hLogFile);
			CloseHandle(m_hLogFile);
			m_hLogFile = NULL;
			DeleteCriticalSection(&m_CriticalSection);
			bRet = true;
		}

		return bRet;
	}

	void CLog::WriteLog(LPTSTR Fmt, ... )
	{

		LARGE_INTEGER	FileSize						= {0};
		TCHAR			strLog[1024]				= {0};
		TCHAR			Tmp[1024]					= {0};
		DWORD	dwWriteLenght = 0;

		CHAR			Format[1024]	= {0};
		CHAR			TmpBuf[1024]	= {0};
		TCHAR		Output[1024]	= {0};
		ULONG			ulExist			= 0;

			EnterCriticalSection(&m_CriticalSection);

			if (m_hLogFile == NULL || m_hLogFile == INVALID_HANDLE_VALUE)
			{
				LeaveCriticalSection(&m_CriticalSection);
				return;
			}

				va_list Args;

				va_start(Args, Fmt);

				FileSize.LowPart = GetFileSize(m_hLogFile, NULL);
				if (SetFilePointer(m_hLogFile, FileSize.LowPart, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				{
					LeaveCriticalSection(&m_CriticalSection);
					return;
				}			

				SYSTEMTIME sysTime;
				ZeroMemory(&sysTime, sizeof(SYSTEMTIME));
				GetLocalTime(&sysTime);

				StringCchPrintf(strLog, _countof(strLog), _T("[%04d/%02d/%02d %02d:%02d:%02d] "), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

				ulExist = _tcslen(Output);

				if (Fmt != NULL)
				{
					ZeroMemory(Tmp, sizeof(Tmp));
					_vsntprintf_s(Tmp, sizeof(Tmp) - 1, Fmt, Args);
				}
				else
				{
					StringCchPrintf(Tmp, _countof(Tmp), _T("参数无效"));
				}

				StringCchCat(strLog, _countof(strLog), Tmp);
				StringCchCat(strLog, _countof(strLog), _T("\r\n"));

				WriteFile(m_hLogFile, strLog, _tcslen(strLog) * sizeof(TCHAR), &dwWriteLenght, NULL);

				va_end(Args);

			LeaveCriticalSection(&m_CriticalSection);
	}

}






