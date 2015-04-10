/* 
* Copyright (c) 2012,�����������¿Ƽ����޹�˾ 
* All rights reserved. 
*  
* �ļ����ƣ�BinStream.cpp
* �ļ���ʶ�� 
* ժ    Ҫ���������������㷨
*  
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�2013��6��14�� 
*/

#include "stdafx.h"
#include "BinStream.h"
#include <strsafe.h>
#include <objbase.h>


HANDLE			CBinStream::ms_Heap					= ::HeapCreate(0, 1024 * 1024 * 32, 0);
TCHAR			CBinStream::ms_TempName[MAX_PATH]	= {0};


// ��ȡ�����ֽڴ洢��Ԫ�Ķ�����λ
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
 * �����ļ�����ӳ��Ϊ�ڴ�
 *
 * ������
 *		lpFileName	�ļ�ȫ·�����ļ��������
 *		pBinStream	���������ṹ��
 *
 * ����ֵ��N/A
 *
 * ��ע��N/A
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
		
		// ��ȡ�ļ�ͷ
		::ReadFile(pBinStream->hFile, &BinHeader, sizeof(BIN_STREAM_HEADER), &dwRet, NULL);
		if(dwRet == sizeof(BIN_STREAM_HEADER) && BinHeader.Flag == BIT_FLAG)
			pBinStream->bBinType = TRUE;

		// ��ȡ�ļ���С
		// ���棺�������4G�ļ�
		::GetFileSizeEx(pBinStream->hFile, &pBinStream->liFileSize);
		if(pBinStream->liFileSize.HighPart)
		{
			break;
		}

		// ��ȡ���ļ��Ķ�������
		pBinStream->liBinSize.QuadPart = pBinStream->liFileSize.QuadPart << 3;

		// ����ֱ�Ӷ�ȡ���ڴ�(��������)
		pBinStream->Addr = ::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY, pBinStream->liFileSize.LowPart);
		if(pBinStream->Addr)
		{
			::SetFilePointer(pBinStream->hFile, 0, NULL, FILE_BEGIN);
			::ReadFile(pBinStream->hFile, pBinStream->Addr, pBinStream->liFileSize.LowPart, &dwRet, NULL);
		}
		else
		{
			// �ڴ�����ʧ����ʹ���ļ��ڴ�ӳ��
			pBinStream->bMap = TRUE;

			pBinStream->hFileMap = ::CreateFileMapping(pBinStream->hFile, NULL, PAGE_READWRITE, 
				pBinStream->liFileSize.HighPart, pBinStream->liFileSize.LowPart, NULL);

			if(pBinStream->hFileMap == NULL)
			{
				break;
			}

			pBinStream->Addr = ::MapViewOfFile(pBinStream->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		}		

		// ��ȡ���ݵ���ʼ��ַ
		pBinStream->DataAddr = pBinStream->Addr;
		if(pBinStream->bBinType)
		{
			pBinStream->liBinSize.QuadPart -= (sizeof(BIN_STREAM_HEADER) << 3);
			pBinStream->DataAddr = (PCHAR)pBinStream->Addr + sizeof(BIN_STREAM_HEADER);
		}

		pBinStream->BinHeader = (PBIN_STREAM_HEADER)pBinStream->Addr;
	} while (FALSE);

	// ʧ��ʱ����ȥ��Դ
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
 * ������������Ϊ�ļ�����
 *
 * ������
 *		pBinStream		���������ڲ��ṹ
 *		lpFileName		����ļ�����
 *
 * ����ֵ���Ƿ�ɹ�
 *
 * ��ע��N/A
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
 * ����һ����������
 *
 * ������
 *		dwType		�ڴ�����ļ�����
 *		pliSize		ָ������������С
 *		FileName	��ѡ�����������ļ�
 *		bFileFlag	��ѡ������ָ���ļ�����
 *		pBinStream	�����������ڲ��ṹ
 *
 * ����ֵ���Ƿ�ɹ�
 *
 * ��ע��N/A
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

	// �������������������С
	// ��ע���洢����ʱ�������ֽ�Ϊ��С����
	if(liSize.QuadPart)
	{
		dwChgOffset = liSize.LowPart % BITUNIT;
		if(dwChgOffset)
			liSize.QuadPart += (BITUNIT - dwChgOffset);
	}

	if(dwType == MEM_STREAM && liSize.QuadPart)
	{
		// ���������ڴ�Ķ�������
		pBinStream->Addr = ::HeapAlloc(ms_Heap, HEAP_ZERO_MEMORY,  sizeof(BIN_STREAM_HEADER) + (SIZE_T)(liSize.QuadPart >> 3));
		if(pBinStream->Addr)
		{
			// ��ʼ���ڲ��ṹ
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
		// �򿪻����ļ��Ķ�������
		if(bFileFlag == OPEN_BIN_STREAM)
		{
			MakeFileMapping(lpFileName, pBinStream);
		}
		else if(bFileFlag == CREATE_BIN_STREAM && liSize.QuadPart)
		{
			// �½�һ��������������
			do 
			{
				// ���������������� - �ļ�
				pBinStream->hFile = ::CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if(pBinStream->hFile == INVALID_HANDLE_VALUE)
				{
					break;
				}

				// ��ȡ�ڴ洢��������ռ�õĴ�С
				liSize.QuadPart += sizeof(BIN_STREAM_HEADER) << 3;
				pBinStream->liFileSize.QuadPart = liSize.QuadPart >> 3;

				// ��ʼ����������ͷ���ݽṹ
				BinHeader.Flag = BIT_FLAG;
				BinHeader.Size = sizeof(BIN_STREAM_HEADER);
				::WriteFile(pBinStream->hFile, &BinHeader, sizeof(BIN_STREAM_HEADER), &dwRet, NULL);

				// �����洢�����ϵ��ļ���С
				::SetFilePointer(pBinStream->hFile, pBinStream->liFileSize.LowPart, &pBinStream->liFileSize.HighPart, FILE_BEGIN);
				::SetEndOfFile(pBinStream->hFile);
				::FlushFileBuffers(pBinStream->hFile);
				::CloseHandle(pBinStream->hFile);

				MakeFileMapping(lpFileName, pBinStream);
			} while (FALSE);
		}
	}else{
		// �����������͵��ļ���
	}

	if(pBinStream->Addr)
		bRet = TRUE;

	return bRet;
}

