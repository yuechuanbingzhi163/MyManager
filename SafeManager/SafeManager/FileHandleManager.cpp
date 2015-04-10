#include "StdAfx.h"
#include "FileHandleManager.h"
#include "function.h"
#include "Single.h"
#include "MainFrameWork.h"
#include "HttpsRequest.h"
#include "DatabaseManager.h"

CFileHandleManager * CFileHandleManager::m_SingleInstance = NULL;

DWORD WINAPI _ThreadPreProc( LPVOID lpParam );
DWORD WINAPI _ThreadUpLoadProc( LPVOID lpParam );
DWORD WINAPI _ThreadDownLoadProc( LPVOID lpParam );
DWORD WINAPI _ThreadDeleteProc( LPVOID lpParam );


CFileHandleManager::CFileHandleManager(void):m_FileHandleTree(NULL)
{
	CSplitLayer cSpMod;
	 m_lpSplitMod = cSpMod.Query(1);

	 m_hPreThreadCreateEvent = ::CreateEvent(0, FALSE, FALSE, 0);
	 m_hUpLoadThreadCreateEvent = ::CreateEvent(0, FALSE, FALSE, 0);
	 m_hDownLoadThreadCreateEvent = ::CreateEvent(0, FALSE, FALSE, 0);
	 m_hDeleteThreadCreateEvent = ::CreateEvent(0, FALSE, FALSE, 0);

	 m_hPreprocThread = CreateThread(NULL, 0, _ThreadPreProc, this, 0, &m_dwPreprocThreadID);
	 m_hUpLoadThread = CreateThread(NULL, 0, _ThreadUpLoadProc, this, 0, &m_dwUpLoadThreadID);
	 m_hDownLoadThread = CreateThread(NULL, 0, _ThreadDownLoadProc, this, 0, &m_dwDownLoadThreadID);
	 m_hDownLoadThread = CreateThread(NULL, 0, _ThreadDeleteProc, this, 0, &m_dwDeleteThreadID);

	 ::WaitForSingleObject(m_hPreThreadCreateEvent, INFINITE);
	 ::WaitForSingleObject(m_hUpLoadThreadCreateEvent, INFINITE);
	 ::WaitForSingleObject(m_hDownLoadThreadCreateEvent, INFINITE);
	 ::WaitForSingleObject(m_hDeleteThreadCreateEvent, INFINITE);
	 CloseHandle(m_hPreThreadCreateEvent);	
	 CloseHandle(m_hUpLoadThreadCreateEvent);	
	 CloseHandle(m_hDownLoadThreadCreateEvent);	 
	 CloseHandle(m_hDeleteThreadCreateEvent);

	 InitializeCriticalSection(&m_CriticalSection_List_File_UpLoad);
	 InitializeCriticalSection(&m_CriticalSection_List_File_DownLoad);
	 InitializeCriticalSection(&m_CriticalSection_List_File_Delete);
}


CFileHandleManager::~CFileHandleManager(void)
{
	::PostThreadMessage(m_dwPreprocThreadID, WM_THREAD_QUIT, NULL, NULL);
	::PostThreadMessage(m_dwUpLoadThreadID, WM_THREAD_QUIT, NULL, NULL);
	::PostThreadMessage(m_dwDownLoadThreadID, WM_THREAD_QUIT, NULL, NULL);
	::PostThreadMessage(m_dwDeleteThreadID, WM_THREAD_QUIT, NULL, NULL);

	DeleteCriticalSection(&m_CriticalSection_List_File_UpLoad);
	DeleteCriticalSection(&m_CriticalSection_List_File_DownLoad);
	DeleteCriticalSection(&m_CriticalSection_List_File_Delete);

	if (m_FileHandleTree != NULL)
	{
		delete m_FileHandleTree;
	}
}

LPSPLIT_MOD_INFO CFileHandleManager::GetSplitMethodInfo()
{
	return m_lpSplitMod;
}

