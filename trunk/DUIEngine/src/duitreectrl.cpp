//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiTreeCtrl
// Description: CDuiTreeCtrl
//     Creator: huangjianxiong
//     Version: 2011.10.14 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "duistd.h"
#include "duitreectrl.h"
#include "duiwndcmnctrl.h"

namespace DuiEngine{

#define IDC_SWITCH	65530

CDuiTreeItem::CDuiTreeItem(CDuiWindow *pFrameHost,TiXmlElement *pXml)
: CDuiItemPanel(pFrameHost,pXml)
, m_bCollapsed(FALSE)
, m_bVisible(TRUE)
, m_nLevel(0)
{
}


CDuiTreeCtrl::CDuiTreeCtrl()
: m_nItemHei(20)
, m_nIndent(10)
, m_hSelItem(NULL)
, m_hHoverItem(NULL)
, m_pCapturedFrame(NULL)
, m_crItemBg(CLR_INVALID)
, m_crItemSelBg(RGB(0,0,128))
, m_pItemSkin(NULL)
, m_nVisibleItems(0)
, m_pTiXmlSwitch(NULL)
{

}

CDuiTreeCtrl::~CDuiTreeCtrl()
{
	if(m_pTiXmlSwitch) delete m_pTiXmlSwitch;
	m_pTiXmlSwitch=NULL;
}

HSTREEITEM CDuiTreeCtrl::InsertItem(TiXmlElement *pTiXmlItem,DWORD dwData,HSTREEITEM hParent/*=STVI_ROOT*/, HSTREEITEM hInsertAfter/*=STVI_LAST*/,BOOL bEnsureVisible/*=FALSE*/)
{
	CDuiTreeItem *pItemObj=new CDuiTreeItem(this,pTiXmlItem);
	pItemObj->m_nLevel=GetItemLevel(hParent)+1;
	pItemObj->m_bCollapsed=FALSE;
	if(hParent!=STVI_ROOT)
	{
		CDuiTreeItem * pParentItem= GetItem(hParent);
		if(pParentItem->m_bCollapsed || !pParentItem->m_bVisible) pItemObj->m_bVisible=FALSE;
		if(!GetChildItem(hParent) && m_pTiXmlSwitch)
		{
			CDuiToggle *pToggle=new CDuiToggle;
			pToggle->SetContainer(pParentItem->GetContainer());
			pToggle->Load(m_pTiXmlSwitch);
			pParentItem->InsertChild(pToggle);
			pToggle->SetToggle(FALSE,FALSE);
			pToggle->SetCmdID(IDC_SWITCH);
			pParentItem->DuiSendMessage(WM_CALCWNDPOS,0,(LPARAM)pToggle);
		}
	}
	pItemObj->SetItemData(dwData);
	pItemObj->SetColor(m_crItemBg,m_crItemSelBg);
	pItemObj->Move(CRect(0,0,m_rcClient.Width()-pItemObj->m_nLevel*m_nIndent,m_nItemHei));
	pItemObj->SetSkin(m_pItemSkin);

	HSTREEITEM hRet= CSTree<CDuiTreeItem*>::InsertItem(pItemObj,hParent,hInsertAfter);
	pItemObj->m_hItem=hRet;

	if(pItemObj->m_bVisible)
	{
		m_nVisibleItems++;

		CSize szView(m_rcWindow.Width(),m_nVisibleItems*m_nItemHei);
		if(szView.cy>m_rcWindow.Height()) szView.cx-=m_nSbWid;
		SetViewSize(szView);
		NotifyInvalidate();
	}

	if(bEnsureVisible) EnsureVisible(hRet);
	return hRet;
}

CDuiPanel* CDuiTreeCtrl::InsertItem(LPCWSTR pszXml,DWORD dwData,HSTREEITEM hParent/*=STVI_ROOT*/, HSTREEITEM hInsertAfter/*=STVI_LAST*/,BOOL bEnsureVisible/*=FALSE*/)
{
	TiXmlDocument xmlDoc;
	CStringA strXml=CW2A(pszXml,CP_UTF8);;

	xmlDoc.Parse(strXml);
	if(xmlDoc.Error()) return NULL;

	HSTREEITEM hItem=InsertItem(xmlDoc.RootElement(),dwData,hParent,hInsertAfter,bEnsureVisible);
	return GetItem(hItem);
}

BOOL CDuiTreeCtrl::RemoveItem(HSTREEITEM hItem)
{
	if(!hItem) return FALSE;
	HSTREEITEM hParent=GetParentItem(hItem);

	CDuiTreeItem * pItem= CSTree<CDuiTreeItem*>::GetItem(hItem);

	BOOL bVisible=pItem->m_bVisible;
	if(bVisible)
	{
		if(GetChildItem(hItem) && pItem->m_bCollapsed==FALSE)
		{
			SetChildrenVisible(hItem,FALSE);
		}
	}

	if(IsAncestor(hItem,m_hHoverItem)) m_hHoverItem=NULL;
	if(IsAncestor(hItem,m_hSelItem)) m_hSelItem=NULL;

	DeleteItem(hItem);

	if(hParent && !GetChildItem(hParent) && m_pTiXmlSwitch)
	{//ȥ�����ڵ��չ����־
		CDuiTreeItem *pParent=GetItem(hParent);
		pParent->m_bCollapsed=FALSE;
		CDuiWindow *pToggle=pParent->GetChild(IDC_SWITCH);
		DUIASSERT(pToggle);
		pParent->DestroyChild(pToggle);
		if(pParent->m_bVisible) NotifyInvalidateRect(pParent->GetItemRect());
	}


	if(bVisible)
	{
		m_nVisibleItems--;

		CSize szView(m_rcWindow.Width(),m_nVisibleItems*m_nItemHei);
		if(szView.cy>m_rcWindow.Height()) szView.cx-=m_nSbWid;
		SetViewSize(szView);
		NotifyInvalidate();
	}
	return TRUE;
}

void CDuiTreeCtrl::RemoveAllItems()
{
	DeleteAllItems();
	m_nVisibleItems=0;
	m_hSelItem=0;
	m_hHoverItem=0;
	m_pCapturedFrame=NULL;
	ReleaseDuiCapture();
	SetViewSize(CSize(0,0));
}

HSTREEITEM CDuiTreeCtrl::GetRootItem()
{
	return GetChildItem(STVI_ROOT);
}

HSTREEITEM CDuiTreeCtrl::GetNextSiblingItem(HSTREEITEM hItem)
{
	return CSTree<CDuiTreeItem*>::GetNextSiblingItem(hItem);
}

HSTREEITEM CDuiTreeCtrl::GetPrevSiblingItem(HSTREEITEM hItem)
{
	return CSTree<CDuiTreeItem*>::GetPrevSiblingItem(hItem);
}

HSTREEITEM CDuiTreeCtrl::GetChildItem(HSTREEITEM hItem,BOOL bFirst/* =TRUE*/)
{
	return CSTree<CDuiTreeItem*>::GetChildItem(hItem,bFirst);
}

HSTREEITEM CDuiTreeCtrl::GetParentItem(HSTREEITEM hItem)
{
	return CSTree<CDuiTreeItem*>::GetParentItem(hItem);
}


void CDuiTreeCtrl::PageUp()
{
	OnScroll(TRUE,SB_PAGEUP,0);
}

void CDuiTreeCtrl::PageDown()
{
	OnScroll(TRUE,SB_PAGEDOWN,0);
}

void CDuiTreeCtrl::OnDestroy()
{
	DeleteAllItems();
	__super::OnDestroy();
}

BOOL CDuiTreeCtrl::Expand(HSTREEITEM hItem , UINT nCode)
{
	BOOL bRet=FALSE;
	if(CSTree<CDuiTreeItem*>::GetChildItem(hItem))
	{
		CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);
		if(nCode==TVE_COLLAPSE && !pItem->m_bCollapsed)
		{
			pItem->m_bCollapsed=TRUE;
			SetChildrenVisible(hItem,FALSE);
			bRet=TRUE;
		}
		if(nCode==TVE_EXPAND && pItem->m_bCollapsed)
		{
			pItem->m_bCollapsed=FALSE;
			SetChildrenVisible(hItem,TRUE);
			bRet=TRUE;
		}
		if(nCode==TVE_TOGGLE)
		{
			pItem->m_bCollapsed=!pItem->m_bCollapsed;
			SetChildrenVisible(hItem,!pItem->m_bCollapsed);
			bRet=TRUE;
		}
		if(bRet)
		{
			if(m_pTiXmlSwitch)
			{
				CDuiToggle *pSwitch=(CDuiToggle*)pItem->GetChild(IDC_SWITCH);
				DUIASSERT(pSwitch);
				pSwitch->SetToggle(pItem->m_bCollapsed,FALSE);
			}
			CSize szView(m_rcWindow.Width(),m_nVisibleItems*m_nItemHei);
			if(szView.cy>m_rcWindow.Height()) szView.cx-=m_nSbWid;
			SetViewSize(szView);
			NotifyInvalidate();
		}
	}
	return bRet;
}