/*
 * �ͷ�һ����������
 *
 * ������
 *		pBinStream		�ļ������ڲ��ṹ
 *
 * ����ֵ����
 *
 * ��ע��N/A
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
		// ��д�����������ļ�
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
 * ��ȡ��������������ֵ
 *
 * ������
 *		pBinStream		���������ṹ
 *		pliIndex		���
 *		pBit			�����صĶ�����ֵ
 *
 * ����ֵ����
 *
 * ��ע��N/A
 */
VOID CBinStream::GetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, OUT BIT* pBit)
{
	PCHAR			pAddr = (PCHAR)pBinStream->DataAddr + (pliIndex->QuadPart >> 3);	// liIndex->QuadPart >> 3 = ��ȡ�ֽڴ�С
	BYTE_UNIT		ByteUnit;

	// ��ȡ��С�洢��Ԫ
	ByteUnit = *(PBYTE_UNIT)pAddr;

	// ��ȡ�洢��Ԫ�еĶ�����
	*pBit = GET_BIT(&ByteUnit, pliIndex->QuadPart & 7);

	return;
}

/*
 * ���ö�������������ֵ
 *
 * ������
 *		pBinStream		���������ṹ
 *		pliIndex		���
 *		Bit				�����õĶ�����ֵ
 *
 * ����ֵ����
 *
 * ��ע��N/A
 */
VOID CBinStream::SetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, IN BIT Bit)
{
	PCHAR			pAddr;
	BIT				Value[2] = {0};

	// ���ö�����
	pAddr = (PCHAR)pBinStream->DataAddr + (pliIndex->QuadPart >> 3);
	Value[1] = BYTE_BIT_VALUE[pliIndex->QuadPart & 7];

	*pAddr |= Value[Bit];
	
	return;
}

/*
 * �����ļ�Ϊ�����������ݷ���
 *
 * ������
 *		lpFileName		�ļ�ȫ·��
 *		pData			��Ƭ��Ϣ
 *
 * ����ֵ��δʹ��
 *
 * ��ע��N/A
 */
