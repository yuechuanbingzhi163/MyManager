#pragma once
#include "WndShadow.h"
#include "FileHandle.h"
#include "WndMainFrame.h"

class CWndMainFrame;
class CWndFileListMenu :	public WindowImplBase
{
public:
	CWndFileListMenu(void);
	virtual ~CWndFileListMenu(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndFileListMenu.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndFileListMenu"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetFileHandle(CFileHandle *pFileHandle);
	CFileHandle *GetFileHandle();

	void ShowMenuItem(LPCTSTR lpItemName, bool bShow = true);

	void SetMainWnd(CWndMainFrame *pWnd);

private:
	CWndShadow m_WndShadow;

	CFileHandle *m_pFileHandle;

	CListUI *m_pMenuList;

	CWndMainFrame *m_pMainWnd;
};