BOOL CDuiTreeCtrl::EnsureVisible(HSTREEITEM hItem)
{
	CDuiTreeItem *pItem=GetItem(hItem);
	if(!pItem->m_bVisible)
	{
		HSTREEITEM hParent=GetParentItem(hItem);
		while(hParent)
		{
			CDuiTreeItem *pParent=GetItem(hParent);
			if(pParent->m_bCollapsed) Expand(hParent,TVE_EXPAND);
			hParent=GetParentItem(hParent);
		}
	}
	int iVisible= GetItemShowIndex(hItem);
	int yOffset=iVisible*m_nItemHei;
	if(yOffset+m_nItemHei>m_ptOrgin.y+m_rcClient.Height())
	{
		SetScrollPos(TRUE,yOffset+m_nItemHei-m_rcClient.Height(),TRUE);
	}else if(yOffset<m_ptOrgin.y)
	{
		SetScrollPos(TRUE,yOffset,TRUE);	
	}
	return TRUE;
}

//�Զ��޸�pt��λ��Ϊ��Ե�ǰ���ƫ����
HSTREEITEM CDuiTreeCtrl::HitTest(CPoint &pt)
{
	CRect rcClient;
	GetClient(&rcClient);
	CPoint pt2=pt;
	pt2.y -= rcClient.top - m_ptOrgin.y;
	int iItem=pt2.y/m_nItemHei;
	if( iItem >= m_nVisibleItems) return NULL;

	HSTREEITEM hRet=NULL;

	int iVisible=-1;
	HSTREEITEM hItem=CSTree<CDuiTreeItem*>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);
		if(pItem->m_bVisible) iVisible++;
		if(iVisible == iItem)
		{
			CRect rcItem(m_nIndent*pItem->m_nLevel,0,m_rcWindow.Width(),m_nItemHei);
			rcItem.OffsetRect(m_rcWindow.left,m_rcWindow.top-m_ptOrgin.y+iVisible*m_nItemHei);
			pt-=rcItem.TopLeft();
			hRet=hItem;
			break;
		}
		if(pItem->m_bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<CDuiTreeItem*>::GetNextItem(hItem);
	}		
	return hRet;
}



