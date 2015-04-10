#pragma once
#include "stdafx.h"

#define DUI_CTR_PATH_EDIT _T("ShortcutEdit")

class CShortcutEditUI : public CLabelUI
{
	friend class CShortcutEditWnd;

public:
	CShortcutEditUI(void);
	virtual ~CShortcutEditUI(void);

	void ShowEditWnd();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	void SetEnabled(bool bEnable = true);
	void SetText(LPCTSTR pstrText);
	void SetMaxChar(UINT uMax);
	UINT GetMaxChar();
	void SetReadOnly(bool bReadOnly);
	bool IsReadOnly() const;
	void SetPasswordMode(bool bPasswordMode);
	bool IsPasswordMode() const;
	void SetPasswordChar(TCHAR cPasswordChar);
	TCHAR GetPasswordChar() const;
	void SetNumberOnly(bool bNumberOnly);
	bool IsNumberOnly() const;
	int GetWindowStyls() const;

	LPCTSTR GetNormalImage();
	void SetNormalImage(LPCTSTR pStrImage);
	LPCTSTR GetHotImage();
	void SetHotImage(LPCTSTR pStrImage);
	LPCTSTR GetFocusedImage();
	void SetFocusedImage(LPCTSTR pStrImage);
	LPCTSTR GetDisabledImage();
	void SetDisabledImage(LPCTSTR pStrImage);
	void SetNativeEditBkColor(DWORD dwBkColor);
	DWORD GetNativeEditBkColor() const;

	void SetSel(long nStartChar, long nEndChar);
	void SetSelAll();
	void SetReplaceSel(LPCTSTR lpszReplace);

	void SetPos(RECT rc);
	void SetVisible(bool bVisible = true);
	void SetInternVisible(bool bVisible = true);
	SIZE EstimateSize(SIZE szAvailable);
	void DoEvent(TEventUI& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void PaintStatusImage(HDC hDC);
	void PaintText(HDC hDC);

protected:
	CShortcutEditWnd* m_pWindow;

	UINT m_uMaxChar;
	bool m_bReadOnly;
	bool m_bPasswordMode;
	TCHAR m_cPasswordChar;
	UINT m_uButtonState;
	CDuiString m_sNormalImage;
	CDuiString m_sHotImage;
	CDuiString m_sFocusedImage;
	CDuiString m_sDisabledImage;
	DWORD m_dwEditbkColor;
	int m_iWindowStyls;
};

class CShortcutEditWnd :	public CWindowWnd
{
public:
	CShortcutEditWnd( );
	virtual ~CShortcutEditWnd( );

	void Init(CShortcutEditUI* pOwner);
	RECT CalPos();

	LPCTSTR GetWindowClassName() const;
	LPCTSTR GetSuperClassName() const;
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	CShortcutEditUI* m_pOwner;
	HBRUSH m_hBkBrush;
	bool m_bInit;

};

