#include "StdAfx.h"
#include "WndMainFrame.h"
#include "MainFrameWork.h"
#include "function.h"
#include "FileHandle.h"
#include "FileHandleManager.h"
#include "Single.h"
#include "DatabaseManager.h"
#include "PathEditUI.h"
#include "WndConfig.h"
#include "WndMessageBox.h"
#include "WndMoveOrPasteTo.h"
#include "WndProgress.h"
#include "FileListUI.h"
#include "WndFileListMenu.h"

HANDLE  hMoveOrCopyEvent = ::CreateEvent(0, FALSE, FALSE, 0);
DWORD WINAPI _MoveThreadProc( LPVOID lpParam );
DWORD WINAPI _CopyThreadProc( LPVOID lpParam );
DWORD WINAPI _SearchThreadProc( LPVOID lpParam );

DWORD WINAPI _OpenFileThreadProc( LPVOID lpParam );

class CWndMainFrame;

CWndMainFrame::CWndMainFrame(void)
{
	m_pDropTarget = new CDropTargetEx(this);
}


CWndMainFrame::~CWndMainFrame(void)
{
	delete m_pDropTarget;
}

void CWndMainFrame::Notify( TNotifyUI &msg )
{
	__super::Notify(msg);

	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		m_pDropTarget->DragDropRegister(m_hWnd);	
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (msg.pSender->GetName() == _T("btn_main_page"))
		{
			OnClick_Btn_MainPage(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_search"))
		{
			OnClick_Btn_Search(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_close"))
		{
			ShowWindow(false);
		}
		else if (msg.pSender->GetName() == _T("btn_forward"))
		{
			OnClick_Btn_Forward(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_next"))
		{
			OnClick_Btn_Next(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_move"))
		{
			OnClick_Btn_Move(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_copy"))
		{
			OnClick_Btn_Copy(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_set"))
		{
			HWND hwnd = ::FindWindow(_T("CWndConfig"), _T("配置窗口"));

			if (hwnd == NULL)
			{
				CWndConfig *pWndConfig = new CWndConfig;
				pWndConfig->Create(NULL, _T("配置窗口"), UI_WNDSTYLE_DIALOG, 0);
				pWndConfig->CenterWindow();
				pWndConfig->ShowWindow();
			}
			else
			{
				::BringWindowToTop(hwnd);
			}
		}
		else if (msg.pSender->GetName() == _T("btn_rename"))
		{
			OnClick_Btn_ReName(msg);			
		}
		else if (msg.pSender->GetName() == _T("btn_new_fold"))
		{
			OnClick_Btn_NewDir(msg);
		}
		else if (msg.pSender->GetName() == _T("checkbox_select_all"))
		{
			CCheckBoxUI *pAllSel = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("checkbox_select_all")));			

			bool bSel = pAllSel->IsSelected();

			int count = m_pFileListUI->GetCount();

			for (int i=0; i<count; i++)
			{
				CListContainerElementOfFileList *pEle = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
				CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(0));
				CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));
				pCheckBox->Selected(!bSel);
			}
		}
		else if ((StrCmp(msg.pSender->GetClass(), _T("CheckBoxUI")) == 0) &&	
			(msg.pSender->GetParent() != NULL) && 
			(msg.pSender->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent() != NULL) &&
			(StrCmp(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent()->GetName().GetData(), _T("list_file")) == 0) &&
			(StrCmp(msg.pSender->GetName().GetData(), _T("checkbox_select_all")) != 0))
		{
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(msg.pSender);
			bool bSel = pCheckBox->IsSelected();

			CCheckBoxUI *pAllSel = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("checkbox_select_all")));

			int count = m_pFileListUI->GetCount();
			int selcount = 0;

			for (int i=0; i<count; i++)
			{
				CListContainerElementOfFileList *pEle = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
				CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(0));
				CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));
				if (pCheckBox->IsSelected())
				{
					selcount++;
				}
			}

			if (bSel)
			{
				selcount--;
			}
			else
			{
				selcount++;
			}

			if (selcount == count)
			{
				pAllSel->Selected(true);
			}
			else
			{
				pAllSel->Selected(false);
			}
		}
		else if (msg.pSender->GetName() == _T("btn_add"))
		{
			OnClick_Btn_Add(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_eport"))
		{
			OnClick_Btn_Export(msg);
		}
		else if (msg.pSender->GetName() == _T("btn_delete"))
		{
			OnClick_Btn_Delete(msg);
		}
		else if ((StrCmp(msg.pSender->GetClass(), _T("ButtonUI")) == 0) &&	
			(msg.pSender->GetParent() != NULL) && 
			(msg.pSender->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent() != NULL) &&
			(StrCmp(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent()->GetName().GetData(), _T("list_file")) == 0) &&
			(StrCmp(msg.pSender->GetText().GetData(), _T("取消")) == 0) &&
			(msg.pSender->GetTag() == 51))
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(msg.pSender->GetParent()->GetParent());
			CFileHandle *pFileHandle = pItem->GetFileHandle();			

			if (pFileHandle->GetFileType() == 0)
			{
				SetFileListEleOptVisible(pItem, false);
			}
			else
			{
				if (pFileHandle->IsUploadOrDownLaodPre())
				{
						CListUI *pFileList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_file")));
						pFileList->Remove(pItem);
				}
				else if (pFileHandle->IsUploadOrDownLoadRuning())
				{
					CWndMessageBox *pWnd = new CWndMessageBox;
					pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
					pWnd->SetTitleAndTip(_T("提示"), _T("文件正在处理，请稍后取消!"));
					pWnd->CenterWindow();
					pWnd->ShowModal();
				}
			}

			CFileHandleManager::GetSingleInstance()->CancelTask(pFileHandle);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		LPCTSTR str = msg.pSender->GetClass();
		CDuiString str1 = msg.pSender->GetParent()->GetParent()->GetName();

		if (StrCmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0  && 
			msg.pSender->GetParent() != NULL && 
			msg.pSender->GetParent()->GetParent() != NULL &&
			msg.pSender->GetParent()->GetParent()->GetName() == _T("list_file"))
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(msg.pSender);
			CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(0));
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));

			bool bSel = pCheckBox->IsSelected();

			if (bSel)
			{
				return;
			}

			pCheckBox->Selected(!bSel);

			
			//判断全选checkbox
			CCheckBoxUI *pAllSelect = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("checkbox_select_all")));

			
			int count = m_pFileListUI->GetCount();

			int selectedcount = 0;

			for (int i=0; i<count; i++)
			{
				CListContainerElementOfFileList *pEle = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
				CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(0));
				CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));

				if (pCheckBox->IsSelected())
				{
					selectedcount++;
				}
			}

			if (selectedcount == count)
			{
				pAllSelect->Selected(true);
			}
			else
			{
				pAllSelect->Selected(false);
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
	{
		if (msg.pSender->GetName() == _T("tree_file"))
		{
			CTreeViewUI *pTreeFile = static_cast<CTreeViewUI*>(msg.pSender);
			int nCurIndex = (int)(msg.wParam);

			CTreeNodeUI *pNode = static_cast<CTreeNodeUI*>(pTreeFile->GetItemAt(nCurIndex));
			CFileHandle *pFileHandle = (CFileHandle *)(pNode->GetTag());
			
			CCheckBoxUI *pAllSelect = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("checkbox_select_all")));
			pAllSelect->Selected(false);

			SetCurFileHandle(pFileHandle);

			SearchFileAddToFileList(pFileHandle);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_TEXTCHANGED)
	{
		if ((StrCmp(msg.pSender->GetClass(), _T("PathEditUI") ) == 0) &&
			(msg.pSender->GetParent() != NULL) && 
			(msg.pSender->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent() != NULL) &&
			(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent() != NULL) &&
			(StrCmp(msg.pSender->GetParent()->GetParent()->GetParent()->GetParent()->GetName().GetData(), _T("list_file")) == 0)	)
		{
			CPathEditUI *pNameEdit = static_cast<CPathEditUI*>(msg.pSender);
			CDuiString strText = pNameEdit->GetText();

			CListContainerElementOfFileList *pEle = static_cast<CListContainerElementOfFileList*>(msg.pSender->GetParent()->GetParent());
			CFileHandle *pFileHandle = pEle->GetFileHandle();
			pFileHandle->SetFileName(strText.GetData());

			CDatabaseManager::UpDateFileName(pFileHandle->GetFileID().c_str(), strText.GetData());

			CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));

			int count = pTreeView->GetCount();
			for (int i=0; i<count; i++)
			{
				CTreeNodeUI *pNode = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(i));
				CFileHandle *pFile = (CFileHandle*)(pNode->GetTag());
				if (pFileHandle == pFile)
				{
					pNode->SetItemText(strText.GetData());
					pNode->GetTreeNodeHoriznotal()->SetToolTip(strText.GetData());
					break;
				}
			}		
		}
	}
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		if (msg.pSender->GetName() == _T("opt_file_name_order"))
		{
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(msg.pSender);
			bool bState = pCheckBox->IsSelected();

			if (bState)
			{
				OrderFileList(0, true);
			}
			else
			{
				OrderFileList(0, false);
			}			
		}
		else if (msg.pSender->GetName() == _T("opt_file_size_order"))
		{
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(msg.pSender);
			bool bState = pCheckBox->IsSelected();

			if (bState)
			{
				OrderFileList(1, true);
			}
			else
			{
				OrderFileList(1, false);
			}		
		}
		else if (msg.pSender->GetName() == _T("opt_file_time_order"))
		{
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(msg.pSender);
			bool bState = pCheckBox->IsSelected();

			if (bState)
			{
				OrderFileList(2, true);
			}
			else
			{
				OrderFileList(2, false);
			}		
		}
	}
	else if (msg.sType == DUI_MSGTYPE_FILE_LIST_DBCLICK)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(msg.pSender);
		CFileHandle *pFileHandle = pItem->GetFileHandle();			
		if (pFileHandle->GetFileType() == 0)
		{
			EmptyFileList();
			SearchFileAddToFileList(pFileHandle);
			SetCurFileHandle(pFileHandle);
		}
		else
		{
			HANDLE hThread = CreateThread(NULL, 0, _OpenFileThreadProc, pFileHandle, NULL, NULL);
			CloseHandle(hThread);		
		}
	}
}

LRESULT CWndMainFrame::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	__super::OnCreate(uMsg, wParam, lParam, bHandled);

	SetIcon(IDI_SAFEMANAGER);

	m_pTreeViewUI = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));	
	m_pHorFilePath = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("hor_file_path")));

	Init();

	return 0;
}

