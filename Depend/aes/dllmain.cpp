/*
* CopyRight (C) 2013 �����������¿Ƽ����޹�˾
* All rights reserved
*
* �ļ����ƣ�dllmain.cpp
* ժҪ��AES�㷨
* �汾��N/A
* ���ߣ�����
* ���ʱ�䣺2013.05.18
* �޸ļ�¼��
* ʱ��    			�޸���    	�޸�����
*
*/

#include "stdafx.h"
#include "AES.h"


/*
* DLLģ���ʼ��
*
* ������
*
* ����ֵ��N/A
*
* ��ע��N/A
*/
BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/*
 * �㷨����
 *
 * ������
 *
 * ����ֵ����������ϵͳ�ڲ��������������ڵ���������
 *
 * ��ע�����ݷ�������ʼ����ֵ����
 */
LONG GetIndex()
{
	return 1;
}

/*
 * ��ȡ�㷨�������С��Ԫ
 *
 * ������
 *
 * ����ֵ��������С��Ԫ
 *
 * ��ע��N/A
 */
ULONG GetAlign(ULONG ulKeyLen)
{
	return ulKeyLen;
}

/*
* ���������Կ
*
* ������
*		lpKey				������Կ�Ļ�����
*		ulKeyLen			��Կ����
*
* ����ֵ���Ƿ�ɹ�
*
* ��ע��N/A
*/
BOOL GenKey(LPVOID lpKey, ULONG ulKeyLen)
{
	BOOL		bRet = FALSE;
	ULONG		ulCount = ulKeyLen / 2;


	do 
	{
		if(ulKeyLen != 16)
			break;

		for(DWORD i = 0; i < ulCount; i++)
		{
			*((WORD*)lpKey + i) = rand();
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL Decrypt(LPVOID lpIn, ULONG ulIn, LPVOID lpOut, ULONG* lpRetOutLen, LPVOID lpKey, ULONG ulKey)
{
	BOOL			bRet				= FALSE;
	CHAR			Chain[32]			= {0};
	CRijndael		AES;


	do 
	{
		if(!lpIn || !ulIn || !lpOut || !lpRetOutLen)
			break;

		// Ҫ��������
		if(ulIn % ulKey || ulKey % 16)
			break;

		// ��ʼ����Կ
		AES.MakeKey((PCHAR)lpKey, Chain, ulKey, ulKey);

		AES.DecryptData((const char*)lpIn, (char*)lpOut, ulIn);
		*lpRetOutLen = ulIn;

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}

BOOL Encrypt(LPVOID lpIn, ULONG ulIn, LPVOID lpOut, ULONG* lpRetOutLen, LPVOID lpKey, ULONG ulKey)
{
	BOOL			bRet				= FALSE;
	CHAR			Chain[32]			= {0};
	CRijndael		AES;


	do 
	{
		if(!lpIn || !ulIn || !lpOut || !lpRetOutLen)
			break;

		// Ҫ��������
		if(ulIn % ulKey || ulKey % 16)
			break;

		// ��ʼ����Կ
		AES.MakeKey((PCHAR)lpKey, Chain, ulKey, ulKey);

		AES.EncryptData((const char*)lpIn, (char*)lpOut, ulIn);
		*lpRetOutLen = ulIn;

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}