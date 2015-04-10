#include "StdAfx.h"
#include "WndTray.h"
#include "ShortcutEditUI.h"
#include "MainFrameWork.h"
#include "WndConfig.h"
#include "WndQuit.h"
#include "WndFeedBack.h"


CWndTray::CWndTray(void)
{
}


CWndTray::~CWndTray(void)
{
}

void CWndTray::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_set"))
		{
			HandleSet();
		}
		else if (msg.pSender->GetName() == _T("btn_feedback"))
		{
			HandleFeedBack();
		}
		else if (msg.pSender->GetName() == _T("btn_quit"))
		{
			HandleQuit();
		}
	}
}

LRESULT CWndTray::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);

	CListUI *pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("menulist")));
	pList->GetHeader()->SetVisible(false);

	return 0;
}

LRESULT CWndTray::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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

void CWndTray::HandleSet()
{
	HWND hwnd = ::FindWindow(_T("CWndConfig"), _T("配置窗口"));

	if (hwnd == NULL)
	{
		CWndConfig *pWndConfig = new CWndConfig;
		pWndConfig->Create(NULL, _T("配置窗口"), UI_WNDSTYLE_DIALOG, 0);
		pWndConfig->CenterWindow();
		pWndConfig->ShowWindow();
	}
	else
	{
		::BringWindowToTop(hwnd);
	}

	Close();
}

void CWndTray::HandleFeedBack()
{
	HWND hwnd = ::FindWindow(_T("CWndFeedBack"), _T("反馈窗口"));

	if (hwnd == NULL)
	{
		CWndFeedBack *pWndConfig = new CWndFeedBack;
		pWndConfig->Create(NULL, _T("反馈窗口"), UI_WNDSTYLE_DIALOG, 0);
		pWndConfig->CenterWindow();
		pWndConfig->ShowWindow();
	}
	else
	{
		::BringWindowToTop(hwnd);
	}

	Close();
}

void CWndTray::HandleQuit()
{
	HWND hwnd = ::FindWindow(_T("CWndQuit"), _T("退出窗口"));

	if (hwnd == NULL)
	{
		CWndQuit *pWndConfig = new CWndQuit;
		pWndConfig->Create(NULL, _T("退出窗口"), UI_WNDSTYLE_DIALOG, 0);
		pWndConfig->CenterWindow();
		pWndConfig->ShowWindow();
	}
	else
	{
		::BringWindowToTop(hwnd);
	}

	Close();
}