void CWndMainFrame::Init()
{
	InitFilePath();

	CreateFileList();

	InitFileTree();
	InitFileList();

	string_t strIconName;
	HBITMAP hBmp;

	strIconName = _T("dir_icon_small");

	if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
	{
		hBmp = GetIconByFileType(NULL, true);
		m_PaintManager.AddImage(strIconName.c_str(), hBmp, 16, 16, false);
	}

	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	CTreeNodeUI *pRoot = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(0));
	pRoot->SetAttribute(_T("checkboxattr"), _T("width=\"20\" height=\"20\" enabled=\"false\" bkimage=\"file='dir_icon_small' dest='4,2,20,18'\""));
}

void CWndMainFrame::InitFileTree()
{
	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	CTreeNodeUI *pRoot = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(0));

	CFileHandle *pFileRoot = CFileHandleManager::GetSingleInstance()->GetFileHandleTree();
	pRoot->SetTag((UINT_PTR)pFileRoot);
	
	SetCurFileHandle(pFileRoot);		

	EnumFileHandleTreeAddToTree(pRoot);
}

void CWndMainFrame::EnumFileHandleTreeAddToTree( CTreeNodeUI *pParent )
{
	CFileHandle *pParentFileHandle = (CFileHandle*)(pParent->GetTag());

	int count = pParentFileHandle->GetChildrenCount();

	for (int i=0; i<count; i++)
	{
		CFileHandle *pFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetChild(i));

		if (pFileHandle->GetFileType() != 0)
		{
			continue;
		}

		CTreeNodeUI *pNode = CreateTreeNode(pFileHandle);

		pParent->Add(pNode);
		pParent->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
		pParent->GetFolderButton()->SetVisible(true);

		EnumFileHandleTreeAddToTree(pNode);
	}
}

void CWndMainFrame::TravFileHandleTreeAddToTree(CTreeNodeUI *pParent, CFileHandle *pRoot )
{
	int count = pRoot->GetChildrenCount();

	for(int i=0; i<count ;i++)
	{
		CFileHandle *pFile = static_cast<CFileHandle*>(pRoot->GetChild(i));

		if (pFile->GetFileType() == 0)
		{
			CTreeNodeUI *pNode = CreateTreeNode(pFile);
			pParent->Add(pNode);
			pParent->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
			pParent->GetFolderButton()->SetVisible(true);	

			TravFileHandleTreeAddToTree(pNode, pFile);
		}
	}
}

CTreeNodeUI * CWndMainFrame::CreateTreeNode(CFileHandle *pFileHandle)
{
	CTreeNodeUI *pRetNode = new CTreeNodeUI;
	pRetNode->SetItemText(pFileHandle->GetFileName().c_str());
	pRetNode->SetAttribute(_T("endellipsis"), _T("true"));
	pRetNode->GetTreeNodeHoriznotal()->SetToolTip(pFileHandle->GetFileName().c_str());
	pRetNode->GetItemButton()->SetFont(6);
	pRetNode->SetFixedHeight(20);

	pRetNode->SetTag((UINT_PTR)pFileHandle);	

	pRetNode->SetAttribute(_T("itemattr"), _T("textpadding=\"2,0,0,0\" textcolor=\"#FF000000\""));
	pRetNode->SetAttribute(_T("checkboxattr"), _T("width=\"20\" height=\"20\" enabled=\"false\" bkimage=\"file='dir_icon_small' fade=\'255\' dest='4,2,20,18'\""));
	pRetNode->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"1\" normalimage=\"\" hotimage=\"\" selectedimage=\"\" selectedhotimage=\"\""));

	pRetNode->GetCheckBox()->SetMouseEnabled(false);
	pRetNode->GetFolderButton()->SetVisible(false);
		
	return pRetNode;
}

