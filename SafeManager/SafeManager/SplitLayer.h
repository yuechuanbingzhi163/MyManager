/* 
* Copyright (c) 2012,�����������¿Ƽ����޹�˾ 
* All rights reserved. 
*  
* �ļ����ƣ�SplitLayer.h
* �ļ���ʶ�� 
* ժ    Ҫ����������
*  
* ��ǰ�汾��1.0
* ��    �ߣ�����
* ������ڣ�2014��8��11�� 
*/

#pragma once

#define _SPLIT_LAYER_MOD_		_T("����")

typedef struct _FRAG_INFO_T
{
	TCHAR		Path[MAX_PATH];				// �洢·��

	ULONG		Count;
	TCHAR		Name[20][MAX_PATH];			// ���֧��20��Ƭ
} FRAG_INFO_T, *PFRAG_INFO_T, *LPFRAG_INFO_T;

typedef LONG (*GETINDEX)();
typedef BOOL (*SPLIT)(IN LPTSTR lpFileName, OUT LPFRAG_INFO_T lpInfo);
typedef BOOL (*MERGE)(OUT LPTSTR lpFileName, IN LPFRAG_INFO_T lpInfo);

typedef struct _SPLIT_MOD_INFO
{
	LONG					Index;          //�㷨������
	
	SPLIT					fpSplit;
	MERGE					fpMerge;

	// ˽�л�����
	HMODULE                  hMod;           //ģ�����ַ
	struct _SPLIT_MOD_INFO	 *NextNode;      //ָ����һ��Ԫ��
} SPLIT_MOD_INFO, *PSPLIT_MOD_INFO, *LPSPLIT_MOD_INFO;

class CSplitLayer
{
public:
	BOOL Load(LPTSTR lpPath);
	VOID Destory();
	BOOL Insert(IN LPTSTR lpFileName);
	LPSPLIT_MOD_INFO Query(IN LONG lIndex);

private:
	//�㷨�ṹ����ͷ
	static LPSPLIT_MOD_INFO		ms_ListHead;
	//�¼�������������ͬ����������
	static HANDLE				ms_hSynObject;
	//˽�жѾ��
	static HANDLE				ms_Heap;
};