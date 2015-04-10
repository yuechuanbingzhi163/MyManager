#pragma once
#include "WndShadow.h"
class CWndQuitInstall : public WindowImplBase
{
public:
	CWndQuitInstall(void);
	~CWndQuitInstall(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\Install\\"); };
	CDuiString GetSkinFile() { return _T("WndQuitInstall.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndQuitInstall"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CWndShadow m_WndShadow;
};

