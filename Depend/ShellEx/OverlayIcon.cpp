// OverlayIcon.cpp : COverlayIcon 的实现

#include "stdafx.h"
#include "OverlayIcon.h"


// COverlayIcon


STDMETHODIMP COverlayIcon::GetOverlayInfo( LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags )
{
	GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, cchMax);

	*pIndex = 0;
	*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;

	return S_OK;
}

STDMETHODIMP COverlayIcon::GetPriority( int* pPriority )
{
	*pPriority =0;
	return S_OK;
}

STDMETHODIMP COverlayIcon::IsMemberOf( LPCWSTR pwszPath,DWORD dwAttrib )
{
	HRESULT rv = S_FALSE;

	if (pwszPath == NULL){
		return E_INVALIDARG;
	}

	//判断文件是否加密
	LPTSTR lpExten = PathFindExtension(pwszPath);
	if ((lpExten != NULL) && (StrCmp(lpExten, _T(".csm")) == 0)){
		rv = S_OK;
	}

	return rv;
}
