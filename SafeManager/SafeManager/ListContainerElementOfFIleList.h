#pragma once
#include "Single.h"
#include "FileHandle.h"

class CListContainerElementOfFileList : public CListContainerElementUI
{
public:
	CListContainerElementOfFileList(void);
	~CListContainerElementOfFileList(void);

public:
	void SetFileHandle(CFileHandle *pFileHandle);
	CFileHandle *GetFileHandle();

	void SetFileIconName(LPCTSTR lpIconName);
	string_t GetFileIconName( );

	void DoEvent(TEventUI& event);

private:
	CFileHandle *m_pFileHandle;

	string_t m_file_icon_name;

};

