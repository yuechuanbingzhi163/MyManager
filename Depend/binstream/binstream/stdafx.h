// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <tchar.h>
#include <windows.h>


typedef struct _FRAG_INFO_T
{
	TCHAR		Path[MAX_PATH];				// �洢·��

	ULONG		Count;
	TCHAR		Name[20][MAX_PATH];			// ���֧��20��Ƭ
} FRAG_INFO_T, *PFRAG_INFO_T, *LPFRAG_INFO_T;

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