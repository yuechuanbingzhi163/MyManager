// PropPageEx.h : CPropPageEx 的声明

#pragma once
#include "ShellEx_i.h"
#include "resource.h"       // 主符号
#include <comsvcs.h>
#include <shobjidl.h>

using namespace ATL;

#define SAFE_PROP_TITLE		_T("密文文件")

BOOL CALLBACK PropPage_DlgProc(HWND, UINT, WPARAM, LPARAM);

// CPropPageEx

class ATL_NO_VTABLE CPropPageEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPropPageEx, &CLSID_PropPageEx>,
	public IDispatchImpl<IPropPageEx, &IID_IPropPageEx, &LIBID_ShellExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IShellPropSheetExt
{
public:
	CPropPageEx()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PROPPAGEEX)

DECLARE_NOT_AGGREGATABLE(CPropPageEx)

BEGIN_COM_MAP(CPropPageEx)
	COM_INTERFACE_ENTRY(IPropPageEx)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()

// IPropPageEx
public:
	// IShellExtInit
	IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

	// IShellPropSheetExt
	IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);
	IFACEMETHODIMP ReplacePage(UINT uPageID, LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam);

};

OBJECT_ENTRY_AUTO(__uuidof(PropPageEx), CPropPageEx)
