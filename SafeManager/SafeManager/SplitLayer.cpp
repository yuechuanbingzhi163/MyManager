/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：SplitLayer.cpp
* 文件标识： 
* 摘    要：破碎管理层
*  
* 当前版本：1.0
* 作    者：陈炫
* 完成日期：2014年8月11日 
*/

#include "stdafx.h"
#include "SplitLayer.h"

LPSPLIT_MOD_INFO	CSplitLayer::ms_ListHead				= NULL;
HANDLE				CSplitLayer::ms_hSynObject				= ::CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE				CSplitLayer::ms_Heap					= ::HeapCreate(0, 0x1000, 0);

BOOL CSplitLayer::Load(LPTSTR lpPath)
{
	HANDLE					hFind				= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA			wfd					= {0};
	BOOL					bResult				= FALSE;
	TCHAR                   FilePath[MAX_PATH]	= {0};
	HANDLE					hProcess			= INVALID_HANDLE_VALUE;


	do 
	{
		// 获取文件路径
		::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s*.dll"), lpPath);

		//遍历相应文件夹，查找Dll
		hFind = ::FindFirstFile(FilePath, &wfd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("遍历破碎算法文件 %S 失败 (%d)"), lpPath, ::GetLastError());
			LOG(_T("遍历破碎算法文件失败!"));
			break;
		}

		do 
		{
			//判断是否为当前目录或父目录
			if (!_tcscmp(wfd.cFileName, _T(".")) || 
				!_tcscmp(wfd.cFileName, _T("..")) ||
				wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}

			//生成Dll路径
			::ZeroMemory(FilePath, MAX_PATH * sizeof(TCHAR));
			::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s%s"), lpPath, wfd.cFileName);

			//加载相应的DLL，将其导出数据加入到链表中
			Insert(FilePath);

		} while (::FindNextFile(hFind, &wfd));

		//没有匹配文件被找到
		if (ERROR_NO_MORE_FILES == GetLastError())
			bResult = TRUE;

	} while (FALSE);

	if (hFind != INVALID_HANDLE_VALUE)
		::FindClose(hFind);

	return bResult;
}

BOOL CSplitLayer::Insert(IN LPTSTR lpFileName)
{
	BOOL					bResult			= FALSE;
	HMODULE					hModule			= NULL;
	LPSPLIT_MOD_INFO		pModInfoAddr	= NULL;
	
	GETINDEX				fpGetIndex;
	SPLIT					fpSplit;
	MERGE					fpMerge;

	do 
	{
		hModule = ::LoadLibrary(lpFileName);
		if (!hModule)
		{
			if (GetLastError() != 193)
			{
				//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("加载破碎算法文件 %S 失败 (%d)"), lpFileName, ::GetLastError());
				LOG(_T("插入破入碎算法文件失败!"));
			}
			break;
		}

		//获取导出各函数地址
		//使用编号导出进行验证Dll有效性
		fpGetIndex = (GETINDEX)GetProcAddress(hModule, "GetIndex");					
		fpSplit = (SPLIT)GetProcAddress(hModule, "Split");			
		fpMerge = (MERGE)GetProcAddress(hModule, "Merge");

		if (!fpGetIndex || !fpSplit || !fpMerge)
		{
			break;
		}

		//查询链表中是否存在节点，存在则不用添加
		pModInfoAddr = Query(fpGetIndex());
		if (pModInfoAddr)
		{
			FreeLibrary(hModule);
			bResult = TRUE;
			break;
		}

		//申请内存块，存放节点
		pModInfoAddr = (LPSPLIT_MOD_INFO)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, sizeof(SPLIT_MOD_INFO));
		if(!pModInfoAddr)
		{
			//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("申请内存失败 (%d)"), ::GetLastError());
			LOG(_T("申请内存失败!"));
			break;
		}

		//初始化
		pModInfoAddr->Index = fpGetIndex();
		pModInfoAddr->fpSplit = fpSplit;
		pModInfoAddr->fpMerge = fpMerge;
		pModInfoAddr->hMod = hModule;

		//将其插入到链表中
		::WaitForSingleObject(ms_hSynObject, INFINITE);

		pModInfoAddr->NextNode = ms_ListHead;
		ms_ListHead = pModInfoAddr;

		::SetEvent(ms_hSynObject);

		//所有操作成功
		bResult = TRUE;

	} while (FALSE);


	if(!bResult && hModule)
		::FreeLibrary(hModule);

	return bResult;
}

LPSPLIT_MOD_INFO CSplitLayer::Query(IN LONG lIndex)
{
	LPSPLIT_MOD_INFO pNodeAddr = ms_ListHead;


	::WaitForSingleObject(ms_hSynObject, INFINITE);

	while(pNodeAddr)
	{
		if(pNodeAddr->Index == lIndex)
			break;

		pNodeAddr = pNodeAddr->NextNode;
	}

	::SetEvent(ms_hSynObject);	

	return pNodeAddr;
}

VOID CSplitLayer::Destory()
{
	LPSPLIT_MOD_INFO	pNode				= ms_ListHead;
	LPSPLIT_MOD_INFO	pTmpNode			= NULL;


	::WaitForSingleObject(ms_hSynObject, INFINITE);

	while (pNode)
	{
		pTmpNode = pNode;
		pNode = pNode->NextNode;

		::FreeLibrary(pTmpNode->hMod);
		::HeapFree(ms_Heap, 0, pTmpNode);
	}

	ms_ListHead = NULL;
	::SetEvent(ms_hSynObject);

	return;
}