void CDuiTreeCtrl::SetChildrenVisible(HSTREEITEM hItem,BOOL bVisible)
{
	HSTREEITEM hChild=GetChildItem(hItem);
	while(hChild)
	{
		CDuiTreeItem *pItem=GetItem(hChild);
		pItem->m_bVisible=bVisible;
		m_nVisibleItems += bVisible?1:-1;
		if(!pItem->m_bCollapsed) SetChildrenVisible(hChild,bVisible);
		hChild=GetNextSiblingItem(hChild);
	}
}

void CDuiTreeCtrl::OnNodeFree(CDuiTreeItem * & pItem)
{
	if(m_pCapturedFrame==pItem)
	{
		m_pCapturedFrame=NULL;
		ReleaseDuiCapture();
	}
	pItem->Release();
}


int CDuiTreeCtrl::GetScrollLineSize(BOOL bVertical)
{
	return m_nItemHei;
}

BOOL CDuiTreeCtrl::LoadChildren(TiXmlElement* pTiXmlChildElem)
{
	if(!pTiXmlChildElem) return FALSE;

	if(m_pTiXmlSwitch) delete m_pTiXmlSwitch;
	m_pTiXmlSwitch=NULL;

	TiXmlElement *pSwitch=NULL;
	if(strcmp("switch",pTiXmlChildElem->Value())==0) 
		pSwitch=pTiXmlChildElem;
	else
		pSwitch=pTiXmlChildElem->NextSiblingElement("switch");

	if(pSwitch) 
		m_pTiXmlSwitch=(TiXmlElement*)pSwitch->Clone();

	RemoveAllItems();

	TiXmlElement *pItem=NULL;
	if(strcmp("item",pTiXmlChildElem->Value())==0) pItem=pTiXmlChildElem;
	else pItem=pTiXmlChildElem->NextSiblingElement("item");

	if(pItem) LoadBranch(STVI_ROOT,pItem);


	return TRUE;
}

