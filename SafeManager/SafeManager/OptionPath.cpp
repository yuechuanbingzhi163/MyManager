#include "StdAfx.h"
#include "OptionPath.h"
#include "WndFilePathMenu.h"


COptionPath::COptionPath(void):m_pFileHandle(NULL)
{
}


COptionPath::~COptionPath(void)
{
}

void COptionPath::SetFileHandle( CFileHandle *pFileHandle )
{
	m_pFileHandle = pFileHandle;
}

CFileHandle * COptionPath::GetFileHandle()
{
	return m_pFileHandle;
}

void COptionPath::SetIsPathName( bool b )
{
	m_bIsPathName = b;
}

bool COptionPath::IsPathName()
{
	return m_bIsPathName;
}

void COptionPath::DoEvent( TEventUI& event )
{
	__super::DoEvent(event);

	if (event.Type == UIEVENT_BUTTONUP)
	{
		if (m_bIsPathName)
		{
			::SendMessage(GetManager()->GetPaintWindow(), WM_LOAD_NEW_FILE_HANDLE, (WPARAM)m_pFileHandle, NULL);
		}
		else
		{
			if (m_pFileHandle->GetChildrenCount() == 0)
			{
				return;
			}

			POINT pt;
			pt.x = m_rcItem.right;
			pt.y = m_rcItem.bottom;
			::ClientToScreen(GetManager()->GetPaintWindow(), &pt);

			CWndFilePathMenu *pWnd = new CWndFilePathMenu;
			pWnd->Create(NULL, _T("FileListMenu"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);		
			pWnd->SetMainWndPaintManagerUI(GetManager());
			pWnd->SetFileHandle(m_pFileHandle);
			pWnd->ShowWindow();

			HDC   hdc=::GetDC(NULL);   //获得屏幕设备描述表句柄   
			int   ScrWidth=GetDeviceCaps(hdc,HORZRES);   //获取屏幕水平分辨率   
			int   ScrHeight=GetDeviceCaps(hdc,VERTRES);     //获取屏幕垂直分辨率  
			::ReleaseDC(NULL,hdc);   //释放屏幕设备描述表

			SetForegroundWindow(pWnd->GetHWND());

			CDuiRect rcWnd;
			::GetWindowRect(pWnd->GetHWND(), &rcWnd);

			POINT ptNew = pt;

			if (pt.x + rcWnd.GetWidth() > ScrWidth)
			{
				ptNew.x = pt.x - rcWnd.GetWidth();			
			}

			if (pt.y + rcWnd.GetHeight() > ScrHeight)
			{
				ptNew.y = pt.y - rcWnd.GetHeight();
			}

			::SetWindowPos(pWnd->GetHWND(), HWND_TOPMOST, ptNew.x, ptNew.y, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_SHOWWINDOW );

		}
	}
}
