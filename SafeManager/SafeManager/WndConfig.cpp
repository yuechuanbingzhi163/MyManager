#include "StdAfx.h"
#include "WndConfig.h"
#include "ShortcutEditUI.h"
#include "MainFrameWork.h"
#include "WndAbout.h"


CWndConfig::CWndConfig(void)
{
}


CWndConfig::~CWndConfig(void)
{
}

void CWndConfig::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_ok"))
		{
			CEditUI *pDiskNameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("disk_name")));
			CComboBoxUI *pVirtualKeyCombo = static_cast<CComboBoxUI*>(m_PaintManager.FindControl(_T("combo_shortcut")));
			COptionUI *pUseShortcutKey = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_shortcut")));
			COptionUI *pOptShowShortcutIcon = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("shortcut_show")));
			COptionUI *pOptShowTrayIcon = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("trayicon_show")));
			COptionUI *pOptSaveSourceFile = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("file_save")));

			CDuiString strDiskName = pDiskNameEdit->GetText();
			bool bUserShortcutKey = pUseShortcutKey->IsSelected();
			UINT nVirtualKey = pVirtualKeyCombo->GetCurSel() + 65;
			bool bShowShortcutIcon = pOptShowShortcutIcon->IsSelected() ? false:true;
			bool bShowTrayIcon = pOptShowTrayIcon->IsSelected()? false:true;
			bool bSaveSourceFile = pOptSaveSourceFile->IsSelected();

			CMainFrameWork::SingleInstance()->SetConfig(strDiskName.GetData(), bUserShortcutKey, MOD_CONTROL | MOD_SHIFT, nVirtualKey, bShowShortcutIcon, bShowTrayIcon, bSaveSourceFile);
					
		Close(1);
		}
		else if (msg.pSender->GetName() == _T("btn_question"))
		{
			HWND hwnd = ::FindWindow(_T("CWndAbout"), _T("关于"));

			if (hwnd == NULL)
			{
				CWndAbout *pWndConfig = new CWndAbout;
				pWndConfig->Create(NULL, _T("关于"), UI_WNDSTYLE_DIALOG, 0);
				pWndConfig->CenterWindow();
				pWndConfig->ShowWindow();
			}
			else
			{
				::BringWindowToTop(hwnd);
			}
		}
	}
}

LRESULT CWndConfig::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	CWndShadow::Initialize(m_PaintManager.GetInstance() );
	m_WndShadow.Create(m_hWnd);
	m_WndShadow.SetSize(4);
	m_WndShadow.SetPosition(0, 0);

	string_t strDiskName = CMainFrameWork::SingleInstance()->GetConfig_DiskName();
	bool bUserShortcutKey = CMainFrameWork::SingleInstance()->GetConfig_UseShortcutKey();
	UINT nVirtualKey = CMainFrameWork::SingleInstance()->GetConfig_VirturlKey();
	bool bShowShortcutIcon = CMainFrameWork::SingleInstance()->GetConfig_ShowShortcutIcon();
	bool bShowTrayIcon = CMainFrameWork::SingleInstance()->GetConfig_ShowTrayIcon();
	bool bSaveSourceFile = CMainFrameWork::SingleInstance()->GetConfig_SaveSourceFile();

	Init(strDiskName.c_str(), bUserShortcutKey, 0, nVirtualKey, bShowShortcutIcon, bShowTrayIcon, bSaveSourceFile);

	return 0;
}

void CWndConfig::Init( LPCTSTR lpDiskName, bool bUserShortcutKey, UINT nModifiers, UINT nVirtualKey, bool bShowShortcutIcon, bool bShowTrayIcon, bool bSaveSource )
{
	CEditUI *pDiskNameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("disk_name")));
	CComboBoxUI *pVirtualKeyCombo = static_cast<CComboBoxUI*>(m_PaintManager.FindControl(_T("combo_shortcut")));
	COptionUI *pUseShortcutKey = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("opt_shortcut")));
	COptionUI *pOptShowShortcutIcon = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("shortcut_show")));
	COptionUI *pOptShowTrayIcon = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("trayicon_show")));
	COptionUI *pOptSaveSourceFile = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("file_save")));

	pDiskNameEdit->SetText(lpDiskName);
	pUseShortcutKey->Selected(bUserShortcutKey);
	pVirtualKeyCombo->SelectItem(nVirtualKey - 65);
	pOptShowShortcutIcon->Selected(!bShowShortcutIcon);
	pOptShowTrayIcon->Selected(!bShowTrayIcon);
	pOptSaveSourceFile->Selected(bSaveSource);
}
