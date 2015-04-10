#include "StdAfx.h"
#include "WndMoveOrPasteTo.h"
#include "FileHandleManager.h"
#include "function.h"
#include "WndMessageBox.h"
#include "WndMainFrame.h"

class CWndMainFrame;

CWndMoveOrPasteTo::CWndMoveOrPasteTo(CWndMainFrame *pWnd, BOOL b):m_pSelFileHandle(NULL),m_WndMain(pWnd),m_IsShowDestFileHandle(b)
{
}


CWndMoveOrPasteTo::~CWndMoveOrPasteTo(void)
{
}

void CWndMoveOrPasteTo::Notify( TNotifyUI &msg )
{
		__super::Notify(msg);

		if (msg.sType == DUI_MSGTYPE_CLICK)
		{
			if (msg.pSender->GetName() == _T("btn_Cancel"))
			{
				Close(0);
			}
			else if (msg.pSender->GetName() == _T("btn_ok"))
			{
				CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
				int nIndex = pTreeView->GetCurSel();

				CTreeNodeUI *pCurItem = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(nIndex));
				if (pCurItem == NULL)
				{
					CWndMessageBox *pMsgBox = new CWndMessageBox;
					pMsgBox->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, NULL);
					pMsgBox->SetTitleAndTip(_T("提示"), _T("请您选择一个目录!"));
					pMsgBox->CenterWindow();
					pMsgBox->ShowModal();
					return;
				}

				CFileHandle *pFileHandle = (CFileHandle*)(pCurItem->GetTag());

				m_WndMain->SetMoveOrCopyTo(pFileHandle);

				Close(1);
			}
		}
}

LRESULT CWndMoveOrPasteTo::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
		__super::OnCreate(uMsg, wParam, lParam, bHandled);

		CWndShadow::Initialize(m_PaintManager.GetInstance() );
		m_WndShadow.Create(m_hWnd);
		m_WndShadow.SetSize(4);
		m_WndShadow.SetPosition(0, 0);

		HBITMAP hBmp;
		int nWidth = 0;
		int nHeight =0;
		string_t strIconName = _T("dir_icon_small");

		if (m_PaintManager.GetImage(strIconName.c_str()) == NULL)
		{
			hBmp = GetIconByFileType(NULL, true);

			BITMAP bmp;
			GetObject(hBmp, sizeof(BITMAP), &bmp);
			nWidth = bmp.bmWidth;
			nHeight = bmp.bmHeight;

			m_PaintManager.AddImage(strIconName.c_str(), hBmp, nWidth, nHeight, false);
		}	

		Init();

		return 0;
}

void CWndMoveOrPasteTo::Init()
{
	CTreeViewUI *pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("tree_file")));
	CTreeNodeUI *pRoot = static_cast<CTreeNodeUI*>(pTreeView->GetItemAt(0));

	CFileHandle *pFileRoot = CFileHandleManager::GetSingleInstance()->GetFileHandleTree();
	pRoot->SetAttribute(_T("checkboxattr"), _T("width=\"20\" height=\"20\" enabled=\"false\" bkimage=\"file='dir_icon_small' dest='4,2,20,18'\""));
	pRoot->SetTag((UINT_PTR)pFileRoot);

	EnumFileHandleTreeAddToTree(pRoot);

	pTreeView->ExpandItem(0,false);
}

void CWndMoveOrPasteTo::EnumFileHandleTreeAddToTree( CTreeNodeUI *pParent )
{
	CFileHandle *pParentFileHandle = (CFileHandle*)(pParent->GetTag());

	int count = pParentFileHandle->GetChildrenCount();

	for (int i=0; i<count; i++)
	{
		CFileHandle *pFileHandle = static_cast<CFileHandle*>(pParentFileHandle->GetChild(i));

		if (!m_IsShowDestFileHandle)
		{
			vector<CFileHandle*> * vecFrom = m_WndMain->GetMoveOrCopyFrom();
			BOOL IsCreate = TRUE;
			int FromCount = vecFrom->size();
			for (int k=0; k<FromCount; k++)
			{
				if (StrCmp(pFileHandle->GetFileID().c_str(), vecFrom->at(k)->GetFileID().c_str()) == 0)
				{
					IsCreate = FALSE;
					break;
				}
			}

			if (!IsCreate)
			{
				continue;
			}
		}

		if (pFileHandle->GetFileType() != 0)
		{
			continue;
		}

		CTreeNodeUI *pNode = CreateTreeNode(pFileHandle);

		pParent->AddChildNode(pNode);		
		pParent->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"9\" normalimage=\"file=\'open.png\'\" hotimage=\"file=\'openHover.png\'\" selectedimage=\"file=\'close.png\'\" selectedhotimage=\"file=\'closeHover.png\'\""));
		pParent->GetFolderButton()->SetVisible(true);

		EnumFileHandleTreeAddToTree(pNode);
	}

	pParent->Expand(false);
}

CTreeNodeUI * CWndMoveOrPasteTo::CreateTreeNode( CFileHandle *pFileHandle )
{
	CTreeNodeUI *pRetNode = new CTreeNodeUI;
	pRetNode->SetItemText(pFileHandle->GetFileName().c_str());
	pRetNode->SetAttribute(_T("endellipsis"), _T("true"));
	pRetNode->GetTreeNodeHoriznotal()->SetToolTip(pFileHandle->GetFileName().c_str());
	pRetNode->GetItemButton()->SetFont(6);
	pRetNode->SetFixedHeight(20);

	pRetNode->SetTag((UINT_PTR)pFileHandle);	

	pRetNode->SetAttribute(_T("itemattr"), _T("textpadding=\"2,0,0,0\" textcolor=\"#FF000000\""));
	pRetNode->SetAttribute(_T("checkboxattr"), _T("width=\"20\" height=\"20\" enabled=\"false\" bkimage=\"file='dir_icon_small' dest='4,2,20,18'\""));
	pRetNode->SetAttribute(_T("folderattr"), _T("padding=\"0,5,0,5\" width=\"7\" height=\"1\""));

	pRetNode->GetFolderButton()->SetVisible(false);

	return pRetNode;
}

CFileHandle * CWndMoveOrPasteTo::GetSelFileHandle()
{
	return m_pSelFileHandle;
}

