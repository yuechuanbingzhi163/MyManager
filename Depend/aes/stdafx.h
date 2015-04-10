/*
 * CopyRight (C) 2013 北京凯锐立德科技有限公司
 * All rights reserved
 *
 * 文件名称：stdafx.h
 * 摘要：预编译头文件
 * 版本：N/A
 * 作者：陈炫
 * 完成时间：2013.05.18
 * 修改记录：
 * 时间    			修改人    	修改内容
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