BOOL CBinStream::SplitFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData)
{
	BIN_STREAM			BinStream = {0};
	PBIN_STREAM			BinStreamSub;
	TCHAR				wstrFileName[MAX_PATH];
	BIT					Bit;
	LARGE_INTEGER		liBinSize;
	LARGE_INTEGER		liTmp, liSegment, liRecord, liData, liGobal;


	// ���ٴ���ʽ
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

	// ���ڶ���������ԭʼ�ļ�
	Gen(FILE_STREAM, &liBinSize, lpFileName, OPEN_BIN_STREAM, &BinStream);

	// ����ÿһ������Ķ�������
	liSegment.QuadPart = BinStream.liFileSize.QuadPart / pData->Count + 1;
	liSegment.QuadPart <<= 3;

	// ����N�������Ʒ��� - BinStreamSub
	for(DWORD i = 0; i < pData->Count; i++)
	{
		// ע�⣺���ص�liTmp�ᱻ��������Ҫ���¸�ֵ
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

	// ��ԭʼ�ļ����ж����Ʒ���
	liRecord.QuadPart = 0;
	liData.QuadPart = 0;
	liGobal.QuadPart = 0;
	for(DWORD j = 0; liGobal.QuadPart < BinStream.liBinSize.QuadPart; liRecord.QuadPart++, liGobal.QuadPart += pData->Count)
	{
		for(DWORD k = 0; k < pData->Count && liData.QuadPart < BinStream.liBinSize.QuadPart; k++)
		{
			// ��ȡԭʼ�ļ��Ķ�����״̬
			GetValue(&BinStream, SET_INDEX(&liTmp, liData.QuadPart), &Bit);

			// ����״̬���¶�������
			SetValue(&BinStreamSub[k], SET_INDEX(&liTmp, liRecord.QuadPart), Bit);

			liData.QuadPart++;
		}
	}

	// �ͷŶ�������
	for(DWORD i = 0; i < pData->Count; i++)
	{
		Free(&BinStreamSub[i]);
	}

	Free(&BinStream);
	::HeapFree(ms_Heap, 0, BinStreamSub);

	return TRUE;
}

/*
 * ���ڶ������������ļ��ϲ�
 *
 * ������
 *		lpFileName		�ļ�ȫ·��
 *		pData			��Ƭ��Ϣ
 *
 * ����ֵ��δʹ��
 *
 * ��ע��N/A
 */
BOOL CBinStream::MergeFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData)
{
	BIN_STREAM			BinStream = {0};
	PBIN_STREAM			BinStreamSub;
	BIT					Bit;
	LARGE_INTEGER		liBinSize;
	LARGE_INTEGER		liTmp, liData, liGobal, liRecord;
	TCHAR				wstrFileName[MAX_PATH];


	// ���ٴ���ʽ
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

	// ����ļ��Ķ�������
	Gen(FILE_STREAM, &liBinSize, lpFileName, CREATE_BIN_STREAM, &BinStream);

	liData.QuadPart = 0;
	liGobal.QuadPart = 0;
	liRecord.QuadPart = 0;
	for(DWORD j = 0; liData.QuadPart < BinStream.liBinSize.QuadPart; liRecord.QuadPart++, liGobal.QuadPart += pData->Count)
	{
		for(DWORD k = 0; k < pData->Count && liData.QuadPart < liBinSize.QuadPart; k++)
		{
			// ��ȡ�������ļ����Ķ�����״̬��Ϣ
			GetValue(&BinStreamSub[k], SET_INDEX(&liTmp, liRecord.QuadPart), &Bit);
			
			// ����״̬���¶�������
			SetValue(&BinStream, SET_INDEX(&liTmp, liData.QuadPart), Bit);

			liData.QuadPart++;
		}
	}

	SaveAsDataFile(&BinStream, lpFileName);

	// �ͷŶ�������
	for(DWORD i = 0; i < pData->Count; i++)
	{
		Free(&BinStreamSub[i]);
	}

	Free(&BinStream);
	::HeapFree(ms_Heap, 0, BinStreamSub);

	return TRUE;
}