CFileHandle * CFileHandleManager::CreateUploadFileTree( UINT nTaskID, CFileHandle *pParent, LPCTSTR lpPath )
{
	CFileHandle *pRet = NULL;
	LPTSTR lpFileName = StrRChr(lpPath, NULL, _T('\\'));

	if (lpFileName != NULL)
	{
		pRet = new CFileHandle;		
		pRet->SetFileName(lpFileName + 1);
		pRet->SetFileID(CreateGUID());
		pRet->SetParentID(pParent->GetFileID().c_str());
		pRet->SetTag(100);
		pRet->SetLoadAllChildrenFile(TRUE);
			
		STTASKINFO _info;
		ZeroMemory(&_info, sizeof(STTASKINFO));
		_info.nTaskID = nTaskID;
		_info.nTaskType = 0;
		_info.nTaskState = 1;
		_info.IsNeedHandle = TRUE;	
		_info.strFilePath = lpPath;
		pRet->AddTask(_info);

		SYSTEMTIME sysModifyTime;
		FILETIME localFileTime;
		ZeroMemory(&localFileTime, sizeof(FILETIME));
		ZeroMemory(&sysModifyTime, sizeof(SYSTEMTIME));
		WIN32_FILE_ATTRIBUTE_DATA FileAttr;
		BOOL bRet = GetFileAttributesEx(lpPath, GetFileExInfoStandard, &FileAttr);
		FileTimeToLocalFileTime(&(FileAttr.ftLastWriteTime), &localFileTime);
		bRet = FileTimeToSystemTime(&localFileTime, &sysModifyTime);
		pRet->SetModifyTime(SystemTimeToINT64(sysModifyTime));

		if (pParent != NULL)
		{
			pParent->AddChildrenNode(pRet);
		}

		if (GetFileAttributes(lpPath) != FILE_ATTRIBUTE_DIRECTORY)
		{
			pRet->SetFileType(1);

			ULARGE_INTEGER uFileSize;
			uFileSize.HighPart = FileAttr.nFileSizeHigh;
			uFileSize.LowPart = FileAttr.nFileSizeLow;
			UINT64 nFileSize = uFileSize.QuadPart;
			pRet->SetFileSize(nFileSize);
		}
		else
		{
			pRet->SetFileType(0);
			pRet->SetFileSize(0);

			WIN32_FIND_DATA FindFileData;
			HANDLE hListFile;
			TCHAR szFilePath[MAX_PATH] = {0};

			lstrcpy(szFilePath, lpPath);
			lstrcat(szFilePath, _T("\\*"));

			hListFile = FindFirstFile(szFilePath, &FindFileData);

			if (hListFile != INVALID_HANDLE_VALUE)
			{
				do 
				{
					if (lstrcmp(FindFileData.cFileName, _T(".")) == 0 || lstrcmp(FindFileData.cFileName, _T("..")) == 0)
					{
						continue;
					}	

					string_t strFilePath = lpPath;
					strFilePath += _T("\\");
					strFilePath += FindFileData.cFileName;

					CreateUploadFileTree(nTaskID, pRet, strFilePath.c_str());

				} while (FindNextFile(hListFile, &FindFileData));
			}
		}
	}

	return pRet;
}

CFileHandleManager * CFileHandleManager::GetSingleInstance()
{
	if (m_SingleInstance == NULL)
	{
		m_SingleInstance = new CFileHandleManager;
		static destroy des;
	}

	return m_SingleInstance;
}

HANDLE CFileHandleManager::GetCreateEvent( int index )
{
	HANDLE hRet = NULL;

	switch(index)
	{
	case 0:
		hRet = m_hPreThreadCreateEvent;
		break;
	case 1:
		hRet = m_hUpLoadThreadCreateEvent;
		break;
	case 2:
		hRet = m_hDownLoadThreadCreateEvent;
		break;
	case 3:
		hRet = m_hDeleteThreadCreateEvent;
		break;
	default:
		;
	}

	return hRet;
}

DWORD CFileHandleManager::GetThreadID( THREAD_ID_INDEX index )
{
	DWORD dwRet = 0;

	switch(index)
	{
	case PRE_PROC_THREAD_ID:
		dwRet = m_dwPreprocThreadID;
		break;
	case UPLOAD_THREAD_ID:
		dwRet = m_dwUpLoadThreadID;
		break;
	case DOWNLOAD_THREAD_ID:
		dwRet = m_dwDownLoadThreadID;
		break;
	case DELETE_THREAD_ID:
		dwRet = m_dwDeleteThreadID;
		break;
	default:
		;
	}

	return dwRet;
}

void CFileHandleManager::AddItemToListUpLoad( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_UpLoad);

	m_listFileUpLoad.push_back(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_UpLoad);
}

void CFileHandleManager::DeleteItemFromListUpLoad( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_UpLoad);

	m_listFileUpLoad.remove(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_UpLoad);
}

void CFileHandleManager::AddItemToListDownLoad( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_DownLoad);

	m_listFileDownLoad.push_back(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_DownLoad);
}

void CFileHandleManager::DeleteItemFromListDownLoad( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_DownLoad);

	m_listFileDownLoad.remove(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_DownLoad);
}

void CFileHandleManager::AddItemToListDelete( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_Delete);

	m_listFileDeleteLoad.push_back(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_Delete);
}

void CFileHandleManager::DeleteItemFromListDelete( CFileHandle *pFileHandle )
{
	EnterCriticalSection(&m_CriticalSection_List_File_Delete);

	m_listFileDeleteLoad.remove(pFileHandle);

	LeaveCriticalSection(&m_CriticalSection_List_File_Delete);
}

