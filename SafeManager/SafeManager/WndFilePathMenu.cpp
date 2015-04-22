#include "StdAfx.h"
#include "WndFilePathMenu.h"
#include "function.h"


CWndFilePathMenu::CWndFilePathMenu(void)
{
}


CWndFilePathMenu::~CWndFilePathMenu(void)
{
}

void CWndFilePathMenu::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		if ((msg.pSender->GetParent() != NULL) && 
			(msg.pSender->GetParent()->GetParent() != NULL) &&
			(StrCmp(msg.pSender->GetParent()->GetParent()->GetName(), _T("menu_list")) == 0))
		{
			CListContainerElementUI *pEle = static_cast<CListContainerElementUI*>(msg.pSender);

			Close();
			::SendMessage(m_pMainWndPaintMangerUI->GetPaintWindow(), WM_LOAD_NEW_FILE_HANDLE, (WPARAM)(pEle->GetTag()), NULL);
		}
	}
}

LRESULT CWndFilePathMenu::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	m_pMenuList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("menu_list")));
	m_pMenuList->GetHeader()->SetVisible(false);

	return 0;
}

LRESULT CWndFilePathMenu::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if( (HWND)wParam == m_hWnd ) bHandled = FALSE;
	else {
		Close();
		bHandled = FALSE;
	}
	return 0;
}

void CWndFilePathMenu::SetMainWndPaintManagerUI( CPaintManagerUI *pPM )
{
	m_pMainWndPaintMangerUI = pPM;
}

void CWndFilePathMenu::SetFileHandle( CFileHandle *pFileHandle )
{
	m_pFileHandle = pFileHandle;
	CControlUI *pControl = NULL;
	CFileHandle *pTempFileHandle = NULL;

	int nCount = pFileHandle->GetChildrenCount();
	for (int i=0; i<nCount; i++)
	{
		pTempFileHandle = static_cast<CFileHandle*>(pFileHandle->GetChild(i));
		if (pTempFileHandle->GetFileType() == 0)
		{
			AddListItem(pTempFileHandle);
		}		
	}
	
	int nHeight = 0;
	int nAllHeight = 0;
	nCount = m_pMenuList->GetCount();
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

void CWndFilePathMenu::AddListItem( CFileHandle *pFileHandle )
{
	CListContainerElementUI *pEle = new CListContainerElementUI;
	pEle->SetFixedHeight(25);
	pEle->SetTag((UINT_PTR)pFileHandle);
	m_pMenuList->Add(pEle);

	CHorizontalLayoutUI *pHor = new CHorizontalLayoutUI;
	pEle->Add(pHor);

	string_t strIconName = _T("dir_icon_small");
	HBITMAP hBmp;
	int nWidth = 0;
	int nHeight =0;

	if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
	{
		hBmp = GetIconByFileType(NULL, true);

		BITMAP bmp;
		GetObject(hBmp, sizeof(BITMAP), &bmp);
		nWidth = bmp.bmWidth;
		nHeight = bmp.bmHeight;

		m_PaintManager.AddImage(strIconName.c_str(), hBmp, nWidth, nHeight, false);
	}

	const TImageInfo *pImgInfo = m_PaintManager.GetImage(strIconName.c_str());
	nWidth = pImgInfo->nX;
	nHeight = pImgInfo->nY;

	TCHAR bkImg[MAX_PATH] = {0};
	StringCchPrintf(bkImg, _countof(bkImg), _T("file='%s' dest='5,4,21,20'"), strIconName.c_str());

	CControlUI *pControl = new CControlUI;
	pControl->SetFixedWidth(26);
	pControl->SetAttribute(_T("bkimage"), bkImg);
	pHor->Add(pControl);

	CLabelUI *pFileName = new CLabelUI;
	pFileName->SetText(pFileHandle->GetFileName().c_str());
	pFileName->SetAttribute(_T("textpadding"), _T("10,0,0,0"));
	pFileName->SetAttribute(_T("endellipsis"), _T("true"));
	pHor->Add(pFileName);
}
