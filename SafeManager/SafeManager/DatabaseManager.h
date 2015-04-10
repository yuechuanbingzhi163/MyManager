#pragma once
#include "CppSQLite3.h"
#include "Single.h"

class CDatabaseManager
{
public:		
	~CDatabaseManager(void);	

	static CDatabaseManager *GetSingleInstance();

	static void GetDirectChildren(LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos);
	static void GetDirectDirectoryChildren(LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos);
	static void GetDirectFileChildren(LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos);

	static void GetAllFileNameHaveSubstring(LPCTSTR lpSubstring, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos);

	static void InsertRecordToTableFileBaseInfo(LPCTSTR lpFileID, int nFileType, LPCTSTR lpFileName, LPCTSTR lpParentID, UINT64 uFileSize, INT64 nModifyTime, int nFileTag);
	static void InsertRecordToTableFileFragInfo(LPCTSTR lpFileID, LPCTSTR lpFragIDFirst, LPCTSTR lpFragIDSecond, LPCTSTR lpFragIDThird);

	static void QueryFileBaseInfoRecordByFileID(LPCTSTR lpID, STDBFILEBASEINFO &Info);
	static void QueryFileFragInfoRecordByFileID(LPCTSTR lpID, FRAG_INFO_T &Info);

	static void DeleteFileBaseInfoRecordByFileID(LPCTSTR lpID);
	static void DeleteFileFragInfoRecordByFileID(LPCTSTR lpID);

	static void UpDateFileName(LPCTSTR lpID, LPCTSTR lpNewName);

	static void UpdateFileParentID(LPCTSTR lpID, LPCTSTR lpParentID);

private:
		static CDatabaseManager *m_SingleInstance;

		void Init();

		static CppSQLite3DB m_SqlliteDB;

		static string_t  m_Table_File_Base_Info_Name;
		static string_t  m_Table_File_Frags_Info_Name;

private:
	CDatabaseManager(void);
	class destroy
	{
	public:
		destroy();
		~destroy();
	};
};