BOOL CFileHandleManager::ParseUploadFileTree(UINT nTask, CFileHandle *pRoot )
{
	BOOL bRet = TRUE;

	UINT nTaskState = pRoot->GetTaskState(nTask);

	if ( (pRoot->GetFileType() == 1) && (nTaskState == 3))		//只取消文件的操作
	{
		pRoot->DeleteTask(nTask);

		bRet = FALSE;

		return bRet;
	}

	pRoot->SetTaskState(nTask, 2);

	//基本信息写入数据库
	CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileBaseInfo(pRoot->GetFileID().c_str(), pRoot->GetFileType(), pRoot->GetFileName().c_str(), pRoot->GetParentID().c_str(), pRoot->GetFileSize(), pRoot->GetModifyTime(), 0);

	if (pRoot->GetFileType() == 1)			//处理文件
	{
		//破碎
		m_lpSplitMod->fpSplit((LPTSTR)pRoot->GetFilePath(nTask).c_str(), pRoot->GetFrageinfo());
		UpdateSplitOrMergerInfo(nTask,pRoot);

		//可以更新界面

		//上传
		for(int i=0; i<(int)EVERY_FILE_CHIPS_NUMBER; i++)
		{
//			CHttpsRequest requst;
//
//			TCHAR SendComment[1000] = {0};
//			string strSend;
//
//#ifdef UNICODE
//			strSend = WStringToUTF8(SendComment);
//#else
//			wstring steTemp = ANSI2Unicode(SendComment);
//			strSend = WStringToUTF8(steTemp);
//#endif 
//
//			CURLcode retcode = requst.HttpsRequest(strSend.c_str());
//
//			if (retcode == CURLE_OK)
//			{
				UpdateUpOrDownLoadChipsInfo(nTask, pRoot);
				//可以更新界面

			//}
			//else
			//{
			//	LOG(_T("%s上传文件碎片失败!"), pRoot->GetFileOldPath().c_str());
			//}
		}

		//测试用------------------------------将相关碎片信息写入数据库//////////////////////////////////////////////////////////////////////////////
		CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileFragInfo(pRoot->GetFileID().c_str(), pRoot->GetFrageinfo()->Name[0], pRoot->GetFrageinfo()->Name[1], pRoot->GetFrageinfo()->Name[2]);
	}
	else							//处理目录
	{
		int count = pRoot->GetChildrenCount();

		vector<CFileHandle *> vecFailIndexs;

		for (int i=0; i<count; i++)
		{
			CFileHandle *pTemp = static_cast<CFileHandle*>(pRoot->GetChild(i));

			BOOL bHandleResult = ParseUploadFileTree(nTask, pTemp);		
			if (!bHandleResult)
			{
				vecFailIndexs.push_back(pTemp);
			}
		}

		int veccount = vecFailIndexs.size();
		for (int i=veccount - 1; i > -1; i--)
		{
			delete vecFailIndexs[i];
			pRoot->RemoveChildNode(vecFailIndexs[i]);			
		}

		if (pRoot->GetAllTaskChildFileCount() == 0)
		{
			SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATA_FILE_LIST_PROGRESS_UI, (WPARAM)pRoot, NULL);
		}
	}

	pRoot->DeleteTask(m_nTaskID);

	return bRet;
}

void CFileHandleManager::ParseDownloadFileTree( UINT nTask, CFileHandle *pRoot )
{	
	UINT nTaskState = pRoot->GetTaskState(nTask);

	if (nTaskState == 3)
	{
		pRoot->DeleteTask(nTask);

		return;
	}

	pRoot->SetTaskState(nTask, 2);

	if (pRoot->GetFileType() == 1)			//处理文件
	{
		//测试用------------------------------//////////////////////////////////////////////////////////////////////////////
		CDatabaseManager::QueryFileFragInfoRecordByFileID(pRoot->GetFileID().c_str(), *(pRoot->GetFrageinfo()));

		//下载碎片
		for(int i=0; i<(int)EVERY_FILE_CHIPS_NUMBER; i++)
		{
			UpdateUpOrDownLoadChipsInfo(nTask, pRoot);
		}

		//重组
		m_lpSplitMod->fpMerge((LPTSTR)pRoot->GetFilePath(nTask).c_str(), pRoot->GetFrageinfo());
		UpdateSplitOrMergerInfo(nTask, pRoot);		
	}
	else							//处理目录
	{
		CreateDirectory(pRoot->GetFilePath(nTask).c_str(), NULL);
		int count = pRoot->GetChildrenCount();
		for (int i=0; i<count; i++)
		{
			CFileHandle *pTemp = static_cast<CFileHandle*>(pRoot->GetChild(i));	

			ParseDownloadFileTree(nTask, pTemp);
		}

		int nAllCount = pRoot->GetAllTaskChildFileCount();
		if (nAllCount == 0)
		{
			SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATA_FILE_LIST_PROGRESS_UI, (WPARAM)pRoot, NULL);
		}
	}

	pRoot->DeleteTask(nTask);
}

