#pragma once
class CTreeNode
{
public:
	CTreeNode(void);
	virtual ~CTreeNode(void);
	
	void AddChildrenNode(CTreeNode *pNode);	

	CTreeNode *GetParentNode();

	int GetChildrenCount();
	CTreeNode* GetChild(int index);

	void RemoveFromParent();

	void RemoveChildNode(CTreeNode *pChild);

private:
	void SetParentNode(CTreeNode *pNode);

protected:
	CTreeNode *m_ParentNode;
	vector<CTreeNode*> m_vecChildrenNodes;
};

