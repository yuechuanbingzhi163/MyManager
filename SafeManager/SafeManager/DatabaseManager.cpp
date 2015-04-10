#include "StdAfx.h"
#include "DatabaseManager.h"
#include "function.h"

CDatabaseManager * CDatabaseManager::m_SingleInstance = NULL;

string_t CDatabaseManager::m_Table_File_Frags_Info_Name;

string_t CDatabaseManager::m_Table_File_Base_Info_Name;

CppSQLite3DB CDatabaseManager::m_SqlliteDB;


CDatabaseManager::CDatabaseManager(void)
{
	Init();
}


CDatabaseManager::~CDatabaseManager(void)
{
	m_SqlliteDB.close();
}

CDatabaseManager * CDatabaseManager::GetSingleInstance()
{
	if (m_SingleInstance == NULL)
	{
		m_SingleInstance = new CDatabaseManager;
		static destroy des;
	}

	return m_SingleInstance;
}

void CDatabaseManager::Init()
{
	string_t strPath =  GetCurrentModulePath();
	strPath += _T("\\data.db");

	if (!PathFileExists(strPath.c_str()))
	{
		HANDLE hFile = CreateFile(strPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL , NULL);
		CloseHandle(hFile);
	}

	m_SqlliteDB.open(String_tToString(strPath).c_str());

	bool bRet = m_SqlliteDB.tableExists("file_base_info");
	if (!bRet)
	{
		m_SqlliteDB.execQuery("CREATE TABLE [file_base_info]( \
													  [file_id]											VARCHAR(300)  PRIMARY KEY  NOT NULL, \
													  [file_type]									INTEGER DEFAULT(0),  \
													  [file_name]									VARCHAR(300) NOT NULL,  \
													  [parent_id]									VARCHAR(300)   NOT NULL, \
													  [file_size]										LARGEINT DEFAULT(0) , \
													  [file_last_modify_time]				INT64 DEFAULT(0)	,\
													  [file_tag]										INTEGER		DEFAULT(0) \
														) ");
	}

	bRet = m_SqlliteDB.tableExists("file_frag_info");
	if (!bRet)
	{
		m_SqlliteDB.execQuery("CREATE TABLE [file_frag_info]( \
							  [file_id]											VARCHAR(300)  PRIMARY KEY  NOT NULL, \
							  [file_frag_first]								VARCHAR(300) NOT NULL,  \
							  [file_frag_second]						VARCHAR(300) NOT NULL,  \
							  [file_frag_third]							VARCHAR(300)   NOT NULL \
							  )");
	}

	m_Table_File_Base_Info_Name = _T("file_base_info");
	m_Table_File_Frags_Info_Name = _T("file_frag_info");
}

void CDatabaseManager::GetDirectChildren( LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos )
{

	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_type,file_name, parent_id, file_size, file_last_modify_time, file_tag from %s where parent_id='%s'"), m_Table_File_Base_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_type = query.fieldName(1);
	string field_file_name = query.fieldName(2);
	string field_file_parent_id = query.fieldName(3);
	string field_file_size = query.fieldName(4);
	string field_file_modifytime = query.fieldName(5);
	string field_file_tag = query.fieldName(6);


	while(!query.eof())
	{
		string id = query.getStringField(field_file_id.c_str());
		int type = query.getIntField(field_file_type.c_str());
		string name = query.getStringField(field_file_name.c_str());
		string parent_id = query.getStringField(field_file_parent_id.c_str());
		UINT64 size = query.getInt64Field(field_file_size.c_str());
		INT64 modifytime = query.getInt64Field(field_file_modifytime.c_str());
		int tag = query.getIntField(field_file_tag.c_str());

		STDBFILEBASEINFO stInfo;
		stInfo.file_id = ANSIToString_t(id);
		stInfo.file_type = type;
		stInfo.file_name = Utf8ToString_t(name);
		stInfo.parent_id = ANSIToString_t(parent_id);
		stInfo.file_size = size;
		stInfo.file_last_modify_time = modifytime;
		stInfo.file_state = tag;

		vecDirectChildrenInfos.push_back(stInfo);

		query.nextRow();
	}
}

