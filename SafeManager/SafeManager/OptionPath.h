#pragma once
#include "FileHandle.h"

class COptionPath :	public COptionUI
{
public:
	COptionPath(void);
	virtual ~COptionPath(void);

	void DoEvent(TEventUI& event);

	void SetFileHandle(CFileHandle *pFileHandle);
	CFileHandle *GetFileHandle( );

	void SetIsPathName(bool b = true);
	bool IsPathName();

private:
	CFileHandle *m_pFileHandle;
	bool m_bIsPathName;
};

