// ContextMenuEx.cpp : CContextMenuEx 的实现

#include "stdafx.h"
#include "ContextMenuEx.h"


// CContextMenuEx


STDMETHODIMP CContextMenuEx::Initialize( /* [in] */ LPCITEMIDLIST pidlFolder, /* [in] */ IDataObject *pdtobj, /* [in] */ HKEY hkeyProgID )
{
	HRESULT hr = S_OK;

	if (NULL == pdtobj)
	{
		return E_INVALIDARG;
	}

	FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stm;

	// The pDataObj pointer contains the objects being acted upon. In this 
	// example, we get an HDROP handle for enumerating the selected files and 
	// folders.
	if (SUCCEEDED(pdtobj->GetData(&fe, &stm)))
	{
		// Get an HDROP handle.
		HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
		if (hDrop != NULL)
		{
			// Determine how many files are involved in this operation. This 
			// code sample displays the custom context menu item when only 
			// one file is selected. 
			UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFiles == 1)
			{
				// Get the path of the file.
				if (0 != DragQueryFile(hDrop, 0, m_pszFileName, 
					ARRAYSIZE(m_pszFileName)))
				{
					hr = S_OK;					
				}
			}

			GlobalUnlock(stm.hGlobal);
		}

		ReleaseStgMedium(&stm);
	}

	return hr;
}

STDMETHODIMP CContextMenuEx::QueryContextMenu( HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags )
{
	MENUITEMINFO mSendinfo;

	if (uFlags & CMF_DEFAULTONLY)
	{
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}

	memset((void*)&mSendinfo, 0, sizeof(mSendinfo));
	mSendinfo.cbSize		= sizeof(mSendinfo);
	mSendinfo.fMask			= MIIM_STRING | MIIM_CHECKMARKS | MIIM_ID | MIIM_STATE;
	mSendinfo.cch			= lstrlen(SZ_MENU_SD_OUT);
	mSendinfo.dwTypeData	= SZ_MENU_SD_OUT;
	mSendinfo.hbmpChecked	= m_hBitmapSend;
	mSendinfo.hbmpUnchecked = m_hBitmapSend;
	mSendinfo.fState		= MFS_ENABLED;
	mSendinfo.wID			= idCmdFirst + indexMenu;

	BOOL bRet = InsertMenuItem(hmenu, indexMenu++, TRUE, &mSendinfo);

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);// 返回值必须是添加的菜单项数
}

STDMETHODIMP CContextMenuEx::InvokeCommand( LPCMINVOKECOMMANDINFO lpici )
{
	HRESULT  hr = E_INVALIDARG;

	MessageBox(NULL, _T("发送到了安全管家"), NULL, MB_OK);

	return hr;
}

STDMETHODIMP CContextMenuEx::GetCommandString( UINT_PTR idCmd, UINT uType, UINT * pwReserved, LPSTR pszName, UINT cchMax )
{
	HRESULT  hr = E_INVALIDARG;

	return hr;
}
