#include "StdAfx.h"
#include "FileHandle.h"
#include "MainFrameWork.h"
#include "Single.h"


CFileHandle::CFileHandle(void):m_bLoadAllChildrenFile(False)
{
	 InitializeCriticalSection(&m_CriticalSection);

	ZeroMemory(&m_FileData.Frageinfo, sizeof(FRAG_INFO_T));

	m_FileData.Frageinfo.Count = EVERY_FILE_CHIPS_NUMBER;

	string_t strPath = CMainFrameWork::GetFilesAndChipsDirectory();
	strPath += _T('\\');
	strPath += _T("frags");

	StringCchPrintf(m_FileData.Frageinfo.Path, MAX_PATH, strPath.c_str());
}

CFileHandle::~CFileHandle(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

void CFileHandle::SetFileID( string_t strFileID )
{
	m_FileData.file_id = strFileID;
}

string_t CFileHandle::GetFileID()
{
	return m_FileData.file_id;
}

void CFileHandle::SetFilePath( UINT nTaskID, string_t strPath )
{
	vector<STTASKINFO>::iterator iter  = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->strFilePath = strPath;
			break;
		}
	}
}

string_t CFileHandle::GetFilePath( UINT nTaskID )
{
	string_t strReturn;

	vector<STTASKINFO>::iterator iter  = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			strReturn = iter->strFilePath;
			break;
		}
	}

	return strReturn;
}

void CFileHandle::SetFileName( string_t strName )
{
	m_FileData.file_name = strName;
}

string_t CFileHandle::GetFileName()
{
	return m_FileData.file_name;
}

LPFRAG_INFO_T CFileHandle::GetFrageinfo()
{
	return &m_FileData.Frageinfo;
}

void CFileHandle::SetFileType( int nFileType )
{
	m_FileData.file_type = nFileType;
}

int CFileHandle::GetFileType()
{
	return m_FileData.file_type;
}

void CFileHandle::SetParentID( string_t strParentID )
{
	m_FileData.parent_id = strParentID;
}

string_t CFileHandle::GetParentID()
{
	return m_FileData.parent_id;
}

void CFileHandle::SetFileSize( UINT64 uSize )
{
	m_FileData.file_size = uSize;
}

UINT64 CFileHandle::GetFileSize()
{
	return m_FileData.file_size;
}

void CFileHandle::SetModifyTime( INT64 nTime )
{
	m_FileData.file_last_modify_time = nTime;
}

INT64 CFileHandle::GetModifyTime()
{
	return m_FileData.file_last_modify_time;
}

float CFileHandle::CountProgress()
{
	float fRet = 0;

	int nChildFileCount = 0;
	int nSplitOrMergerCount = 0;
	int nUpOrDownLoadChipsCount = 0;
	
	vector<STTASKINFO>::iterator iter  = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		nChildFileCount += iter->nHandleChildrenFileCount;
		nSplitOrMergerCount += iter->nSplitOrMergerCount;
		nUpOrDownLoadChipsCount += iter->nUpOrDownLoadChipsCount;
	}

	if (nChildFileCount == 0)
	{
		fRet = 1.0;
	}
	else
	{
		float progress1 = ((float)FILE_SPLIT_OR_MERGER_PROGERSS_VALUE) * ((float)nSplitOrMergerCount / nChildFileCount);
		float progress2 = ((float)FILE_GET_FRAGS_PROGERSS_VALUE) * (nUpOrDownLoadChipsCount / (nChildFileCount * ((int)EVERY_FILE_CHIPS_NUMBER)));
		fRet = progress1 + progress2;
	}

	return fRet;
}

void CFileHandle::SetTag( int tag )
{
	m_FileData.file_state = tag;
}

int CFileHandle::GetTag()
{
	return m_FileData.file_state;
}

int CFileHandle::GetState()
{
	return m_FileData.file_state;
}

void CFileHandle::AddTask( STTASKINFO stTaskInfo )
{
	EnterCriticalSection(&m_CriticalSection);
	m_vecAllTaskInfo.push_back(stTaskInfo);
	LeaveCriticalSection(&m_CriticalSection);
}

void CFileHandle::SetOneTaskChildFileCount( UINT nTaskID, int count )
{
	//EnterCriticalSection(&m_CriticalSection);

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->nHandleChildrenFileCount = count;
			break;
		}
	}

	//LeaveCriticalSection(&m_CriticalSection);

}

int CFileHandle::GetOneTaskChildFileCount(UINT nTaskID)
{
	int nRet = 0;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			nRet = iter->nHandleChildrenFileCount;
			break;
		}
	}

	return nRet;
}

void CFileHandle::SetHaveSplitOrMergerCount( UINT nTaskID, int count )
{
	//EnterCriticalSection(&m_CriticalSection);

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->nSplitOrMergerCount = count;
			break;
		}
	}

	//LeaveCriticalSection(&m_CriticalSection);
}