void CWndMainFrame::CreateFileList()
{
	CContainerUI *pContainer = static_cast<CContainerUI*>(m_PaintManager.FindControl(_T("con_file_list")));

	CFileListUI *pList = new CFileListUI;
	m_pFileListUI = pList;
	pContainer->Add(pList);
	pList->SetName(_T("list_file"));
	pList->ApplyAttributeList(_T("bkcolor=\"#FFf4f3f3\" itemfont=\"2\" itemalign=\"center\" itembkcolor=\"#FFF4F3F3\" itemselectedbkcolor=\"#FFCBDDFF\" itemhotbkcolor=\"#FFCBDDFF\" itemaltbk=\"false\" vscrollbar=\"true\" hscrollbar=\"true\" headerbkimage=\"list_header_bg.png\""));
	
			CListHeaderUI *pHeader = pList->GetHeader();
			pHeader->ApplyAttributeList(_T("height=\"25\" bkcolor=\"#FFf7f8f8\""));

					CListHeaderItemUI *pChkBoxItem = new CListHeaderItemUI;
					pHeader->Add(pChkBoxItem);
					pChkBoxItem->ApplyAttributeList(_T("inset=\"1,0,1,0\" minwidth=\"27\" maxwidth=\"27\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));
							
										CHorizontalLayoutUI *pHor = new CHorizontalLayoutUI;
										pChkBoxItem->Add(pHor);

														CControlUI *pControl = new CControlUI;
														pHor->Add(pControl);
														CCheckBoxUI *pCheckBox = new CCheckBoxUI;
														pCheckBox->SetName(_T("checkbox_select_all"));	
														pCheckBox->SetAttribute(_T("padding"), _T("0,3,0,0"));
														pCheckBox->SetAttribute(_T("normalimage"), _T("file='imagelist.checkbox.png' source='0,0,19,19'"));
														pCheckBox->SetAttribute(_T("hotimage"), _T("file='imagelist.checkbox.png' source='20,0,39,19'"));
														pCheckBox->SetAttribute(_T("selectedimage"), _T("file='imagelist.checkbox.png' source='60,0,79,19'"));
														pCheckBox->SetAttribute(_T("disabledimage"), _T("file='imagelist.checkbox.png' source='40,0,59,19'"));
														pCheckBox->SetAttribute(_T("width"), _T("19"));
														pCheckBox->SetAttribute(_T("height"), _T("19"));
														pHor->Add(pCheckBox);	
														pControl = new CControlUI;
														pHor->Add(pControl);

					CListHeaderItemUI *pNameItem = new CListHeaderItemUI;
					pHeader->Add(pNameItem);
					pNameItem->ApplyAttributeList(_T("text=\"文件名\" textpadding=\"10,0,0,0\" inset=\"1,0,1,0\" endellipsis=\"true\" textcolor=\"#FF999999\" align=\"left\" font=\"1\" width=\"260\" minwidth=\"260\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));

							COptionUI *pOption = new COptionUI;
							pOption->SetName(_T("opt_file_name_order"));
							pNameItem->Add(pOption);							
							pOption->ApplyAttributeList(_T("selectedimage=\"file='' dest='0,0,0,0'\""));

					CListHeaderItemUI *pSizeItem = new CListHeaderItemUI;
					pHeader->Add(pSizeItem);
					pSizeItem->ApplyAttributeList(_T("text=\"文件大小\" textpadding=\"10,0,0,0\" inset=\"1,0,1,0\" endellipsis=\"true\" textcolor=\"#FF999999\" align=\"left\" font=\"1\" width=\"160\" minwidth=\"57\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));

							pOption = new COptionUI;
							pOption->ApplyAttributeList(_T("name=\"opt_file_size_order\" selectedimage=\"file='' dest='0,0,0,0'\""));
							pSizeItem->Add(pOption);							

					CListHeaderItemUI *pModifyTimeItem = new CListHeaderItemUI;
					pHeader->Add(pModifyTimeItem);
					pModifyTimeItem->ApplyAttributeList(_T("text=\"修改时间\" textpadding=\"10,0,0,0\" inset=\"1,0,1,0\" endellipsis=\"true\" textcolor=\"#FF999999\" align=\"left\" font=\"1\" width=\"120\" minwidth=\"150\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));

							pOption = new COptionUI;
							pOption->ApplyAttributeList(_T("name=\"opt_file_time_order\" selectedimage=\"file='' dest='0,0,0,0'\""));
							pModifyTimeItem->Add(pOption);							

					CListHeaderItemUI *pProgressItem = new CListHeaderItemUI;
					pHeader->Add(pProgressItem);
					pProgressItem->ApplyAttributeList(_T("textcolor=\"#FF20afe4\" align=\"left\" font=\"1\" width=\"120\" minwidth=\"120\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));

					CListHeaderItemUI *pTipItem = new CListHeaderItemUI;
					pHeader->Add(pTipItem);
					pTipItem->ApplyAttributeList(_T("textcolor=\"#FF20afe4\" align=\"left\" font=\"1\" width=\"50\" minwidth=\"50\" maxwidth=\"50\" sepimage=\"list_header_sep.png\" sepwidth=\"1\" sepimm=\"true\""));
}

void CWndMainFrame::InitFileList()
{
	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	CTreeNodeUI *pRoot = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(0));

	CFileHandle *pFileHandle = (CFileHandle*)(pRoot->GetTag());

	SearchFileAddToFileList(pFileHandle);
}



void CWndMainFrame::EmptyFileList()
{	
	m_pFileListUI->RemoveAll();
	SetAllSelCheckBoxState(false);
}

CListContainerElementOfFileList * CWndMainFrame::CreateFileListItem( CFileHandle *pFileHandle )
{	
	if (pFileHandle->IsDelete() || ((pFileHandle->GetFileType() == 1) && pFileHandle->IsUpLoadCancel()))
	{
		return NULL;
	}	

	CHorizontalLayoutUI *pHor = NULL;

	CListContainerElementOfFileList *pEle = new CListContainerElementOfFileList;	
	pEle->SetFileHandle(pFileHandle);
	pEle->SetFixedHeight(25);	

	int nHeadCount = m_pFileListUI->GetHeader()->GetCount();
	for (int i=0; i<nHeadCount; i++)
	{
		CListHeaderItemUI *pHeadItem = static_cast<CListHeaderItemUI*>(m_pFileListUI->GetHeader()->GetItemAt(0));
		pHor = new CHorizontalLayoutUI;

		pEle->AddAt(pHor, i);

		if (i != 0 && i != 5)
		{
			pHor->SetMouseEnabled(false);
			pHor->SetMouseChildEnabled(false);
		}
	}

	//多选框
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(0));
	pHor->SetAttribute(_T("inset"), _T("0,2,0,2"));
	CCheckBoxUI *pCheckBox = new CCheckBoxUI; 
	pCheckBox->SetAttribute(_T("normalimage"), _T("file='imagelist.checkbox.png' source='0,0,19,19'"));
	pCheckBox->SetAttribute(_T("hotimage"), _T("file='imagelist.checkbox.png' source='20,0,39,19'"));
	pCheckBox->SetAttribute(_T("selectedimage"), _T("file='imagelist.checkbox.png' source='60,0,79,19'"));
	pCheckBox->SetAttribute(_T("disabledimage"), _T("file='imagelist.checkbox.png' source='40,0,59,19'"));
	pCheckBox->SetAttribute(_T("width"), _T("19"));
	pCheckBox->SetAttribute(_T("height"), _T("19"));
	pHor->Add(new CControlUI);
	pHor->Add(pCheckBox);
	pHor->Add(new CControlUI);

	//图标和文件名
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(1));
	pHor->SetAttribute(_T("inset"), _T("10,0,0,0"));
	CControlUI *pIconLabel = new CControlUI;
	pHor->Add(pIconLabel);

	string_t strIconName;	
	
	HBITMAP hBmp;
	int nWidth = 0;
	int nHeight =0;
	
	if (pFileHandle->GetFileType() == 0)
	{
		m_pFileListUI->Add(pEle);
		m_IsDirIndex++;

		strIconName = _T("dir_icon_small");
	
		if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
		{
			hBmp = GetIconByFileType(NULL, true);

			BITMAP bmp;
			GetObject(hBmp, sizeof(BITMAP), &bmp);
			nWidth = bmp.bmWidth;
			nHeight = bmp.bmHeight;

			m_PaintManager.AddImage(strIconName.c_str(), hBmp, nWidth, nHeight, false);
		}			
	}
	else
	{
		m_pFileListUI->Add(pEle);

		string_t strName = pFileHandle->GetFileName();

		LPTSTR lpTemp = StrRChr(strName.c_str(), NULL, _T('.'));
	
		if (lpTemp == NULL)
		{
			strIconName = _T("NULL_icon_small");
	
			if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
			{
				hBmp = GetIconByFileType(NULL, false);			

				BITMAP bmp;
				GetObject(hBmp, sizeof(BITMAP), &bmp);
				nWidth = bmp.bmWidth;
				nHeight = bmp.bmHeight;

				m_PaintManager.AddImage(strIconName.c_str(), hBmp, nWidth, nHeight, false);
			}
		}
		else
		{
			strIconName = lpTemp + 1;
			strIconName += _T("_icon_small");
	
			if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
			{
				hBmp = GetIconByFileType(lpTemp, false);

				BITMAP bmp;
				GetObject(hBmp, sizeof(BITMAP), &bmp);
				nWidth = bmp.bmWidth;
				nHeight = bmp.bmHeight;

				m_PaintManager.AddImage(strIconName.c_str(), hBmp, nWidth, nHeight, false);
			}
		}
	}

	const TImageInfo *pImgInfo = m_PaintManager.GetImage(strIconName.c_str());
	nWidth = pImgInfo->nX;
	nHeight = pImgInfo->nY;

	TCHAR bkImg[MAX_PATH] = {0};
	StringCchPrintf(bkImg, _countof(bkImg), _T("file='%s' dest='0,%d,%d,%d'"), strIconName.c_str(), (25 - nHeight) / 2, nWidth, (25 - nHeight) / 2 + nHeight);

	pEle->SetFileIconName(strIconName.c_str());

	pIconLabel->SetFixedWidth(nWidth);
	pIconLabel->SetFixedHeight(25);
	pIconLabel->SetAttribute(_T("bkimage"), bkImg);

	CPathEditUI *pFileNameEdit = new CPathEditUI;
	pHor->Add(pFileNameEdit);	
	pFileNameEdit->SetText(pFileHandle->GetFileName().c_str());
	pFileNameEdit->SetToolTip(pFileHandle->GetFileName().c_str());
	pFileNameEdit->SetBkColor(0x00FFFFFF);
	pFileNameEdit->SetFont(6);
	pFileNameEdit->SetAttribute(_T("textpadding"), _T("3,0,0,0"));
	pFileNameEdit->SetAttribute(_T("endellipsis"), _T("true"));

	//文件大小
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(2));
	CLabelUI *pFileSizeLabel = new CLabelUI;
	pFileSizeLabel->SetFont(6);
	pFileSizeLabel->SetTextColor(0xff999999);
	pFileSizeLabel->SetAttribute(_T("textpadding"), _T("10,0,10,0"));
	pFileSizeLabel->SetAttribute(_T("align"), _T("right"));
	pHor->Add(pFileSizeLabel);
	TCHAR strFileSize[MAX_PATH] = {0};		
	if (pFileHandle->GetFileType() == 0)
	{
		StringCchPrintf(strFileSize, _countof(strFileSize), _T("---"));	
	}
	else
	{		
		ModifyFileSizeFormat(strFileSize, MAX_PATH, pFileHandle->GetFileSize());		
	}
	pFileSizeLabel->SetText(strFileSize);	

	//修改时间
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(3));
	pHor->SetAttribute(_T("inset"), _T("10,0,0,0"));
	CLabelUI *pFileTimeLabel = new CLabelUI;
	pHor->Add(pFileTimeLabel);
	INT64	N64Time = pFileHandle->GetModifyTime();
	pFileTimeLabel->SetText(INT64TimeToString(N64Time).c_str());
	pFileTimeLabel->SetFont(6);
	pFileTimeLabel->SetTextColor(0xff999999);

	//操作提示文件
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(4));
	CLabelUI *pOpTip = new CLabelUI;
	pHor->Add(pOpTip);
	pOpTip->SetAttribute(_T("align"), _T("center"));	
	pOpTip->SetVisible(false);
	pOpTip->SetFont(6);
	pOpTip->SetTextColor(0xff999999);

	//取消
	pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(5));
	CButtonUI *pOpBtn = new CButtonUI;
	pHor->Add(pOpBtn);
	pOpBtn->SetTag(51);
	pOpBtn->SetAttribute(_T("font"), _T("5"));
	pOpBtn->SetText(_T("取消"));
	pOpBtn->SetAttribute(_T("textcolor"), _T("#FFFF0000"));
	pOpBtn->SetVisible(false);

	//计算进度
	if (pFileHandle->IsUploadOrDownLoading())
	{
		float progress = pFileHandle->CountProgress();

		if ( (progress - 1.0 > -0.0001) && (progress - 1.0 < 0.0001))
		{
			SetFileListEleOptVisible(pEle, false);
		}	
		else
		{
			TCHAR szTip[MAX_PATH] = {0};
			StringCchPrintf(szTip, _countof(szTip), _T("正在处理(%0.1f"), progress * 100);
			StringCchCat(szTip, _countof(szTip), _T("%)"));

			pOpTip->SetText(szTip);
			pOpTip->SetVisible(true);
			pOpBtn->SetVisible(true);
		}
	}

	TCHAR szAllTip[300] = {0};
	StringCchPrintf(szAllTip, _countof(szAllTip),_T("%s\n%s\n%s"), pFileHandle->GetFileName().c_str(), strFileSize, INT64TimeToString(N64Time).c_str());
	pEle->SetToolTip(szAllTip);

	return pEle;
}

void CWndMainFrame::SetFileListEleOptVisible( CListContainerElementOfFileList *pItem ,bool visble /*= false*/ )
{
	CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(4));
	CLabelUI *pLable = static_cast<CLabelUI*>(pHor->GetItemAt(0));
	pLable->SetVisible(visble);

	pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(5));
	CButtonUI *pOpBtn = static_cast<CButtonUI*>(pHor->GetItemAt(0));
	pOpBtn->SetVisible(visble);
}

void CWndMainFrame::SetFileListEleOptTip( CListContainerElementOfFileList *pItem, LPCTSTR lpTip )
{
	SetFileListEleOptVisible(pItem, true);

	CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(4));
	CLabelUI *pLable = static_cast<CLabelUI*>(pHor->GetItemAt(0));
	pLable->SetText(lpTip);
}

bool CWndMainFrame::IsDropValidRect( const POINTL &pt )
{
	bool bRet = false;

	string_t strRectName;

	CControlUI *pControl = NULL;

	POINT tempPt;
	tempPt.x = pt.x ;
	tempPt.y = pt.y ;

	RECT rcListUI = m_pFileListUI->GetPos();
	RECT rcTreeUI = m_pTreeViewUI->GetPos();

	if (::PtInRect(&rcListUI, tempPt))
	{
		bRet = true;

		pControl = m_PaintManager.FindControl(tempPt);

		while(pControl != NULL)
		{
			LPCTSTR lpName = pControl->GetClass();
			if (StrCmp(lpName, _T("ListContainerElementUI")) == 0)
			{
				break;
			}

			pControl = pControl->GetParent();
		}

		if (pControl == NULL)
		{
			strRectName = m_Current_File_Handle->GetFileName();
		}
		else
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(pControl);
			CFileHandle *pFileHandle = pItem->GetFileHandle();	

			if (pFileHandle->GetFileType() == 0)
			{
				strRectName = pFileHandle->GetFileName();
				pItem->Select(true);
			}
			else
			{
				m_pFileListUI->SelectItem(-1);
				strRectName = m_Current_File_Handle->GetFileName();
			}					
		}	
	}
	else if(::PtInRect(&rcTreeUI, tempPt))
	{
		bRet = true;

		pControl = m_PaintManager.FindControl(tempPt);

		while(pControl != NULL)
		{
			LPCTSTR lpName = pControl->GetClass();
			if (StrCmp(lpName, _T("TreeNodeUI")) == 0)
			{
				break;
			}

			pControl = pControl->GetParent();
		}

		if (pControl == NULL)
		{
			m_pTreeViewUI->SelectItem(-1);
			strRectName = m_Current_File_Handle->GetFileName();
		}
		else
		{
			CTreeNodeUI *pCurTreeNode = static_cast<CTreeNodeUI*>(pControl);
			CFileHandle *pFileHandle = (CFileHandle*)(pCurTreeNode->GetTag());
			strRectName = pFileHandle->GetFileName();
			pCurTreeNode->Select(true);
		}
	}

	return bRet;
}