void CDatabaseManager::GetDirectDirectoryChildren( LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_type,file_name, parent_id, file_size, file_last_modify_time, file_tag from %s where parent_id='%s' and file_type=0"), m_Table_File_Base_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_type = query.fieldName(1);
	string field_file_name = query.fieldName(2);
	string field_file_parent_id = query.fieldName(3);
	string field_file_size = query.fieldName(4);
	string field_file_modifytime = query.fieldName(5);
	string field_file_tag = query.fieldName(6);


	while(!query.eof())
	{
		string id = query.getStringField(field_file_id.c_str());
		int type = query.getIntField(field_file_type.c_str());
		string name = query.getStringField(field_file_name.c_str());
		string parent_id = query.getStringField(field_file_parent_id.c_str());
		UINT64 size = query.getInt64Field(field_file_size.c_str());
		INT64 modifytime = query.getInt64Field(field_file_modifytime.c_str());
		int tag = query.getIntField(field_file_tag.c_str());

		STDBFILEBASEINFO stInfo;
		stInfo.file_id = ANSIToString_t(id);
		stInfo.file_type = type;
		stInfo.file_name = Utf8ToString_t(name);
		stInfo.parent_id = ANSIToString_t(parent_id);
		stInfo.file_size = size;
		stInfo.file_last_modify_time = modifytime;
		stInfo.file_state = tag;

		vecDirectChildrenInfos.push_back(stInfo);

		query.nextRow();
	}
}

void CDatabaseManager::GetDirectFileChildren( LPCTSTR lpID, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_type,file_name, parent_id, file_size, file_last_modify_time, file_tag from %s where parent_id='%s' and file_type=1"), m_Table_File_Base_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_type = query.fieldName(1);
	string field_file_name = query.fieldName(2);
	string field_file_parent_id = query.fieldName(3);
	string field_file_size = query.fieldName(4);
	string field_file_modifytime = query.fieldName(5);
	string field_file_tag = query.fieldName(6);


	while(!query.eof())
	{
		string id = query.getStringField(field_file_id.c_str());
		int type = query.getIntField(field_file_type.c_str());
		string name = query.getStringField(field_file_name.c_str());
		string parent_id = query.getStringField(field_file_parent_id.c_str());
		UINT64 size = query.getInt64Field(field_file_size.c_str());
		INT64 modifytime = query.getInt64Field(field_file_modifytime.c_str());
		int tag = query.getIntField(field_file_tag.c_str());

		STDBFILEBASEINFO stInfo;
		stInfo.file_id = ANSIToString_t(id);
		stInfo.file_type = type;
		stInfo.file_name = Utf8ToString_t(name);
		stInfo.parent_id = ANSIToString_t(parent_id);
		stInfo.file_size = size;
		stInfo.file_last_modify_time = modifytime;
		stInfo.file_state = tag;

		vecDirectChildrenInfos.push_back(stInfo);

		query.nextRow();
	}
}

void CDatabaseManager::InsertRecordToTableFileBaseInfo( LPCTSTR lpFileID, int nFileType, LPCTSTR lpFileName, LPCTSTR lpParentID, UINT64 uFileSize, INT64 nModifyTime, int nFileTag )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("insert into [%s](file_id, file_type, file_name,parent_id,file_size,file_last_modify_time,file_tag) values('%s',%d,'%s','%s',%llu,%lld,%d)"), \
								m_Table_File_Base_Info_Name.c_str(), lpFileID, nFileType, lpFileName, lpParentID, uFileSize, nModifyTime, nFileTag);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::InsertRecordToTableFileFragInfo( LPCTSTR lpFileID, LPCTSTR lpFragIDFirst, LPCTSTR lpFragIDSecond, LPCTSTR lpFragIDThird )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("insert into [%s](file_id, file_frag_first, file_frag_second, file_frag_third) values('%s','%s','%s', '%s')"), \
		m_Table_File_Frags_Info_Name.c_str(), lpFileID, lpFragIDFirst, lpFragIDSecond, lpFragIDThird);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::QueryFileBaseInfoRecordByFileID( LPCTSTR lpID, STDBFILEBASEINFO &Info )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_type,file_name, parent_id, file_size, file_last_modify_time, file_tag from %s where file_id='%s'"), m_Table_File_Base_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_type = query.fieldName(1);
	string field_file_name = query.fieldName(2);
	string field_file_parent_id = query.fieldName(3);
	string field_file_size = query.fieldName(4);
	string field_file_modifytime = query.fieldName(5);
	string field_file_tag = query.fieldName(6);

	string id = query.getStringField(field_file_id.c_str());
	int type = query.getIntField(field_file_type.c_str());
	string name = query.getStringField(field_file_name.c_str());
	string parent_id = query.getStringField(field_file_parent_id.c_str());
	UINT64 size = query.getInt64Field(field_file_size.c_str());
	INT64 modifytime = query.getInt64Field(field_file_modifytime.c_str());
	int tag = query.getIntField(field_file_tag.c_str());
	
	Info.file_id = ANSIToString_t(id);
	Info.file_type = type;
	Info.file_name = Utf8ToString_t(name);
	Info.parent_id = ANSIToString_t(parent_id);
	Info.file_size = size;
	Info.file_last_modify_time = modifytime;
	Info.file_state = tag;		
}