void CDuiTreeCtrl::LoadBranch(HSTREEITEM hParent,TiXmlElement* pItem)
{
	while(pItem)
	{
		int dwData=0;
		pItem->Attribute("itemdata",&dwData);
		HSTREEITEM hItem=InsertItem(pItem,dwData,hParent);

		TiXmlElement *pChildItem=pItem->FirstChildElement("item");
		if(pChildItem) LoadBranch(hItem,pChildItem);

		pItem=pItem->NextSiblingElement("item");
	}
}

LRESULT CDuiTreeCtrl::OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
{
	__super::OnNcCalcSize(bCalcValidRects,lParam);
	HSTREEITEM hItem=GetNextItem(STVI_ROOT);
	while(hItem)
	{
		CDuiTreeItem * pItem= GetItem(hItem);
		pItem->Move(CRect(0,0,m_rcClient.Width()-pItem->m_nLevel*m_nIndent,m_nItemHei));
		hItem=GetNextItem(hItem);
	}
	NotifyInvalidate();
	return 0;
}

int CDuiTreeCtrl::GetItemShowIndex(HSTREEITEM hItemObj)
{
	int iVisible=-1;
	HSTREEITEM hItem=GetNextItem(STVI_ROOT);
	while(hItem)
	{
		CDuiTreeItem *pItem=GetItem(hItem);
		if(pItem->m_bVisible) iVisible++;
		if(hItem==hItemObj)
		{
			return iVisible;
		}
		if(pItem->m_bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=GetNextItem(hItem);
	}
	return -1;
}

void CDuiTreeCtrl::RedrawItem(HSTREEITEM hItem)
{
	if(!IsVisible(TRUE)) return;
	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(m_rcWindow.Height()+m_nItemHei-1)/m_nItemHei+1;
	int iItem=GetItemShowIndex(hItem);


	if(iItem!=-1 && iItem>=iFirstVisible && iItem<iFirstVisible+nPageItems)
	{
		CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);

		CRect rcItem(pItem->m_nLevel*m_nIndent,0,m_rcClient.Width(),m_nItemHei);
		rcItem.OffsetRect(0,m_nItemHei*iItem-m_ptOrgin.y);
		rcItem.OffsetRect(m_rcClient.TopLeft());

		CDCHandle dc=GetDuiDC(&rcItem,OLEDC_PAINTBKGND);
		DuiDCPaint duiDC;
		BeforePaint(dc,duiDC);

		DuiSendMessage(WM_ERASEBKGND,(WPARAM)(HDC)dc);
		DrawItem(dc,rcItem,hItem);

		AfterPaint(dc,duiDC);
		ReleaseDuiDC(dc);
	}
}

void CDuiTreeCtrl::DrawItem(CDCHandle & dc, CRect & rc, HSTREEITEM hItem)
{
	CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);
	DUINMGETTBDISPINFO nms;
	nms.hdr.code    = DUINM_GETTBDISPINFO;
	nms.hdr.hwndFrom = NULL;
	nms.hdr.idFrom  = GetCmdID();
	nms.bHover      = hItem==m_hHoverItem;
	nms.bSelect     = hItem == m_hSelItem;
	nms.hItem = hItem;
	nms.pItem = pItem;
	nms.pHostDuiWin   = this;
	LockUpdate();
	GetContainer()->OnDuiNotify((LPNMHDR)&nms);
	UnlockUpdate();
	pItem->Draw(dc,rc);
}

void CDuiTreeCtrl::OnPaint(CDCHandle dc)
{
	if(IsUpdateLocked()) return;

	DuiDCPaint duiDC;
	BeforePaint(dc,duiDC);

	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(m_rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;

	int iVisible=-1;
	HSTREEITEM hItem=CSTree<CDuiTreeItem*>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);
		if(pItem->m_bVisible) iVisible++;
		if(iVisible > iFirstVisible+nPageItems) break;
		if(iVisible>=iFirstVisible)
		{
			CRect rcItem(m_nIndent*pItem->m_nLevel,0,m_rcWindow.Width(),m_nItemHei);
			rcItem.OffsetRect(m_rcWindow.left,m_rcWindow.top-m_ptOrgin.y+iVisible*m_nItemHei);
			DrawItem(dc,rcItem,hItem);
		}
		if(pItem->m_bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<CDuiTreeItem*>::GetNextItem(hItem);
	}
	AfterPaint(dc,duiDC);
}

