#include "stdafx.h"
#include "Lib2Sql.h"
#include <string>
#include <assert.h>
#include "..\..\Common\StrConvert\StrConvert.h"

using namespace std;
#define LIB_EDIT_VERSION 0
#define LIB_FORMAT_VERSION 1

Lib2Sql::Lib2Sql(LPCWSTR lpFilePath, FILEHEAD* LibFileHead)
{
	m_lpFilePath = lpFilePath;
	if(!InitDB())
	{
		ASSERT(0);
	}
	InsertLibVersion(LibFileHead->LibVersion);
	ErgodicAndSave(LibFileHead);
}

BOOL Lib2Sql::ErgodicAndSave(FILEHEAD* LibFileHead)
{
	ACTIVE_INFO* CurrentActiveInfo = LibFileHead->FirstActive;
	OPERATE_INFO* CurrentOperateInfo;
	OPERATE_ASSEMBLES* CurrentOperateAssebles = LibFileHead->OperateAssembles;
	while(CurrentActiveInfo)
	{
		InsertActiveInfo(CurrentActiveInfo->activeName, CurrentActiveInfo->activeDescribe);
		int ActiveNameID = GetActiveNameID(CurrentActiveInfo->activeName);

		CurrentOperateInfo = CurrentActiveInfo->OperateInfo;
		while(CurrentOperateInfo)
		{
			InsertOperateInfo(CurrentOperateInfo->OperateBlackParam, CurrentOperateInfo->OperateWhiteParam,(int)CurrentOperateInfo->OperateType, ActiveNameID);
			CurrentOperateInfo = CurrentOperateInfo->NextOperateInfo;
		}

		CurrentActiveInfo = CurrentActiveInfo->NextActiveInfo;
	}

	//导出动作组
	
	for(int i=0; i!=100; i++)
	{
		WCHAR szTypes[50] = {0};
		WCHAR* CurrentTypeName = CurrentOperateAssebles->types[i].OperateAssembleName;
		if(wcscmp(CurrentTypeName, L"") == 0)
			continue;

		for(int typeIndex=0; typeIndex!=10; typeIndex++)
		{
			int CurrentType = CurrentOperateAssebles->types[i].type[typeIndex];
			if(CurrentType != NULL)
			{
				WCHAR chTemp[10] = {0};
				_itow(CurrentType, chTemp, 10);
				wcscat(szTypes, chTemp);
				wcscat(szTypes, L",");
			}
		}
		InsertOperateTypeInfo(i, szTypes, CurrentTypeName);
	}
	
	return TRUE;
}

int Lib2Sql::GetActiveNameID(LPCWSTR ActiveName)
{
	try 
	{
		WCHAR szSql[1024] = {0};
		wsprintf(szSql, L"select ID from ActiveInfo where ActiveName='%s'", ActiveName);

		CppSQLite3Query query = m_DB.execQuery(UnicodeToANSI(szSql).c_str());
		if(!query.eof())
		{
			int ActiveNameID = query.getIntField("ID");
			return ActiveNameID;
		}
		else
		{
			return FALSE;
		}
	}
	catch (CppSQLite3Exception& e)
	{
		e.errorMessage();
		return FALSE;
	}
}