void CWndMainFrame::OnDropFiles( CWndMainFrame *pMainWnd, HDROP hDrop, POINT pt )
{
	TCHAR szFilePathName[MAX_PATH] = {0};
	CFileHandle *pCurFileHandle = NULL;

	CControlUI *pControl = NULL;
	RECT rcListUI = m_pFileListUI->GetPos();
	RECT rcTreeUI = m_pTreeViewUI->GetPos();

	if (::PtInRect(&rcListUI, pt))
	{
		pControl = m_PaintManager.FindControl(pt);

		while(pControl != NULL)
		{
			LPCTSTR lpName = pControl->GetClass();
			if (StrCmp(lpName, _T("ListContainerElementUI")) == 0)
			{
				break;
			}

			pControl = pControl->GetParent();
		}

		if (pControl == NULL)
		{
			pCurFileHandle = m_Current_File_Handle;
		}
		else
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(pControl);
			CFileHandle *pFileHandle = pItem->GetFileHandle();	

			if (pFileHandle->GetFileType() != 0)
			{
				pCurFileHandle = m_Current_File_Handle;
			}
			else
			{
				pCurFileHandle = pFileHandle;
			}					
		}	
	}
	else if(::PtInRect(&rcTreeUI, pt))
	{
		pControl = m_PaintManager.FindControl(pt);

		while(pControl != NULL)
		{
			LPCTSTR lpName = pControl->GetClass();
			if (StrCmp(lpName, _T("TreeNodeUI")) == 0)
			{
				break;
			}

			pControl = pControl->GetParent();
		}

		if (pControl == NULL)
		{
			pCurFileHandle = m_Current_File_Handle;
		}
		else
		{
			CTreeNodeUI *pCurTreeNode = static_cast<CTreeNodeUI*>(pControl);
			CFileHandle *pFileHandle = (CFileHandle*)(pCurTreeNode->GetTag());
			pCurFileHandle = pFileHandle;
		}
	}

	UINT  nNumOfFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); //得到文件个数  

	for (UINT nIndex=0 ; nIndex< nNumOfFiles; ++nIndex)  
	{  
		DragQueryFile(hDrop, nIndex, szFilePathName, _MAX_PATH);  //得到文件名  
		//获取了文件名，开始处理  

		LPSTTEMP lpTemp = new STTEMP;
		lpTemp->Path = szFilePathName;
		lpTemp->CurFileHandle = pCurFileHandle;
		lpTemp->nTag = 0;

		::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)lpTemp, NULL);	
	}  

	DragFinish(hDrop); 
}

bool CWndMainFrame::GetFileListPos( RECT &rc ) const
{
	CListUI *pFileList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_file")));
	if (pFileList != NULL)
	{
		rc = pFileList->GetPos();
		return true;
	}

	return false;
}

LRESULT CWndMainFrame::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	switch(uMsg)
	{
	case WM_LOAD_NEW_FILE_HANDLE:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;

			SearchFileAddToFileList(pFileHandle);
			SetCurFileHandle(pFileHandle);
			break;
		}
	case WM_COMMAND_ATTRIBUTE:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Attribute(pFileHandle);
			break;
		}
	case WM_COMMAND_DELETE:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Delete(pFileHandle);
			break;
		}
	case WM_COMMAND_RENAME:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Rename(pFileHandle);
			break;
		}
	case WM_COMMAND_EXPORT:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Export(pFileHandle);
			break;
		}
	case WM_COMMAND_COPY:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Copy(pFileHandle);
			break;
		}
	case WM_COMMAND_MOVE:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Move(pFileHandle);
			break;
		}
	case WM_COMMAND_OPEN:
		{
			CFileHandle *pFileHandle = (CFileHandle*)wParam;
			ExecutCommand_Open(pFileHandle);
			break;
		}
	case WM_COMMAND_REFRESH:
		{
			ExecutCommand_Refresh();
			break;
		}
	case WM_COMMAND_NEW_FOLDER:
		{
			ExecutCommand_NewFolder();
			break;
		}
	case WM_COMMAND_UPLOAD:
		{
			ExecutCommand_Upload();
			break;
		}
	case  WM_POP_MENU:
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>((CControlUI*)wParam);
			CFileHandle *pFileHandle = NULL;

			if (pItem != NULL)
			{
				pFileHandle = pItem->GetFileHandle();
			}

			POINT  pPt = *((POINT*)lParam);
			::ClientToScreen(m_hWnd, &pPt);

			if (pItem != NULL)
			{
				PopFileListMenu(pPt, pFileHandle);
			}
			else
			{
				PopFileListMenu(pPt, pFileHandle);
			}
			break;
		}
	case WM_UPDATE_FILE_LIST_UI:
		{
			CFileHandle *pFile = (CFileHandle*)wParam;
			int nTag = (int)lParam;

			CFileHandle *pParent = static_cast<CFileHandle*>(pFile->GetParentNode());			

			if (nTag == 0)
			{
				//更新目录树
				if (pFile->GetFileType() == 0)
				{				
					CTreeNodeUI *pNode = GetTreeNodeByFileHandle(static_cast<CFileHandle*>(pParent));

					CTreeNodeUI *pNewNode = CreateTreeNode(pFile);
					pNode->AddChildNode(pNewNode);
					pNode->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
					pNode->GetFolderButton()->SetVisible(true);						

					TravFileHandleTreeAddToTree(pNewNode, pFile);	
				}				

				//更新列表	
				if (pParent == m_Current_File_Handle)
				{
					CListContainerElementOfFileList *pItem = CreateFileListItem(pFile);
					SetFileListEleOptVisible(pItem, true);
					SetFileListEleOptTip(pItem, _T("正在处理(0.0%)"));
				}
			}
			else
			{
				CListContainerElementOfFileList *pItem = GetFileListItem(pFile);
				if (pItem != NULL)
				{
					SetFileListEleOptVisible(pItem, true);
					SetFileListEleOptTip(pItem, _T("正在处理(0.0%)"));
				}
			}

				break;
		}
	case WM_UPDATA_FILE_LIST_PROGRESS_UI:
		{
			CFileHandle * pFileHandle = (CFileHandle *)wParam;
			CFileHandle *pParent = static_cast<CFileHandle*>(pFileHandle->GetParentNode());			

			if (pParent == m_Current_File_Handle)
			{
					CListContainerElementOfFileList *pItem = GetFileListItem(pFileHandle);

					float progress = pFileHandle->CountProgress();

					TCHAR szTip[MAX_PATH] = {0};

					StringCchPrintf(szTip, _countof(szTip), _T("正在处理(%0.1f"), progress * 100);
					StringCchCat(szTip, _countof(szTip), _T("%)"));

					SetFileListEleOptTip(pItem, szTip);

					if ( (progress - 1.0 > -0.0001) && (progress - 1.0 < 0.0001))
					{
						SetFileListEleOptVisible(pItem, false);
					}				
			}		

			break;
		}
	}

	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

CTreeNodeUI * CWndMainFrame::GetTreeNodeByFileHandle( CFileHandle *pFileHandle )
{
	CTreeNodeUI *pRet = NULL;

	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	int count = pTreeView->GetCount();
	for (int i = 0; i<count; i++)
	{				
		CTreeNodeUI *pNode = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(i));
		CFileHandle *pFile = (CFileHandle*)(pNode->GetTag());
		if (pFile == pFileHandle)
		{
			pRet = pNode;
			break;
		}
	}

	return pRet;
}

void CWndMainFrame::SetCurFileHandle( CFileHandle *pFileHandle )
{
	m_Current_File_Handle = pFileHandle;

	AddToSelectedFileHandleHistory(m_Current_File_Handle);

	ParseCurFileHandlePaths(m_Current_File_Handle);
}

CListContainerElementOfFileList * CWndMainFrame::GetFileListItem( CFileHandle *pFileHandle )
{
	CListContainerElementOfFileList *pRet = NULL;
	
	int count = m_pFileListUI->GetCount();

	for (int i=0; i<count; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
		CFileHandle *pFile = pItem->GetFileHandle();
		if (pFileHandle == pFile)
		{
			pRet = pItem;
			break;
		}
	}

	return pRet;
}

void CWndMainFrame::SearchFileAddToFileList( CFileHandle *pFileHandle )
{
	BOOL bLoadAll = pFileHandle->IsLoadAllChildrenFile();

	if (!bLoadAll)		//判断子文件未加载完全
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

	EmptyFileList();

	int count = pFileHandle->GetChildrenCount();

	for (int i=0; i<count; i++)
	{
		CFileHandle *pChild = static_cast<CFileHandle*>(pFileHandle->GetChild(i));

		CreateFileListItem(pChild);
	}
}

void CWndMainFrame::RenameFile( CListContainerElementOfFileList *pEle )
{
	CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pEle->GetItemAt(1));
	CPathEditUI *pNameEdit = static_cast<CPathEditUI*>(pHor->GetItemAt(1));

	CDuiString str = pNameEdit->GetText();

	pNameEdit->ShowEditWnd();

	int i = 0;
}

void CWndMainFrame::SetMoveOrCopyTo( CFileHandle *pFileHandle )
{
	m_pMoveOrCopyTo = pFileHandle;
}

BOOL CWndMainFrame::IsFileListItemSelected( CListContainerElementOfFileList *pItem )
{
	BOOL bRet =FALSE;

	CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(0));
	CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));

	if (pCheckBox->IsSelected())
	{
		bRet = TRUE;
	}

	return bRet;
}

vector<CFileHandle*> * CWndMainFrame::GetMoveOrCopyFrom()
{
	return &m_vecMoveOrCopyFrom;
}