void CFileHandleManager::ParseDeleteFileTree(UINT nTask, CFileHandle *pRoot )
{
	pRoot->SetTaskState(nTask, 2);

	if (pRoot->GetFileType() == 1)			//处理文件
	{
		CDatabaseManager::DeleteFileBaseInfoRecordByFileID(pRoot->GetFileID().c_str());

		//测试用------------------------------//////////////////////////////////////////////////////////////////////////////
		CDatabaseManager::QueryFileFragInfoRecordByFileID(pRoot->GetFileID().c_str(), *(pRoot->GetFrageinfo()));

		//下载碎片并删除
		for(int i=0; i<(int)EVERY_FILE_CHIPS_NUMBER; i++)
		{	
			TCHAR szChipPath[MAX_PATH] = {0};
			StringCchPrintf(szChipPath, _countof(szChipPath), _T("%s\\%s"), pRoot->GetFrageinfo()->Path, pRoot->GetFrageinfo()->Name[i]);
			DeleteFile(szChipPath);
		}

		CDatabaseManager::DeleteFileFragInfoRecordByFileID(pRoot->GetFileID().c_str());
		//可以更新界面
	}
	else							//处理目录
	{	
		CDatabaseManager::DeleteFileBaseInfoRecordByFileID(pRoot->GetFileID().c_str());

		int count = pRoot->GetChildrenCount();
		for (int i=0; i<count; i++)
		{
			CFileHandle *pTemp = static_cast<CFileHandle*>(pRoot->GetChild(i));
			ParseDeleteFileTree(nTask, pTemp);
		}		
	}	

	pRoot->SetTaskState(nTask, 0);	
}

int CFileHandleManager::CountFileTreeNodeProgressInfo(UINT nTaskID, CFileHandle *pRoot )
{
	pRoot->SetOneTaskChildFileCount(nTaskID, 0);
	pRoot->SetHaveSplitOrMergerCount(nTaskID, 0);
	pRoot->SetHaveUpOrDownLoadChipsCount(nTaskID, 0);

	int AllChildrenFileCount = 0;

	if (pRoot->GetFileType() == 1)		//是文件
	{
		AllChildrenFileCount = 1;
	}
	else														//是目录
	{
		int count = pRoot->GetChildrenCount();
		for (int i=0; i<count; i++)
		{
			CFileHandle *pFile = static_cast<CFileHandle*>(pRoot->GetChild(i));

			AllChildrenFileCount += CountFileTreeNodeProgressInfo(nTaskID, pFile);
		}	
	}

	pRoot->SetOneTaskChildFileCount(nTaskID, AllChildrenFileCount);
	return AllChildrenFileCount;
}

CFileHandle * CFileHandleManager::CreateDownloadOrDeleteFileTree( UINT nTaskID, CFileHandle *pFileHandle, int nType )
{
	if (pFileHandle->IsDelete())
	{
		return pFileHandle;
	}

	pFileHandle->SetTaskState(nTaskID, 1);

	BOOL bIsLoadAllChildren = pFileHandle->IsLoadAllChildrenFile();

	if (!bIsLoadAllChildren)
	{
		vector<STDBFILEBASEINFO> vecInfos;
		CDatabaseManager::GetDirectFileChildren(pFileHandle->GetFileID().c_str(), vecInfos);

		int count = vecInfos.size();

		for (int i=0; i <count; i++)
		{
			CFileHandle *pChild = new CFileHandle;
			pChild->SetFileName(vecInfos[i].file_name);
			pChild->SetFileID(vecInfos[i].file_id);
			pChild->SetFileType(vecInfos[i].file_type);
			pChild->SetParentID(vecInfos[i].parent_id);
			pChild->SetFileSize(vecInfos[i].file_size);
			pChild->SetModifyTime(vecInfos[i].file_last_modify_time);
			pChild->SetTag(vecInfos[i].file_state);
			pChild->SetLoadAllChildrenFile(TRUE);

			pFileHandle->AddChildrenNode(pChild);
		}

		pFileHandle->SetLoadAllChildrenFile(TRUE);
	}

	int count = pFileHandle->GetChildrenCount();
	for (int i=0; i<count; i++)
	{
		CFileHandle *pChild = static_cast<CFileHandle*>(pFileHandle->GetChild(i));
		string_t strPath = pFileHandle->GetFilePath(nTaskID);
		strPath += _T("\\");
		strPath += pChild->GetFileName();		

		STTASKINFO _info;
		ZeroMemory(&_info, sizeof(STTASKINFO));
		_info.nTaskID = nTaskID;
		_info.nTaskType = nType;
		_info.IsNeedHandle = TRUE;
		_info.nTaskState = 1;
		_info.strFilePath = strPath;
		pChild->AddTask(_info);

		if (pChild->GetFileType() == 0)
		{
			CreateDownloadOrDeleteFileTree(nTaskID, pChild);
		}
	}

	return pFileHandle;
}