LRESULT CDuiTreeCtrl::OnSetItemCapture(UINT uMsg,WPARAM wParam, LPARAM lParam)
{
	m_pCapturedFrame= (CDuiItemPanel*)wParam;
	if(m_pCapturedFrame) SetDuiCapture();
	else ReleaseDuiCapture();
	return 0;
}

LRESULT CDuiTreeCtrl::OnGetItemRect(UINT,WPARAM wParam,LPARAM lParam)
{
	CDuiTreeItem *pItemObj=dynamic_cast<CDuiTreeItem*>((CDuiItemPanel*)wParam);
	if(pItemObj->m_bVisible==FALSE) return 0;

	LPRECT pRcItem=(LPRECT)lParam;

	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(m_rcWindow.Height()+m_nItemHei-1)/m_nItemHei+1;

	int iVisible=-1;
	HSTREEITEM hItem=CSTree<CDuiTreeItem*>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		CDuiTreeItem *pItem=CSTree<CDuiTreeItem*>::GetItem(hItem);
		if(pItem->m_bVisible) iVisible++;
		if(iVisible > iFirstVisible+nPageItems) break;
		if(iVisible>=iFirstVisible && pItem==pItemObj)
		{
			CRect rcRet(m_nIndent*pItemObj->m_nLevel,0,m_rcWindow.Width(),m_nItemHei);
			rcRet.OffsetRect(m_rcWindow.left,m_rcWindow.top-m_ptOrgin.y+iVisible*m_nItemHei);
			*pRcItem=rcRet;
			break;
		}
		if(pItem->m_bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<CDuiTreeItem*>::GetNextItem(hItem);
	}
	return 1;
}


void CDuiTreeCtrl::OnLButtonDown(UINT nFlags,CPoint pt)
{
	if(m_pCapturedFrame)
	{
		CRect rcItem=m_pCapturedFrame->GetItemRect();
		if(!rcItem.IsRectEmpty())
		{
			pt.Offset(-rcItem.left,-rcItem.top);
			m_pCapturedFrame->DoFrameEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(pt.x,pt.y));;
			return;
		}
	}
	m_hHoverItem=HitTest(pt);

	if(m_hHoverItem!=m_hSelItem)
	{
		DUINMTBSELCHANGED nms;
		nms.hdr.code=DUINM_TBSELCHANGED;
		nms.hdr.hwndFrom=NULL;
		nms.hdr.idFrom=GetCmdID();
		nms.hOldSel=m_hSelItem;
		nms.hNewSel=m_hHoverItem;

		if(m_hSelItem)
		{
			CSTree<CDuiTreeItem*>::GetItem(m_hSelItem)->ModifyItemState(0,DuiWndState_Check);
			RedrawItem(m_hSelItem);
		}
		m_hSelItem=m_hHoverItem;
		if(m_hSelItem)
		{
			CSTree<CDuiTreeItem*>::GetItem(m_hSelItem)->ModifyItemState(DuiWndState_Check,0);
			RedrawItem(m_hSelItem);
		}
		DuiNotify((LPNMHDR)&nms);			
	}
	if(m_hHoverItem)
	{//pt �Ѿ���HitTest�б��޸Ĺ�
		CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_LBUTTONDOWN,nFlags,MAKELPARAM(pt.x,pt.y));
	}
}

void CDuiTreeCtrl::OnLButtonUp(UINT nFlags,CPoint pt)
{
	if(m_pCapturedFrame)
	{
		CRect rcItem=m_pCapturedFrame->GetItemRect();
		if(!rcItem.IsRectEmpty())
		{
			pt.Offset(-rcItem.left,-rcItem.top);
			m_pCapturedFrame->DoFrameEvent(WM_LBUTTONUP,nFlags,MAKELPARAM(pt.x,pt.y));
			return;
		}
	}
	m_hHoverItem=HitTest(pt);
	if(m_hHoverItem)
	{
		CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_LBUTTONUP,nFlags,MAKELPARAM(pt.x,pt.y));
	}
}

