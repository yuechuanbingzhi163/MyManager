// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#if 0
#define  UI_USE_ZIP
#endif

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息

#include <afxwin.h>

// Windows 头文件:
#include <windows.h>

#include <atlimage.h>



// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"


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

#include "..\..\Depend\\DuiLib\UIlib.h"
using namespace DuiLib;

#ifndef _DEBUG
#pragma comment(lib, "..\\..\\Lib\\release\\DuiLib.lib")
#else
#pragma comment(lib, "..\\..\\Lib\\debug\\DuiLib_d.lib")
#endif


#define GWL_WNDPROC         (-4)
#define GWLP_WNDPROC		(-4)
#define GWL_USERDATA        (-21)

#define WM_FILE_PROCESS												(WM_USER + 0X820)
#define WM_THREAD_QUIT												(WM_USER + 0X821)
#define  WM_UPDATE_FILE_LIST_UI									(WM_USER + 0X822)
#define  WM_UPDATA_FILE_LIST_PROGRESS_UI			(WM_USER + 0X823)
#define  WM_NIDTIP															(WM_USER + 0X824)
#define  WM_UPDATE_PROGRESS									(WM_USER + 0X825)
#define  WM_POP_MENU													(WM_USER + 0X826)

#define  WM_COMMAND_UPLOAD								(WM_USER + 0X827)
#define	WM_COMMAND_NEW_FOLDER						(WM_USER + 0X828)
#define  WM_COMMAND_REFRESH								(WM_USER + 0X829)
#define  WM_COMMAND_OPEN										(WM_USER + 0X830)
#define  WM_COMMAND_COPY										(WM_USER + 0X831)
#define  WM_COMMAND_MOVE										(WM_USER + 0X832)
#define  WM_COMMAND_EXPORT									(WM_USER + 0X833)
#define  WM_COMMAND_RENAME								(WM_USER + 0X834)
#define  WM_COMMAND_DELETE									(WM_USER + 0X835)
#define  WM_COMMAND_ATTRIBUTE							(WM_USER + 0X836)

#define  WM_LOAD_NEW_FILE_HANDLE						(WM_USER + 0X837)




#define DUI_MSGTYPE_FILE_LIST_DBCLICK			_T("self_def_list_item_dbclick")

#define EVERY_FILE_CHIPS_NUMBER										(3)
#define  FILE_SPLIT_OR_MERGER_PROGERSS_VALUE			(0.6)
#define  FILE_GET_FRAGS_PROGERSS_VALUE						(0.4)

#define  FILE_HANDLE_STRING_END						_T("OPERATOR_END")
