/*
* CopyRight (C) 2013 北京凯锐立德科技有限公司
* All rights reserved
*
* 文件名称：dllmain.cpp
* 摘要：AES算法
* 版本：N/A
* 作者：陈炫
* 完成时间：2013.05.18
* 修改记录：
* 时间    			修改人    	修改内容
*
*/

#include "stdafx.h"
#include "AES.h"


/*
* DLL模块初始化
*
* 参数：
*
* 返回值：N/A
*
* 备注：N/A
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
 * 算法索引
 *
 * 参数：
 *
 * 返回值：负数属于系统内部索引，正数属于第三方索引
 *
 * 备注：根据服务器初始化数值而定
 */
LONG GetIndex()
{
	return 1;
}

/*
 * 获取算法处理的最小单元
 *
 * 参数：
 *
 * 返回值：返回最小单元
 *
 * 备注：N/A
 */
ULONG GetAlign(ULONG ulKeyLen)
{
	return ulKeyLen;
}

/*
* 生成随机密钥
*
* 参数：
*		lpKey				返回密钥的缓冲区
*		ulKeyLen			密钥长度
*
* 返回值：是否成功
*
* 备注：N/A
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

		// 要求必须对齐
		if(ulIn % ulKey || ulKey % 16)
			break;

		// 初始化密钥
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

		// 要求必须对齐
		if(ulIn % ulKey || ulKey % 16)
			break;

		// 初始化密钥
		AES.MakeKey((PCHAR)lpKey, Chain, ulKey, ulKey);

		AES.EncryptData((const char*)lpIn, (char*)lpOut, ulIn);
		*lpRetOutLen = ulIn;

		bRet = TRUE;

	} while (FALSE);

	return bRet;
}