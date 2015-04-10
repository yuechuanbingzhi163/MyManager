#include "StdAfx.h"
#include "WndScanQRCodeOpen.h"
#include "function.h"
#include "MainFrameWork.h"

class CMainFrameWork;

CWndScanQRCodeOpen::CWndScanQRCodeOpen(void)
{
}


CWndScanQRCodeOpen::~CWndScanQRCodeOpen(void)
{
}

void CWndScanQRCodeOpen::SetQRCodeData( LPCTSTR lpData )
{
	m_QR_Code_Data = lpData;
}

void CWndScanQRCodeOpen::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_close"))
		{
			int nRet = MessageBox(NULL, _T("模仿扫描打开(有效期20秒)"), _T("模仿扫描打开"), MB_OKCANCEL);
			if (nRet == IDOK)
			{
				CMainFrameWork::SingleInstance()->SetValidTime(20000);
				CMainFrameWork::SingleInstance()->ShowWndMainFrame();				
			}
			else
			{
			}
			Close();
		}
	}
}

LRESULT CWndScanQRCodeOpen::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	SetIcon(IDI_SAFEMANAGER);

	CLabelUI *pTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("tip")));

	if (m_nFlag == 0)
	{
		pTip->SetText(_T("APP扫码打开"));
		pTip->SetTextColor(0xff505050);
	}
	else if (m_nFlag == 1)
	{
		pTip->SetText(_T("对不起，文件授权已过期，请重新扫码继续授权"));
		pTip->SetTextColor(0xffFF0000);
	}

	CControlUI *pQR_Code = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("QR_Code")));
	int nWidth = pQR_Code->GetFixedWidth();
	int nHeight = pQR_Code->GetFixedHeight();

	HBITMAP hQRBitmap = CreateQRCode( m_QR_Code_Data.c_str(), nWidth, nHeight);

	if (m_PaintManager.GetImage(_T("CWndScanQRCodeAndOpen_QR_bmp.png")) != NULL)
	{
		m_PaintManager.RemoveImage(_T("CWndScanQRCodeAndOpen_QR_bmp.png"));
	}

	m_PaintManager.AddImage(_T("CWndScanQRCodeAndOpen_QR_bmp.png"), hQRBitmap, 122, 122, false);
	pQR_Code->SetBkImage(_T("CWndScanQRCodeAndOpen_QR_bmp.png"));


	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);

	return 0;
}

LRESULT CWndScanQRCodeOpen::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRet = __super::HandleMessage(uMsg, wParam, lParam);

	return lRet;
}

void CWndScanQRCodeOpen::SetFlag( int flag )
{
	m_nFlag = flag;
}
