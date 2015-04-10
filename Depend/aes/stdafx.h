/*
 * CopyRight (C) 2013 �����������¿Ƽ����޹�˾
 * All rights reserved
 *
 * �ļ����ƣ�stdafx.h
 * ժҪ��Ԥ����ͷ�ļ�
 * �汾��N/A
 * ���ߣ�����
 * ���ʱ�䣺2013.05.18
 * �޸ļ�¼��
 * ʱ��    			�޸���    	�޸�����
 *
 */

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdlib.h>
#include <time.h>

typedef struct _AES_HEADER
{
	ULONG				ulFlag;
	LARGE_INTEGER		liFileSize;
	LARGE_INTEGER		liStoreSize;
} AES_HEADER, *PAES_HEADER, *LPAES_HEADER;

#define ENCRYPT		0
#define DECRYPT		1

#define PROCDATA(class, type, buf, size) (type?class.Decrypt(buf, buf, size):class.Encrypt(buf, buf, size))

// TODO: reference additional headers your program requires here
