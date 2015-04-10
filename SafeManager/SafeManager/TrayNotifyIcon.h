#pragma once

class CTrayNotifyIcon
{
public:
	CTrayNotifyIcon(void);
	~CTrayNotifyIcon(void);

	void Init(HINSTANCE hInstance, HWND hwnd);

	void SetIcon(UINT nIcon);

	bool AddNofityIcon();
	bool ModifyNotifyIcon();
	bool DeleteNotifyIcon();

private:
	NOTIFYICONDATA m_NotifyData;

	HINSTANCE m_hInstance;
};

