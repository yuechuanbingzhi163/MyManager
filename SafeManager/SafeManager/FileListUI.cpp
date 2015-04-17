#include "StdAfx.h"
#include "FileListUI.h"


CFileListUI::CFileListUI(void)
{
}


CFileListUI::~CFileListUI(void)
{
}

void CFileListUI::DoEvent( TEventUI& event )
{
	if (event.Type == UIEVENT_RBUTTONUP)
	{
		CControlUI *pControl = GetItemByPt(event.ptMouse);
		
		if (pControl != NULL)
		{
			::SendMessage(m_pManager->GetPaintWindow(), WM_POP_MENU, (WPARAM)pControl, (LPARAM)(&event.ptMouse));
		}
		else
		{
			::SendMessage(m_pManager->GetPaintWindow(), WM_POP_MENU, (WPARAM)pControl, (LPARAM)(&event.ptMouse));
		}
	}
}

CControlUI * CFileListUI::GetItemByPt( POINT pt )
{
	CControlUI *pRet = NULL;

	CControlUI *pTemp = NULL;
	int nCount = GetCount();
	for (int i=0; i<nCount; i++)
	{
		pTemp = GetItemAt(i);

		const RECT rcPos = pTemp->GetPos();

		if (::PtInRect(&rcPos, pt))
		{
			pRet = pTemp;
			break;
		}
	}

	return pRet;
}
