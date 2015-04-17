#include "StdAfx.h"
#include "WndFeedBack.h"
#include "function.h"


CWndFeedBack::CWndFeedBack(void)
{
	m_QR_Code_WeiXin = _T("http://www.cherilead.com");
	m_QR_Code_WeiBo = _T("http://www.cherilead.com");
}


CWndFeedBack::~CWndFeedBack(void)
{
}

void CWndFeedBack::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_web"))
		{
			CButtonUI *pBtn = static_cast<CButtonUI*>(msg.pSender);
			CDuiString strLink = pBtn->GetText();
			::ShellExecute(NULL, _T("open"), strLink.GetData(), NULL, NULL, SW_SHOWNORMAL);
		}
		else if (msg.pSender->GetName() == _T("btn_feedback"))
		{
			CButtonUI *pBtn = static_cast<CButtonUI*>(msg.pSender);
			CDuiString strLink = pBtn->GetText();
			::ShellExecute(NULL, _T("open"), strLink.GetData(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

LRESULT CWndFeedBack::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CControlUI *pQR_Code = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("con_weixin")));
	int nWidth = pQR_Code->GetFixedWidth();
	int nHeight = pQR_Code->GetFixedHeight();

	HBITMAP hQRBitmap = CreateQRCode( m_QR_Code_WeiXin.c_str(), 100, 100);

	if (m_PaintManager.GetImage(_T("weixin_QR_bmp.png")) != NULL)
	{
		m_PaintManager.RemoveImage(_T("weixin_QR_bmp.png"));
	}

	m_PaintManager.AddImage(_T("weixin_QR_bmp.png"), hQRBitmap, 100, 100, false);
	pQR_Code->SetBkImage(_T("weixin_QR_bmp.png"));


	pQR_Code = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("con_weibo")));
	nWidth = pQR_Code->GetFixedWidth();
	nHeight = pQR_Code->GetFixedHeight();

	HBITMAP hQRBitmap1 = CreateQRCode( m_QR_Code_WeiBo.c_str(), 100, 100);

	if (m_PaintManager.GetImage(_T("weibo_QR_bmp.png")) != NULL)
	{
		m_PaintManager.RemoveImage(_T("weibo_QR_bmp.png"));
	}

	m_PaintManager.AddImage(_T("weibo_QR_bmp.png"), hQRBitmap1, 100, 100, false);
	pQR_Code->SetBkImage(_T("weibo_QR_bmp.png"));

	return 0;
}