void CFileHandleManager::UpdateSplitOrMergerInfo( UINT nTaskID, CFileHandle *pFileHandle )
{
	CFileHandle *pParent = static_cast<CFileHandle*>(pFileHandle->GetParentNode());

	if (pFileHandle != NULL)
	{
		pFileHandle->AutoUpdateSplitOrMergerInfo(nTaskID);

		SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATA_FILE_LIST_PROGRESS_UI, (WPARAM)pFileHandle, NULL);
		
		if (pParent != NULL)
		{
			UpdateSplitOrMergerInfo(nTaskID, pParent);		
		}
	}
}

void CFileHandleManager::UpdateUpOrDownLoadChipsInfo( UINT nTaskID, CFileHandle *pFileHandle )
{
	CFileHandle *pParent = static_cast<CFileHandle*>(pFileHandle->GetParentNode());

	if (pFileHandle != NULL)
	{
		pFileHandle->AutoUpdateUpOrDownLoadChipsInfo(nTaskID);

		SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATA_FILE_LIST_PROGRESS_UI, (WPARAM)pFileHandle, NULL);
		
		if (pParent != NULL)
		{
			UpdateUpOrDownLoadChipsInfo(nTaskID, pParent);				
		}
	}
}

void CFileHandleManager::SuspendAllThread()
{
	SuspendThread(m_hPreprocThread);
	SuspendThread(m_hUpLoadThread);
	SuspendThread(m_hDownLoadThread);
	SuspendThread(m_hDeleteThread);
}

void CFileHandleManager::ResumeAllThread()
{
	ResumeThread(m_hPreprocThread);
	ResumeThread(m_hUpLoadThread);
	ResumeThread(m_hDownLoadThread);
	ResumeThread(m_hDeleteThread);
}

void CFileHandleManager::SuspendUploadThread()
{
	SuspendThread(m_hUpLoadThread);
}

void CFileHandleManager::ResumeUpLoadThread()
{
	ResumeThread(m_hUpLoadThread);
}

void CFileHandleManager::SuspendDownLoadThread()
{
	SuspendThread(m_hDownLoadThread);
}

void CFileHandleManager::ResumeDownLoadThread()
{
	ResumeThread(m_hDownLoadThread);
}


void CFileHandleManager::SearchDirecChildByID( LPCTSTR lpParentID, vector<CFileHandle*> &vecFileHandle )
{
	//SuspendUpAndDownThread();

	list<CFileHandle*>::iterator iter = m_listFileUpLoad.begin();
	for (iter ; iter != m_listFileUpLoad.end(); iter++)
	{
		SearchFileHandleTreeByParentID(lpParentID, *iter, vecFileHandle);
	}

	iter = m_listFileDownLoad.begin();
	for (iter ; iter != m_listFileDownLoad.end(); iter++)
	{
		SearchFileHandleTreeByParentID(lpParentID, *iter, vecFileHandle);
	}

	iter = m_listFileDeleteLoad.begin();
	for (iter ; iter != m_listFileDeleteLoad.end(); iter++)
	{
		SearchFileHandleTreeByParentID(lpParentID, *iter, vecFileHandle);
	}

	//ResumeUpAndDownThread();
}

void CFileHandleManager::SearchFileHandleTreeByParentID( LPCTSTR lpParentID, CFileHandle *pRoot, vector<CFileHandle*> &vecFileHandle )
{
	if (StrCmp(pRoot->GetParentID().c_str(), lpParentID) == 0)
	{
		vecFileHandle.push_back(pRoot);
	}

	int count = pRoot->GetChildrenCount();
	for (int i=0; i<count; i++)
	{
		CFileHandle *pFile = static_cast<CFileHandle*>(pRoot->GetChild(i));
		SearchFileHandleTreeByParentID(lpParentID, pFile, vecFileHandle);
	}
}

CFileHandle * CFileHandleManager::GetFileHandleTree()
{
	return m_FileHandleTree;
}

void CFileHandleManager::InitFileHandleTree()
{
	m_FileHandleTree = new CFileHandle;

	m_FileHandleTree->SetFileName(_T("所有文件"));
	m_FileHandleTree->SetFileID(_T("0"));
	m_FileHandleTree->SetLoadAllChildrenFile(FALSE);

	AddAllDirectoryFileHandle(m_FileHandleTree);
}

