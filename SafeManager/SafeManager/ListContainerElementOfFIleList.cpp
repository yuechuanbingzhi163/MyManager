#include "StdAfx.h"
#include "ListContainerElementOfFIleList.h"


CListContainerElementOfFileList::CListContainerElementOfFileList(void):m_pFileHandle(NULL)
{
}


CListContainerElementOfFileList::~CListContainerElementOfFileList(void)
{
}

void CListContainerElementOfFileList::SetFileIconName( LPCTSTR lpIconName )
{
	m_file_icon_name = lpIconName;
}

string_t CListContainerElementOfFileList::GetFileIconName()
{
	return m_file_icon_name;
}

void CListContainerElementOfFileList::SetFileHandle( CFileHandle *pFileHandle )
{
	m_pFileHandle = pFileHandle;
}

CFileHandle * CListContainerElementOfFileList::GetFileHandle()
{
	return m_pFileHandle;
}

void CListContainerElementOfFileList::DoEvent( TEventUI& event )
{
	__super::DoEvent(event);	

	if( event.Type == UIEVENT_DBLCLICK )
	{
		m_pManager->SendNotify(this, DUI_MSGTYPE_FILE_LIST_DBCLICK, event.wParam, event.lParam);
	}
	else if (event.Type == UIEVENT_RBUTTONUP)
	{
		
	}

}
