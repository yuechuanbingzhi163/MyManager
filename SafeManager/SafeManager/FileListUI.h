#pragma once



class CFileListUI :	public CListUI
{
public:
	CFileListUI(void);
	virtual ~CFileListUI(void);

	 void DoEvent(TEventUI& event);

	 CControlUI *GetItemByPt(POINT pt);
};

