// STreeT.hpp: interface for the CSTree class.
//	��ģ�壺�ṩ��һ�����������ʵ�����ṹ
//	���в�������MFC��CTreeCtrl��ֻ��û����ʾ����
//	�ƽ���(haungjianxiong@sina.com)
//	version: 1.0��2003-10-24	ʵ�ֻ�������
//			 2.0  2004-12-29	�������������ӿڣ��޸��ڴ��ͷŲ��ֿ��ܴ��ڵ�bug
//			 2.1  2006-10-17	Ϊ�ڵ�����һ��hChildLast����,�Լӿ������ݲ���ʱ���ٶ�
//			 2.2  2008-10-16    �޸�һ�������ӿڵ�����
//			 2.3  2011-10-17	�������ͷŵĽӿڴӻص������ĳ��麯��
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_)
#define AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DUIASSERT
#define DUIASSERT(x)
#endif

typedef ULONG_PTR HSTREEITEM;

#define STVI_ROOT	((HSTREEITEM)0xFFFF0000)//=TVI_ROOT
#define STVI_FIRST	((HSTREEITEM)0xFFFF0001)//=TVI_FIRST
#define STVI_LAST	((HSTREEITEM)0xFFFF0002)//=TVI_LAST

#define STVN_ROOT	((HSTREENODE)(ULONG_PTR)0xFFFF0000)//=STVN_ROOT
#define STVN_FIRST	((HSTREENODE)(ULONG_PTR)0xFFFF0001)//=STVN_FIRST
#define STVN_LAST	((HSTREENODE)(ULONG_PTR)0xFFFF0002)//=STVN_LAST

#define STVL_ROOT	((PSTREELINK)0xFFFF0000)

template<class T>
class DUI_EXP CSTree  
{
	typedef struct _STREENODE{
		struct _STREENODE *	hParent;
		struct _STREENODE *	hChildFirst;
		struct _STREENODE * hChildLast;
		struct _STREENODE *	hPrevSibling;
		struct _STREENODE *	hNextSibling;
		T					data;
	}STREENODE,*HSTREENODE;//�����

	//����������
	typedef struct _STREELINK{
		HSTREENODE	hParent;
		HSTREENODE	hChildFirst;
		HSTREENODE  hChildLast;
		HSTREENODE	hPrevSibling;
		HSTREENODE	hNextSibling;
	}STREELINK,*PSTREELINK;

	//**********************************************************
	//	����һ�������Ļص�����ԭ�ͣ�
	//	T *:��ǰ�������Ľ�������
	//	LPARAM:�ص�ʱʹ�õĲ���
	//	return:FALSE��������TRUE���жϱ���
	//**********************************************************
	typedef BOOL (*CBTRAVERSING)(T*,LPARAM);

public:
	CSTree()
	{
		m_hRootFirst=NULL;
		m_hRootLast=NULL;
	}
	virtual ~CSTree(){DeleteAllItems();}

	void DeleteAllItems(){
		if(m_hRootFirst)
		{
			FreeNode(STVN_ROOT);
			m_hRootFirst=NULL;
			m_hRootLast=NULL;
		}
	}
	
	//*********************************************
	//	��ȡ��һ���ֵ�
	//*********************************************
	static HSTREEITEM GetNextSiblingItem(HSTREEITEM hItem){
		PSTREELINK pLink= (PSTREELINK)hItem;
		DUIASSERT(pLink&&pLink!=STVL_ROOT);
		return (HSTREEITEM)pLink->hNextSibling;
	}

	//***********************************************
	//	��ȡ��һ���ֵ�
	//*********************************************
	static HSTREEITEM GetPrevSiblingItem(HSTREEITEM hItem)
	{
		PSTREELINK pLink= (PSTREELINK)hItem;
		DUIASSERT(pLink&&pLink!=STVL_ROOT);
		return (HSTREEITEM)pLink->hPrevSibling;
	}
	
	//*********************************************
	//����ȡ�����
	//*********************************************
	static HSTREEITEM GetParentItem(HSTREEITEM hItem)
	{
		PSTREELINK pLink= (PSTREELINK)hItem;
		DUIASSERT(pLink&&pLink!=STVL_ROOT);
		return (HSTREEITEM)pLink->hParent;
	}
	

