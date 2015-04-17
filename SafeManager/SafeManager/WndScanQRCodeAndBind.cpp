#include "StdAfx.h"
#include "WndScanQRCodeAndBind.h"
#include "function.h"
#include "MainFrameWork.h"

class CMainFrameWork;

CWndScanQRCodeAndBind::CWndScanQRCodeAndBind(void)
{
}


CWndScanQRCodeAndBind::~CWndScanQRCodeAndBind(void)
{
}

void CWndScanQRCodeAndBind::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_close"))
		{
			Close();
			CMainFrameWork::SingleInstance()->Exit_App();
		}
		else if (msg.pSender->GetName() == _T("btn_skip"))
		{
			int nRet = MessageBox(NULL, _T("模仿绑定"), _T("模仿绑定"), MB_OKCANCEL);
			if (nRet == IDOK)
			{
				Close();
				CMainFrameWork::SingleInstance()->RegisterApp();
				CMainFrameWork::SingleInstance()->ShowWndScanQRCodeOpen(NULL, 0, _T("http:\\www.baidu.com"));
			}
			else
			{
				Close();
				CMainFrameWork::SingleInstance()->ShowWndScanQRCodeOpen(NULL, 0, _T("http:\\www.baidu.com"));
			}
		}
	}

}

LRESULT CWndScanQRCodeAndBind::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	SetIcon(IDI_SAFEMANAGER);

	CControlUI *pQR_Code = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("QR_Code")));
	int nWidth = pQR_Code->GetFixedWidth();
	int nHeight = pQR_Code->GetFixedHeight();

	HBITMAP hQRBitmap = CreateQRCode( m_QR_Code_Data.c_str(), nWidth, nHeight);

	if (m_PaintManager.GetImage(_T("CWndScanQRCodeAndBind_QR_bmp.png")) != NULL)
	{
		m_PaintManager.RemoveImage(_T("CWndScanQRCodeAndBind_QR_bmp.png"));
	}

	m_PaintManager.AddImage(_T("CWndScanQRCodeAndBind_QR_bmp.png"), hQRBitmap, 122, 122, false);
	pQR_Code->SetBkImage(_T("CWndScanQRCodeAndBind_QR_bmp.png"));

	return 0;
}

LRESULT CWndScanQRCodeAndBind::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
	LRESULT lRet = __super::HandleMessage(uMsg, wParam, lParam);

	return lRet;
}

void CWndScanQRCodeAndBind::SetQRCodeData( LPCTSTR lpData )
{
	m_QR_Code_Data = lpData;
}
