/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：BinStream.cpp
* 文件标识： 
* 摘    要：二进制流核心算法
*  
* 当前版本：1.0
* 作    者：陈炫
* 完成日期：2013年6月14日 
*/

#include "stdafx.h"
#include "BinStream.h"
#include <strsafe.h>
#include <objbase.h>


HANDLE			CBinStream::ms_Heap					= ::HeapCreate(0, 1024 * 1024 * 32, 0);
TCHAR			CBinStream::ms_TempName[MAX_PATH]	= {0};


// 获取基于字节存储单元的二进制位
BYTE BYTE_BIT_VALUE[16] = {
	1,
	2,
	4,
	8,
	16,
	32,
	64,
	128,
	254,
	253,
	251,
	247,
	239,
	223,
	191,
	127
};

CBinStream::CBinStream()
{
}

LPTSTR CBinStream::GetTempName()
{
	ZeroMemory(ms_TempName, sizeof(ms_TempName));
	GUID guid;

	if (S_OK == ::CoCreateGuid(&guid))
	{
		wsprintf(ms_TempName,  _T("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X"), guid.Data1, guid.Data2,	guid.Data3,	guid.Data4[0], guid.Data4[1],guid.Data4[2], guid.Data4[3],guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
		_tcscat(ms_TempName, _T(".csm")); 
	}

	return ms_TempName;
}

/*
 * 基于文件数据映射为内存
 *
 * 参数：
 *		lpFileName	文件全路径，文件必须存在
 *		pBinStream	二进制流结构体
 *
 * 返回值：N/A
 *
 * 备注：N/A
 */
VOID CBinStream::MakeFileMapping(IN PWSTR lpFileName, IN PBIN_STREAM pBinStream)
{
	BIN_STREAM_HEADER	BinHeader	= {0};
	DWORD				dwRet		= 0;

	do 
	{
		pBinStream->hFile = ::CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, \
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(pBinStream->hFile == INVALID_HANDLE_VALUE)
		{
			break;
		}
		
		// 读取文件头
		::ReadFile(pBinStream->hFile, &BinHeader, sizeof(BIN_STREAM_HEADER), &dwRet, NULL);
		if(dwRet == sizeof(BIN_STREAM_HEADER) && BinHeader.Flag == BIT_FLAG)
			pBinStream->bBinType = TRUE;

		// 读取文件大小
		// 警告：必须低于4G文件
		::GetFileSizeEx(pBinStream->hFile, &pBinStream->liFileSize);
		if(pBinStream->liFileSize.HighPart)
		{
			break;
		}

		// 获取该文件的二进制数
		pBinStream->liBinSize.QuadPart = pBinStream->liFileSize.QuadPart << 3;

		// 尝试直接读取到内存(提升性能)
		pBinStream->Addr = ::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, pBinStream->liFileSize.LowPart);
		if(pBinStream->Addr)
		{
			::SetFilePointer(pBinStream->hFile, 0, NULL, FILE_BEGIN);
			::ReadFile(pBinStream->hFile, pBinStream->Addr, pBinStream->liFileSize.LowPart, &dwRet, NULL);
		}
		else
		{
			// 内存申请失败则使用文件内存映射
			pBinStream->bMap = TRUE;

			pBinStream->hFileMap = ::CreateFileMapping(pBinStream->hFile, NULL, PAGE_READWRITE, 
				pBinStream->liFileSize.HighPart, pBinStream->liFileSize.LowPart, NULL);

			if(pBinStream->hFileMap == NULL)
			{
				break;
			}

			pBinStream->Addr = ::MapViewOfFile(pBinStream->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		}		

		// 获取数据的起始地址
		pBinStream->DataAddr = pBinStream->Addr;
		if(pBinStream->bBinType)
		{
			pBinStream->liBinSize.QuadPart -= (sizeof(BIN_STREAM_HEADER) << 3);
			pBinStream->DataAddr = (PCHAR)pBinStream->Addr + sizeof(BIN_STREAM_HEADER);
		}

		pBinStream->BinHeader = (PBIN_STREAM_HEADER)pBinStream->Addr;
	} while (FALSE);

	// 失败时清理去资源
	if(pBinStream->Addr == NULL && pBinStream->bMap)
	{
		if(pBinStream->hFileMap != INVALID_HANDLE_VALUE)
			::CloseHandle(pBinStream->hFileMap);

		if(pBinStream->hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(pBinStream->hFile);
	}
	else if(pBinStream->Addr == NULL)
	{
		if(pBinStream->hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(pBinStream->hFile);
	}

	return;
}

/*
 * 将二进制流作为文件保存
 *
 * 参数：
 *		pBinStream		二进制流内部结构
 *		lpFileName		输出文件名称
 *
 * 返回值：是否成功
 *
 * 备注：N/A
 */
BOOL CBinStream::SaveAsDataFile(IN PBIN_STREAM pBinStream, IN PWSTR lpFileName)
{
	BOOL			bRet = TRUE;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	DWORD			dwRet = 0;


	if(pBinStream->hFile)
	{
		pBinStream->liFileSize.QuadPart = pBinStream->liBinSize.LowPart >> 3;
		::CopyMemory(pBinStream->Addr, pBinStream->DataAddr, pBinStream->liFileSize.LowPart);		
	}else{
		hFile = ::CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return FALSE;
		}

		::WriteFile(hFile, pBinStream->DataAddr, pBinStream->liFileSize.LowPart, &dwRet, NULL);
		::CloseHandle(hFile);
	}
	return TRUE;
}


/*
 * 生成一个二进制流
 *
 * 参数：
 *		dwType		内存或者文件类型
 *		pliSize		指定二进制流大小
 *		FileName	可选参数，基于文件
 *		bFileFlag	可选参数，指定文件操作
 *		pBinStream	二进制流的内部结构
 *
 * 返回值：是否成功
 *
 * 备注：N/A
 */
BOOL CBinStream::Gen(IN DWORD dwType, IN PLARGE_INTEGER pliSize, IN PWSTR lpFileName, IN BOOL bFileFlag, OUT PBIN_STREAM pBinStream)
{
	BOOL				bRet		= FALSE;
	DWORD				dwRet		= 0;
	DWORD				dwChgOffset = 0;
	BIN_STREAM_HEADER	BinHeader	= {0};
	LARGE_INTEGER		liSize		= {0};


	if(!pBinStream)
	{
		return FALSE;
	}

	::ZeroMemory(pBinStream, sizeof(BIN_STREAM));

	if(pliSize)
		liSize.QuadPart = pliSize->QuadPart;
	else
		liSize.QuadPart = 0;

	// 修正二进制输入参数大小
	// 备注：存储介质时必须以字节为大小对齐
	if(liSize.QuadPart)
	{
		dwChgOffset = liSize.LowPart % BITUNIT;
		if(dwChgOffset)
			liSize.QuadPart += (BITUNIT - dwChgOffset);
	}

	if(dwType == MEM_STREAM && liSize.QuadPart)
	{
		// 创建基于内存的二进制流
		pBinStream->Addr = ::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY,  sizeof(BIN_STREAM_HEADER) + (SIZE_T)(liSize.QuadPart >> 3));
		if(pBinStream->Addr)
		{
			// 初始化内部结构
			pBinStream->BinHeader = (PBIN_STREAM_HEADER)pBinStream->Addr;
			pBinStream->BinHeader->Flag = BIT_FLAG;
			pBinStream->BinHeader->Size = sizeof(BIN_STREAM_HEADER);
			pBinStream->bBinType = TRUE;
			pBinStream->Addr = pBinStream->Addr;
			pBinStream->DataAddr = (PCHAR)pBinStream->Addr + sizeof(BIN_STREAM_HEADER);
		}		
	}
	else if(dwType == FILE_STREAM)
	{
		// 打开基于文件的二进制流
		if(bFileFlag == OPEN_BIN_STREAM)
		{
			MakeFileMapping(lpFileName, pBinStream);
		}
		else if(bFileFlag == CREATE_BIN_STREAM && liSize.QuadPart)
		{
			// 新建一个二进制数据流
			do 
			{
				// 创建二进制流宿主 - 文件
				pBinStream->hFile = ::CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(pBinStream->hFile == INVALID_HANDLE_VALUE)
				{
					break;
				}

				// 获取在存储介质上所占用的大小
				liSize.QuadPart += sizeof(BIN_STREAM_HEADER) << 3;
				pBinStream->liFileSize.QuadPart = liSize.QuadPart >> 3;

				// 初始化二进制流头数据结构
				BinHeader.Flag = BIT_FLAG;
				BinHeader.Size = sizeof(BIN_STREAM_HEADER);
				::WriteFile(pBinStream->hFile, &BinHeader, sizeof(BIN_STREAM_HEADER), &dwRet, NULL);

				// 修正存储介质上的文件大小
				::SetFilePointer(pBinStream->hFile, pBinStream->liFileSize.LowPart, &pBinStream->liFileSize.HighPart, FILE_BEGIN);
				::SetEndOfFile(pBinStream->hFile);
				::FlushFileBuffers(pBinStream->hFile);
				::CloseHandle(pBinStream->hFile);

				MakeFileMapping(lpFileName, pBinStream);
			} while (FALSE);
		}
	}else{
		// 基于其他类型的文件流
	}

	if(pBinStream->Addr)
		bRet = TRUE;

	return bRet;
}

/*
 * 释放一个二进制流
 *
 * 参数：
 *		pBinStream		文件流的内部结构
 *
 * 返回值：无
 *
 * 备注：N/A
 */
VOID CBinStream::Free(IN PBIN_STREAM pBinStream)
{
	if (!pBinStream)
	{
		return;
	}

	if(pBinStream->bMap)
	{
		if(pBinStream->Addr != NULL)
			::UnmapViewOfFile(pBinStream->Addr);

		if(pBinStream->hFileMap != INVALID_HANDLE_VALUE)
			::CloseHandle(pBinStream->hFileMap);
	}
	else
	{
		// 回写二进制流到文件
		::SetFilePointer(pBinStream->hFile, 0, NULL, FILE_BEGIN);
		::WriteFile(pBinStream->hFile, pBinStream->Addr, pBinStream->liFileSize.LowPart, (LPDWORD)&(pBinStream->liFileSize.HighPart), NULL);
		::HeapFree(ms_Heap, 0, pBinStream->Addr);
	}	

	::SetFilePointer(pBinStream->hFile, pBinStream->liFileSize.LowPart, NULL, FILE_BEGIN);
	::SetEndOfFile(pBinStream->hFile);
	::FlushFileBuffers(pBinStream->hFile);

	if(pBinStream->hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(pBinStream->hFile);

	return;
}

/*
 * 获取二进制流中任意值
 *
 * 参数：
 *		pBinStream		二进制流结构
 *		pliIndex		序号
 *		pBit			待返回的二进制值
 *
 * 返回值：无
 *
 * 备注：N/A
 */
VOID CBinStream::GetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, OUT BIT* pBit)
{
	PCHAR			pAddr = (PCHAR)pBinStream->DataAddr + (pliIndex->QuadPart >> 3);	// liIndex->QuadPart >> 3 = 获取字节大小
	BYTE_UNIT		ByteUnit;

	// 获取最小存储单元
	ByteUnit = *(PBYTE_UNIT)pAddr;

	// 获取存储单元中的二进制
	*pBit = GET_BIT(&ByteUnit, pliIndex->QuadPart & 7);

	return;
}

/*
 * 设置二进制流中任意值
 *
 * 参数：
 *		pBinStream		二进制流结构
 *		pliIndex		序号
 *		Bit				待设置的二进制值
 *
 * 返回值：无
 *
 * 备注：N/A
 */
VOID CBinStream::SetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, IN BIT Bit)
{
	PCHAR			pAddr;
	BIT				Value[2] = {0};

	// 设置二进制
	pAddr = (PCHAR)pBinStream->DataAddr + (pliIndex->QuadPart >> 3);
	Value[1] = BYTE_BIT_VALUE[pliIndex->QuadPart & 7];

	*pAddr |= Value[Bit];
	
	return;
}