void CWndMainFrame::MoveFileTask(  )
{
	CFileHandle *pFileFrom = NULL;
	CFileHandle *pFileTo = m_pMoveOrCopyTo;

	int count = m_vecMoveOrCopyFrom.size();
	for (int i=0; i<count; i++)
	{
		pFileFrom = m_vecMoveOrCopyFrom[i];

		//处理内存树结构
		CFileHandle *pParent = static_cast<CFileHandle*>(pFileFrom->GetParentNode());
		pParent->RemoveChildNode(pFileFrom);

		pFileTo->AddChildrenNode(pFileFrom);
		pFileFrom->SetParentID(pFileTo->GetFileID());

		CDatabaseManager::UpdateFileParentID(pFileFrom->GetFileID().c_str(), pFileTo->GetFileID().c_str());

		TCHAR szTip[300] = {0};
		StringCchPrintf(szTip, _countof(szTip), _T("正在移动:%s"), pFileFrom->GetFileName().c_str()); 

		::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)(30 / count), (LPARAM)szTip);

		//处理界面
		if (pFileFrom->GetFileType() != 0)	//只是文件的话
		{

		}
		else				//是目录
		{
			CTreeNodeUI * pTreeItemFrom = NULL;
			CTreeNodeUI * pTreeItemTo = NULL;
			CFileHandle *pFileTemp = NULL;

			int nItemCount = m_pTreeViewUI->GetCount();	
			for (int i=0; i<nItemCount; i++)
			{
				pTreeItemFrom = static_cast<CTreeNodeUI*>(m_pTreeViewUI->GetItemAt(i));
				pFileTemp = (CFileHandle *)(pTreeItemFrom->GetTag());
				if (pFileTemp == pFileFrom)
				{
					break;
				}
			}

			for (int i=0; i<nItemCount; i++)
			{
				pTreeItemTo = static_cast<CTreeNodeUI*>(m_pTreeViewUI->GetItemAt(i));
				pFileTemp = (CFileHandle *)(pTreeItemTo->GetTag());
				if (pFileTemp == pFileTo)
				{
					break;
				}
			}

			CTreeNodeUI *pParentFrom = static_cast<CTreeNodeUI*>(pTreeItemFrom->GetParentNode());
			bool bRet = m_pTreeViewUI->Remove(pTreeItemFrom);

			if (pParentFrom->GetCountChild() == 0)
			{
				pParentFrom->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"1\" normalimage=\"\" hotimage=\"\" selectedimage=\"\" selectedhotimage=\"\""));
			}

			CTreeNodeUI *pNewNode = CreateTreeNode(pFileFrom);
			pTreeItemTo->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
			pTreeItemTo->AddChildNode(pNewNode);	
			TravFileHandleTreeAddToTree(pNewNode, pFileFrom);
		}
	

		::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)(70 / count), (LPARAM)szTip);

		int nListCount = m_pFileListUI->GetCount();
		for (int i=0; i<nListCount; i++)
		{
			CListContainerElementOfFileList *pListItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
			CFileHandle *pTemp = pListItem->GetFileHandle();

			if (m_pMoveOrCopyTo == m_Current_File_Handle)
			{
				break;
			}

			if (pTemp == pFileFrom)
			{
				m_pFileListUI->Remove(pListItem);
				break;
			}
		}

		::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)(100 / count), (LPARAM)szTip);
	}

	SetAllSelCheckBoxState(false);
	Sleep(2000);
	m_WndProgress->Close();
}

void CWndMainFrame::CopyFileTask()
{
	CFileHandle *pFileFrom = NULL;
	CFileHandle *pFileTo = m_pMoveOrCopyTo;

	//之后不要使用递归遍历了，可以按层遍历
	vector<CFileHandle*> vecNewFileHandle;

	int count = m_vecMoveOrCopyFrom.size();
	for (int i=0; i<count; i++)
	{
		pFileFrom = m_vecMoveOrCopyFrom[i];

		CFileHandle *pNew = CFileHandleManager::GetSingleInstance()->LoadFileAndAllChildrenFile(pFileFrom->GetFileID().c_str(), NULL);
		pFileTo->AddChildrenNode(pNew);
		
		vecNewFileHandle.push_back(pNew);
	}

	int j=0; 
	while(j < vecNewFileHandle.size())
	{
		int nChildrenCount = vecNewFileHandle[j]->GetChildrenCount();
		for (int k=0; k<nChildrenCount; k++)
		{
			vecNewFileHandle.push_back(static_cast<CFileHandle*>(vecNewFileHandle[j]->GetChild(k)));
		}

		j++;
	}

	TCHAR szTip[300] = {0};

	count = vecNewFileHandle.size();
	for (int i=0; i<count; i++)
	{
		CFileHandle *pCur = vecNewFileHandle[i];
		CFileHandle *pParent = static_cast<CFileHandle*>(pCur->GetParentNode());

		ZeroMemory(szTip, sizeof(szTip));
		StringCchPrintf(szTip, _countof(szTip), _T("正在复制: %s"), pCur->GetFileName().c_str());	
		::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)(100 * i / count), (LPARAM)szTip);

		SYSTEMTIME sysModifyTime;
		ZeroMemory(&sysModifyTime, sizeof(SYSTEMTIME));
		GetLocalTime(&sysModifyTime);
		pCur->SetModifyTime(SystemTimeToINT64(sysModifyTime));

		if (pCur->GetFileType() == 0)		//如果是文件夹
		{
			pCur->SetFileID(CreateGUID());
			pCur->SetParentID(pParent->GetFileID().c_str());
			CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileBaseInfo(pCur->GetFileID().c_str(), pCur->GetFileType(), pCur->GetFileName().c_str(), pCur->GetParentID().c_str(), pCur->GetFileSize(), pCur->GetModifyTime(), 0);

			CTreeNodeUI *pParentNode = GetTreeNodeByFileHandle(pParent);
			CTreeNodeUI *pNewNode = CreateTreeNode(pCur);
			pParentNode->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
			pParentNode->Add(pNewNode);
		}
		else														//如果是文件，需要拿到碎片ID，拷贝碎片文件，写入数据库
		{
			//测试用------------------------------//////////////////////////////////////////////////////////////////////////////
			CDatabaseManager::QueryFileFragInfoRecordByFileID(pCur->GetFileID().c_str(), *(pCur->GetFrageinfo()));

			for (int k = 0; k<((int)EVERY_FILE_CHIPS_NUMBER); k++)
			{
				string_t strNewFragID = CreateGUID();

				TCHAR szOldFragPath[MAX_PATH] = {0};
				TCHAR szNewFragPath[MAX_PATH] = {0};

				StringCchPrintf(szOldFragPath, _countof(szOldFragPath), _T("%s\\%s"), pCur->GetFrageinfo()->Path, pCur->GetFrageinfo()->Name[k]);
				StringCchPrintf(szNewFragPath, _countof(szNewFragPath), _T("%s\\%s"), pCur->GetFrageinfo()->Path, strNewFragID.c_str());

				CopyFile(szOldFragPath, szNewFragPath, TRUE);

				ZeroMemory(pCur->GetFrageinfo()->Name[k], sizeof(pCur->GetFrageinfo()->Name[k]));
				lstrcpy(pCur->GetFrageinfo()->Name[k], strNewFragID.c_str());
			}

			pCur->SetFileID(CreateGUID());
			pCur->SetParentID(pParent->GetFileID().c_str());
			CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileBaseInfo(pCur->GetFileID().c_str(), pCur->GetFileType(), pCur->GetFileName().c_str(), pCur->GetParentID().c_str(), pCur->GetFileSize(), pCur->GetModifyTime(), 0);

			//测试用------------------------------将相关碎片信息写入数据库//////////////////////////////////////////////////////////////////////////////
			CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileFragInfo(pCur->GetFileID().c_str(), pCur->GetFrageinfo()->Name[0], pCur->GetFrageinfo()->Name[1], pCur->GetFrageinfo()->Name[2]);
		
			if (pParent != m_Current_File_Handle)
			{
				pParent->RemoveChildNode(pCur);
				pParent->SetLoadAllChildrenFile(FALSE);
				delete pCur;
			}
		
		}	

		if (pParent == m_Current_File_Handle)
		{
			CreateFileListItem(pCur);

			SetAllSelCheckBoxState(false);
		}
	}

	::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)(100), (LPARAM)szTip);

	Sleep(2000);
	m_WndProgress->Close();
}

void CWndMainFrame::SetAllSelCheckBoxState( bool bState )
{
	CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(_T("checkbox_select_all")));
	pCheckBox->Selected(bState);
}

void CWndMainFrame::AddToSelectedFileHandleHistory( CFileHandle *pFileHandle )
{
	if (m_listSelFileHandleHistory.size() > 10)
	{
		m_listSelFileHandleHistory.pop_front();
	}

	m_listSelFileHandleHistory.push_back(pFileHandle);
	m_listHistoryIter = m_listSelFileHandleHistory.rbegin();
}

void CWndMainFrame::OnClick_Btn_Copy( TNotifyUI &msg )
{	
	BOOL bCanOpt = TRUE;
	int count = m_pFileListUI->GetCount();

	m_vecMoveOrCopyFrom.clear();

	for (int i=0; i<count; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
		if (IsFileListItemSelected(pItem))
		{
			CFileHandle *pSelFile = (CFileHandle*)(pItem->GetFileHandle());
			if (pSelFile->GetTaskCount() != 0)
			{
				CWndMessageBox *pMsgBox = new CWndMessageBox;
				pMsgBox->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
				pMsgBox->SetTitleAndTip(_T("提示"), _T("有文件正在进行处理，不可复制或移动!"));
				pMsgBox->CenterWindow();
				pMsgBox->ShowModal();
				return;
			}

			m_vecMoveOrCopyFrom.push_back(pSelFile);
		}
	}

	CWndMoveOrPasteTo *pWnd = new CWndMoveOrPasteTo(this, TRUE);
	pWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
	pWnd->CenterWindow();
	int nRet = pWnd->ShowModal();
	CFileHandle *pSelFileHandle = NULL;
	if (nRet == 1)
	{
		//下面进行文件复制操作流程
		pSelFileHandle = m_pMoveOrCopyTo;	

		ResetEvent(hMoveOrCopyEvent);
		HANDLE hThread = CreateThread(NULL, 0, _CopyThreadProc, this, NULL, NULL);
		CloseHandle(hThread);

		CWndProgress *pWndProgress = new CWndProgress;
		m_WndProgress = pWndProgress;
		pWndProgress->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
		pWndProgress->SetTip(_T("正在准备..."));
		pWndProgress->CenterWindow();
		pWndProgress->ShowModal();
	}
}

void CWndMainFrame::OnClick_Btn_MainPage( TNotifyUI &msg )
{
	CTreeNodeUI *pRoot = static_cast<CTreeNodeUI*>(m_pTreeViewUI->GetItemAt(0));
	CFileHandle *pFileHandle = (CFileHandle*)(pRoot->GetTag());

	SearchFileAddToFileList(pFileHandle);

	SetCurFileHandle(pFileHandle);
}

void CWndMainFrame::OnClick_Btn_ReName( TNotifyUI &msg )
{
	CListContainerElementOfFileList *pEle = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(m_pFileListUI->GetCurSel()));

	if (pEle != NULL)
	{
		RenameFile(pEle);
	}
}