void CDuiTreeCtrl::OnLButtonDbClick(UINT nFlags,CPoint pt)
{
	if(m_pCapturedFrame)
	{
		CRect rcItem=m_pCapturedFrame->GetItemRect();
		if(!rcItem.IsRectEmpty())
		{
			pt.Offset(-rcItem.left,-rcItem.top);
			m_pCapturedFrame->DoFrameEvent(WM_LBUTTONDBLCLK,nFlags,MAKELPARAM(pt.x,pt.y));
			return;
		}
	}
	m_hHoverItem=HitTest(pt);
	if(m_hHoverItem)
	{
		Expand(m_hHoverItem,TVE_TOGGLE);
		CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_LBUTTONDBLCLK,nFlags,MAKELPARAM(pt.x,pt.y));
	}else
	{
		DUINMITEMMOUSEEVENT nms;
		nms.hdr.code=DUINM_ITEMMOUSEEVENT;
		nms.hdr.hwndFrom=NULL;
		nms.hdr.idFrom=GetCmdID();
		nms.pItem=NULL;
		nms.uMsg=WM_LBUTTONDBLCLK;
		nms.wParam=nFlags;
		nms.lParam=MAKELPARAM(pt.x,pt.y);
		DuiNotify((LPNMHDR)&nms);
	}

}

void CDuiTreeCtrl::OnMouseMove(UINT nFlags,CPoint pt)
{
	if(m_pCapturedFrame)
	{
		CRect rcItem=m_pCapturedFrame->GetItemRect();
		if(!rcItem.IsRectEmpty())
		{
			pt.Offset(-rcItem.left,-rcItem.top);
			m_pCapturedFrame->DoFrameEvent(WM_MOUSEMOVE,nFlags,MAKELPARAM(pt.x,pt.y));
			return;
		}
	}
	HSTREEITEM hHitTest=HitTest(pt);
	if(hHitTest!=m_hHoverItem)
	{
		if(m_hHoverItem)
			CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_MOUSELEAVE,0,0);
		m_hHoverItem=hHitTest;
		if(m_hHoverItem)
			CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_MOUSEHOVER,0,0);
	}
	if(m_hHoverItem)
	{
		CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_MOUSEMOVE,nFlags,MAKELPARAM(pt.x,pt.y));
	}
}

void CDuiTreeCtrl::OnMouseLeave()
{
	if(m_pCapturedFrame)
	{
		m_pCapturedFrame->DoFrameEvent(WM_MOUSELEAVE,0,0);
	}
	if(m_hHoverItem)
	{
		CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_MOUSELEAVE,0,0);
		m_hHoverItem=NULL;
	}
}

LRESULT CDuiTreeCtrl::DuiNotify(LPNMHDR pnms)
{
	if(pnms->code==DUINM_LBITEMNOTIFY)
	{
		DUINMITEMNOTIFY *pItemNotify=(DUINMITEMNOTIFY*)pnms;
		if(pItemNotify->pOriginHdr->idFrom==IDC_SWITCH)
		{
			CDuiTreeItem *pItem=dynamic_cast<CDuiTreeItem*> (pItemNotify->pItem);
			Expand(pItem->m_hItem,TVE_TOGGLE);
			return 0;
		}
	}
	return __super::DuiNotify(pnms);
}

BOOL CDuiTreeCtrl::OnDuiSetCursor(const CPoint &pt)
{
	BOOL bRet=FALSE;
	if(m_pCapturedFrame)
	{
		CRect rcItem=m_pCapturedFrame->GetItemRect();
		bRet=m_pCapturedFrame->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
	}else if(m_hHoverItem)
	{
		CRect rcItem=CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->GetItemRect();
		bRet=CSTree<CDuiTreeItem*>::GetItem(m_hHoverItem)->DoFrameEvent(WM_SETCURSOR,0,MAKELPARAM(pt.x-rcItem.left,pt.y-rcItem.top))!=0;
	}
	if(!bRet)
	{
		bRet=__super::OnDuiSetCursor(pt);
	}
	return bRet;
}


BOOL CDuiTreeCtrl::IsAncestor(HSTREEITEM hItem1,HSTREEITEM hItem2)
{
	while(hItem2)
	{
		if(hItem2==hItem1) return TRUE;
		hItem2=GetParentItem(hItem2);
	}
	return FALSE;
}

}//namespace DuiEngine