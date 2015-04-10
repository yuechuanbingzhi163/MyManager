#pragma once
#include "..\..\common\Sqlite\CppSQLite3.h"
#include "SingleActiveLib.h"

class Lib2Sql
{
public:
	Lib2Sql(LPCWSTR lpFilePath, FILEHEAD* LibFileHead);
	~Lib2Sql(){};


private:
	CppSQLite3DB m_DB;
	LPCWSTR m_lpFilePath;
	BOOL InitDB();
	int GetActiveNameID(LPCWSTR ActiveName);
	BOOL ErgodicAndSave(FILEHEAD* LibFileHead);
	BOOL InsertLibVersion(LIBVERSION* libVersion);
	BOOL InsertActiveInfo(LPCWSTR ActiveName, LPCWSTR ActiveDescribe);
	BOOL InsertOperateInfo(LPCWSTR OperateBlackPath, LPCWSTR OperateWhitePath, int OperateType, int ActiveNameID);
	BOOL InsertOperateTypeInfo(int OperateIndex, LPCWSTR OperateType, LPCWSTR OperateTypeName);
};