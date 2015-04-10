/* 
* Copyright (c) 2012,北京凯锐立德科技有限公司 
* All rights reserved. 
*  
* 文件名称：FileOperation.h
* 文件标识： 
* 摘    要：文件处理接口
*  
* 当前版本：1.0
* 作    者：高永吉
* 完成日期：2013年7月1日 
* 修改记录: 
*			2013.07.08		陈炫	简化代码
*
*/
#ifndef _CRYPTOLAYER_H
#define _CRYPTOLAYER_H

#include <windef.h>

#define _CRYPTO_LAYER_MOD_			_T("加密")

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
	LONG					Index;				//算法索引号

	PDECRYPT                 fpDecrypt;
	PENCRYPT					fpEncrypt;

	// 私有化数据
	HMODULE                  hMod;           //模块基地址
	struct _CRYPTO_MOD_INFO *NextNode;       //指向下一个元素

}CRYPTO_MOD_INFO, *PCRYPTO_MOD_INFO, *LPCRYPTO_MOD_INFO;

class CCryptoLayer
{
public:
	//算法层文件全路径
	static TCHAR				ms_CryptoPath[MAX_PATH];

	PCRYPTO_MOD_INFO Query(IN LONG lIndex);
	BOOL Load(IN LPTSTR lpPath);
	VOID Destory();
	BOOL Delete(IN LONG lIndex);
	BOOL Insert(IN LPTSTR lpFileName);

private:
	//算法结构链表头
	static PCRYPTO_MOD_INFO		ms_ListHead;
	//事件对象句柄，用来同步访问链表
	static HANDLE				ms_hSynObject;
	//私有堆句柄
	static HANDLE				ms_Heap;
};

#endif