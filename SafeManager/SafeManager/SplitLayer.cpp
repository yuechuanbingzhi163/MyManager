/* 
* Copyright (c) 2012,�����������¿Ƽ����޹�˾ 
* All rights reserved. 
*  
* �ļ����ƣ�SplitLayer.cpp
* �ļ���ʶ�� 
* ժ    Ҫ����������
*  
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�2014��8��11�� 
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
		// ��ȡ�ļ�·��
		::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s*.dll"), lpPath);

		//������Ӧ�ļ��У�����Dll
		hFind = ::FindFirstFile(FilePath, &wfd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("���������㷨�ļ� %S ʧ�� (%d)"), lpPath, ::GetLastError());
			LOG(_T("���������㷨�ļ�ʧ��!"));
			break;
		}

		do 
		{
			//�ж��Ƿ�Ϊ��ǰĿ¼��Ŀ¼
			if (!_tcscmp(wfd.cFileName, _T(".")) || 
				!_tcscmp(wfd.cFileName, _T("..")) ||
				wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}

			//����Dll·��
			::ZeroMemory(FilePath, MAX_PATH * sizeof(TCHAR));
			::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s%s"), lpPath, wfd.cFileName);

			//������Ӧ��DLL�����䵼�����ݼ��뵽������
			Insert(FilePath);

		} while (::FindNextFile(hFind, &wfd));

		//û��ƥ���ļ����ҵ�
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
				//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("���������㷨�ļ� %S ʧ�� (%d)"), lpFileName, ::GetLastError());
				LOG(_T("�����������㷨�ļ�ʧ��!"));
			}
			break;
		}

		//��ȡ������������ַ
		//ʹ�ñ�ŵ���������֤Dll��Ч��
		fpGetIndex = (GETINDEX)GetProcAddress(hModule, "GetIndex");					
		fpSplit = (SPLIT)GetProcAddress(hModule, "Split");			
		fpMerge = (MERGE)GetProcAddress(hModule, "Merge");

		if (!fpGetIndex || !fpSplit || !fpMerge)
		{
			break;
		}

		//��ѯ�������Ƿ���ڽڵ㣬�����������
		pModInfoAddr = Query(fpGetIndex());
		if (pModInfoAddr)
		{
			FreeLibrary(hModule);
			bResult = TRUE;
			break;
		}

		//�����ڴ�飬��Žڵ�
		pModInfoAddr = (LPSPLIT_MOD_INFO)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, sizeof(SPLIT_MOD_INFO));
		if(!pModInfoAddr)
		{
			//LOG(ErrorLevel, _SPLIT_LAYER_MOD_, _T("�����ڴ�ʧ�� (%d)"), ::GetLastError());
			LOG(_T("�����ڴ�ʧ��!"));
			break;
		}

		//��ʼ��
		pModInfoAddr->Index = fpGetIndex();
		pModInfoAddr->fpSplit = fpSplit;
		pModInfoAddr->fpMerge = fpMerge;
		pModInfoAddr->hMod = hModule;

		//������뵽������
		::WaitForSingleObject(ms_hSynObject, INFINITE);

		pModInfoAddr->NextNode = ms_ListHead;
		ms_ListHead = pModInfoAddr;

		::SetEvent(ms_hSynObject);

		//���в����ɹ�
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
