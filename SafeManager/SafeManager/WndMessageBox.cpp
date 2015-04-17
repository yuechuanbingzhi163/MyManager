#include "StdAfx.h"
#include "WndMessageBox.h"


CWndMessageBox::CWndMessageBox(void)
{
}


CWndMessageBox::~CWndMessageBox(void)
{
}

void CWndMessageBox::Notify( TNotifyUI &msg )
{
		__super::Notify(msg);
}

LRESULT CWndMessageBox::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
		__super::OnCreate(uMsg, wParam, lParam, bHandled);

		return 0;
}

void CWndMessageBox::SetTitleAndTip( LPCTSTR lpTitle, LPCTSTR lpTip, DWORD dwTipColor/*=0xffff0000*/ )
{
	CLabelUI *pTitle = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("title")));
	CLabelUI *pTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("tip")));

	pTitle->SetText(lpTitle);
	pTip->SetText(lpTip);
	pTip->SetTextColor(dwTipColor);
}
