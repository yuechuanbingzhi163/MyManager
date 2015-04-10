#pragma once
#include "WndShadow.h"
class CWndFeedBack : public WindowImplBase
{
public:
	CWndFeedBack(void);
	~CWndFeedBack(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndFeedBack.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndFeedBack"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

private:
	CWndShadow m_WndShadow;

	string_t m_QR_Code_WeiXin;
	string_t m_QR_Code_WeiBo;
};

