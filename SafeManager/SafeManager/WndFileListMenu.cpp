#include "StdAfx.h"
#include "WndFileListMenu.h"


CWndFileListMenu::CWndFileListMenu(void):m_pFileHandle(NULL)
{
}


CWndFileListMenu::~CWndFileListMenu(void)
{
}

LRESULT CWndFileListMenu::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);

	m_pMenuList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("menu_list")));
	m_pMenuList->GetHeader()->SetVisible(false);

	return 0;
}

void CWndFileListMenu::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);
	if (msg.sType == DUI_MSGTYPE_SETFOCUS)
	{
		if (msg.pSender->GetName() == _T("menu_item_open_way"))
		{
			int i = 0;								
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		if (msg.pSender->GetName() == _T("menu_item_upload"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_UPLOAD, NULL, NULL);						
		}
		else if (msg.pSender->GetName() == _T("menu_item_newfolder"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_NEW_FOLDER, NULL, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_refresh"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_REFRESH, NULL, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_open"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_OPEN, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_open_way"))
		{
		}
		else if (msg.pSender->GetName() == _T("menu_item_copy"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_COPY, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_move"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_MOVE, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_export"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_EXPORT, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_rename"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_RENAME, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_delete"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_DELETE, (WPARAM)m_pFileHandle, NULL);
		}
		else if (msg.pSender->GetName() == _T("menu_item_attr"))
		{
			Close();
			::PostMessage(m_pMainWnd->GetHWND(), WM_COMMAND_ATTRIBUTE, (WPARAM)m_pFileHandle, NULL);
		}		
	}
}

LRESULT CWndFileListMenu::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( (HWND)wParam == m_hWnd ) bHandled = FALSE;
	else if(  (HWND)wParam == m_WndShadow.GetHwnd() ) {
		CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		::SetFocus( m_hWnd );
		bHandled = FALSE;
	}
	else {
		Close();
		bHandled = FALSE;
	}
	return 0;
}

void CWndFileListMenu::SetFileHandle( CFileHandle *pFileHandle )
{
	m_pFileHandle = pFileHandle;
	CControlUI *pControl = NULL;
	//未详细判断当前状态，比如正在上传或下载
	if (m_pFileHandle == NULL)
	{
		ShowMenuItem(_T("menu_item_open"),false);
		ShowMenuItem(_T("line_open"),false);
		ShowMenuItem(_T("menu_item_open_way"),false);
		ShowMenuItem(_T("line_open_way"),false);
		ShowMenuItem(_T("menu_item_copy"),false);
		ShowMenuItem(_T("menu_item_move"),false);
		ShowMenuItem(_T("line_copy_move"),false);
		ShowMenuItem(_T("menu_item_export"),false);
		ShowMenuItem(_T("line_export"),false);
		ShowMenuItem(_T("menu_item_rename"),false);
		ShowMenuItem(_T("line_rename"),false);
		ShowMenuItem(_T("menu_item_delete"),false);
		ShowMenuItem(_T("line_delete"),false);
	}
	else
	{
		if (m_pFileHandle->GetFileType() == 0)
		{
			ShowMenuItem(_T("menu_item_upload"),false);
			ShowMenuItem(_T("line_upload"),false);
			ShowMenuItem(_T("menu_item_newfolder"),false);
			ShowMenuItem(_T("line_newfolder"),false);
			ShowMenuItem(_T("menu_item_newfolder"),false);
			ShowMenuItem(_T("line_newfolder"),false);
			ShowMenuItem(_T("menu_item_open_way"),false);
			ShowMenuItem(_T("line_open_way"),false);		
			ShowMenuItem(_T("menu_item_refresh"), false);
			ShowMenuItem(_T("line_refresh"), false);
		}
		else
		{
			ShowMenuItem(_T("menu_item_upload"),false);
			ShowMenuItem(_T("line_upload"),false);
			ShowMenuItem(_T("menu_item_newfolder"),false);
			ShowMenuItem(_T("line_newfolder"),false);
			ShowMenuItem(_T("menu_item_newfolder"),false);
			ShowMenuItem(_T("line_newfolder"),false);
			ShowMenuItem(_T("menu_item_refresh"), false);
			ShowMenuItem(_T("line_refresh"), false);
		}
	}


	int nHeight = 0;
	int nAllHeight = 0;
	int nCount = m_pMenuList->GetCount();
	for (int i=0; i<nCount; i++)
	{
		pControl = m_pMenuList->GetItemAt(i);
		if (pControl->IsVisible())
		{
			nHeight = pControl->GetFixedHeight();
			nAllHeight += nHeight;
		}		
	}

	ASSERT(::IsWindow(m_hWnd));
	RECT rc = { 0 };
	::GetClientRect(m_hWnd, &rc);
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
	::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, nAllHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

CFileHandle * CWndFileListMenu::GetFileHandle()
{
	return m_pFileHandle;
}

void CWndFileListMenu::ShowMenuItem( LPCTSTR lpItemName, bool bShow /*= true*/ )
{
	CControlUI *pControl = m_PaintManager.FindControl(lpItemName);
	pControl->SetVisible(bShow);
}

void CWndFileListMenu::SetMainWnd( CWndMainFrame *pWnd )
{
	m_pMainWnd = pWnd;
}


