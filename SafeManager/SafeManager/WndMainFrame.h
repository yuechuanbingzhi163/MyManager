#pragma once
#include "WndShadow.h"
#include "ListContainerElementOfFIleList.h"
#include "DropTargetEx.h"
#include "FileHandle.h"
#include "WndProgress.h"
#include <list>
#include "FileListUI.h"

class CDropTargetEx;
class CWndMoveOrPasteTo;

class CWndMainFrame : public WindowImplBase
{
public:
	CWndMainFrame(void);
	~CWndMainFrame(void);

	void OnFinalMessage( HWND hWnd ) { __super::OnFinalMessage(hWnd); delete this; };
	CDuiString GetSkinFolder() { return _T("..\\Res\\SafeManager\\"); };
	CDuiString GetSkinFile() { return _T("WndMainFrame.xml"); };
	LPCTSTR GetWindowClassName() const { return _T("CWndMainFrame"); };

#ifdef UI_USE_ZIP
	UILIB_RESOURCETYPE GetResourceType() const { return UILIB_ZIPRESOURCE;};
	LPCTSTR GetResourceID() const { return  MAKEINTRESOURCE(IDR_ZIPRES1);};
#endif

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Notify( TNotifyUI &msg );
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	string_t GetCurrentOpenDir();

	void OnDropFiles(CWndMainFrame *pMainWnd, HDROP hDrop, POINT pt);
	bool IsDropValidRect(const POINTL &pt);

	bool GetFileListPos(RECT &rc) const;

	void SetFileListEleOptVisible(CListContainerElementOfFileList *pItem ,bool visble = false);
	void SetFileListEleOptTip(CListContainerElementOfFileList *pItem, LPCTSTR lpTip);

	void SetMoveOrCopyTo(CFileHandle *pFileHandle);
	vector<CFileHandle*> *GetMoveOrCopyFrom();

	void MoveFileTask( );		//参数为m_vecMoveOrCopyFrom、m_pMoveOrCopyTo
	void CopyFileTask( );      //参数为m_vecMoveOrCopyFrom、m_pMoveOrCopyTo

	void SearchFile( );

	void PopFileListMenu(const POINT & pt, CFileHandle *pFileHandle = NULL);

	void ExecutCommand_Upload();
	void ExecutCommand_Refresh();
	void ExecutCommand_NewFolder();
	void ExecutCommand_Attribute(CFileHandle *pFileHandle);
	void ExecutCommand_Open(CFileHandle *pFileHandle);
	void ExecutCommand_Copy(CFileHandle *pFileHandle);
	void ExecutCommand_Move(CFileHandle *pFileHandle);
	void ExecutCommand_Export(CFileHandle *pFileHandle);
	void ExecutCommand_Delete(CFileHandle *pFileHandle);
	void ExecutCommand_Rename(CFileHandle *pFileHandle);

private:
	void SetCurFileHandle(CFileHandle *pFileHandle);

	void Init();

	void InitFileTree();
	void EnumFileHandleTreeAddToTree(CTreeNodeUI *pParent);
	void TravFileHandleTreeAddToTree(CTreeNodeUI *pParent, CFileHandle *pRoot);
	CTreeNodeUI *CreateTreeNode(CFileHandle *pFileHandle);
	CTreeNodeUI *GetTreeNodeByFileHandle(CFileHandle *pFileHandle);
	
	void InitFileList();

	void CreateFileList();

	void EmptyFileList();
	void SearchFileAddToFileList(CFileHandle *pFileHandle);
	CListContainerElementOfFileList *CreateFileListItem(CFileHandle *pFileHandle);

	CListContainerElementOfFileList *GetFileListItem(CFileHandle *pFileHandle);

	BOOL IsFileListItemSelected(CListContainerElementOfFileList *pItem);

	void RenameFile(CListContainerElementOfFileList *pEle);

	void SetAllSelCheckBoxState(bool bState);

	void AddToSelectedFileHandleHistory(CFileHandle *pFileHandle);


	void OnClick_Btn_MainPage(TNotifyUI &msg);
	void OnClick_Btn_Forward(TNotifyUI &msg);
	void OnClick_Btn_Next(TNotifyUI &msg);
	void OnClick_Btn_Add(TNotifyUI &msg);
	void OnClick_Btn_Copy(TNotifyUI &msg);
	void OnClick_Btn_ReName(TNotifyUI &msg);
	void OnClick_Btn_Move(TNotifyUI &msg);
	void OnClick_Btn_NewDir(TNotifyUI &msg);
	void OnClick_Btn_Export(TNotifyUI &msg);
	void OnClick_Btn_Delete(TNotifyUI &msg);
	void OnClick_Btn_Search(TNotifyUI &msg);

	void OrderFileList(int nindex, bool bAscend = true); //0:文件名	1：文件大小 2:修改时间


private:
	CWndShadow m_WndShadow;

	CDropTargetEx * m_pDropTarget;	//使窗体支持拖放操作

	int m_IsDirIndex;

	CFileHandle *   m_Current_File_Handle;			//目录的id

	vector<CFileHandle*> m_vecMoveOrCopyFrom;
	CFileHandle *m_pMoveOrCopyTo;
	CWndProgress *m_WndProgress;

	CTreeViewUI *m_pTreeViewUI;
	CFileListUI *m_pFileListUI;

	list<CFileHandle*> m_listSelFileHandleHistory;
	list<CFileHandle*>::reverse_iterator m_listHistoryIter;

	string_t m_strSearchContent;
};	

