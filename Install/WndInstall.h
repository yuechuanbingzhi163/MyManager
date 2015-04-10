#pragma once
#include "WndShadow.h"

class CWndInstall : public WindowImplBase
{
public:
	CWndInstall(void);
	~CWndInstall(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\Install\\"); };
	CDuiString GetSkinFile() { return _T("WndInstall.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndInstall"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	void SetInstallProgress(LPCTSTR lpTip, int value);

private:
	void ExpandSelfInstall();
	void CollapseSelfInstall();

	void Init();	


private:
	CWndShadow m_WndShadow;

	int m_nCurPage;
};

