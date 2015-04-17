#pragma once
#include "stdafx.h"


class CWndProgress :	public WindowImplBase
{
public:
	CWndProgress(void);
	~CWndProgress(void);

	LPCTSTR    GetWindowClassName() const   {   return _T("ProgerssWnd");  };
	CDuiString GetSkinFile() { return _T("WndProgress.xml");};
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnFinalMessage( HWND hWnd );

	void SetTip(LPCTSTR lpTip);
	void SetProgress(int value);

	void SetSecondCount(ULONG ul);
	ULONG GetSecondCount();
	CTextUI *GetAnimation();

private:
	ULONG m_SecondCount;
};

