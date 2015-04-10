#pragma once
#include "..\..\common\Sqlite\CppSQLite3.h"
#include "SingleActiveLib.h"

class Sql2Lib
{
public:
	Sql2Lib(LPCWSTR lpFilePath, AssessLog* assessLog);
	~Sql2Lib(){};

private:
	CppSQLite3DB m_DB;
	LPCWSTR m_lpFilePath;
	BOOL InitDB();
	int GetActiveNameID(LPCWSTR ActiveName);
	BOOL ErgodicAndSave(AssessLog* assessLog);
};