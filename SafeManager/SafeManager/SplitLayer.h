/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：SplitLayer.h
* 文件标识： 
* 摘    要：破碎管理层
*  
* 当前版本：1.0
* 作    者：陈炫
* 完成日期：2014年8月11日 
*/

#pragma once

#define _SPLIT_LAYER_MOD_		_T("破碎")

typedef struct _FRAG_INFO_T
{
	TCHAR		Path[MAX_PATH];				// 存储路径

	ULONG		Count;
	TCHAR		Name[20][MAX_PATH];			// 最高支持20分片
} FRAG_INFO_T, *PFRAG_INFO_T, *LPFRAG_INFO_T;

typedef LONG (*GETINDEX)();
typedef BOOL (*SPLIT)(IN LPTSTR lpFileName, OUT LPFRAG_INFO_T lpInfo);
typedef BOOL (*MERGE)(OUT LPTSTR lpFileName, IN LPFRAG_INFO_T lpInfo);

typedef struct _SPLIT_MOD_INFO
{
	LONG					Index;          //算法索引号
	
	SPLIT					fpSplit;
	MERGE					fpMerge;

	// 私有化数据
	HMODULE                  hMod;           //模块基地址
	struct _SPLIT_MOD_INFO	 *NextNode;      //指向下一个元素
} SPLIT_MOD_INFO, *PSPLIT_MOD_INFO, *LPSPLIT_MOD_INFO;

class CSplitLayer
{
public:
	BOOL Load(LPTSTR lpPath);
	VOID Destory();
	BOOL Insert(IN LPTSTR lpFileName);
	LPSPLIT_MOD_INFO Query(IN LONG lIndex);

private:
	//算法结构链表头
	static LPSPLIT_MOD_INFO		ms_ListHead;
	//事件对象句柄，用来同步访问链表
	static HANDLE				ms_hSynObject;
	//私有堆句柄
	static HANDLE				ms_Heap;
};