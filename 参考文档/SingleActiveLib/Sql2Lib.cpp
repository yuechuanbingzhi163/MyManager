#include "stdafx.h"
#include "sql2lib.h"
#include <string>
#include <assert.h>
#include "..\..\Common\StrConvert\StrConvert.h"

using namespace std;

#define LIB_EDIT_VERSION 0
#define LIB_FORMAT_VERSION 1

Sql2Lib::Sql2Lib(LPCWSTR lpFilePath, AssessLog* assessLog)
{
	m_lpFilePath = lpFilePath;
	if(!InitDB())
	{
		assert(0);
		return;
	}
	ErgodicAndSave(assessLog);
}

BOOL Sql2Lib::InitDB()
{
	try 
	{	
		m_DB.open(UnicodeToANSI(m_lpFilePath).c_str());
	} 
	catch (CppSQLite3Exception& e)
	{
		e.errorMessage();
	}

	if (m_DB.tableExists("ActiveInfo") && m_DB.tableExists("OperateInfo") && m_DB.tableExists("OperateTypeInfo"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void TranslpNum(LPCWSTR OrgNum, int* resultCount, int* resultNum)
{
	WCHAR LpNum[50] = {0};
	wcscpy(LpNum, OrgNum);
	int count= wcslen(LpNum);
	*resultCount = 0;
	LPWSTR CurrentStr = LpNum;
	for(int index=0; index!=count; index++)
	{
		if(LpNum[index] == ',')
		{
			LpNum[index] = '\0';
			resultNum[*resultCount] = _wtoi(CurrentStr);
			CurrentStr = LpNum+index+1;
			(*resultCount)++;
		}
	}
}

BOOL Sql2Lib::ErgodicAndSave(AssessLog* assessLog)
{
	try 
	{
		WCHAR szSql[1024] = {0};
		wsprintf(szSql, L"select * from ActiveInfo");

		CppSQLite3Query query = m_DB.execQuery(UnicodeToANSI(szSql).c_str());
		while(!query.eof())
		{
			//查找添加Active
			int ActiveNameID = query.getIntField("ID");
			LPCSTR ActiveName = query.getStringField("ActiveName");
			LPCSTR ActiveDescribe = query.getStringField("ActiveDescribe");

			ACTIVE_INFO ActivInfo;
			memset(&ActivInfo, 0, sizeof(ACTIVE_INFO));
			wstring strActiveName = ANSIToUnicode(ActiveName);
			wstring strActiveDescribe = ANSIToUnicode(ActiveDescribe);
			ActivInfo.activeName = strActiveName.c_str();
			ActivInfo.activeDescribe = strActiveDescribe.c_str();

			ACTIVE_INFO* RealActivInfo;
			RealActivInfo = assessLog->AddActiveStruct(&ActivInfo);

			//查找添加operate
			wsprintf(szSql, L"select * from OperateInfo where ActiveNameID=%d", ActiveNameID);
			CppSQLite3Query query1 = m_DB.execQuery(UnicodeToANSI(szSql).c_str());
			while(!query1.eof())
			{
				OPERATE_INFO OperateInfo;
				int ActiveNameID = query1.getIntField("ActiveNameID");
				int OperateType = query1.getIntField("OperateType");
				LPCSTR OperateBlackParam = query1.getStringField("OperateBlackParam");
				LPCSTR OperateWhiteParam = query1.getStringField("OperateWhiteParam");

				memset(&OperateInfo, 0, sizeof(OPERATE_INFO));
				wstring strOperateBlackParam = ANSIToUnicode(OperateBlackParam);
				wstring strOperateWhiteParam = ANSIToUnicode(OperateWhiteParam);
				OperateInfo.OperateBlackParam = strOperateBlackParam.c_str();
				OperateInfo.OperateWhiteParam = strOperateWhiteParam.c_str();
				OperateInfo.OperateType = OperateType;
				assessLog->AddOperateStruct(RealActivInfo, &OperateInfo);

				query1.nextRow();
			}

			query.nextRow();
		}

		//查找添加动组合
		wsprintf(szSql, L"select * from OperateTypeInfo");
		CppSQLite3Query query1 = m_DB.execQuery(UnicodeToANSI(szSql).c_str());
		while(!query1.eof())
		{
			OPERATE_INFO OperateInfo;
			int OperateTypeIndex	=	query1.getIntField("OperateIndex");
			LPCSTR OperateType		=	query1.getStringField("OperateType");
			LPCSTR OperateTypeName	=	query1.getStringField("OperateTypeName");

			memset(&OperateInfo, 0, sizeof(OPERATE_INFO));
			wstring szOperateType = ANSIToUnicode(OperateType);
			wstring szOperateTypeName = ANSIToUnicode(OperateTypeName);
			
			wcscpy(assessLog->m_FileHead_Analyzed->OperateAssembles->types[OperateTypeIndex].OperateAssembleName, szOperateTypeName.c_str());
			
			int ResultCount = 0;
			int Result[10] = {0};
			TranslpNum(szOperateType.c_str(), &ResultCount, Result);

			for(int Pos=0; Pos!=ResultCount; Pos++)
			{
				assessLog->m_FileHead_Analyzed->OperateAssembles->types[OperateTypeIndex].type[Pos] = Result[Pos];
			}
			query1.nextRow();
		}
	}
	catch (CppSQLite3Exception& e)
	{
		e.errorMessage();
		return FALSE;
	}

	return TRUE;
}