#include "StdAfx.h"
#include "MainFrameWork.h"
#include "WndQuit.h"


CWndQuit::CWndQuit(void)
{
}


CWndQuit::~CWndQuit(void)
{
}

void CWndQuit::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_ok"))
		{
			Close();
			HandleOk();
		}
	}
}

LRESULT CWndQuit::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);

	return 0;
}

void CWndQuit::HandleOk()
{
	CMainFrameWork::SingleInstance()->Exit_App();
}