	//*********************************************
	//����ȡ������
	//*********************************************
	static int GetItemLevel(HSTREEITEM hItem)
	{
		int nRet=-1;
		if(hItem==STVI_ROOT) hItem=NULL;
		while(hItem)
		{
			nRet++;
			hItem=GetParentItem(hItem);
		}
		return nRet;
	}

	//************************************************
	//	��ȡ��һ���ӽ��
	//************************************************
	HSTREEITEM GetChildItem(HSTREEITEM hItem,BOOL bFirst=TRUE)
	{
		HSTREENODE hsNode= (HSTREENODE)hItem;
		DUIASSERT(hsNode);
		if(hsNode==STVN_ROOT)
		{
			if(bFirst)
				return (HSTREEITEM)m_hRootFirst;
			else
				return (HSTREEITEM)m_hRootLast;
		}
		else
		{
			if(bFirst)
				return (HSTREEITEM)hsNode->hChildFirst;
			else
				return (HSTREEITEM)hsNode->hChildLast;
		}
	}
	
	//************************************************
	//	��ȡ�ӽ������
	//************************************************
	int GetChildrenCount(HSTREEITEM hItem)
	{
		int nRet=0;
		HSTREEITEM hChild=GetChildItem(hItem);
		while(hChild)
		{
			nRet++;
			hChild=GetNextSiblingItem(hChild);
		}
		return nRet;
	}

	//************************************************
	//	ɾ��һ���ڵ㣬���Ա�����������
	//***********************************************
	virtual void DeleteItem(HSTREEITEM hItem)
	{
		HSTREENODE hsNode= (HSTREENODE)hItem;
		DUIASSERT(hsNode);
		if(hsNode==STVN_ROOT)
		{
			FreeNode(STVN_ROOT);
			m_hRootFirst=NULL;
			m_hRootLast=NULL;
			return;
		}
		STREENODE nodeCopy=*hsNode;
		BOOL bRootFirst=hsNode==m_hRootFirst;
		BOOL bRootLast=hsNode==m_hRootLast;
		FreeNode(hsNode);

		if(nodeCopy.hPrevSibling)//has prevsibling
			nodeCopy.hPrevSibling->hNextSibling=nodeCopy.hNextSibling;
		else if(nodeCopy.hParent)//parent's first child
			nodeCopy.hParent->hChildFirst=nodeCopy.hNextSibling;
		if(nodeCopy.hNextSibling)// update next sibling's previous sibling
			nodeCopy.hNextSibling->hPrevSibling=nodeCopy.hPrevSibling;
		else if(nodeCopy.hParent)//parent's last child
			nodeCopy.hParent->hChildLast=nodeCopy.hPrevSibling;
		//update root item
		if(bRootFirst)	m_hRootFirst=nodeCopy.hNextSibling;
		if(bRootLast)   m_hRootLast=nodeCopy.hPrevSibling;
	}


	//******************************************
	//	ɾ��һ������֦������ý��ĸ����û�������ӽڵ���һ��ɾ��
	//******************************************
	BOOL DeleteItemEx(HSTREEITEM hItem)
	{
		if(GetChildItem(hItem)) return FALSE;
		while(hItem && !GetChildItem(hItem))
		{
			HSTREEITEM hParent=GetParentItem(hItem);
			DeleteItem(hItem);
			hItem=hParent;
		}
		return TRUE;
	}

	//******************************************
	//	��ȡ����б��������
	//******************************************
	static T GetItem(HSTREEITEM hItem){
		DUIASSERT(hItem!=STVI_ROOT);
		HSTREENODE hsNode= (HSTREENODE)hItem;
		DUIASSERT(hsNode);
		return hsNode->data;
	}
	

