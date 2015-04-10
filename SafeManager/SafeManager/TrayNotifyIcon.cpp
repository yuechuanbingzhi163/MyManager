#include "StdAfx.h"
#include "TrayNotifyIcon.h"
#include "Single.h"


CTrayNotifyIcon::CTrayNotifyIcon(void)
{
}


CTrayNotifyIcon::~CTrayNotifyIcon(void)
{
}

void CTrayNotifyIcon::Init( HINSTANCE hInstance, HWND hWnd)
{
	ZeroMemory(&m_NotifyData, sizeof(NOTIFYICONDATA));

	m_NotifyData.cbSize = sizeof(NOTIFYICONDATA);
	m_NotifyData.hWnd = hWnd;
	m_NotifyData.uID = 0;
	m_NotifyData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_NotifyData.uCallbackMessage = WM_NIDTIP;
	
	m_hInstance = hInstance;
}

void CTrayNotifyIcon::SetIcon( UINT nIcon )
{
	m_NotifyData.hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(nIcon));
	m_NotifyData.uID = nIcon;
}

bool CTrayNotifyIcon::AddNofityIcon()
{
	return Shell_NotifyIcon(NIM_ADD, &m_NotifyData);
}

bool CTrayNotifyIcon::ModifyNotifyIcon()
{
	return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyData);
}

bool CTrayNotifyIcon::DeleteNotifyIcon()
{
	return Shell_NotifyIcon(NIM_DELETE, &m_NotifyData);
}
