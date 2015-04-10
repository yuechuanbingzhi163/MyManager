#pragma once
#include "SplitLayer.h"
#include "FileHandle.h"
#include <list>

enum THREAD_ID_INDEX
{
	PRE_PROC_THREAD_ID = 0,
	UPLOAD_THREAD_ID,
	DOWNLOAD_THREAD_ID,
	DELETE_THREAD_ID,
};

class CFileHandleManager
{
public:	
	~CFileHandleManager(void);

	static CFileHandleManager *GetSingleInstance();

	void InitFileHandleTree();

	CFileHandle *GetFileHandleTree();
	CFileHandle * SearchFileHandleFromTreeByFileID(LPCTSTR lpID);

	LPSPLIT_MOD_INFO GetSplitMethodInfo();

	CFileHandle *CreateUploadFileTree(UINT nTaskID, CFileHandle *pParent, LPCTSTR lpPath);
	CFileHandle *CreateDownloadOrDeleteFileTree(UINT nTaskID, CFileHandle *pFileHandle, int tag = 0);  //1为下载树， 2为删除树
	
	int CountFileTreeNodeProgressInfo(UINT nTaskID, CFileHandle *pRoot);		//计算每个树节点的叶孩子总个数，方便后面计算进度

	BOOL ParseUploadFileTree(UINT nTask, CFileHandle *pRoot);	//解析文件目录树，包括：破碎、上传碎片ID、更新界面
	void ParseDownloadFileTree(UINT nTask, CFileHandle *pRoot); //解析文件目录树，包括：下载碎片ID、重组、更新界面
	void ParseDeleteFileTree(UINT nTask, CFileHandle *pRoot);		//解析文件目录树，包括：下载碎片ID、删除相关文件、更新界面

	void CancelTask(CFileHandle *pFileHandle);
	void StopFileHandle(CFileHandle *pFileHandle);

	HANDLE GetCreateEvent(int index);

	DWORD GetThreadID(THREAD_ID_INDEX index);

	void AddItemToListUpLoad(CFileHandle *pFileHandle);
	void DeleteItemFromListUpLoad(CFileHandle *pFileHandle);

	void AddItemToListDownLoad(CFileHandle *pFileHandle);
	void DeleteItemFromListDownLoad(CFileHandle *pFileHandle);

	void AddItemToListDelete(CFileHandle *pFileHandle);
	void DeleteItemFromListDelete(CFileHandle *pFileHandle);

	void UpdateSplitOrMergerInfo(UINT nTaskID, CFileHandle *pFileHandle);
	void UpdateUpOrDownLoadChipsInfo(UINT nTaskID, CFileHandle *pFileHandle);

	void SuspendAllThread();
	void ResumeAllThread();

	void SuspendUploadThread();
	void ResumeUpLoadThread();

	void SuspendDownLoadThread();
	void ResumeDownLoadThread();

	void SearchFileHandleTreeByParentID(LPCTSTR lpParentID, CFileHandle *pRoot, vector<CFileHandle*> &vecFileHandle);
	void SearchDirecChildByID(LPCTSTR lpParentID, vector<CFileHandle*> &vecFileHandle);

	static UINT AllotTaskID();

	void LoadAllChilrenFile(CFileHandle *pFileHandle);
	CFileHandle *LoadFileAndAllChildrenFile(LPCTSTR lpFileID, STDBFILEBASEINFO *pInfo = NULL);

private:
	void AddAllDirectoryFileHandle(CFileHandle *pParent);

private:
	static UINT m_nTaskID;

	CFileHandle *m_FileHandleTree;

	LPSPLIT_MOD_INFO m_lpSplitMod;

	static CFileHandleManager *m_SingleInstance;

	list<CFileHandle*> m_listFileUpLoad;
	list<CFileHandle*> m_listFileDownLoad;
	list<CFileHandle*> m_listFileDeleteLoad;

	CRITICAL_SECTION  m_CriticalSection_List_File_UpLoad;
	CRITICAL_SECTION  m_CriticalSection_List_File_DownLoad;
	CRITICAL_SECTION  m_CriticalSection_List_File_Delete;

	HANDLE		m_hPreprocThread;
	DWORD		m_dwPreprocThreadID;

	HANDLE		m_hUpLoadThread;
	DWORD		m_dwUpLoadThreadID;

	HANDLE		m_hDownLoadThread;
	DWORD		m_dwDownLoadThreadID;

	HANDLE		m_hDeleteThread;
	DWORD		m_dwDeleteThreadID;

	HANDLE  m_hPreThreadCreateEvent;
	HANDLE	 m_hUpLoadThreadCreateEvent;
	HANDLE  m_hDownLoadThreadCreateEvent;
	HANDLE  m_hDeleteThreadCreateEvent;	

private:
	CFileHandleManager(void);
	class destroy
	{
	public:
		destroy();
		~destroy();
	};
};

