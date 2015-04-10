#include "StdAfx.h"
#include "WndInstall.h"
#include "MainFrameWork.h"



CWndInstall::CWndInstall(void)
{
	m_nCurPage = 0;
}


CWndInstall::~CWndInstall(void)
{
}

void CWndInstall::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("protocol_option"))
		{
			COptionUI *pProtocol = static_cast<COptionUI*>(msg.pSender);
			CButtonUI *pBtnQuickInstall = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("quick_install")));
			bool bRet = pProtocol->IsSelected();

			if (bRet)
			{
				pBtnQuickInstall->SetEnabled(false);			
			}
			else
			{
				pBtnQuickInstall->SetEnabled(true);			
			}			
		}
		else if (msg.pSender->GetName() == _T("protocol_btn"))
		{
			CVerticalLayoutUI *pProtocolVer = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("firstpage_protocol")));
			pProtocolVer->SetVisible(true);
		}
		else if (msg.pSender->GetName() == _T("protocol_close_btn"))
		{
			CVerticalLayoutUI *pProtocolVer = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("firstpage_protocol")));
			pProtocolVer->SetVisible(false);
		}
		else if (msg.pSender->GetName() == _T("sef_install"))
		{
			COptionUI *pSelfInstall = static_cast<COptionUI*>(msg.pSender);
			bool bRet = pSelfInstall->IsSelected();
			if (bRet)
			{
				CollapseSelfInstall();
			}
			else
			{
				ExpandSelfInstall();
			}
		}
		else if (msg.pSender->GetName() == _T("btn_install_path"))
		{
			TCHAR szPathName[MAX_PATH] = {0};
			BROWSEINFO bInfo = {0};
			bInfo.hwndOwner = m_hWnd;
			bInfo.lpszTitle = _T("请选择安装路径");
			bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			LPITEMIDLIST lpDlist;
			lpDlist = SHBrowseForFolder(&bInfo);
			if (lpDlist != NULL)
			{
				SHGetPathFromIDList(lpDlist, szPathName);
				
				int Len = _tcslen(szPathName);
				if (Len == 3)
				{
					szPathName[2] = _T('\0');
				}

				StringCchCat(szPathName, _countof(szPathName), _T("\\CherileadSafeManager"));

				CEditUI *pEditInstallPath = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_install_path")));
				pEditInstallPath->SetText(szPathName);
				pEditInstallPath->SetToolTip(szPathName);

			}

		}
		else if (msg.pSender->GetName() == _T("quick_install"))
		{
			CTabLayoutUI *pTab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab_install_set_and_progress")));
			pTab->SelectItem(1);

			m_nCurPage = 1;

			CEditUI *pInstallPath = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_install_path")));
			CDuiString strPath = pInstallPath->GetText();
			CMainFrameWork::SingleInstance()->SetInstallPath(strPath.GetData());

			CButtonUI *pBtnClose = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("close_btn")));
			pBtnClose->SetEnabled(false);

			CMainFrameWork::SingleInstance()->StartInstall();
		}
		else if (msg.pSender->GetName() == _T("close_btn"))
		{
			if (m_nCurPage == 0)
			{
				CWndQuitInstall *pQuitWnd = new CWndQuitInstall;
				pQuitWnd->Create(m_hWnd, _T("退出安装"), UI_WNDSTYLE_DIALOG , 0);
				pQuitWnd->CenterWindow();
				UINT uRet = pQuitWnd->ShowModal();

				if (uRet == 1)
				{
					Close();
					PostQuitMessage(0);					
				}
			}
			else if (m_nCurPage == 1)
			{
				HANDLE hThread_Install = CMainFrameWork::SingleInstance()->GetThreadInstallHandle();
				if (hThread_Install != NULL)
				{
					SuspendThread(hThread_Install);
				}

				CWndQuitInstall *pQuitWnd = new CWndQuitInstall;
				pQuitWnd->Create(m_hWnd, _T("退出安装"), UI_WNDSTYLE_DIALOG , 0);
				pQuitWnd->CenterWindow();
				UINT uRet = pQuitWnd->ShowModal();

				if (hThread_Install != NULL)
				{
					if (uRet == 0)
					{
						ResumeThread(hThread_Install);
					}
					else if (uRet == 1)
					{
						TerminateThread(hThread_Install, 0);

						CMainFrameWork::SingleInstance()->RollBack();
					}
				}				
			}
		}
	}

}

LRESULT CWndInstall::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	SetIcon(IDI_INSTALL);

	Init();

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);	

	return 0;
}

void CWndInstall::ExpandSelfInstall()
{
	CVerticalLayoutUI *pVerSelf = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ver_self_install")));
	pVerSelf->SetVisible(true);

	CControlUI *pSpace = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("con_quick_install")));
	pSpace->SetVisible(false);

	CButtonUI *pQuickInstall = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("quick_install")));
	pQuickInstall->SetVisible(false);
}

void CWndInstall::CollapseSelfInstall()
{
	CVerticalLayoutUI *pVerSelf = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("ver_self_install")));
	pVerSelf->SetVisible(false);

	CControlUI *pSpace = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("con_quick_install")));
	pSpace->SetVisible(true);

	CButtonUI *pQuickInstall = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("quick_install")));
	pQuickInstall->SetVisible(true);
}

void CWndInstall::Init()
{
	TCHAR systemdir[MAX_PATH] = {0};
	GetWindowsDirectory  (systemdir, MAX_PATH);

	LPTSTR lpTemp = StrChr(systemdir, _T(':'));

	TCHAR lpInstallPath[MAX_PATH] = {0};

	if (lpTemp != NULL)
	{
		*(lpTemp + 1) = _T('\0');
		StringCchPrintf(lpInstallPath, _countof(lpInstallPath), _T("%s"), systemdir);
		StringCchCat(lpInstallPath, _countof(lpInstallPath), _T("\\Program Files\\CheriLeadSafeManager\\"));

		CEditUI *pEditInstallPath = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_install_path")));
		pEditInstallPath->SetText(lpInstallPath);
		pEditInstallPath->SetToolTip(lpInstallPath);
	}
}

void CWndInstall::SetInstallProgress( LPCTSTR lpTip, int value )
{
	CLabelUI *pTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_progress_tip")));
	CLabelUI *pValue = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_progress_value")));
	CProgressUI *pProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("install_progress")));

	pTip->SetText(lpTip);
	pProgress->SetValue(value);

	TCHAR szValueTip[10] = {0};
	_itot(value, szValueTip, 10);

	StringCchCat(szValueTip, _countof(szValueTip), _T("%"));
	pValue->SetText(szValueTip);

	if (value > 100)
	{
		PostMessage(WM_EXIT_APP);
		int i = 0;
	}
}

LRESULT CWndInstall::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if (uMsg == WM_EXIT_APP)
	{
		Close(0);
		PostQuitMessage(0);
	}

	return 0;
}
