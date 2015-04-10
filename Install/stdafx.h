// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#if 1
#define  UI_USE_ZIP
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

#include "Resource.h"

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <Shlwapi.h>
#include <Strsafe.h>
#include <shlobj.h>

#pragma comment(lib, "shlwapi.lib")

#include "Log.h"
using namespace MyLog;

#ifdef UNICODE
#define string_t	wstring
#else
#define string_t	string
#endif

#include "..\Depend\\DuiLib\UIlib.h"
using namespace DuiLib;

#ifndef _DEBUG
#pragma comment(lib, "..\\Lib\\release\\DuiLib.lib")
#else
#pragma comment(lib, "..\\Lib\\debug\\DuiLib_d.lib")
#endif

#define WM_EXIT_APP (WM_USER + 0x12345)
