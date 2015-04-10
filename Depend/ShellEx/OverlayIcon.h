// OverlayIcon.h : COverlayIcon µÄÉùÃ÷

#pragma once
#include "ShellEx_i.h"
#include "resource.h"       // Ö÷·ûºÅ
#include <comsvcs.h>
#include <shlobj.h>

using namespace ATL;



// COverlayIcon

class ATL_NO_VTABLE COverlayIcon :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COverlayIcon, &CLSID_OverlayIcon>,
	public IDispatchImpl<IOverlayIcon, &IID_IOverlayIcon, &LIBID_ShellExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	 public IShellIconOverlayIdentifier
{
public:
	COverlayIcon()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	// IShellIconOverlayIdentifier Methods
	STDMETHOD(GetOverlayInfo)(LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
	STDMETHOD(GetPriority)(int* pPriority);
	STDMETHOD(IsMemberOf)(LPCWSTR pwszPath,DWORD dwAttrib);

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OVERLAYICON)

DECLARE_NOT_AGGREGATABLE(COverlayIcon)

BEGIN_COM_MAP(COverlayIcon)
	COM_INTERFACE_ENTRY(IOverlayIcon)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
END_COM_MAP()


// IOverlayIcon
public:
};

OBJECT_ENTRY_AUTO(__uuidof(OverlayIcon), COverlayIcon)
