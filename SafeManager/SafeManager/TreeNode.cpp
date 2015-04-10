#include "StdAfx.h"
#include "TreeNode.h"


CTreeNode::CTreeNode(void):m_ParentNode(NULL)
{
}

CTreeNode::~CTreeNode(void)
{
	vector<CTreeNode*>::iterator  iter= m_vecChildrenNodes.begin();
	for (iter; iter != m_vecChildrenNodes.end(); iter++)
	{
		delete *iter;
	}
}

void CTreeNode::SetParentNode( CTreeNode *pNode )
{
	m_ParentNode =  pNode;
}

void CTreeNode::AddChildrenNode( CTreeNode *pNode )
{
	m_vecChildrenNodes.push_back(pNode);
	pNode->SetParentNode(this);
}

int CTreeNode::GetChildrenCount()
{
	return m_vecChildrenNodes.size();
}

CTreeNode* CTreeNode::GetChild( int index )
{
	CTreeNode *pRet = NULL;

	if (index >= 0 && (index < m_vecChildrenNodes.size()))
	{
		pRet =  m_vecChildrenNodes[index];
	}

	return pRet;
}

CTreeNode * CTreeNode::GetParentNode()
{
	return m_ParentNode;
}

void CTreeNode::RemoveFromParent()
{
	if (m_ParentNode != NULL)
	{
		m_ParentNode->RemoveChildNode(this);
		m_ParentNode = NULL;
	}
}

void CTreeNode::RemoveChildNode( CTreeNode *pChild )
{
	vector<CTreeNode*>::iterator  iter= m_vecChildrenNodes.begin();

	for (iter; iter != m_vecChildrenNodes.end(); iter++)
	{
		if (pChild == *iter)
		{
			m_vecChildrenNodes.erase(iter);
			break;
		}		
	}
}
