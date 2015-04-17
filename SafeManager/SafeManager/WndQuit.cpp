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

	return 0;
}

void CWndQuit::HandleOk()
{
	CMainFrameWork::SingleInstance()->Exit_App();
}