/*
 * 基于文件为宿主进行数据分流
 *
 * 参数：
 *		lpFileName		文件全路径
 *		pData			分片信息
 *
 * 返回值：未使用
 *
 * 备注：N/A
 */
BOOL CBinStream::SplitFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData)
{
	BIN_STREAM			BinStream = {0};
	PBIN_STREAM			BinStreamSub;
	TCHAR				wstrFileName[MAX_PATH];
	BIT					Bit;
	LARGE_INTEGER		liBinSize;
	LARGE_INTEGER		liTmp, liSegment, liRecord, liData, liGobal;


	// 快速处理方式
	if(pData->Count == 1)
	{
		wcscat_s(pData->Name[0], sizeof(pData->Name[0]) / sizeof(TCHAR), GetTempName());
		::StringCbPrintf(wstrFileName, sizeof(wstrFileName), _T("%s\\%s"), pData->Path, pData->Name[0]);		// c:\xxxxxx\xxxx\0
		::CopyFile(lpFileName, wstrFileName, FALSE);
		return TRUE;
	}

	BinStreamSub = (PBIN_STREAM)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, pData->Count * sizeof(BIN_STREAM));
	if(!BinStreamSub)
	{
		return FALSE;
	}

	// 基于二进制流打开原始文件
	Gen(FILE_STREAM, &liBinSize, lpFileName, OPEN_BIN_STREAM, &BinStream);

	// 计算每一块所需的二进制数
	liSegment.QuadPart = BinStream.liFileSize.QuadPart / pData->Count + 1;
	liSegment.QuadPart <<= 3;

	// 创建N个二进制分流 - BinStreamSub
	for(DWORD i = 0; i < pData->Count; i++)
	{
		// 注意：返回的liTmp会被修正，需要重新赋值
		liTmp.QuadPart = liSegment.QuadPart;

		wcscat_s(pData->Name[i], sizeof(pData->Name[i]) / sizeof(TCHAR), GetTempName());
		ZeroMemory(wstrFileName, sizeof(wstrFileName));
		::StringCbPrintf(wstrFileName, sizeof(wstrFileName), _T("%s\\%s"), pData->Path, pData->Name[i]);		// c:\xxxxxx\xxxx\0
		if(!Gen(FILE_STREAM, &liTmp, wstrFileName, CREATE_BIN_STREAM, &BinStreamSub[i]))
			break;

		if((pData->Count - 1) == i)
			BinStreamSub[i].BinHeader->BinCount.QuadPart = BinStream.liBinSize.QuadPart - BinStream.liBinSize.QuadPart / pData->Count * i;
		else
			BinStreamSub[i].BinHeader->BinCount.QuadPart = BinStream.liBinSize.QuadPart / pData->Count;
	}

	// 对原始文件进行二进制分流
	liRecord.QuadPart = 0;
	liData.QuadPart = 0;
	liGobal.QuadPart = 0;
	for(DWORD j = 0; liGobal.QuadPart < BinStream.liBinSize.QuadPart; liRecord.QuadPart++, liGobal.QuadPart += pData->Count)
	{
		for(DWORD k = 0; k < pData->Count && liData.QuadPart < BinStream.liBinSize.QuadPart; k++)
		{
			// 获取原始文件的二进制状态
			GetValue(&BinStream, SET_INDEX(&liTmp, liData.QuadPart), &Bit);

			// 设置状态到新二进制流
			SetValue(&BinStreamSub[k], SET_INDEX(&liTmp, liRecord.QuadPart), Bit);

			liData.QuadPart++;
		}
	}

	// 释放二进制流
	for(DWORD i = 0; i < pData->Count; i++)
	{
		Free(&BinStreamSub[i]);
	}

	Free(&BinStream);
	::HeapFree(ms_Heap, 0, BinStreamSub);

	return TRUE;
}

