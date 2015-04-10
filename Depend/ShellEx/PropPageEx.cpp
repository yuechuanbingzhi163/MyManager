// PropPageEx.cpp : CPropPageEx 的实现

#include "stdafx.h"
#include "PropPageEx.h"
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

#define EXT_POINTER_PROP            SAFE_PROP_TITLE

// CPropPageEx

extern HINSTANCE         _hInstance;

INT_PTR CALLBACK FilePropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
UINT CALLBACK FilePropPageCallbackProc(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp);

IFACEMETHODIMP CPropPageEx::Initialize( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID )
{
	if(pDataObj == NULL)
		return E_INVALIDARG;

	// 初始化通用控件(属性页是通用控件)
	InitCommonControls();

	// 从IDataObject获得选中文件名，数据以CF_HDROP格式存储
	STGMEDIUM medium;
	FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	HRESULT hr = pDataObj->GetData(&fe, &medium);
	if(FAILED(hr))
		return E_INVALIDARG;

	HDROP hDrop = static_cast<HDROP>(medium.hGlobal);	


	if(DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0) == 1)
	{
		TCHAR szFileName[MAX_PATH] = {0};
		DragQueryFile(hDrop, 0, szFileName, sizeof(szFileName));

		LPTSTR lpExtension = PathFindExtension(szFileName);

		if (StrCmp(lpExtension, _T(".csm")) == 0)
		{
			hr = NOERROR;
		}
		else
		{
			hr = E_INVALIDARG;
		}		
	}else
		hr = E_INVALIDARG;

	ReleaseStgMedium(&medium);
	return hr;
}

IFACEMETHODIMP CPropPageEx::AddPages( LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam )
{
	// Create a property sheet page.

	PROPSHEETPAGE psp = { sizeof(psp) };
	psp.dwFlags = PSP_USETITLE | PSP_USECALLBACK ;
	psp.hInstance = _hInstance;
	psp.pszTemplate = MAKEINTRESOURCE(IDD_FILE_PROPPAGE);
	//psp.hIcon = hIcon;
	psp.pszTitle = SAFE_PROP_TITLE;
	psp.pfnDlgProc = FilePropPageDlgProc;
	psp.pcRefParent = NULL;
	psp.pfnCallback = FilePropPageCallbackProc;
	psp.lParam = reinterpret_cast<LPARAM>(this);

	HPROPSHEETPAGE hPage = CreatePropertySheetPage(&psp);
	if (hPage == NULL)
	{
		return E_OUTOFMEMORY;
	}

	if (pfnAddPage(hPage, lParam))
	{

		this->AddRef();
	}
	else
	{
		DestroyPropertySheetPage(hPage);
		return E_FAIL;
	}

	return S_OK;
}

INT_PTR CALLBACK FilePropPageDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			 LPPROPSHEETPAGE pPage = reinterpret_cast<LPPROPSHEETPAGE>(lParam);
			 if (pPage != NULL)
			 {
				 CPropPageEx *pExt = reinterpret_cast<CPropPageEx *>(pPage->lParam);

				 if (pExt != NULL)
				 {
					 HWND hwndMeta = GetDlgItem(hWnd, IDC_DATA);
					 SetWindowText(hWnd, _T("202222222222"));

					 // Store the object pointer with this particular page dialog.
					 SetProp(hWnd, EXT_POINTER_PROP, static_cast<HANDLE>(pExt));
				 }		
			 }		
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_CHANGEPROP_BUTTON:
				// User clicks the "Simulate Property Changing" button...

				// Simulate property changing. Inform the property sheet to 
				// enable the Apply button.
				SendMessage(GetParent(hWnd), PSM_CHANGED, 
					reinterpret_cast<WPARAM>(hWnd), 0);
				return TRUE;
			}
		}
		break;
		case WM_NOTIFY:
        {
            switch ((reinterpret_cast<LPNMHDR>(lParam))->code)
            {
            case PSN_APPLY:
                // The PSN_APPLY notification code is sent to every page in the 
                // property sheet to indicate that the user has clicked the OK, 
                // Close, or Apply button and wants all changes to take effect. 

                // Get the property sheet extension object pointer that was 
                // stored in the page dialog (See the handling of WM_INITDIALOG 
                // in FilePropPageDlgProc).
               /* FilePropSheetExt *pExt = static_cast<FilePropSheetExt *>(
                    GetProp(hWnd, EXT_POINTER_PROP));*/

                // Access the property sheet extension object.
                // ...

                // Return PSNRET_NOERROR to allow the property dialog to close if 
                // the user clicked OK.
                SetWindowLong(hWnd, DWLP_MSGRESULT, PSNRET_NOERROR);               
            }
			      break;
        }
		case WM_DESTROY:
			{
				// Remove the EXT_POINTER_PROP property from the page. 
				// The EXT_POINTER_PROP property stored the pointer to the 
				// FilePropSheetExt object.
				RemoveProp(hWnd, EXT_POINTER_PROP);
				break;
			}
	}
	
	return TRUE;
}

UINT CALLBACK FilePropPageCallbackProc( HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp )
{
	switch(uMsg)
	{
	case PSPCB_CREATE:
		{
			// Must return TRUE to enable the page to be created.
			return TRUE;
		}

	case PSPCB_RELEASE:
		{
			// When the callback function receives the PSPCB_RELEASE notification, 
			// the ppsp parameter of the PropSheetPageProc contains a pointer to 
			// the PROPSHEETPAGE structure. The lParam member of the PROPSHEETPAGE 
			// structure contains the extension pointer which can be used to 
			// release the object.

			// Release the property sheet extension object. This is called even 
			// if the property page was never actually displayed.
			CPropPageEx *pExt = reinterpret_cast<CPropPageEx *>(ppsp->lParam);
			if (pExt != NULL)
			{
				pExt->Release();
			}
		}
		break;
	}

	return FALSE;
	return 0;
}


IFACEMETHODIMP CPropPageEx::ReplacePage( UINT uPageID, LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam )
{
	 return E_NOTIMPL;
}
