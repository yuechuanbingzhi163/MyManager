#pragma once
#include "WndShadow.h"

class CWndMessageBox: public WindowImplBase
{
public:
	CWndMessageBox(void);
	~CWndMessageBox(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndMessageBox.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndMessageBox"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	void SetTitleAndTip(LPCTSTR lpTitle, LPCTSTR lpTip, DWORD dwTipColor=0xffff0000);

private:
	CWndShadow m_WndShadow;
};

