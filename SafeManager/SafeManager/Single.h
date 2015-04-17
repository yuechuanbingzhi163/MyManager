#pragma once
#include "SplitLayer.h"

class CFileHandle;

typedef struct st_TaskInfo
{
	UINT			nTaskID;
	UINT			nTaskType;			//0:上传		1:下载		2:删除
	UINT			nTaskState;			//0:完成		1:准备		2:进行中		3:取消中
	BOOL			IsNeedHandle;		//是否是本身需要处理

	string_t		strFilePath;

	//计算进度用
	int nHandleChildrenFileCount;							//包括所有子文件的个数：若是目录，则指子文件个数；若是文件，则为1
	int nSplitOrMergerCount;									//已破碎或重组的文件个数：若是目录，则指子文件的个数；若是文件，则为0或1
	int nUpOrDownLoadChipsCount;					//已上传或下载的碎片个数：若是目录，则指子文件的碎片；若是文件，则为0到单个文件破碎的最大碎片数
}STTASKINFO, *LPSTTASKINFO;


typedef struct st_Temp
{
	string_t   Path;
	CFileHandle *  CurFileHandle;
	int			nTag;				//0上传  1下载 2删除
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

