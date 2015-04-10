/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：FileOperation.h
* 文件标识： 
* 摘    要：文件处理接口
*  
* 当前版本：1.0
* 作    者：高永吉
* 完成日期：2013年7月1日 
* 修改记录: 
*			2013.07.08		陈炫	简化代码
*
*/

#include "stdafx.h"
#include "CryptoLayer.h"

PCRYPTO_MOD_INFO	CCryptoLayer::ms_ListHead				= NULL;
HANDLE				CCryptoLayer::ms_hSynObject				= ::CreateEvent(NULL, FALSE, TRUE, NULL);
HANDLE				CCryptoLayer::ms_Heap					= ::HeapCreate(0, 0x1000, 0);
TCHAR				CCryptoLayer::ms_CryptoPath[MAX_PATH]	= {0};

/*
*
*函数简介：查询模块导出信息
*
*参数介绍：
*		lIndex		算法索引号
*                           
*返回值：查询成功，返回模块相应信息；否则返回NULL
*
*/
PCRYPTO_MOD_INFO CCryptoLayer::Query(IN LONG lIndex)
{
	PCRYPTO_MOD_INFO pNodeAddr = ms_ListHead;


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

/*
*
*函数简介：删除模块对应节点
*
*参数介绍：
*		lIndex		算法索引号
*                           
*返回值：删除成功，返回TRUE；否则返回FALSE
*
*/
BOOL CCryptoLayer::Delete(IN LONG lIndex)
{
	PCRYPTO_MOD_INFO      pNodeAddr     = ms_ListHead;
	PCRYPTO_MOD_INFO      pPreviousNode = NULL;
	BOOL                  bResult       = FALSE;    


	::WaitForSingleObject(ms_hSynObject, INFINITE);

	while(pNodeAddr)
	{
		if(pNodeAddr->Index == lIndex)
		{
			bResult = TRUE;
			break;
		}

		pPreviousNode = pNodeAddr;
		pNodeAddr = pNodeAddr->NextNode;
	}

	//找到相应节点
	if (bResult)
	{
		if(pPreviousNode)
			pPreviousNode->NextNode = pNodeAddr->NextNode;
		else
			ms_ListHead = pNodeAddr->NextNode;

		::FreeLibrary(pNodeAddr->hMod);
		::HeapFree(ms_Heap, 0, pNodeAddr);
	}

	::SetEvent(ms_hSynObject);

	return bResult;
}

/*
*
*函数简介：加载DLL，并将导出信息填入节点中
*
*参数介绍：
*		lpFileName		模块文件
*                           
*返回值：执行成功，返回TRUE；否则返回FALSE
*
*/
BOOL CCryptoLayer::Insert(IN LPTSTR lpFileName)
{
	BOOL                  bResult			= FALSE;
	HMODULE               hModule			= NULL;
	PCRYPTO_MOD_INFO      pModInfoAddr		= NULL;

	GETINDEX			  lpfnGetIndex		= NULL;
	PDECRYPT               lpfpDecrypt			= NULL;
	PENCRYPT               lpfpEncrypt			= NULL;


	do 
	{
		hModule = ::LoadLibrary(lpFileName);
		if (!hModule)
		{
			if (GetLastError() != 193)
			{
				//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("加载加密算法文件 %S 失败 (%d)"), lpFileName, ::GetLastError());
				LOG(_T("插入加密算法文件失败!"));
			}
				

			break;
		}

		//获取导出各函数地址
		//使用编号导出进行验证Dll有效性
		lpfnGetIndex = (GETINDEX)GetProcAddress(hModule, "GetIndex");	
		lpfpDecrypt = (PDECRYPT)GetProcAddress(hModule, "Decrypt");
		lpfpEncrypt = (PENCRYPT)GetProcAddress(hModule, "Encrypt");

		if (!lpfnGetIndex || !lpfpDecrypt || !lpfpEncrypt)
		{
			break;
		}

		//查询链表中是否存在节点，存在则不用添加
		pModInfoAddr = Query(lpfnGetIndex());
		if (pModInfoAddr)
		{
			FreeLibrary(hModule);
			bResult = TRUE;
			break;
		}

		//申请内存块，存放节点
		pModInfoAddr = (PCRYPTO_MOD_INFO)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, sizeof(CRYPTO_MOD_INFO));
		if(!pModInfoAddr)
		{
			//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("申请内存失败 (%d)"), ::GetLastError());
			break;
		}

		//初始化
		pModInfoAddr->Index = lpfnGetIndex();
		pModInfoAddr->fpDecrypt = lpfpDecrypt;
		pModInfoAddr->fpEncrypt = lpfpEncrypt;
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

/*
*
*函数简介：销毁算法层
*
*参数介绍：无
*                           
*返回值：N/A
*
*/
VOID CCryptoLayer::Destory()
{
	PCRYPTO_MOD_INFO	pNode				= ms_ListHead;
	PCRYPTO_MOD_INFO	pTmpNode			= NULL;


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

/*
*
*函数简介：生成算法层
*
*参数介绍：
*	PathName		被扫描路径
*                           
*返回值：执行成功，返回TRUE；否则返回FALSE
*
*/
BOOL CCryptoLayer::Load(IN LPTSTR lpPath)
{
	HANDLE					hFind				= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA			wfd					= {0};
	BOOL					bResult				= FALSE;
	TCHAR                   FilePath[MAX_PATH]	= {0};
	HANDLE					hProcess			= INVALID_HANDLE_VALUE;


	do 
	{
		// 获取算法文件路径
		::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s*.dll"), lpPath);


		//遍历相应文件夹，查找Dll
		hFind = ::FindFirstFile(FilePath, &wfd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("遍历加密算法文件 %S 失败 (%d)"), lpPath, ::GetLastError());
			LOG(_T("遍历加密算法文件失败!"));
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