void CWndMainFrame::OnClick_Btn_Move( TNotifyUI &msg )
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可移动！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}
	
	BOOL bCanOpt = TRUE;
	int count = m_pFileListUI->GetCount();

	m_vecMoveOrCopyFrom.clear();

	for (int i=0; i<count; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
		if (IsFileListItemSelected(pItem))
		{
			CFileHandle *pSelFile = (CFileHandle*)(pItem->GetFileHandle());
			if (pSelFile->GetTaskCount() != 0)
			{
				CWndMessageBox *pMsgBox = new CWndMessageBox;
				pMsgBox->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
				pMsgBox->SetTitleAndTip(_T("提示"), _T("有文件正在进行处理，不可复制或移动!"));
				pMsgBox->CenterWindow();
				pMsgBox->ShowModal();
				return;
			}

			m_vecMoveOrCopyFrom.push_back(pSelFile);
		}
	}

	CWndMoveOrPasteTo *pWnd = new CWndMoveOrPasteTo(this, FALSE);
	pWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
	pWnd->CenterWindow();
	int nRet = pWnd->ShowModal();
	CFileHandle *pSelFileHandle = NULL;
	if (nRet == 1)
	{
		pSelFileHandle = m_pMoveOrCopyTo;	

		//下面进行文件移动操作流程
		ResetEvent(hMoveOrCopyEvent);
		HANDLE hThread = CreateThread(NULL, 0, _MoveThreadProc, this, NULL, NULL);
		CloseHandle(hThread);

		CWndProgress *pWndProgress = new CWndProgress;
		m_WndProgress = pWndProgress;
		pWndProgress->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
		pWndProgress->SetTip(_T("正在准备..."));
		pWndProgress->CenterWindow();
		pWndProgress->ShowModal();
	}
}

void CWndMainFrame::OnClick_Btn_NewDir( TNotifyUI &msg )
{
	ExecutCommand_NewFolder();
}

void CWndMainFrame::OnClick_Btn_Export( TNotifyUI &msg )
{
	TCHAR szPathName[MAX_PATH] = {0};
	BROWSEINFO bInfo={0};
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("请选择存放路径");
	bInfo.ulFlags      =  BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE ; 
	bInfo.lpfn		 = NULL;

	LPITEMIDLIST lpDlist; 
	lpDlist   =   SHBrowseForFolder(&bInfo); 
	if (lpDlist != NULL)
	{
		SHGetPathFromIDList(lpDlist, szPathName);	

		CListUI *pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_file")));

		int count = pList->GetCount();
		for (int i=0; i<count; i++)
		{
			CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(pList->GetItemAt(i));
			CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(0));
			CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));

			if (pCheckBox->IsSelected())
			{
				LPSTTEMP lpTemp = new STTEMP;
				lpTemp->Path = szPathName;
				lpTemp->CurFileHandle = pItem->GetFileHandle();
				lpTemp->nTag = 1;

				::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)lpTemp, NULL);				
			}
		}
	}
}

void CWndMainFrame::OnClick_Btn_Delete( TNotifyUI &msg )
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可删除！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}

	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));	

	vector<LPSTTEMP> vecTemp;
	vector<CListContainerElementOfFileList*> vecListEle;
	vector<CTreeNodeUI*> vecTreeNodes;

	int count = m_pFileListUI->GetCount();
	for (int i=0; i<count; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
		CHorizontalLayoutUI *pHor = static_cast<CHorizontalLayoutUI*>(pItem->GetItemAt(0));
		CCheckBoxUI *pCheckBox = static_cast<CCheckBoxUI*>(pHor->GetItemAt(1));

		if (pCheckBox->IsSelected())
		{
			LPSTTEMP lpTemp = new STTEMP;
			lpTemp->CurFileHandle = pItem->GetFileHandle();
			lpTemp->nTag = 2;

			vecTemp.push_back(lpTemp);
			vecListEle.push_back(pItem);	

			int nodescount = pTreeView->GetCount();

			for (int k = 0; k<nodescount; k++)
			{
				CTreeNodeUI *pNode = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(k));

				if (pNode != NULL)
				{
					CFileHandle *pTempFile = (CFileHandle*)(pNode->GetTag());

					if (pTempFile == pItem->GetFileHandle())
					{								
						vecTreeNodes.push_back(pNode);
					}
				}
			}
		}
	}

	int vecEleCount = vecListEle.size();
	for (int  i = 0; i<vecEleCount; i++)
	{	
		m_pFileListUI->Remove(vecListEle[i]);
	}

	int vecTreeCount = vecTreeNodes.size();
	for (int i=0; i<vecTreeCount; i++)
	{
		pTreeView->Remove(vecTreeNodes[i]);
	}

	int VecTempCount = vecTemp.size();
	for (int k=0; k<VecTempCount; k++)
	{
		::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)vecTemp[k], NULL);
	}
}

void CWndMainFrame::OnClick_Btn_Add( TNotifyUI &msg )
{
	ExecutCommand_Upload();
}

void CWndMainFrame::OnClick_Btn_Forward( TNotifyUI &msg )
{
	m_listHistoryIter++;
	if (m_listHistoryIter == m_listSelFileHandleHistory.crend())
	{
		m_listHistoryIter--;
		return;
	}

	m_Current_File_Handle = *m_listHistoryIter;
	SearchFileAddToFileList(m_Current_File_Handle);	

	ParseCurFileHandlePaths(m_Current_File_Handle);
}

void CWndMainFrame::OnClick_Btn_Next( TNotifyUI &msg )
{
	if (m_listHistoryIter == m_listSelFileHandleHistory.rbegin())
	{
		return;
	}

	m_listHistoryIter--;

	m_Current_File_Handle = *m_listHistoryIter;
	SearchFileAddToFileList(m_Current_File_Handle);

	ParseCurFileHandlePaths(m_Current_File_Handle);
}

void CWndMainFrame::OnClick_Btn_Search( TNotifyUI &msg )
{
	CEditUI *pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit_search")));
	CDuiString strText = pEdit->GetText();

	if (strText.IsEmpty())
	{
		return ;
	}

	m_strSearchContent = strText.GetData();

	ResetEvent(hMoveOrCopyEvent);
	HANDLE hThread = CreateThread(NULL, 0, _SearchThreadProc, this, NULL, NULL);
	CloseHandle(hThread);

	CWndProgress *pWndProgress = new CWndProgress;
	m_WndProgress = pWndProgress;
	pWndProgress->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
	pWndProgress->SetTip(_T("正在查询..."));
	pWndProgress->CenterWindow();
	pWndProgress->ShowModal();	
}

void CWndMainFrame::SearchFile()
{
	m_Current_File_Handle =NULL;

	TCHAR szTip[100] = {0};
	lstrcpy(szTip, _T("正在查询文件..."));
	::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)0, (LPARAM)szTip);

	vector<STDBFILEBASEINFO> vecInfos;
	CDatabaseManager::GetAllFileNameHaveSubstring(m_strSearchContent.c_str(), vecInfos);

	ZeroMemory(szTip, sizeof(szTip));
	lstrcpy(szTip, _T("正在加载文件和更新界面..."));
	::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)60, (LPARAM)szTip);

	EmptyFileList();

	CFileHandle * pFileHandle = NULL;

	int nCount = vecInfos.size();
	for (int i=0; i<nCount; i++)
	{
		if (vecInfos[i].file_type == 0)			//文件的话直接搜索内存树
		{
			pFileHandle = CFileHandleManager::GetSingleInstance()->SearchFileHandleFromTreeByFileID(vecInfos[i].file_id.c_str());
			CreateFileListItem(pFileHandle);
		}
		else			//文件的话找到父目录，父目录加载完全再找到它
		{
			pFileHandle = CFileHandleManager::GetSingleInstance()->SearchFileHandleFromTreeByFileID(vecInfos[i].parent_id.c_str());

			CFileHandleManager::GetSingleInstance()->LoadAllChilrenFile(pFileHandle);

			int nChildrenCount = pFileHandle->GetChildrenCount();
			for (int k=0; k<nChildrenCount; k++)
			{
				CFileHandle *pChild = static_cast<CFileHandle*>(pFileHandle->GetChild(k));
				if (StrCmp(pChild->GetFileID().c_str(), vecInfos[i].file_id.c_str()) == 0)
				{
					pFileHandle = pChild;
					break;
				}
			}

			CreateFileListItem(pFileHandle);
		}
	}

	ZeroMemory(szTip, sizeof(szTip));
	lstrcpy(szTip, _T("查询完成..."));
	::SendMessage(m_WndProgress->GetHWND(), WM_UPDATE_PROGRESS, (WPARAM)100, (LPARAM)szTip);


	SetAllSelCheckBoxState(false);
	Sleep(2000);
	m_WndProgress->Close();
}

void CWndMainFrame::OrderFileList( int nIndex, bool bAscend /*= true*/ )
{
	int nCount = m_pFileListUI->GetCount();

	vector<CFileHandle*> vecFileHandlesDirs;
	vector<CFileHandle*> vecFileHandlesFiles;

	for (int i=0; i<nCount; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));

		CFileHandle *pTemp = pItem->GetFileHandle();

		if (pTemp->GetFileType() == 0)
		{
			vecFileHandlesDirs.push_back(pTemp);
		}
		else
		{
			vecFileHandlesFiles.push_back(pTemp);
		}		
	}

		int nDirCount = vecFileHandlesDirs.size();

		for (int i=0; i<nDirCount; i++)
		{
			int nTemp = 0;

			for (int k=0; k<nDirCount - i; k++)
			{
				if (nIndex == 0)		//文件名
				{
					if (StrCmp(vecFileHandlesDirs[k]->GetFileName().c_str(), vecFileHandlesDirs[nTemp]->GetFileName().c_str()) > 0)
					{
						nTemp = k;
					}
				}
				else if (nIndex == 1)		//文件大小
				{
					if (vecFileHandlesDirs[k]->GetFileSize() > vecFileHandlesDirs[nTemp]->GetFileSize())
					{
						nTemp = k;
					}
				}
				else if (nIndex == 2)		//修改时间
				{
					if (vecFileHandlesDirs[k]->GetModifyTime() > vecFileHandlesDirs[nTemp]->GetModifyTime())
					{
						nTemp = k;
					}
				}
			}

			CFileHandle *pFileTemp = vecFileHandlesDirs[nDirCount - i - 1];
			vecFileHandlesDirs[nDirCount - i - 1] = vecFileHandlesDirs[nTemp];
			vecFileHandlesDirs[nTemp] = pFileTemp;
		}

		int nFileCount = vecFileHandlesFiles.size();

		for (int i=0; i<nFileCount; i++)
		{
			int nTemp = 0;

			for (int k=0; k<nFileCount - i; k++)
			{
				if (nIndex == 0)		//文件名
				{
					if (StrCmp(vecFileHandlesFiles[k]->GetFileName().c_str(), vecFileHandlesFiles[nTemp]->GetFileName().c_str()) > 0)
					{
						nTemp = k;
					}
				}
				else if (nIndex == 1)		//文件大小
				{
					if (vecFileHandlesFiles[k]->GetFileSize() > vecFileHandlesFiles[nTemp]->GetFileSize())
					{
						nTemp = k;
					}
				}
				else if (nIndex == 2)		//修改时间
				{
					if (vecFileHandlesFiles[k]->GetModifyTime() > vecFileHandlesFiles[nTemp]->GetModifyTime())
					{
						nTemp = k;
					}
				}
			}

			CFileHandle *pFileTemp = vecFileHandlesFiles[nFileCount - i - 1];
			vecFileHandlesFiles[nFileCount - i - 1] = vecFileHandlesFiles[nTemp];
			vecFileHandlesFiles[nTemp] = pFileTemp;
		}

		EmptyFileList();
		
		if (bAscend)
		{
			for (int i=0; i<nDirCount; i++)
			{
				CreateFileListItem(vecFileHandlesDirs[i]);
			}

			for (int i=0; i<nFileCount; i++)
			{
				CreateFileListItem(vecFileHandlesFiles[i]);
			}
		}
		else
		{
			for (int i=nDirCount - 1; i > -1; i--)
			{
				CreateFileListItem(vecFileHandlesDirs[i]);
			}

			for (int i=nFileCount - 1; i > -1; i--)
			{
				CreateFileListItem(vecFileHandlesFiles[i]);
			}
		}
}

