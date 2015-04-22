#pragma once
#include "FileHandle.h"

class CWndFilePathMenu :	public WindowImplBase
{
public:
	CWndFilePathMenu(void);
	virtual ~CWndFilePathMenu(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndFilePathMenu.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndFilePathMenu"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetMainWndPaintManagerUI(CPaintManagerUI *pPM);

	void SetFileHandle(CFileHandle *pFileHandle);


	void AddListItem(CFileHandle *pFileHandle);

private:
	CListUI *m_pMenuList;

	CPaintManagerUI* m_pMainWndPaintMangerUI;

	CFileHandle *m_pFileHandle;
};

