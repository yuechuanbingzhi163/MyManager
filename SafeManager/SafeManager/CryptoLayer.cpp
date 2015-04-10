/* 
* Copyright (c) 2012,�����������¿Ƽ����޹�˾ 
* All rights reserved. 
*  
* �ļ����ƣ�FileOperation.h
* �ļ���ʶ�� 
* ժ    Ҫ���ļ�����ӿ�
*  
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2013��7��1�� 
* �޸ļ�¼: 
*			2013.07.08		����	�򻯴���
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
*������飺��ѯģ�鵼����Ϣ
*
*�������ܣ�
*		lIndex		�㷨������
*                           
*����ֵ����ѯ�ɹ�������ģ����Ӧ��Ϣ�����򷵻�NULL
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
*������飺ɾ��ģ���Ӧ�ڵ�
*
*�������ܣ�
*		lIndex		�㷨������
*                           
*����ֵ��ɾ���ɹ�������TRUE�����򷵻�FALSE
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

	//�ҵ���Ӧ�ڵ�
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
*������飺����DLL������������Ϣ����ڵ���
*
*�������ܣ�
*		lpFileName		ģ���ļ�
*                           
*����ֵ��ִ�гɹ�������TRUE�����򷵻�FALSE
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
				//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("���ؼ����㷨�ļ� %S ʧ�� (%d)"), lpFileName, ::GetLastError());
				LOG(_T("��������㷨�ļ�ʧ��!"));
			}
				

			break;
		}

		//��ȡ������������ַ
		//ʹ�ñ�ŵ���������֤Dll��Ч��
		lpfnGetIndex = (GETINDEX)GetProcAddress(hModule, "GetIndex");	
		lpfpDecrypt = (PDECRYPT)GetProcAddress(hModule, "Decrypt");
		lpfpEncrypt = (PENCRYPT)GetProcAddress(hModule, "Encrypt");

		if (!lpfnGetIndex || !lpfpDecrypt || !lpfpEncrypt)
		{
			break;
		}

		//��ѯ�������Ƿ���ڽڵ㣬�����������
		pModInfoAddr = Query(lpfnGetIndex());
		if (pModInfoAddr)
		{
			FreeLibrary(hModule);
			bResult = TRUE;
			break;
		}

		//�����ڴ�飬��Žڵ�
		pModInfoAddr = (PCRYPTO_MOD_INFO)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, sizeof(CRYPTO_MOD_INFO));
		if(!pModInfoAddr)
		{
			//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("�����ڴ�ʧ�� (%d)"), ::GetLastError());
			break;
		}

		//��ʼ��
		pModInfoAddr->Index = lpfnGetIndex();
		pModInfoAddr->fpDecrypt = lpfpDecrypt;
		pModInfoAddr->fpEncrypt = lpfpEncrypt;
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

/*
*
*������飺�����㷨��
*
*�������ܣ���
*                           
*����ֵ��N/A
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
*������飺�����㷨��
*
*�������ܣ�
*	PathName		��ɨ��·��
*                           
*����ֵ��ִ�гɹ�������TRUE�����򷵻�FALSE
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
		// ��ȡ�㷨�ļ�·��
		::StringCbPrintf(FilePath, MAX_PATH * sizeof(TCHAR), _T("%s*.dll"), lpPath);


		//������Ӧ�ļ��У�����Dll
		hFind = ::FindFirstFile(FilePath, &wfd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			//LOG(ErrorLevel, _CRYPTO_LAYER_MOD_, _T("���������㷨�ļ� %S ʧ�� (%d)"), lpPath, ::GetLastError());
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