/*
 * 基于二进制流进行文件合并
 *
 * 参数：
 *		lpFileName		文件全路径
 *		pData			分片信息
 *
 * 返回值：未使用
 *
 * 备注：N/A
 */
BOOL CBinStream::MergeFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData)
{
	BIN_STREAM			BinStream = {0};
	PBIN_STREAM			BinStreamSub;
	BIT					Bit;
	LARGE_INTEGER		liBinSize;
	LARGE_INTEGER		liTmp, liData, liGobal, liRecord;
	TCHAR				wstrFileName[MAX_PATH];


	// 快速处理方式
	if(pData->Count == 1)
	{
		::ZeroMemory(wstrFileName, MAX_PATH * sizeof(TCHAR));
		::StringCbPrintf(wstrFileName, MAX_PATH * sizeof(TCHAR), _T("%s\\%s"), pData->Path, pData->Name[0]);
		if(!::CopyFile(wstrFileName, lpFileName, FALSE))
			return FALSE;
		else
			return TRUE;
	}

	liBinSize.QuadPart = 0;
	BinStreamSub = (PBIN_STREAM)::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, pData->Count * sizeof(BIN_STREAM));
	if(!BinStreamSub)
	{
		return FALSE;
	}

	for(DWORD i = 0; i < pData->Count; i++)
	{
		::ZeroMemory(wstrFileName, MAX_PATH * sizeof(TCHAR));
		::StringCbPrintf(wstrFileName, MAX_PATH * sizeof(TCHAR), _T("%s\\%s"), pData->Path, pData->Name[i]);
		Gen(FILE_STREAM, NULL, wstrFileName, OPEN_BIN_STREAM, &BinStreamSub[i]);
		liBinSize.QuadPart += BinStreamSub[i].BinHeader->BinCount.QuadPart;
	}

	// 输出文件的二进制流
	Gen(FILE_STREAM, &liBinSize, lpFileName, CREATE_BIN_STREAM, &BinStream);

	liData.QuadPart = 0;
	liGobal.QuadPart = 0;
	liRecord.QuadPart = 0;
	for(DWORD j = 0; liData.QuadPart < BinStream.liBinSize.QuadPart; liRecord.QuadPart++, liGobal.QuadPart += pData->Count)
	{
		for(DWORD k = 0; k < pData->Count && liData.QuadPart < liBinSize.QuadPart; k++)
		{
			// 获取二进制文件流的二进制状态信息
			GetValue(&BinStreamSub[k], SET_INDEX(&liTmp, liRecord.QuadPart), &Bit);
			
			// 设置状态到新二进制流
			SetValue(&BinStream, SET_INDEX(&liTmp, liData.QuadPart), Bit);

			liData.QuadPart++;
		}
	}

	SaveAsDataFile(&BinStream, lpFileName);

	// 释放二进制流
	for(DWORD i = 0; i < pData->Count; i++)
	{
		Free(&BinStreamSub[i]);
	}

	Free(&BinStream);
	::HeapFree(ms_Heap, 0, BinStreamSub);

	return TRUE;
}
