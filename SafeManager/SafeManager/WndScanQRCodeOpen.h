#pragma once


class CWndScanQRCodeOpen : public WindowImplBase
{
public:
	CWndScanQRCodeOpen(void);
	~CWndScanQRCodeOpen(void);

	void SetFlag(int flag);  //0:APP扫码打开    1:对不起，文件授权已过期，请重新扫码继续授权

	void SetQRCodeData(LPCTSTR lpData);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndScanQRCodeOpen.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndScanQRCodeOpen"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:


	string_t m_QR_Code_Data;

	int m_nFlag;
};

