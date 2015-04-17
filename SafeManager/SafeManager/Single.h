#pragma once
#include "SplitLayer.h"

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