void CDatabaseManager::QueryFileFragInfoRecordByFileID( LPCTSTR lpID, FRAG_INFO_T &Info )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_frag_first, file_frag_second, file_frag_third from %s where file_id='%s'"), m_Table_File_Frags_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_frag_first = query.fieldName(1);
	string field_file_frag_second = query.fieldName(2);
	string field_file_frag_third = query.fieldName(3);

	string id = query.getStringField(field_file_id.c_str());
	string strFirst = query.getStringField(field_file_frag_first.c_str());
	string strSecond = query.getStringField(field_file_frag_second.c_str());
	string strThird = query.getStringField(field_file_frag_third.c_str());

	StringCchPrintf(Info.Name[0], _countof(Info.Name[0]), _T("%s"), Utf8ToString_t(strFirst).c_str());
	StringCchPrintf(Info.Name[1], _countof(Info.Name[1]), _T("%s"), Utf8ToString_t(strSecond).c_str());
	StringCchPrintf(Info.Name[2], _countof(Info.Name[2]), _T("%s"), Utf8ToString_t(strThird).c_str());
}

void CDatabaseManager::DeleteFileBaseInfoRecordByFileID( LPCTSTR lpID )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("delete from %s where file_id='%s'"), m_Table_File_Base_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::DeleteFileFragInfoRecordByFileID( LPCTSTR lpID )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("delete from %s where file_id='%s'"), m_Table_File_Frags_Info_Name.c_str(), lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::UpDateFileName( LPCTSTR lpID, LPCTSTR lpNewName )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("update %s set file_name='%s' where file_id='%s'"), m_Table_File_Base_Info_Name.c_str(), lpNewName, lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::UpdateFileParentID( LPCTSTR lpID, LPCTSTR lpParentID )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("update %s set parent_id='%s' where file_id='%s'"), m_Table_File_Base_Info_Name.c_str(), lpParentID, lpID);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());
}

void CDatabaseManager::GetAllFileNameHaveSubstring( LPCTSTR lpSubstring, vector<STDBFILEBASEINFO> &vecDirectChildrenInfos )
{
	TCHAR szSql[1000] = {0};
	StringCchPrintf(szSql, _countof(szSql), _T("select file_id, file_type,file_name, parent_id, file_size, file_last_modify_time, file_tag from %s where file_name like '%%%s%%'"), m_Table_File_Base_Info_Name.c_str(), lpSubstring);

	string_t strSql = szSql;
	CppSQLite3Query query = m_SqlliteDB.execQuery(String_tToUtf8(strSql).c_str());

	string field_file_id = query.fieldName(0);
	string field_file_type = query.fieldName(1);
	string field_file_name = query.fieldName(2);
	string field_file_parent_id = query.fieldName(3);
	string field_file_size = query.fieldName(4);
	string field_file_modifytime = query.fieldName(5);
	string field_file_tag = query.fieldName(6);


	while(!query.eof())
	{
		string id = query.getStringField(field_file_id.c_str());
		int type = query.getIntField(field_file_type.c_str());
		string name = query.getStringField(field_file_name.c_str());
		string parent_id = query.getStringField(field_file_parent_id.c_str());
		UINT64 size = query.getInt64Field(field_file_size.c_str());
		INT64 modifytime = query.getInt64Field(field_file_modifytime.c_str());
		int tag = query.getIntField(field_file_tag.c_str());

		STDBFILEBASEINFO stInfo;
		stInfo.file_id = ANSIToString_t(id);
		stInfo.file_type = type;
		stInfo.file_name = Utf8ToString_t(name);
		stInfo.parent_id = ANSIToString_t(parent_id);
		stInfo.file_size = size;
		stInfo.file_last_modify_time = modifytime;
		stInfo.file_state = tag;

		vecDirectChildrenInfos.push_back(stInfo);

		query.nextRow();
	}
}









CDatabaseManager::destroy::destroy()
{

}

CDatabaseManager::destroy::~destroy()
{
	if (m_SingleInstance != NULL)
	{
		delete m_SingleInstance;
	}
}
