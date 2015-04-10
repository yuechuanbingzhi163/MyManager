#pragma once
#include "WndShadow.h"
class CWndConfig : public WindowImplBase
{
public:
	CWndConfig(void);
	~CWndConfig(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndConfig.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndConfig"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	void Init(LPCTSTR lpDiskName, bool bUserShortcutKey, UINT nModifiers, UINT nVirtualKey, bool bShowShortcutIcon, bool bShowTrayIcon, bool bSaveSource);

private:
	CWndShadow m_WndShadow;
};

