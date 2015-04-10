// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#if 1
#define  UI_USE_ZIP
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

#include "Resource.h"

// C 运行时头文件
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
