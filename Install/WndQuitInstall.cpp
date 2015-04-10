#include "StdAfx.h"
#include "WndQuitInstall.h"


CWndQuitInstall::CWndQuitInstall(void)
{
}


CWndQuitInstall::~CWndQuitInstall(void)
{
}

void CWndQuitInstall::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_cancel"))
		{
			Close(0);
		}
		else if (msg.pSender->GetName() == _T("btn_ok"))
		{
			Close(1);
		}
		else if (msg.pSender->GetName() == _T("btn_close"))
		{
			Close(0);
		}
	}

}

LRESULT CWndQuitInstall::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	SetIcon(IDI_INSTALL);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);	

	return 0;
}
