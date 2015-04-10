/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：BinStream.h
* 文件标识： 
* 摘    要：二进制流核心算法
*  
* 当前版本：1.0
* 作    者：陈炫
* 完成日期：2013年6月14日 
*/

#ifndef _BINSTREAM_H
#define _BINSTREAM_H

#define _CORE_ALGORITHM_MOD_		_T("核心算法")

// 二进制流识别码
#define BIT_FLAG			0x12345678

// 二进制流操作单元
#define BIT					CHAR

// 二进制流最小编译单元
#define BITUNIT				8

// 二进制流类型
#define MEM_STREAM			0
#define FILE_STREAM			1
#define CREATE_BIN_STREAM	0
#define OPEN_BIN_STREAM		1

#define GET_BIT(x, index)	((*((PCHAR)x) >> (index)) & 1)
#define SET_INDEX(x, pos)	(PLARGE_INTEGER)(&(((PLARGE_INTEGER)x)->QuadPart = pos))

// 二进制数据流
// 编译器最低存储单元大小为一个字节
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


// 基于公共部分的数据碎片头
typedef struct _BIN_STREAM_HEADER
{
	// 数据碎片公共部分
	DWORD			Flag;
	DWORD			Size;
	DWORD			CRC;

	// 自定义
	LARGE_INTEGER	BinCount;
} BIN_STREAM_HEADER, *PBIN_STREAM_HEADER;

// 二进制流内部结构
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