void CFileHandleManager::AddAllDirectoryFileHandle( CFileHandle *pParent )
{
	vector<STDBFILEBASEINFO> vecInfos;

	CDatabaseManager::GetDirectDirectoryChildren(pParent->GetFileID().c_str(), vecInfos);

	int count = vecInfos.size();

	for (int i=0; i<count; i++)
	{
		CFileHandle *pFileHandle = new CFileHandle;

		pFileHandle->SetFileName(vecInfos[i].file_name);
		pFileHandle->SetFileID(vecInfos[i].file_id);
		pFileHandle->SetFileType(vecInfos[i].file_type);
		pFileHandle->SetParentID(vecInfos[i].parent_id);
		pFileHandle->SetFileSize(vecInfos[i].file_size);
		pFileHandle->SetModifyTime(vecInfos[i].file_last_modify_time);
		pFileHandle->SetTag(vecInfos[i].file_state);
		pFileHandle->SetLoadAllChildrenFile(FALSE);

		pParent->AddChildrenNode(pFileHandle);

		AddAllDirectoryFileHandle(pFileHandle);
	}	
}

UINT CFileHandleManager::AllotTaskID()
{
	return ++m_nTaskID;
}

void CFileHandleManager::CancelTask( CFileHandle *pFileHandle )
{
	SuspendUploadThread();
	SuspendDownLoadThread();

	StopFileHandle(pFileHandle);

	ResumeUpLoadThread();
	ResumeDownLoadThread();
}

void CFileHandleManager::StopFileHandle( CFileHandle *pFileHandle)
{
	pFileHandle->SetUpLoadOrDownLoadCancel();	

	int count = pFileHandle->GetChildrenCount();
	for (int i=0; i<count; i++)
	{
		CFileHandle *pTemp = static_cast<CFileHandle*>(pFileHandle->GetChild(i));	

		StopFileHandle(pTemp);
	}	

}

void CFileHandleManager::LoadAllChilrenFile( CFileHandle *pFileHandle )
{
	if (pFileHandle->IsLoadAllChildrenFile())
	{
		return;
	}

	vector<STDBFILEBASEINFO> vecInfos;
	CDatabaseManager::GetDirectFileChildren(pFileHandle->GetFileID().c_str(), vecInfos);

	int nCount = vecInfos.size();
	for (int i=0; i<nCount; i++)
	{
		CFileHandle *pChildFileHandle = new CFileHandle;

		pChildFileHandle->SetFileName(vecInfos[i].file_name);
		pChildFileHandle->SetFileID(vecInfos[i].file_id);
		pChildFileHandle->SetFileType(vecInfos[i].file_type);
		pChildFileHandle->SetParentID(vecInfos[i].parent_id);
		pChildFileHandle->SetFileSize(vecInfos[i].file_size);
		pChildFileHandle->SetModifyTime(vecInfos[i].file_last_modify_time);
		pChildFileHandle->SetTag(vecInfos[i].file_state);
		pChildFileHandle->SetLoadAllChildrenFile(TRUE);

		pFileHandle->AddChildrenNode(pChildFileHandle);
	}

	pFileHandle->SetLoadAllChildrenFile(TRUE);
}

CFileHandle * CFileHandleManager::LoadFileAndAllChildrenFile( LPCTSTR lpFileID , STDBFILEBASEINFO *pInfo)
{
	CFileHandle *pFileHandle = NULL;

	STDBFILEBASEINFO _info;

	if (pInfo != NULL)
	{
		_info = *pInfo;
	}
	else
	{
		CDatabaseManager::QueryFileBaseInfoRecordByFileID(lpFileID, _info);
	}	

	pFileHandle = new CFileHandle;
	pFileHandle->SetFileName(_info.file_name);
	pFileHandle->SetFileID(_info.file_id);
	pFileHandle->SetFileType(_info.file_type);
	pFileHandle->SetParentID(_info.parent_id);
	pFileHandle->SetFileSize(_info.file_size);
	pFileHandle->SetModifyTime(_info.file_last_modify_time);
	pFileHandle->SetTag(_info.file_state);

	if (_info.file_type == 0)
	{
		vector<STDBFILEBASEINFO> vecInfos;
		CDatabaseManager::GetDirectChildren(lpFileID, vecInfos);

		int count = vecInfos.size();
		for (int i=0; i<count; i++)
		{
			CFileHandle *pChild = LoadFileAndAllChildrenFile(vecInfos[i].file_id.c_str(), &(vecInfos[i]));
			pFileHandle->AddChildrenNode(pChild);
		}
	}

	pFileHandle->SetLoadAllChildrenFile(TRUE);

	return pFileHandle;
}

