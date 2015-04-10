// ContextMenuEx.h : CContextMenuEx ������

#pragma once
#include "ShellEx_i.h"
#include "resource.h"       // ������
#include <comsvcs.h>
#include <shobjidl.h>

using namespace ATL;

extern HINSTANCE         _hInstance;

#define SZ_MENU_SD_OUT		TEXT("���͵���ȫ�ܼ�")

// CContextMenuEx

class ATL_NO_VTABLE CContextMenuEx :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CContextMenuEx, &CLSID_ContextMenuEx>,
	public IDispatchImpl<IContextMenuEx, &IID_IContextMenuEx, &LIBID_ShellExLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CContextMenuEx()
	{
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_hBitmapSend = LoadBitmap(_hInstance,MAKEINTRESOURCE(IDB_BITMAP1));

		return S_OK;
	}

	void FinalRelease()
	{
		if (m_hBitmapSend)
		{
			DeleteObject(m_hBitmapSend);
		}
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CONTEXTMENUEX)

DECLARE_NOT_AGGREGATABLE(CContextMenuEx)

BEGIN_COM_MAP(CContextMenuEx)
	COM_INTERFACE_ENTRY(IContextMenuEx)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()




// IContextMenuEx
public:
	STDMETHODIMP  Initialize( 
		/* [in] */ LPCITEMIDLIST pidlFolder,
		/* [in] */ IDataObject *pdtobj,
		/* [in] */ HKEY hkeyProgID);

	//
	//���Ҽ��˵��в�������Ҫ��ӵĲ˵���
	//
	STDMETHODIMP QueryContextMenu(
		HMENU hmenu,
		UINT indexMenu,
		UINT idCmdFirst,
		UINT idCmdLast,
		UINT uFlags);

	//
	//
	// �û�����˲˵�������˵�������Ĳ�����������ִ��
	//
	//
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici);


	//
	//
	// չʾ������Ϣ������û�У���
	//
	//
	STDMETHODIMP GetCommandString(
		UINT_PTR    idCmd,
		UINT        uType,
		UINT      * pwReserved,
		LPSTR       pszName,
		UINT        cchMax);

private:
	HBITMAP m_hBitmapSend;

	TCHAR   m_pszFileName[MAX_PATH];
};

OBJECT_ENTRY_AUTO(__uuidof(ContextMenuEx), CContextMenuEx)
