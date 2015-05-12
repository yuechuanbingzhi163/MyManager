#pragma once
#include "stdafx.h"
#include "SplitLayer.h"
#include "TreeNode.h"
#include "Single.h"

class CFileHandle:public CTreeNode
{
public:
	CFileHandle(void);
	virtual ~CFileHandle(void);

public:
	void SetFileID(string_t strFileID);
	string_t GetFileID();

	void SetFileName(string_t strName);
	string_t GetFileName();

	void SetFileType(int nFileType);
	int GetFileType( );

	void SetParentID(string_t strParentID);
	string_t GetParentID();

	void SetFileSize(UINT64 uSize);
	UINT64 GetFileSize();

	void SetModifyTime(INT64 nTime);
	INT64 GetModifyTime();

	void SetTag(int tag);
	int GetTag();

	int GetState();

	LPFRAG_INFO_T GetFrageinfo();

	void SetLoadAllChildrenFile(BOOL b);
	BOOL IsLoadAllChildrenFile();

	void SetOneTaskChildFileCount(UINT nTaskID, int count);
	int GetOneTaskChildFileCount(UINT nTaskID);

	int GetAllTaskChildFileCount( );

	void SetHaveSplitOrMergerCount(UINT nTaskID, int count);
	int GetHaveSplitOrMergerCount(UINT nTaskID);

	void SetHaveUpOrDownLoadChipsCount(UINT nTaskID, int count);
	int GetHaveUpOrDownLoadChipsCount(UINT nTaskID);

	void AutoUpdateSplitOrMergerInfo(UINT nTaskID );
	void AutoUpdateUpOrDownLoadChipsInfo(UINT nTaskID);

	void AutoDecreaseHandleFileNum(UINT nTaskID);

	float CountProgress();

	void AddTask(STTASKINFO stTaskInfo);
	void DeleteTask(UINT nTaskID);

	void SetFilePath(UINT nTaskID, string_t strPath);
	string_t GetFilePath(UINT nTaskID);

	int GetTaskCount();

	void SetTaskState(UINT nTaskID, UINT lTag);
	ULONG GetTaskState(UINT nTaskID);

	int GetTaskType(UINT nTaskID);

	BOOL IsNeedHandle(UINT nTaskID);

	BOOL IsUploadOrDownLaodPre();

	BOOL IsUploadOrDownLoadRuning();

	BOOL IsUploadOrDownLoading();
	BOOL IsDelete();

	BOOL IsUpLoadCancel();

	void SetUpLoadOrDownLoadCancel();

	void GetUploadOrDownLoadTaskIDs(vector<UINT> &vecIDs);

	void SetExtrenTag(int tag = 0);
	int GetExtrenTag();

private:
	STDBFILEBASEINFO    m_FileData;

	vector<STTASKINFO> m_vecAllTaskInfo;

	BOOL m_bLoadAllChildrenFile;

	CRITICAL_SECTION  m_CriticalSection;

	int m_Extren_tag;
};