CFileHandle * CFileHandleManager::SearchFileHandleFromTreeByFileID( LPCTSTR lpID )
{
	CFileHandle *pReturn = NULL;

	vector<CFileHandle *>  vecInfos;

	if (m_FileHandleTree != NULL)
	{
		vecInfos.push_back(m_FileHandleTree);

		int i = 0;
		while(i < vecInfos.size())
		{
			if (StrCmp(vecInfos[i]->GetFileID().c_str(), lpID) == 0)
			{
				pReturn = vecInfos[i];
				break;
			}

			int nChildrenCount = vecInfos[i]->GetChildrenCount();
			for (int k=0; k<nChildrenCount; k++)
			{
				CFileHandle *pTemp = static_cast<CFileHandle*>(vecInfos[i]->GetChild(k));
				vecInfos.push_back(pTemp);
			}

			i++;
		}
	}
	
	return pReturn;
}





UINT CFileHandleManager::m_nTaskID = 0;



CFileHandleManager::destroy::destroy()
{

}

CFileHandleManager::destroy::~destroy()
{
	if (m_SingleInstance != NULL)
	{
		delete m_SingleInstance;
	}
}

DWORD WINAPI _ThreadPreProc( LPVOID lpParam )
{
	CFileHandleManager *pManager = (CFileHandleManager*)lpParam;

	if (NULL == pManager)
	{
		return 0;
	}

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SetEvent(pManager->GetCreateEvent(0));

	while(true)
	{
		if (GetMessage( &msg, 0, 0, 0 ))
		{
			switch(msg.message)
			{
			case WM_FILE_PROCESS:
				{
					LPSTTEMP lpTemp = (LPSTTEMP)msg.wParam;
					
					if (lpTemp->nTag == 0)		//上传
					{	
						UINT nTaskID = CFileHandleManager::GetSingleInstance()->AllotTaskID();

						CFileHandle *pFileHandle = CFileHandleManager::GetSingleInstance()->CreateUploadFileTree(nTaskID, lpTemp->CurFileHandle, lpTemp->Path.c_str());

						pManager->CountFileTreeNodeProgressInfo(nTaskID, pFileHandle);

						STTASKINFO _info;
						ZeroMemory(&_info, sizeof(STTASKINFO));
						_info.nTaskID = nTaskID;
						_info.nTaskType = 0;
						_info.IsNeedHandle = FALSE;

						CFileHandle *pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
						while(pParentFileHandle != NULL)
						{
							pParentFileHandle->AddTask(_info);
							pParentFileHandle->SetOneTaskChildFileCount(nTaskID, pFileHandle->GetOneTaskChildFileCount(nTaskID));
							pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
						}

						pManager->AddItemToListUpLoad(pFileHandle);		

						SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATE_FILE_LIST_UI,(WPARAM)pFileHandle, (LPARAM)0);

						::PostThreadMessage(pManager->GetThreadID(UPLOAD_THREAD_ID), WM_FILE_PROCESS, (WPARAM)pFileHandle, (LPARAM)nTaskID);
					}
					else if (lpTemp->nTag == 1)		//下载
					{	
						CFileHandle *pFileHandle = lpTemp->CurFileHandle;

						string_t strSavePath = lpTemp->Path.c_str();
						strSavePath += _T("\\");
						strSavePath += pFileHandle->GetFileName();

						TCHAR szSavePath[2 * MAX_PATH] = {0};

						BOOL bRet = PathYetAnotherMakeUniqueName(szSavePath, strSavePath.c_str(), NULL, pFileHandle->GetFileName().c_str());						

						UINT nTaskID = CFileHandleManager::GetSingleInstance()->AllotTaskID();						

						STTASKINFO _info;
						ZeroMemory(&_info, sizeof(STTASKINFO));
						_info.nTaskID = nTaskID;
						_info.nTaskType = 1;
						_info.IsNeedHandle = TRUE;
						_info.nTaskState = 1;

						pFileHandle->AddTask(_info);
						pFileHandle->SetFilePath(nTaskID, szSavePath);
						pFileHandle = CFileHandleManager::GetSingleInstance()->CreateDownloadOrDeleteFileTree(nTaskID, pFileHandle, 1);

						pManager->CountFileTreeNodeProgressInfo(nTaskID, pFileHandle);

						_info.IsNeedHandle = FALSE;
						CFileHandle *pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
						while(pParentFileHandle != NULL)
						{
							pParentFileHandle->AddTask(_info);
							pParentFileHandle->SetOneTaskChildFileCount(nTaskID, pFileHandle->GetOneTaskChildFileCount(nTaskID));
							pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
						}

						pManager->AddItemToListDownLoad(pFileHandle);

						SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATE_FILE_LIST_UI,(WPARAM)pFileHandle, (LPARAM)1);

						::PostThreadMessage(pManager->GetThreadID(DOWNLOAD_THREAD_ID), WM_FILE_PROCESS, (WPARAM)pFileHandle, (LPARAM)nTaskID);
					}
					else if (lpTemp->nTag == 2)		//删除
					{
						CFileHandle *pFileHandle = lpTemp->CurFileHandle;

						if (!pFileHandle->IsDelete())
						{
							UINT nTaskID = CFileHandleManager::GetSingleInstance()->AllotTaskID();

							STTASKINFO _info;
							ZeroMemory(&_info, sizeof(STTASKINFO));
							_info.nTaskID = nTaskID;
							_info.nTaskType = 2;
							_info.IsNeedHandle = TRUE;
							_info.nTaskState = 1;

							pFileHandle->AddTask(_info);
							pFileHandle = CFileHandleManager::GetSingleInstance()->CreateDownloadOrDeleteFileTree( nTaskID, pFileHandle, 2);

							pManager->CountFileTreeNodeProgressInfo(nTaskID, pFileHandle);

							pManager->AddItemToListDelete(pFileHandle);

							::PostThreadMessage(pManager->GetThreadID(DELETE_THREAD_ID), WM_FILE_PROCESS, (WPARAM)pFileHandle, (LPARAM)nTaskID);
						}
						
					}
					
					delete lpTemp;
					break;
				}
			case WM_THREAD_QUIT:
				{
					return 0;
					break;
				}
			}
		}
	}

	return 0;
}