	static T *GetItemPt(HSTREEITEM hItem){
		DUIASSERT(hItem!=STVI_ROOT);
		HSTREENODE hsNode= (HSTREENODE)hItem;
		DUIASSERT(hsNode);
		return &hsNode->data;
	}
	//*******************************************
	//	����һ���½��
	//	data:�������
	//	hParant:�½��ĸ����
	//	hInsertAfter:�½���ǰһ���ֵܽ��
	//	return:�½���ָ��
	//*******************************************
	HSTREEITEM InsertItem(const T &data,HSTREEITEM hParent=STVI_ROOT,HSTREEITEM hInsertAfter=STVI_LAST){
		HSTREENODE hParentNode=(HSTREENODE) hParent;
		HSTREENODE hInsertAfterNode=(HSTREENODE) hInsertAfter;
		if(hParentNode==STVN_ROOT)
			hParentNode=NULL;
		DUIASSERT(hInsertAfter);
		if(hInsertAfterNode!=STVN_FIRST && hInsertAfterNode!=STVN_LAST)
		{
			if(hInsertAfterNode->hParent!=hParentNode) return NULL;
			if(hInsertAfterNode->hNextSibling==NULL) hInsertAfterNode=STVN_LAST;
		}

		HSTREENODE hInserted=new STREENODE;
		hInserted->data=data;
		hInserted->hParent=hParentNode;
		hInserted->hChildFirst=NULL;
		hInserted->hChildLast=NULL;

		if(hInsertAfterNode==STVN_FIRST)
		{
			hInserted->hPrevSibling=NULL;
			if(hParentNode==NULL)//root
			{
				hInserted->hNextSibling=m_hRootFirst;
				if(m_hRootFirst) m_hRootFirst->hPrevSibling=hInserted;
				m_hRootFirst=hInserted;
				if(m_hRootLast==NULL) m_hRootLast=hInserted;
			}else	//has parent
			{
				hInserted->hNextSibling=hParentNode->hChildFirst;
				if(hInserted->hNextSibling)
				{
					hInserted->hNextSibling->hPrevSibling=hInserted;
					hParentNode->hChildFirst=hInserted;
				}else
				{
					hParentNode->hChildLast=hParentNode->hChildFirst=hInserted;
				}
			}
		}else if(hInsertAfterNode==STVN_LAST)
		{
			hInserted->hNextSibling=NULL;
			if(hParentNode==NULL)//root
			{
				hInserted->hPrevSibling=m_hRootLast;
				if(m_hRootLast) m_hRootLast->hNextSibling=hInserted;
				m_hRootLast=hInserted;
				if(!m_hRootFirst) m_hRootFirst=hInserted;
			}else
			{
				hInserted->hPrevSibling=hParentNode->hChildLast;
				if(hParentNode->hChildLast) 
				{
					hInserted->hPrevSibling->hNextSibling=hInserted;
					hParentNode->hChildLast=hInserted;
				}else
				{
					hParentNode->hChildLast=hParentNode->hChildFirst=hInserted;
				}				
			}
		}else
		{
			HSTREENODE hNextSibling=hInsertAfterNode->hNextSibling;
			hInserted->hPrevSibling=hInsertAfterNode;
			hInserted->hNextSibling=hNextSibling;
			hNextSibling->hPrevSibling = hInsertAfterNode->hNextSibling = hInserted;
		}
		return (HSTREEITEM)hInserted;
	}

	//********************************************************
	//	���õݹ鷽ʽ����һ�������
	//	HSTREEITEM hItem:��ʼ���
	//	CBTRAVERSING funTraversing:ִ��ʵ�ʲ����Ļص�����
	//	LPARAM lParam:�ص�ʱʹ�õĲ���
	//********************************************************
	HSTREEITEM TraversingRecursion(HSTREEITEM hItem,CBTRAVERSING funTraversing,LPARAM lParam)
	{
		DUIASSERT(hItem);
		if(hItem!=STVI_ROOT)
		{
			if(funTraversing(GetItemPt(hItem),lParam)) return hItem;
		}
		HSTREEITEM hChild=GetChildItem(hItem);
		while(hChild)
		{
			HSTREEITEM hTmp=GetChildItem(hChild);
			if(hTmp)
			{
				HSTREEITEM hRet=TraversingRecursion(hTmp,funTraversing,lParam);
				if(hRet) return hRet;
			}else
			{
				if(funTraversing(GetItemPt(hChild),lParam)) return hChild;
			}
			hChild=GetNextSiblingItem(hChild);
		}
		return NULL;
	}
	
	//********************************************************
	//	��˳��ʽ��ָ����㿪ʼ���Һ���Ľ�㣬�����Լ����ӽڵ㼰�Լ����µ��ֵܽ��
	//	HSTREEITEM hItem:��ʼ���
	//	CBTRAVERSING funTraversing:ִ��ʵ�ʲ����Ļص�����
	//	LPARAM lParam:�ص�ʱʹ�õĲ���
	//	remark:��ִ��˳�����ʱ��Ҫ���ַ�ʽ
	//********************************************************
	HSTREEITEM TraversingSequence(HSTREEITEM hItem,CBTRAVERSING funTraversing,LPARAM lParam)
	{
		if(!m_hRootFirst) return NULL;
		if(hItem!=STVI_ROOT)
		{
			if(funTraversing(GetItemPt(hItem),lParam)) return hItem;
		}
		HSTREEITEM hNext=GetNextItem(hItem);
		while(hNext)
		{
			if(funTraversing(GetItemPt(hNext),lParam)) return hNext;
			hNext=GetNextItem(hNext);
		}
		return NULL;
	}