void CWndMainFrame::PopFileListMenu( const POINT & pt, CFileHandle *pFileHandle)
{
	CWndFileListMenu *pWnd = new CWndFileListMenu;	
	pWnd->Create(NULL, _T("FileListMenu"), UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW);		
	pWnd->SetMainWnd(this);
	pWnd->SetFileHandle(pFileHandle);
	pWnd->ShowWindow();

	HDC   hdc=::GetDC(NULL);   //获得屏幕设备描述表句柄   
	int   ScrWidth=GetDeviceCaps(hdc,HORZRES);   //获取屏幕水平分辨率   
	int   ScrHeight=GetDeviceCaps(hdc,VERTRES);     //获取屏幕垂直分辨率  
	::ReleaseDC(NULL,hdc);   //释放屏幕设备描述表

	SetForegroundWindow(pWnd->GetHWND());

	CDuiRect rcWnd;
	::GetWindowRect(pWnd->GetHWND(), &rcWnd);

	POINT ptNew = pt;

	if (pt.x + rcWnd.GetWidth() > ScrWidth)
	{
		ptNew.x = pt.x - rcWnd.GetWidth();			
	}

	if (pt.y + rcWnd.GetHeight() > ScrHeight)
	{
		ptNew.y = pt.y - rcWnd.GetHeight();
	}

	::SetWindowPos(pWnd->GetHWND(), HWND_TOPMOST, ptNew.x, ptNew.y, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_SHOWWINDOW );
}

void CWndMainFrame::ExecutCommand_Upload()
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可添加！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}

	TCHAR chDir[512] = {0};

	LPMALLOC ShellMalloc;
	if (SUCCEEDED(SHGetMalloc(&ShellMalloc)))
	{
		LPITEMIDLIST DesktopPidl;
		if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP  , &DesktopPidl)))
		{
			if (SUCCEEDED(SHGetPathFromIDList(DesktopPidl, chDir)))
			{
				ShellMalloc->Free(DesktopPidl);
				ShellMalloc->Release();						
			}
		}
	}	

	OPENFILENAME ofn;
	TCHAR strFile[MAX_PATH] = {0};
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	ofn.lpstrFile = strFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST  | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_NOCHANGEDIR  | OFN_ENABLEHOOK ;
	ofn.hwndOwner = m_hWnd;
	ofn.lpfnHook = OpenFileDlgProc;
	ofn.lpstrInitialDir = chDir;

	BOOL bRet = GetOpenFileName(&ofn);

	if (StrCmp(ofn.lpstrFile, _T("")) != 0)
	{
		LPSTTEMP lpTemp = new STTEMP;
		lpTemp->Path = ofn.lpstrFile;
		lpTemp->CurFileHandle = m_Current_File_Handle;
		lpTemp->nTag = 0;

		::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)lpTemp, NULL);				
	}
}

void CWndMainFrame::ExecutCommand_NewFolder()
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可新建文件夹！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}

	SYSTEMTIME sysModifyTime;
	GetLocalTime(&sysModifyTime);
	TCHAR strModifyTime[MAX_PATH] = {0};	
	StringCchPrintf(strModifyTime, _countof(strModifyTime), _T("%d/%02d/%02d %02d:%02d:%02d"), sysModifyTime.wYear, sysModifyTime.wMonth, sysModifyTime.wDay, sysModifyTime.wHour, sysModifyTime.wMinute, sysModifyTime.wSecond);

	CFileHandle *pNewFileHandle = new CFileHandle;
	pNewFileHandle->SetFileID(CreateGUID());
	pNewFileHandle->SetFileName( _T("新建文件夹"));
	pNewFileHandle->SetFileSize(0);
	pNewFileHandle->SetParentID(m_Current_File_Handle->GetFileID());
	pNewFileHandle->SetFileType(0);
	pNewFileHandle->SetLoadAllChildrenFile(TRUE);
	pNewFileHandle->SetModifyTime(SystemTimeToINT64(sysModifyTime));

	m_Current_File_Handle->AddChildrenNode(pNewFileHandle);

	CreateFileListItem(pNewFileHandle);			

	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	int count = pTreeView->GetCount();

	CTreeNodeUI *pParent = NULL;
	CTreeNodeUI *pRoot = NULL;
	for (int i=0; i<count; i++)
	{
		pRoot = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(i));
		CFileHandle *pParentFile = (CFileHandle*)(pRoot->GetTag());

		if (pParentFile == m_Current_File_Handle)
		{
			pParent = pRoot;
			break;
		}
	}

	CTreeNodeUI *pCurNode = CreateTreeNode(pNewFileHandle);
	pParent->Add(pCurNode);

	CDatabaseManager::GetSingleInstance()->InsertRecordToTableFileBaseInfo(pNewFileHandle->GetFileID().c_str(), pNewFileHandle->GetFileType(), pNewFileHandle->GetFileName().c_str(), pNewFileHandle->GetParentID().c_str(), pNewFileHandle->GetFileSize(), pNewFileHandle->GetModifyTime(), pNewFileHandle->GetState());
}

void CWndMainFrame::ExecutCommand_Refresh()
{
	EmptyFileList();
	SearchFileAddToFileList(m_Current_File_Handle);
}

void CWndMainFrame::ExecutCommand_Open( CFileHandle *pFileHandle )
{
	if (pFileHandle->GetFileType() == 0)
	{
		EmptyFileList();
		SearchFileAddToFileList(pFileHandle);
		SetCurFileHandle(pFileHandle);
	}
	else
	{
		HANDLE hThread = CreateThread(NULL, 0, _OpenFileThreadProc, pFileHandle, NULL, NULL);
		CloseHandle(hThread);		
	}
}

void CWndMainFrame::ExecutCommand_Copy( CFileHandle *pFileHandle )
{
	BOOL bCanOpt = TRUE;

	m_vecMoveOrCopyFrom.clear();

	CFileHandle *pSelFile = pFileHandle;

	if (pSelFile->GetTaskCount() != 0)
	{
		CWndMessageBox *pMsgBox = new CWndMessageBox;
		pMsgBox->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
		pMsgBox->SetTitleAndTip(_T("提示"), _T("有文件正在进行处理，不可复制或移动!"));
		pMsgBox->CenterWindow();
		pMsgBox->ShowModal();
		return;
	}

	m_vecMoveOrCopyFrom.push_back(pSelFile);

	CWndMoveOrPasteTo *pWnd = new CWndMoveOrPasteTo(this, TRUE);
	pWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
	pWnd->CenterWindow();
	int nRet = pWnd->ShowModal();
	CFileHandle *pSelFileHandle = NULL;
	if (nRet == 1)
	{
		//下面进行文件复制操作流程
		pSelFileHandle = m_pMoveOrCopyTo;	

		ResetEvent(hMoveOrCopyEvent);
		HANDLE hThread = CreateThread(NULL, 0, _CopyThreadProc, this, NULL, NULL);
		CloseHandle(hThread);

		CWndProgress *pWndProgress = new CWndProgress;
		m_WndProgress = pWndProgress;
		pWndProgress->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
		pWndProgress->SetTip(_T("正在准备..."));
		pWndProgress->CenterWindow();
		pWndProgress->ShowModal();
	}
}

void CWndMainFrame::ExecutCommand_Move( CFileHandle *pFileHandle )
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可移动！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}

	BOOL bCanOpt = TRUE;


	m_vecMoveOrCopyFrom.clear();


			CFileHandle *pSelFile = pFileHandle;
			if (pSelFile->GetTaskCount() != 0)
			{
				CWndMessageBox *pMsgBox = new CWndMessageBox;
				pMsgBox->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
				pMsgBox->SetTitleAndTip(_T("提示"), _T("有文件正在进行处理，不可复制或移动!"));
				pMsgBox->CenterWindow();
				pMsgBox->ShowModal();
				return;
			}

			m_vecMoveOrCopyFrom.push_back(pSelFile);

	CWndMoveOrPasteTo *pWnd = new CWndMoveOrPasteTo(this, FALSE);
	pWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
	pWnd->CenterWindow();
	int nRet = pWnd->ShowModal();
	CFileHandle *pSelFileHandle = NULL;
	if (nRet == 1)
	{
		pSelFileHandle = m_pMoveOrCopyTo;	

		//下面进行文件移动操作流程
		ResetEvent(hMoveOrCopyEvent);
		HANDLE hThread = CreateThread(NULL, 0, _MoveThreadProc, this, NULL, NULL);
		CloseHandle(hThread);

		CWndProgress *pWndProgress = new CWndProgress;
		m_WndProgress = pWndProgress;
		pWndProgress->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
		pWndProgress->SetTip(_T("正在准备..."));
		pWndProgress->CenterWindow();
		pWndProgress->ShowModal();
	}
}

