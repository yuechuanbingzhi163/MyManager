#include "stdafx.h"
#include "UIOption.h"

namespace DuiLib
{
	COptionUI::COptionUI() : m_bSelected(false), m_dwSelectedTextColor(0)
	{
	}

	COptionUI::~COptionUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR COptionUI::GetClass() const
	{
		return _T("OptionUI");
	}

	LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<COptionUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CControlUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
	}

	LPCTSTR COptionUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void COptionUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool COptionUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionUI::Selected(bool bSelected)
	{
		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
						COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) {
							pControl->Selected(false);							
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool COptionUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void COptionUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	LPCTSTR COptionUI::GetSelectedImage()
	{
		return m_sSelectedImage;
	}

	void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
	{
		m_sSelectedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedHotImage()
	{
		return m_sSelectedHotImage;
	}

	void COptionUI::SetSelectedHotImage( LPCTSTR pStrImage )
	{
		m_sSelectedHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedPushedImage()
	{
		return m_sSelectedPushedImage;
	}

	void COptionUI::SetSelectedPushedImage(LPCTSTR pStrImage)
	{
		m_sSelectedPushedImage = pStrImage;
		Invalidate();
	}

	void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_dwSelectedTextColor = dwTextColor;
	}

	DWORD COptionUI::GetSelectedTextColor()
	{
		if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
		return m_dwSelectedTextColor;
	}

	void COptionUI::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD COptionUI::GetSelectBkColor()
	{
		return m_dwSelectedBkColor;
	}

	LPCTSTR COptionUI::GetForeImage()
	{
		return m_sForeImage;
	}

	void COptionUI::SetForeImage(LPCTSTR pStrImage)
	{
		m_sForeImage = pStrImage;
		Invalidate();
	}

	SIZE COptionUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcscmp(pstrName, _T("selected")) == 0 ) Selected(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("selectedimage")) == 0 ) SetSelectedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedhotimage")) == 0 ) SetSelectedHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedpushedimage")) == 0 ) SetSelectedPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedTextColor(clrColor);
		}
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void COptionUI::PaintStatusImage(HDC hDC)
	{

		if( (m_uButtonState & UISTATE_PUSHED) != 0 && IsSelected() && !m_sSelectedPushedImage.IsEmpty()) {
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedPushedImage) )
				m_sSelectedPushedImage.Empty();
			else goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 && IsSelected() && !m_sSelectedHotImage.IsEmpty()) {
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage) )
				m_sSelectedHotImage.Empty();
			else goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
			if( !m_sSelectedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedImage) ) m_sSelectedImage.Empty();
				else goto Label_ForeImage;
			}
			else if(m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
				return;
			}	
		}

		CButtonUI::PaintStatusImage(hDC);

Label_ForeImage:
		if( !m_sForeImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}

	}

	void COptionUI::PaintText(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				NULL, NULL, nLinks, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
			CButtonUI::PaintText(hDC);
	}

	void COptionUI::DoEvent( TEventUI& event )
	{

		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KEYDOWN )
		{
			if (IsKeyboardEnabled()) {
				if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_SETCURSOR ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CLabelUI::DoEvent(event);
	}

}