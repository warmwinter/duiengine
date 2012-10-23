//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiItemBox
// Description: Items Container
//     Creator: huangjianxiong
//     Version: 2011.7.8 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "duistd.h"
#include "duiitembox.h"

namespace DuiEngine{

CDuiItemBox::CDuiItemBox():m_nItemWid(100),m_nItemHei(100),m_nSepHei(5),m_nSepWid(5)
{

}

CDuiPanel* CDuiItemBox::InsertItem(LPCWSTR pszXml,int iItem/*=-1*/,BOOL bEnsureVisible/*=FALSE*/)
{
	TiXmlDocument xmlDoc;
	CStringA strXml=CW2A(pszXml,CP_UTF8);;

	xmlDoc.Parse(strXml);
	if(xmlDoc.Error()) return NULL;


	CDuiWindow *pChild=m_pFirstChild,*pPrevChild=ICWND_FIRST;
	for(int iChild=0;iChild<iItem || iItem==-1;iChild++)
	{
		if(!pChild) break;
		pPrevChild=pChild;
		pChild=pChild->GetDuiWindow(GDUI_NEXTSIBLING);
	}

	CDuiPanel *pPanel=new CDuiPanel;
	InsertChild(pPanel, pPrevChild);

	pPanel->LoadChildren(xmlDoc.RootElement());
	pPanel->SetVisible(TRUE);
	pPanel->SetFixSize(m_nItemWid,m_nItemHei);

	UpdateScroll();
	ReLayout();

	if(bEnsureVisible) EnsureVisible(pPanel);

	NotifyInvalidate();
	return pPanel;
}

BOOL CDuiItemBox::RemoveItem(UINT iItem)
{
	if(iItem>=GetItemCount()) return FALSE;
	CDuiWindow *pChild=m_pFirstChild;
	for(UINT iChild=0;iChild<iItem ;iChild++)
	{
		pChild=pChild->GetDuiWindow(GDUI_NEXTSIBLING);
	}
	DestroyChild(pChild);

	UpdateScroll();
	ReLayout();
	NotifyInvalidate();

	return TRUE;
}

BOOL CDuiItemBox::RemoveItem(CDuiWindow * pChild)
{
	if (DestroyChild(pChild))
	{
		UpdateScroll();
		ReLayout();
		NotifyInvalidate();
		return TRUE;
	}
	return FALSE; 
}

void CDuiItemBox::BringWindowAfter(CDuiWindow * pChild, CDuiWindow * pInsertAfter)
{
	RemoveChild(pChild);
	InsertChild(pChild, pInsertAfter);
}

BOOL CDuiItemBox::SetNewPosition(CDuiWindow * pChild, DWORD nPos, BOOL bEnsureVisible)
{
	if (pChild == NULL) { return FALSE; }

	CDuiWindow * pCurChild = m_pFirstChild; 
	DWORD nCurPos = 0; 
	for (; pCurChild != NULL; pCurChild = pCurChild->GetDuiWindow(GDUI_NEXTSIBLING))
	{
		if (pCurChild == pChild) { break; }
		++nCurPos; 
	}

	if (pCurChild == NULL) { return FALSE; }

	if (nCurPos == nPos)
	{
		if (bEnsureVisible) { EnsureVisible(pChild); }

		NotifyInvalidate();
		return TRUE; 
	}

	if (nPos == 0)
	{
		BringWindowAfter(pChild, ICWND_FIRST); 
	}
	else
	{
		CDuiWindow * pNewNext = m_pFirstChild; 
		for (UINT i = 0; i < nPos && pNewNext != NULL; i++)
		{
			pNewNext = pNewNext->GetDuiWindow(GDUI_NEXTSIBLING);
		}

		BringWindowAfter(pChild, pNewNext); 
	}

	UpdateScroll();
	ReLayout();
	if (bEnsureVisible) { EnsureVisible(pChild); }
	NotifyInvalidate();
	return TRUE; 
}

int CDuiItemBox::GetItemPos(CDuiWindow * lpCurItem)
{
	if (lpCurItem == NULL) { return -1; }
	int nPos = 0; 
	for (CDuiWindow *pChild = m_pFirstChild; pChild != NULL; pChild = pChild->GetDuiWindow(GDUI_NEXTSIBLING), ++nPos)
	{
		if (pChild == lpCurItem)
		{
			return nPos; 
		}
	}
	return -1; 
}

void CDuiItemBox::RemoveAllItems()
{
	CDuiWindow::OnDestroy();
	UpdateScroll();
	NotifyInvalidate();
}

int CDuiItemBox::GetItemCount()
{
	return GetChildrenCount();
}

void CDuiItemBox::PageUp()
{
	OnScroll(TRUE,SB_PAGEUP,0);
}

void CDuiItemBox::PageDown()
{
	OnScroll(TRUE,SB_PAGEDOWN,0);
}

void CDuiItemBox::EnsureVisible(CDuiWindow *pItem)
{
	if(!HasScrollBar(TRUE)) return;
	DUIASSERT(pItem);
	CRect rcItem;
	pItem->GetRect(&rcItem);
	int yOffset=0;
	if(rcItem.bottom>m_rcWindow.bottom)
	{
		yOffset=rcItem.bottom-m_rcWindow.bottom;
	}else if(rcItem.top<m_rcWindow.top)
	{
		yOffset=rcItem.top-m_rcWindow.top;
	}
	SetScrollPos(TRUE,GetScrollPos(TRUE)+yOffset,TRUE);
}

void CDuiItemBox::UpdateScroll()
{
	CRect rcClient=m_rcWindow;

	int nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
	int nPageRows=(rcClient.Height()+m_nSepHei)/(m_nItemHei+m_nSepHei);
	if(nPageCols<1) nPageCols=1;
	int nPageItems=nPageCols*nPageRows;
	if(nPageItems<GetItemCount())
	{
		rcClient.right-=m_nSbWid;
		nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
		if(nPageCols<1) nPageCols=1;
		nPageItems=nPageCols*nPageRows;
	}


	SCROLLINFO si;
	si.fMask=SIF_RANGE|SIF_PAGE;
	si.nPage=(nPageItems<GetItemCount())?rcClient.Height():0;
	si.nMin=0;
	si.nMax=(GetItemCount()+nPageCols-1)/nPageCols*(m_nItemHei+m_nSepHei)-m_nSepHei;
	SetScrollInfo(si,TRUE);

	SetViewSize(CSize(rcClient.Width(),si.nMax));
	if(si.nPage==0) SetViewOrigin(CPoint(0,0));
	else SetViewOrigin(CPoint(0,GetScrollPos(TRUE)));
}

CRect CDuiItemBox::GetItemRect(int iItem)
{
	CRect rcClient=m_rcWindow;

	int nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
	int nPageRows=(rcClient.Height()+m_nSepHei)/(m_nItemHei+m_nSepHei);
	if(nPageCols<1) nPageCols=1;
	int nPageItems=nPageCols*nPageRows;
	if(nPageItems<GetItemCount())
	{
		rcClient.right-=m_nSbWid;
		nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
		if(nPageCols<1) nPageCols=1;
		nPageItems=nPageCols*nPageRows;
	}

	CDuiWindow *pChild=m_pFirstChild;
	int nMargin=(rcClient.Width()-((m_nItemWid+m_nSepWid)*nPageCols-m_nSepWid))/2;
	CRect rcItem(CPoint(nMargin,0),CSize(m_nItemWid,m_nItemHei));
	int iCol=iItem%nPageCols;
	int iRow=iItem/nPageCols;
	rcItem.OffsetRect(iCol*(m_nItemWid+m_nSepWid),iRow*(m_nItemHei+m_nSepHei));
	return rcItem;
}

void CDuiItemBox::OnSize(UINT nType, CSize size)
{
	if(m_rcWindow.IsRectEmpty()) return;

	m_ptOrgin=CPoint(0,0);
	m_siVer.nPos=0;
	UpdateScroll();
	ReLayout();
}

void CDuiItemBox::OnCalcChildPos(CDuiWindow *pDuiWndChild)
{//leave it unhandled
}

void CDuiItemBox::ReLayout()
{
	CRect rcItem;
	int iItem=0;
	CDuiWindow *pChild=m_pFirstChild;
	while(pChild)
	{
		rcItem=GetItemRect(iItem);
		rcItem.OffsetRect(m_rcWindow.TopLeft()-m_ptOrgin);
		pChild->Move(rcItem);
		pChild=pChild->GetDuiWindow(GDUI_NEXTSIBLING);
		iItem++;
	}
}

 BOOL CDuiItemBox::OnScroll(BOOL bVertical,UINT uCode,int nPos)
{
	if(!__super::OnScroll(bVertical,uCode,nPos)) return FALSE;

	ReLayout();

	return TRUE;
}

 int CDuiItemBox::GetScrollLineSize(BOOL bVertical)
{
	if(bVertical) return m_nItemHei+m_nSepHei;
	else return m_nItemWid+m_nSepWid;
}

 BOOL CDuiItemBox::LoadChildren(TiXmlElement* pTiXmlChildElem)
{
	if(!pTiXmlChildElem) return FALSE;
	RemoveAllItems();

	TiXmlElement *pItem=NULL;
	if(strcmp("item",pTiXmlChildElem->Value())==0) pItem=pTiXmlChildElem;
	else pItem=pTiXmlChildElem->NextSiblingElement("item");

	while(pItem)
	{
		CDuiPanel *pChild=new CDuiPanel;

		InsertChild(pChild);

		pChild->Load(pItem);
		pChild->SetVisible(TRUE);
		pChild->SetFixSize(m_nItemWid,m_nItemHei);


		pItem=pItem->NextSiblingElement("item");
	}
	return TRUE;
}

}//namespace DuiEngine