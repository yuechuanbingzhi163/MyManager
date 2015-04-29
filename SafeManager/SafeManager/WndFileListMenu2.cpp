#include "StdAfx.h"
#include "WndFileListMenu2.h"


CWndFileListMenu2::CWndFileListMenu2(void)
{
}


CWndFileListMenu2::~CWndFileListMenu2(void)
{
}

void CWndFileListMenu2::SetFileHandle( CFileHandle *pFileHandle, bool UpdateItems)
{
	m_pFileHandle = pFileHandle;
}