	HSTREEITEM GetRootItem(BOOL bFirst=TRUE){
		return (HSTREEITEM)(bFirst?m_hRootFirst:m_hRootLast);
	}

	//******************************************
	//	��ȡ������������
	//******************************************
	int GetDesendants(HSTREEITEM hItem)
	{
		int nRet=0;
		HSTREEITEM hChild=GetChildItem(hItem);
		while(hChild)
		{
			nRet += 1+GetDesendants(hChild);
			hChild=GetNextSiblingItem(hChild);
		}
		return nRet;
	}

	//****************************************************
	//	��ȡ��ǰ������һ�����
	//	HSTREEITEM hItem:��ǰ���
	//	return:��ǰ������һ�����
	//	remark�������ǰ������ӽ�㣬�򷵻��Լ��ĵ�һ���ӽ�㣬
	//			������������µ��ֵܽ�㣬�򷵻��Լ������ֵܽ�㡢
	//			���������Լ��ĸ����������ֵܽ��
	//****************************************************
	HSTREEITEM GetNextItem(HSTREEITEM hItem)
	{
		if(hItem==STVI_ROOT) return (HSTREEITEM)m_hRootFirst;

		HSTREEITEM hRet=GetChildItem(hItem);
		if(hRet) return hRet;
		HSTREEITEM hParent=hItem;
		while(hParent)
		{
			hRet=GetNextSiblingItem(hParent);
			if(hRet) return hRet;
			hParent=GetParentItem(hParent);
		}
		return NULL;
	}

	//****************************************************
	//	��ȡ��ǰ������һ�����
	//	HSTREEITEM hItem:��ǰ���
	//	int &nLevel:��ǰ���(hItem)��Ŀ����(return)�Ĳ�ι�ϵ,1-���ӹ�ϵ��0���ֵܹ�ϵ��-n����->�����ֵ�
	//	return:��ǰ������һ�����
	//	remark�������ǰ������ӽ�㣬�򷵻��Լ��ĵ�һ���ӽ�㣬
	//			������������µ��ֵܽ�㣬�򷵻��Լ������ֵܽ�㡢
	//			���������Լ��ĸ����������ֵܽ��
	//****************************************************
	HSTREEITEM GetNextItem(HSTREEITEM hItem,int &nLevel)
	{
		if(hItem==STVI_ROOT)
		{
			nLevel=1;
			return (HSTREEITEM)m_hRootFirst;
		}

		HSTREEITEM hRet=GetChildItem(hItem);
		if(hRet)
		{
			nLevel=1;
			return hRet;
		}
		HSTREEITEM hParent=hItem;
		nLevel=0;
		while(hParent)
		{
			hRet=GetNextSiblingItem(hParent);
			if(hRet) return hRet;
			nLevel--;
			hParent=GetParentItem(hParent);
		}
		return NULL;
	}
private:
	//**********************************************
	//	���ú�������ķ�ʽ�ͷŽ��ռ�õĿռ䡣
	//**********************************************
	void FreeNode(HSTREENODE hsNode)
	{
		DUIASSERT(hsNode);
		HSTREENODE hSibling=(HSTREENODE)GetChildItem((HSTREEITEM)hsNode);
		while(hSibling)
		{
			HSTREENODE hNextSibling=hSibling->hNextSibling;
			FreeNode(hSibling);
			hSibling=hNextSibling;
		}
		if(hsNode!=STVN_ROOT)
		{
			OnNodeFree(hsNode->data);
			delete hsNode;
		}
	}

protected:
	//����������ʵ�����ݵ��ͷŲ���
	virtual void OnNodeFree(T & data){}

	HSTREENODE	m_hRootFirst;	//��һ�����ڵ�
	HSTREENODE  m_hRootLast;	//���һ�����ڵ�
};

#endif // !defined(AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_)
