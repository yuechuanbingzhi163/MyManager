// dllmain.cpp : DllMain ��ʵ�֡�

#include "stdafx.h"
#include "resource.h"
#include "ShellEx_i.h"
#include "dllmain.h"

CShellExModule _AtlModule;

HINSTANCE			_hInstance;

// DLL ��ڵ�
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	_hInstance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
