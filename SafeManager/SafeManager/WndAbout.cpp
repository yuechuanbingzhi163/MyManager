#include "StdAfx.h"
#include "WndAbout.h"


CWndAbout::CWndAbout(void)
{
}


CWndAbout::~CWndAbout(void)
{
}

void CWndAbout::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_link"))
		{
			CButtonUI *pBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btn_link")));
			CDuiString strLink = pBtn->GetText();
			::ShellExecute(NULL, _T("open"), strLink.GetData(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

LRESULT CWndAbout::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);


	return 0;
}
