// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "binstream.h"


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

LONG GetIndex()
{
	return 1;
}

BOOL Split(IN LPTSTR lpFileName, OUT LPFRAG_INFO_T lpInfo)
{
	BOOL			bResult				= FALSE;
	CBinStream		BinStream;


	do 
	{
		if(!lpFileName || !lpInfo)
			break;

		lpInfo->Count = lpInfo->Count;
		if(BinStream.SplitFile(lpFileName, lpInfo))
			bResult = TRUE;

	} while (FALSE);

	return bResult;
}

BOOL Merge(OUT LPTSTR lpFileName, IN LPFRAG_INFO_T lpInfo)
{
	BOOL			bResult				= FALSE;
	CBinStream		BinStream;


	do 
	{
		if(!lpFileName || !lpInfo)
			break;

		lpInfo->Count = lpInfo->Count;
		if(BinStream.MergeFile(lpFileName, lpInfo))
			bResult = TRUE;

	} while (FALSE);

	return bResult;
}

