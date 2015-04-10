/* 
* Copyright (c) 2012,�����������¿Ƽ����޹�˾ 
* All rights reserved. 
*  
* �ļ����ƣ�BinStream.h
* �ļ���ʶ�� 
* ժ    Ҫ���������������㷨
*  
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�2013��6��14�� 
*/

#ifndef _BINSTREAM_H
#define _BINSTREAM_H

#define _CORE_ALGORITHM_MOD_		_T("�����㷨")

// ��������ʶ����
#define BIT_FLAG			0x12345678

// ��������������Ԫ
#define BIT					CHAR

// ����������С���뵥Ԫ
#define BITUNIT				8

// ������������
#define MEM_STREAM			0
#define FILE_STREAM			1
#define CREATE_BIN_STREAM	0
#define OPEN_BIN_STREAM		1

#define GET_BIT(x, index)	((*((PCHAR)x) >> (index)) & 1)
#define SET_INDEX(x, pos)	(PLARGE_INTEGER)(&(((PLARGE_INTEGER)x)->QuadPart = pos))

// ������������
// ��������ʹ洢��Ԫ��СΪһ���ֽ�
typedef struct _BYTE_UNIT
{
	CHAR	Bit0:1;
	CHAR	Bit1:1;
	CHAR	Bit2:1;
	CHAR	Bit3:1;
	CHAR	Bit4:1;
	CHAR	Bit5:1;
	CHAR	Bit6:1;
	CHAR	Bit7:1;
} BYTE_UNIT, *PBYTE_UNIT;


// ���ڹ������ֵ�������Ƭͷ
typedef struct _BIN_STREAM_HEADER
{
	// ������Ƭ��������
	DWORD			Flag;
	DWORD			Size;
	DWORD			CRC;

	// �Զ���
	LARGE_INTEGER	BinCount;
} BIN_STREAM_HEADER, *PBIN_STREAM_HEADER;

// ���������ڲ��ṹ
typedef struct _BIN_STREAM
{
	PBIN_STREAM_HEADER	BinHeader;

	BOOL					bBinType;
	HANDLE					hFile;
	LARGE_INTEGER			liFileSize;
	LARGE_INTEGER			liBinSize;

	BOOL					bMap;
	HANDLE					hFileMap;
	PVOID					Addr;
	PVOID					DataAddr;
} BIN_STREAM, *PBIN_STREAM;

class CBinStream
{
public:
	CBinStream();

	BOOL SplitFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData);
	BOOL MergeFile(IN PWSTR lpFileName, IN LPFRAG_INFO_T pData);

private:
	static TCHAR		ms_TempName[MAX_PATH];

	LPTSTR GetTempName();
	VOID GetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, OUT BIT* pBit);
	VOID SetValue(IN PBIN_STREAM pBinStream, IN PLARGE_INTEGER pliIndex, IN BIT Bit);

	BOOL SaveAsDataFile(IN PBIN_STREAM pBinStream, IN PWSTR lpFileName);

	BOOL Gen(IN DWORD dwType, IN PLARGE_INTEGER pliSize, IN PWSTR lpFileName OPTIONAL, IN BOOL bFileFlag OPTIONAL, OUT PBIN_STREAM pBinStream);
	VOID MakeFileMapping(IN PWSTR lpFileName, IN PBIN_STREAM pBinStream);
	VOID Free(IN PBIN_STREAM pBinStream);

	static HANDLE ms_Heap;
};

#endif