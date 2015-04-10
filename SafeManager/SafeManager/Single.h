#pragma once
#include "SplitLayer.h"

#define WM_FILE_PROCESS												(WM_USER + 0X820)
#define WM_THREAD_QUIT												(WM_USER + 0X821)
#define  WM_UPDATE_FILE_LIST_UI									(WM_USER + 0X822)
#define  WM_UPDATA_FILE_LIST_PROGRESS_UI			(WM_USER + 0X823)
#define  WM_NIDTIP															(WM_USER + 0X824)
#define  WM_UPDATE_PROGRESS									(WM_USER + 0X825)

#define DUI_MSGTYPE_FILE_LIST_DBCLICK			_T("self_def_list_item_dbclick")

#define EVERY_FILE_CHIPS_NUMBER										(3)
#define  FILE_SPLIT_OR_MERGER_PROGERSS_VALUE			(0.6)
#define  FILE_GET_FRAGS_PROGERSS_VALUE						(0.4)

//#define FILE_HANDLE_LOAD_CHILDREN_ALL_YES					0x80000000L
//#define FILE_HANDLE_LOAD_CHILDREN_ALL_NO					0x40000000L
//#define FILE_HANDLE_UPLOAD_PREPARE									0x20000000L
//#define FILE_HANDLE_UPLOAD_RUNNING								0x10000000L
//#define FILE_HANDLE_UPLOAD_END											0x08000000L
//#define FILE_HANDLE_DOWNLOAD_PREPARE							0x04000000L
//#define FILE_HANDLE_DOWNLOAD_RUNNING							0x02000000L
//#define FILE_HANDLE_DOWNLOAD_END									0x01000000L
//#define FILE_HANDLE_DELETE_PREPARE									0x00C00000L
//#define FILE_HANDLE_DELETE_RUNNING									0x00800000L
//#define FILE_HANDLE_DELETE_END												0x00400000L
//#define FILE_HANDLE_PASTE_PREPARE										0x00200000L
//#define FILE_HANDLE_PASTE_RUNNING									0x00100000L
//#define FILE_HANDLE_PASTE_END												0x00080000L
//#define FILE_HANDLE_CUT_PREPARE											0x00040000L
//#define FILE_HANDLE_CUT_RUNNING										0x00020000L
//#define FILE_HANDLE_CUT_END													0x00010000L
													
//#define FILE_HANDLE_CANCEL														0x00008000L


#define  FILE_HANDLE_STRING_END						_T("OPERATOR_END")

class CFileHandle;

typedef struct st_TaskInfo
{
	UINT			nTaskID;
	UINT			nTaskType;			//0:�ϴ�		1:����		2:ɾ��
	UINT			nTaskState;			//0:���		1:׼��		2:������		3:ȡ����
	BOOL			IsNeedHandle;		//�Ƿ��Ǳ�����Ҫ����

	string_t		strFilePath;

	//���������
	int nHandleChildrenFileCount;							//�����������ļ��ĸ���������Ŀ¼����ָ���ļ������������ļ�����Ϊ1
	int nSplitOrMergerCount;									//�������������ļ�����������Ŀ¼����ָ���ļ��ĸ����������ļ�����Ϊ0��1
	int nUpOrDownLoadChipsCount;					//���ϴ������ص���Ƭ����������Ŀ¼����ָ���ļ�����Ƭ�������ļ�����Ϊ0�������ļ�����������Ƭ��
}STTASKINFO, *LPSTTASKINFO;


typedef struct st_Temp
{
	string_t   Path;
	CFileHandle *  CurFileHandle;
	int			nTag;				//0�ϴ�  1���� 2ɾ��
}STTEMP, *LPSTTEMP, *PSTTEMP;

typedef struct st_db_file_base_info
{
	string_t					file_id;
	int							file_type;
	string_t					file_path;
	string_t					file_name;
	string_t					parent_id;
	UINT64					file_size;
	INT64					file_last_modify_time;
	int							file_state;								
	FRAG_INFO_T		Frageinfo;
}STDBFILEBASEINFO, *LPSTDBFILEBASEINFO;

class CSingle
{
public:
	CSingle(void);
	~CSingle(void);
};