DWORD WINAPI _ThreadUpLoadProc( LPVOID lpParam )
{
	CFileHandleManager *pManager = (CFileHandleManager*)lpParam;

	if (NULL == pManager)
	{
		return 0;
	}

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SetEvent(pManager->GetCreateEvent(1));

	while(true)
	{
		if (GetMessage( &msg, 0, 0, 0 ))
		{
			switch(msg.message)
			{
			case WM_FILE_PROCESS:
				{
					CFileHandle *pFileHandle = (CFileHandle*)msg.wParam;		
					UINT nTaskID = (UINT)msg.lParam;

					pManager->ParseUploadFileTree(nTaskID, pFileHandle);
					LOG(_T("文件：%s上传完毕"), pFileHandle->GetFilePath(nTaskID).c_str());

					CFileHandle *pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
					while(pParentFileHandle != NULL)
					{
						pParentFileHandle->DeleteTask(nTaskID);
						pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
					}

					pManager->DeleteItemFromListUpLoad(pFileHandle);					
					break;
				}
			case WM_THREAD_QUIT:
				{
					return 0;
					break;
				}
			}
		}
	}

	return 0;
}

DWORD WINAPI _ThreadDownLoadProc( LPVOID lpParam )
{
	CFileHandleManager *pManager = (CFileHandleManager*)lpParam;

	if (NULL == pManager)
	{
		return 0;
	}

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SetEvent(pManager->GetCreateEvent(2));

	while(true)
	{
		if (GetMessage( &msg, 0, 0, 0 ))
		{
			switch(msg.message)
			{
			case WM_FILE_PROCESS:
				{
					CFileHandle *pFileHandle = (CFileHandle*)msg.wParam;
					UINT nTaskID = (UINT)msg.lParam;

					pManager->ParseDownloadFileTree(nTaskID, pFileHandle);
					LOG(_T("文件：%s下载完毕"), pFileHandle->GetFilePath(nTaskID).c_str());

					CFileHandle *pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
					while(pParentFileHandle != NULL)
					{
						pParentFileHandle->DeleteTask(nTaskID);
						pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
					}

					pManager->DeleteItemFromListDownLoad(pFileHandle);					
					break;
				}
			case WM_THREAD_QUIT:
				{
					return 0;
					break;
				}
			}
		}
	}

	return 0;
}

DWORD WINAPI _ThreadDeleteProc( LPVOID lpParam )
{
	CFileHandleManager *pManager = (CFileHandleManager*)lpParam;

	if (NULL == pManager)
	{
		return 0;
	}

	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	SetEvent(pManager->GetCreateEvent(3));

	while(true)
	{
		if (GetMessage( &msg, 0, 0, 0 ))
		{
			switch(msg.message)
			{
			case WM_FILE_PROCESS:
				{
					CFileHandle *pFileHand = (CFileHandle*)msg.wParam;
					UINT nTaskID = (UINT)msg.lParam;

					pManager->ParseDeleteFileTree(nTaskID, pFileHand);						
					pManager->DeleteItemFromListDelete(pFileHand);	
					CFileHandle *pParent = static_cast<CFileHandle*>(pFileHand->GetParentNode());
					if (pParent != NULL)
					{
						pParent->RemoveChildNode(pFileHand);
					}
					delete pFileHand;
					break;
				}
			case WM_THREAD_QUIT:
				{
					return 0;
					break;
				}
			}
		}
	}

	return 0;
}
