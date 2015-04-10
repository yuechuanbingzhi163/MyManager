#include "StdAfx.h"
#include <Strsafe.h>
#include "WndProgress.h"
#include "Single.h"

 extern HANDLE  hMoveOrCopyEvent;

CWndProgress::CWndProgress(void)
{
	m_SecondCount = 0;
}


CWndProgress::~CWndProgress(void)
{
}

VOID CALLBACK TimerProc2( HWND hwnd, UINT uMsg,	UINT_PTR idEvent,	DWORD dwTime)
{
		CWndProgress *pWnd = (CWndProgress*)(GetWindowLong(hwnd, GWL_USERDATA));
		if (pWnd->GetSecondCount() % 4 == 0)
		{
			pWnd->GetAnimation()->SetText(_T("¡ª"));
		}
		else if (pWnd->GetSecondCount() % 4 == 1 )
		{
			pWnd->GetAnimation()->SetText(_T("\\"));
		}
		else if (pWnd->GetSecondCount() % 4 == 2 )
		{
			pWnd->GetAnimation()->SetText(_T("|"));
		}
		else if (pWnd->GetSecondCount() % 4 == 3 )
		{
			pWnd->GetAnimation()->SetText(_T("/"));
		}

		pWnd->SetSecondCount((pWnd->GetSecondCount()) + 1);

}

LRESULT CWndProgress::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg,wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(3);
	m_WndShadow.SetPosition(0, 0);	

	SetTimer(m_hWnd, NULL, 200, TimerProc2);
	
	SetEvent(hMoveOrCopyEvent);

	return 0;
}

void CWndProgress::Notify( TNotifyUI &msg )
{

}

void CWndProgress::SetTip( LPCTSTR lpTip )
{
	CTextUI *pTip = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("tip")));
	if (pTip != NULL)
	{
		pTip->SetText(lpTip);
	}
}

void CWndProgress::SetProgress( int value )
{
	CProgressUI *pProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("progress")));
	if (pProgress != NULL)
	{
		pProgress->SetValue(value);
	}

	if (value > 100)
	{
		Sleep(1000);
	}
}

void CWndProgress::OnFinalMessage( HWND hWnd )
{
	__super::OnFinalMessage(hWnd);
	KillTimer(m_hWnd, NULL);
	delete this;
}

ULONG CWndProgress::GetSecondCount()
{
	return m_SecondCount;
}

void CWndProgress::SetSecondCount( ULONG ul )
{
	m_SecondCount = ul;
}

CTextUI * CWndProgress::GetAnimation()
{
	CTextUI *pUI = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("animation")));
	return pUI;
}

LRESULT CWndProgress::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(uMsg)
	{
	case WM_UPDATE_PROGRESS:
		{
			int nProgress = (int)wParam;
			LPCTSTR lp = (LPCTSTR)lParam;
			SetProgress(nProgress);
			SetTip(lp);
			break;
		}
	default:
		{

		}
	}

	return 0;
}










