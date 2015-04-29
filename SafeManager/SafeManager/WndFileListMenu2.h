#pragma once
#include "FileHandle.h"

class CWndFileListMenu2 : public CMenuWnd
{
public:
	CWndFileListMenu2(void);
	~CWndFileListMenu2(void);

	void SetFileHandle(CFileHandle *pFileHandle, bool UpdateItems = false);

private:
	CFileHandle *m_pFileHandle;
};