BOOL Lib2Sql::InitDB()
{
	try 
	{	
		m_DB.open(UnicodeToANSI(m_lpFilePath).c_str());
	} 
	catch (CppSQLite3Exception& e)
	{
		e.errorMessage();
	}

	if (!m_DB.tableExists("LibVersion"))
	{
		try
		{
			m_DB.execQuery("CREATE TABLE [LibVersion] (\
						   [ID] INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL,\
						   [VersionType] INTEGER  NULL,\
						   [Version1] INTEGER  NULL,\
						   [Version2] INTEGER  NULL\
						   )");

		}
		catch (CppSQLite3Exception& e) 
		{
			e.errorMessage();
			return FALSE;
		}
	}

	if (!m_DB.tableExists("ActiveInfo"))
	{
		try
		{
			m_DB.execQuery("CREATE TABLE [ActiveInfo] (\
						   [ID] INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL,\
						   [ActiveName] VARCHAR(300)  NULL,\
						   [ActiveDescribe] VARCHAR(300)  NULL\
						   )");

		}
		catch (CppSQLite3Exception& e) 
		{
			e.errorMessage();
			return FALSE;
		}
	}

	if (!m_DB.tableExists("OperateInfo"))
	{
		try
		{
			m_DB.execQuery("CREATE TABLE [OperateInfo] (\
						   [ID] INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL,\
						   [OperateBlackParam] VARCHAR(1000)  NULL,\
						   [OperateWhiteParam] VARCHAR(1000)  NULL,\
						   [OperateType] INTEGER  NULL,\
						   [ActiveNameID] INTEGER  NULL\
						   )");

		}
		catch (CppSQLite3Exception& e) 
		{
			e.errorMessage();
			return FALSE;
		}
	}

	if (!m_DB.tableExists("OperateTypeInfo"))
	{
		try
		{
			m_DB.execQuery("CREATE TABLE [OperateTypeInfo] (\
						   [ID] INTEGER  PRIMARY KEY AUTOINCREMENT NOT NULL,\
						   [OperateIndex]	INTEGER  NULL,\
						   [OperateType]	VARCHAR(300)  NULL,\
						   [OperateTypeName] VARCHAR(300)  NULL\
						   )");

		}
		catch (CppSQLite3Exception& e) 
		{
			e.errorMessage();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Lib2Sql::InsertLibVersion(LIBVERSION* libVersion)
{
	WCHAR szSql[1024] = {0};
	//编辑版本
	wsprintf(szSql, L"insert into Libversion(VersionType, Version1, Version2) values(%d, %d, %d)",
		LIB_EDIT_VERSION,libVersion->LibEditVersion1, libVersion->LibEditVersion2);
	m_DB.execDML(UnicodeToANSI(szSql).c_str());

	//格式版本
	wsprintf(szSql, L"insert into Libversion(VersionType, Version1, Version2) values(%d, %d, %d)",
		LIB_FORMAT_VERSION,libVersion->LibFormatVersion1, libVersion->LibFormatVersion2);
	m_DB.execDML(UnicodeToANSI(szSql).c_str());
	return TRUE;
}

BOOL Lib2Sql::InsertActiveInfo(LPCWSTR ActiveName, LPCWSTR ActiveDescribe)
{
	WCHAR szSql[1024] = {0};
	wsprintf(szSql, L"insert into ActiveInfo(ActiveName, ActiveDescribe) values('%s', '%s')", ActiveName, ActiveDescribe);
	m_DB.execDML(UnicodeToANSI(szSql).c_str());
	return TRUE;
}

BOOL Lib2Sql::InsertOperateInfo(LPCWSTR OperateBlackPath, LPCWSTR OperateWhitePath, int OperateType, int ActiveNameID)
{
	WCHAR szSql[1024] = {0};
	wsprintf(szSql, L"insert into OperateInfo(OperateBlackParam, OperateWhiteParam, OperateType, ActiveNameID) values('%s','%s', %d, %d)", OperateBlackPath, OperateWhitePath, OperateType, ActiveNameID);
	m_DB.execDML(UnicodeToANSI(szSql).c_str());
	return TRUE;
}

BOOL Lib2Sql::InsertOperateTypeInfo(int OperateIndex, LPCWSTR OperateType, LPCWSTR OperateTypeName)
{
	WCHAR szSql[1024] = {0};
	wsprintf(szSql, L"insert into OperateTypeInfo(OperateIndex, OperateType, OperateTypeName) values(%d, '%s', '%s')",
		OperateIndex, OperateType, OperateTypeName);
	m_DB.execDML(UnicodeToANSI(szSql).c_str());
	return TRUE;
}
