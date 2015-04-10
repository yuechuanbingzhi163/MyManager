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
#ifndef _CRYPTOLAYER_H
#define _CRYPTOLAYER_H

#include <windef.h>

#define _CRYPTO_LAYER_MOD_			_T("����")

enum CryptoMethod
{
	CM_ENCRYPT,
	CM_DECRYPT
};

typedef LONG (*GETINDEX)();
typedef BOOL (*PDECRYPT)(LPVOID lpIn, ULONG ulIn, LPVOID lpOut, ULONG* lpRetOutLen, LPVOID lpKey, ULONG ulKey);
typedef BOOL (*PENCRYPT)(LPVOID lpIn, ULONG ulIn, LPVOID lpOut, ULONG* lpRetOutLen, LPVOID lpKey, ULONG ulKey);

typedef struct _CRYPTO_MOD_INFO
{
	LONG					Index;				//�㷨������

	PDECRYPT                 fpDecrypt;
	PENCRYPT					fpEncrypt;

	// ˽�л�����
	HMODULE                  hMod;           //ģ�����ַ
	struct _CRYPTO_MOD_INFO *NextNode;       //ָ����һ��Ԫ��

}CRYPTO_MOD_INFO, *PCRYPTO_MOD_INFO, *LPCRYPTO_MOD_INFO;

class CCryptoLayer
{
public:
	//�㷨���ļ�ȫ·��
	static TCHAR				ms_CryptoPath[MAX_PATH];

	PCRYPTO_MOD_INFO Query(IN LONG lIndex);
	BOOL Load(IN LPTSTR lpPath);
	VOID Destory();
	BOOL Delete(IN LONG lIndex);
	BOOL Insert(IN LPTSTR lpFileName);

private:
	//�㷨�ṹ����ͷ
	static PCRYPTO_MOD_INFO		ms_ListHead;
	//�¼�������������ͬ����������
	static HANDLE				ms_hSynObject;
	//˽�жѾ��
	static HANDLE				ms_Heap;
};

#endif