int CFileHandle::GetHaveSplitOrMergerCount(UINT nTaskID)
{
	int nRet = 0;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			nRet = iter->nSplitOrMergerCount;
			break;
		}
	}

	return nRet;
}

void CFileHandle::SetHaveUpOrDownLoadChipsCount( UINT nTaskID, int count )
{
	//EnterCriticalSection(&m_CriticalSection);

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->nUpOrDownLoadChipsCount = count;
			break;
		}
	}

	//LeaveCriticalSection(&m_CriticalSection);
}

int CFileHandle::GetHaveUpOrDownLoadChipsCount(UINT nTaskID)
{
	int nRet = 0;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			nRet = iter->nUpOrDownLoadChipsCount;
			break;
		}
	}

	return nRet;
}

void CFileHandle::AutoUpdateSplitOrMergerInfo(UINT nTaskID )
{
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			(iter->nSplitOrMergerCount)++;
			break;
		}
	}	
}

void CFileHandle::AutoUpdateUpOrDownLoadChipsInfo( UINT nTaskID  )
{
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			(iter->nUpOrDownLoadChipsCount)++;
			break;
		}
	}
}

void CFileHandle::SetTaskState(UINT nTaskID, UINT lTag )
{
	//EnterCriticalSection(&m_CriticalSection);

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->nTaskState = lTag;
			break;
		}
	}

	//LeaveCriticalSection(&m_CriticalSection);
}

ULONG CFileHandle::GetTaskState(UINT nTaskID)
{
	ULONG nRet = 0;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			nRet = iter->nTaskState;
			break;
		}
	}

	return nRet;
}

void CFileHandle::DeleteTask( UINT nTaskID )
{
	EnterCriticalSection(&m_CriticalSection);
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			m_vecAllTaskInfo.erase(iter);
			break;
		}
	}
	LeaveCriticalSection(&m_CriticalSection);
}

void CFileHandle::SetLoadAllChildrenFile( BOOL b )
{
	m_bLoadAllChildrenFile = b;
}

BOOL CFileHandle::IsLoadAllChildrenFile()
{
	return m_bLoadAllChildrenFile;
}

BOOL CFileHandle::IsUpLoadCancel()
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{		
		if ((iter->nTaskType == 0) && (iter->nTaskState == 3))
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CFileHandle::IsDelete()
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{		
		if (iter->nTaskType == 2)
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CFileHandle::IsUploadOrDownLaodPre()
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{		
		if ( (iter->IsNeedHandle) && (iter->nTaskState == 1))
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CFileHandle::IsUploadOrDownLoadRuning()
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{		
		if ((iter->IsNeedHandle) && (iter->nTaskState == 2))
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

BOOL CFileHandle::IsUploadOrDownLoading()
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{		
		if ((iter->IsNeedHandle) && ((iter->nTaskState == 1) || (iter->nTaskState == 2)))
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

int CFileHandle::GetAllTaskChildFileCount()
{
	int nRet = 0;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		nRet += iter->nHandleChildrenFileCount;
	}

	return nRet;
}

void CFileHandle::SetUpLoadOrDownLoadCancel()
{
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if ((iter->nTaskType == 0 || iter->nTaskType == 1) && (iter->nTaskState == 1) && (iter->IsNeedHandle))
		{
			iter->nTaskState = 3;

			if (GetFileType() == 1)				//如果是文件取消的话，要依次向上减少该任务ID处理的文件数
			{
				CFileHandle *pParent = static_cast<CFileHandle*>(GetParentNode());

				while(pParent != NULL)
				{
					pParent->AutoDecreaseHandleFileNum(iter->nTaskID);

					if ((pParent->GetAllTaskChildFileCount() == 0) && (!pParent->IsNeedHandle(iter->nTaskID)))
					{
						pParent->DeleteTask(iter->nTaskID);
					}

					pParent = static_cast<CFileHandle*>(pParent->GetParentNode());
				}
			}
		}
	}
}

int CFileHandle::GetTaskType( UINT nTaskID )
{
	int nRet = -1;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			nRet = iter->nTaskType;
			break;
		}
	}

	return nRet;
}

BOOL CFileHandle::IsNeedHandle( UINT nTaskID )
{
	BOOL bRet = FALSE;

	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			bRet = iter->IsNeedHandle;
			break;
		}
	}

	return bRet;
}

void CFileHandle::GetUploadOrDownLoadTaskIDs( vector<UINT> &vecIDs )
{
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (((iter->nTaskType == 0) || (iter->nTaskType == 1)))
		{
			vecIDs.push_back(iter->nTaskID);
			break;
		}
	}
}

void CFileHandle::AutoDecreaseHandleFileNum( UINT nTaskID )
{
	vector<STTASKINFO>::iterator iter = m_vecAllTaskInfo.begin();
	for (iter; iter != m_vecAllTaskInfo.end(); iter++)
	{
		if (iter->nTaskID == nTaskID)
		{
			iter->nHandleChildrenFileCount--;
			break;
		}
	}
}

int CFileHandle::GetTaskCount()
{
	return m_vecAllTaskInfo.size();
}