void CWndMainFrame::ExecutCommand_Export( CFileHandle *pFileHandle )
{
	TCHAR szPathName[MAX_PATH] = {0};
	BROWSEINFO bInfo={0};
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("请选择存放路径");
	bInfo.ulFlags      =  BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE ; 
	bInfo.lpfn		 = NULL;

	LPITEMIDLIST lpDlist; 
	lpDlist   =   SHBrowseForFolder(&bInfo); 
	if (lpDlist != NULL)
	{
		SHGetPathFromIDList(lpDlist, szPathName);	

		LPSTTEMP lpTemp = new STTEMP;
		lpTemp->Path = szPathName;
		lpTemp->CurFileHandle =pFileHandle;
		lpTemp->nTag = 1;

		::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)lpTemp, NULL);				
	}
}

void CWndMainFrame::ExecutCommand_Rename( CFileHandle *pFileHandle )
{
	CListContainerElementOfFileList *pEle = NULL;
	CListContainerElementOfFileList *pTager = NULL;
	
	int nCount = m_pFileListUI->GetCount();
	for (int i=0; i<nCount; i++)
	{
		pEle = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));
		if (pEle->GetFileHandle() == pFileHandle)
		{
			pTager = pEle;
			break;
		}
	}

	if (pTager != NULL)
	{
		RenameFile(pEle);
	}
}

void CWndMainFrame::ExecutCommand_Delete( CFileHandle *pFileHandle )
{
	if (m_Current_File_Handle == NULL)
	{
		CWndMessageBox *pWnd = new CWndMessageBox;
		pWnd->Create(m_hWnd, _T("提示"), UI_WNDSTYLE_FRAME, 0);
		pWnd->SetTitleAndTip(_T("提示"), _T("不好意思，查询结果中不可删除！"));
		pWnd->CenterWindow();
		pWnd->ShowModal();
		return;
	}

	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));	

	vector<LPSTTEMP> vecTemp;
	vector<CListContainerElementOfFileList*> vecListEle;
	vector<CTreeNodeUI*> vecTreeNodes;

	int count = m_pFileListUI->GetCount();
	for (int i=0; i<count; i++)
	{
		CListContainerElementOfFileList *pItem = static_cast<CListContainerElementOfFileList*>(m_pFileListUI->GetItemAt(i));		

		if (pItem->GetFileHandle() == pFileHandle)
		{
			LPSTTEMP lpTemp = new STTEMP;
			lpTemp->CurFileHandle = pItem->GetFileHandle();
			lpTemp->nTag = 2;

			vecTemp.push_back(lpTemp);
			vecListEle.push_back(pItem);	

			int nodescount = pTreeView->GetCount();

			for (int k = 0; k<nodescount; k++)
			{
				CTreeNodeUI *pNode = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(k));

				if (pNode != NULL)
				{
					CFileHandle *pTempFile = (CFileHandle*)(pNode->GetTag());

					if (pTempFile == pItem->GetFileHandle())
					{								
						vecTreeNodes.push_back(pNode);
					}
				}
			}

			break;
		}
	}

	int vecEleCount = vecListEle.size();
	for (int  i = 0; i<vecEleCount; i++)
	{	
		m_pFileListUI->Remove(vecListEle[i]);
	}

	int vecTreeCount = vecTreeNodes.size();
	for (int i=0; i<vecTreeCount; i++)
	{
		pTreeView->Remove(vecTreeNodes[i]);
	}

	int VecTempCount = vecTemp.size();
	for (int k=0; k<VecTempCount; k++)
	{
		::PostThreadMessage(CFileHandleManager::GetSingleInstance()->GetThreadID(PRE_PROC_THREAD_ID), WM_FILE_PROCESS, (WPARAM)vecTemp[k], NULL);
	}
}

void CWndMainFrame::ExecutCommand_Attribute( CFileHandle *pFileHandle )
{
	if (pFileHandle == NULL)
	{
		MessageBox(NULL, m_Current_File_Handle->GetFileName().c_str(), _T("属性"), MB_OK);
	}
	else
	{
		MessageBox(NULL, pFileHandle->GetFileName().c_str(), _T("属性"), MB_OK);
	}	
}

void CWndMainFrame::InitFilePath()
{	
	m_pHorFilePath->SetChildPadding(1);
	ParseCurFileHandlePaths(CFileHandleManager::GetSingleInstance()->GetFileHandleTree());
}

void CWndMainFrame::AddFilePathMain(  )
{
	COptionPath *pPathUI = new COptionPath;
	pPathUI->SetFileHandle(CFileHandleManager::GetSingleInstance()->GetFileHandleTree());
	pPathUI->SetFixedWidth(22);
	pPathUI->SetIsPathName(true);	
	pPathUI->SetGroup(_T("file_path_UIs"));
	pPathUI->SetAttribute(_T("endellipsis"), _T("true"));
	pPathUI->SetAttribute(_T("normalimage"), _T("file=\'fold.png\'  dest=\'1,3,21,23\'"));
	pPathUI->SetAttribute(_T("selectedimage"), _T("file=\'fold.png\' dest=\'1,3,21,23\'"));
	m_pHorFilePath->Add(pPathUI);
	m_listFilePathUIs.push_back(pPathUI);
}

void CWndMainFrame::AddFilePathPath( CFileHandle *pFileHandle )
{
	COptionPath *pPathUI = new COptionPath;
	pPathUI->SetText(pFileHandle->GetFileName().c_str());

	RECT rect = {0};
	rect = CalTextRect(pFileHandle->GetFileName().c_str(), m_PaintManager.GetFont(pPathUI->GetFont()));
	
	pPathUI->SetFixedWidth(rect.right - rect.left + 4);	

	pPathUI->SetFileHandle(pFileHandle);	
	pPathUI->SetIsPathName(true);
	pPathUI->SetHotTextColor(0xFFFF0000);
	pPathUI->SetGroup(_T("file_path_UIs"));
	pPathUI->SetAttribute(_T("endellipsis"), _T("true"));
	
	m_pHorFilePath->Add(pPathUI);
	m_listFilePathUIs.push_back(pPathUI);
}

void CWndMainFrame::AddFilePathPullDownMenu(CFileHandle *pFileHandle)
{
	COptionPath *pPathUI = new COptionPath;
	pPathUI->SetFileHandle(pFileHandle);
	pPathUI->SetFixedWidth(20);
	pPathUI->SetIsPathName(false);
	pPathUI->SetGroup(_T("file_path_UIs"));
	pPathUI->SetAttribute(_T("endellipsis"), _T("true"));
	pPathUI->SetAttribute(_T("normalimage"), _T("file=\'NextBtn.png\' source=\'0,0,16,16\' dest=\'2,5,20,21\'"));
	pPathUI->SetAttribute(_T("selectedimage"), _T("file=\'NextBtn.png\' source=\'16,0,32,16\' dest=\'2,5,20,21\'"));
	m_pHorFilePath->Add(pPathUI);
	m_listFilePathUIs.push_back(pPathUI);
}

void CWndMainFrame::EmptyFilePathUIs()
{
	m_pHorFilePath->RemoveAll();
	m_listFilePathUIs.clear();
}

void CWndMainFrame::ParseCurFileHandlePaths( CFileHandle *pFileHandle )
{
	EmptyFilePathUIs();

	list<CFileHandle*> ListTemp;

	CFileHandle *pTempFileHandle = pFileHandle;

	while(pTempFileHandle != NULL)
	{
		ListTemp.push_front(pTempFileHandle);
		pTempFileHandle = static_cast<CFileHandle*>(pTempFileHandle->GetParentNode());
	}

	list<CFileHandle*>::iterator iter = ListTemp.begin();
	for (iter; iter != ListTemp.end(); iter++)
	{
		pTempFileHandle = *iter;
		if (StrCmp(pTempFileHandle->GetFileID().c_str(), _T("0")) == 0)
		{
			AddFilePathMain();			
		}

		AddFilePathPath(pTempFileHandle);
		AddFilePathPullDownMenu(pTempFileHandle);
	}
}

DWORD WINAPI _MoveThreadProc( LPVOID lpParam )
{
	WaitForSingleObject(hMoveOrCopyEvent, INFINITE);

	CWndMainFrame *pWnd = (CWndMainFrame*)lpParam;
	pWnd->MoveFileTask( );

	return 0;
}

DWORD WINAPI _CopyThreadProc( LPVOID lpParam )
{
	WaitForSingleObject(hMoveOrCopyEvent, INFINITE);

	CWndMainFrame *pWnd = (CWndMainFrame*)lpParam;
	pWnd->CopyFileTask( );

	return 0;
}

DWORD WINAPI _SearchThreadProc( LPVOID lpParam )
{
	WaitForSingleObject(hMoveOrCopyEvent, INFINITE);
	CWndMainFrame *pWnd = (CWndMainFrame*)lpParam;

	pWnd->SearchFile();

	return 0;
}

DWORD WINAPI _OpenFileThreadProc( LPVOID lpParam )
{
	CFileHandle *pFileHandle = (CFileHandle *)lpParam;

	string_t strPath = CMainFrameWork::GetFilesAndChipsDirectory();
	strPath += _T('\\');
	strPath += _T("temp");
	strPath += _T("\\");
	strPath += pFileHandle->GetFileName();
	TCHAR szSavePath[2 * MAX_PATH] = {0};
	BOOL bRet = PathYetAnotherMakeUniqueName(szSavePath, strPath.c_str(), NULL, pFileHandle->GetFileName().c_str());	

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

	CFileHandleManager::GetSingleInstance()->CountFileTreeNodeProgressInfo(nTaskID, pFileHandle);

	_info.IsNeedHandle = FALSE;
	CFileHandle *pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
	while(pParentFileHandle != NULL)
	{
		pParentFileHandle->AddTask(_info);
		pParentFileHandle->SetOneTaskChildFileCount(nTaskID, pFileHandle->GetOneTaskChildFileCount(nTaskID));
		pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
	}

	SendMessage(CMainFrameWork::SingleInstance()->GetWndMainFranme()->GetHWND(), WM_UPDATE_FILE_LIST_UI,(WPARAM)pFileHandle, (LPARAM)1);

	CFileHandleManager::GetSingleInstance()->ParseDownloadFileTree(nTaskID, pFileHandle);			

	pParentFileHandle = static_cast<CFileHandle*>(pFileHandle->GetParentNode());
	while(pParentFileHandle != NULL)
	{
		pParentFileHandle->DeleteTask(nTaskID);
		pParentFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetParentNode());
	}

	HINSTANCE hInstance = ShellExecute(NULL, _T("open"), szSavePath, NULL, NULL, SW_SHOWDEFAULT);	
	
	Sleep(10000);
	while(!DeleteFile(szSavePath))
	{
		Sleep(10000);
	}

	return 0;
}
