#pragma once

#include "FileHandle.h"

class CWndMainFrame;

class CWndMoveOrPasteTo: public WindowImplBase
{
public:
	CWndMoveOrPasteTo(CWndMainFrame *pWnd, BOOL b);
	~CWndMoveOrPasteTo(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndMoveOrPasteTo.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndMoveOrPasteTo"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	

	void Init();
	void EnumFileHandleTreeAddToTree(CTreeNodeUI *pParent);
	CTreeNodeUI *CreateTreeNode(CFileHandle *pFileHandle);

	CFileHandle *GetSelFileHandle();

private:


	CFileHandle *m_pSelFileHandle;

	CWndMainFrame *m_WndMain;

	BOOL m_IsShowDestFileHandle;
};

