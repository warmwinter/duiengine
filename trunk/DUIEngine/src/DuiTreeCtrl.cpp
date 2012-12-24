//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiTreeCtrl
// Description: CDuiTreeCtrl
//     Creator: JinHui
//     Version: 2012.12.16 - 1.1 - Create
//////////////////////////////////////////////////////////////////////////

#include "duistd.h"
#include "DuiTreeCtrl.h"

namespace DuiEngine{

CDuiTreeCtrl::CDuiTreeCtrl()
: m_nItemHei(20)
, m_nIndent(16)
, m_nItemMargin(4)
, m_hSelItem(NULL)
, m_hHoverItem(NULL)
, m_hCaptureItem(NULL)
, m_pItemBgSkin(NULL)
, m_pItemSelSkin(NULL)
, m_pIconSkin(NULL)
, m_pToggleSkin(NULL)
, m_pCheckSkin(NULL)
, m_crItemBg(RGB(255,255,255))
, m_crItemSelBg(RGB(0,0,136))
, m_crItemText(RGB(0,0,0))
, m_crItemSelText(RGB(255,255,255))
, m_nVisibleItems(0)
, m_nMaxItemWidth(0)
, m_bCheckBox(FALSE)
, m_uItemMask(0)
, m_nItemOffset(0)
, m_nItemHoverBtn(DuiTVIBtn_None)
, m_nItemPushDownBtn(DuiTVIBtn_None)
{
	m_bClipClient = TRUE;
}

CDuiTreeCtrl::~CDuiTreeCtrl()
{
}

////////////////////////////////////////////////////////////////////////////////////////////

HSTREEITEM CDuiTreeCtrl::InsertItem(LPCTSTR lpszItem, HSTREEITEM hParent, HSTREEITEM hInsertAfter)
{
	return InsertItem(lpszItem, -1, -1, 0, NULL,  hParent, hInsertAfter);
}

HSTREEITEM CDuiTreeCtrl::InsertItem(LPCTSTR lpszItem, int nImage,
		int nSelectedImage, HSTREEITEM hParent, HSTREEITEM hInsertAfter)
{
	return InsertItem(lpszItem, nImage, nSelectedImage, 0, NULL,  hParent, hInsertAfter);
}

HSTREEITEM CDuiTreeCtrl::InsertItem(LPCTSTR lpszItem, int nImage,
	int nSelectedImage, DWORD dwItemData, LPARAM lParam,
	HSTREEITEM hParent, HSTREEITEM hInsertAfter)
{
	LPTVITEM pItemObj = new TVITEM();

	pItemObj->strText = lpszItem;
	pItemObj->nImage = nImage;
	pItemObj->nSelectedImage  = nSelectedImage;
	pItemObj->dwData = dwItemData;
	pItemObj->lParam = lParam;

	return InsertItem(pItemObj, hParent, hInsertAfter, TRUE);
}

BOOL CDuiTreeCtrl::RemoveItem(HSTREEITEM hItem)
{	
	if(!hItem) return FALSE;
	HSTREEITEM hParent=GetParentItem(hItem);

	LPTVITEM pItem= CSTree<LPTVITEM>::GetItem(hItem);

	BOOL bVisible=pItem->bVisible;
	int nItemWidth =GetMaxItemWidth(hItem);
	int nCheckBoxValue = pItem->nCheckBoxValue;
	if(bVisible)
	{
		if(GetChildItem(hItem) && pItem->bCollapsed==FALSE)
		{
			SetChildrenVisible(hItem,FALSE);
		}
	}

	if(IsAncestor(hItem,m_hHoverItem)) m_hHoverItem=NULL;
	if(IsAncestor(hItem,m_hSelItem)) m_hSelItem=NULL;
	if(IsAncestor(hItem,m_hCaptureItem)) m_hCaptureItem=NULL;

	DeleteItem(hItem);
	
	//ȥ�����ڵ��չ����־
	if(hParent && !GetChildItem(hParent))
	{
		LPTVITEM pParent = GetItem(hParent);
		pParent->bHasChildren = FALSE;
		pParent->bCollapsed = FALSE;
		CalaItemWidth(pParent);			
	}

	if(m_bCheckBox && hParent && GetChildItem(hParent))
	{
		//ԭ��㸴ѡ��ѡ�У���鸸����Ƿ��ɰ�ѡ�䲻ѡ	
		if (nCheckBoxValue == DuiTVICheckBox_Checked )
			CheckState(hParent, FALSE);	
		//ԭ��㸴ѡ��δѡ�У���鸸����Ƿ��ɰ�ѡ��ȫѡ	
		else
			CheckState(hParent, TRUE);	
	}

	if(bVisible)
	{
		m_nVisibleItems--;

		//���¼���x���ߴ�
		if (nItemWidth == m_nMaxItemWidth)		
			GetMaxItemWidth();		

		CSize szView(m_nMaxItemWidth,m_nVisibleItems*m_nItemHei);
		SetViewSize(szView);
		NotifyInvalidate();
	}
	return TRUE;
}

void CDuiTreeCtrl::RemoveAllItems()
{
	DeleteAllItems();
	m_nVisibleItems=0;
	m_hSelItem=NULL;
	m_hHoverItem=NULL;
	m_hCaptureItem=NULL;
	m_nMaxItemWidth=0;	
	SetViewSize(CSize(0,0));
	SetViewOrigin(CPoint(0,0));
}

HSTREEITEM CDuiTreeCtrl::GetRootItem()
{
	return GetChildItem(STVI_ROOT);
}

HSTREEITEM CDuiTreeCtrl::GetNextSiblingItem(HSTREEITEM hItem)
{
	return CSTree<LPTVITEM>::GetNextSiblingItem(hItem);
}

HSTREEITEM CDuiTreeCtrl::GetPrevSiblingItem(HSTREEITEM hItem)
{
	return CSTree<LPTVITEM>::GetPrevSiblingItem(hItem);
}

HSTREEITEM CDuiTreeCtrl::GetChildItem(HSTREEITEM hItem,BOOL bFirst/* =TRUE*/)
{
	return CSTree<LPTVITEM>::GetChildItem(hItem,bFirst);
}

HSTREEITEM CDuiTreeCtrl::GetParentItem(HSTREEITEM hItem)
{
	return CSTree<LPTVITEM>::GetParentItem(hItem);
}

HSTREEITEM CDuiTreeCtrl::GetSelectedItem()
{
	return m_hSelItem;
}

BOOL CDuiTreeCtrl::GetItemText(HSTREEITEM hItem, CString& strText) const
{
	if (hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if (pItem)
		{
			strText = pItem->strText;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDuiTreeCtrl::SetItemText(HSTREEITEM hItem, LPCTSTR lpszItem)
{
	if (hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if (pItem)
		{
			pItem->strText = lpszItem;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDuiTreeCtrl::GetItemImage(HSTREEITEM hItem, int& nImage, int& nSelectedImage) const
{
	if (hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if (pItem)
		{
			nImage = pItem->nImage;
			nSelectedImage = pItem->nSelectedImage;
			return TRUE;
		}
	}
	return FALSE;
}

DWORD_PTR CDuiTreeCtrl::GetItemData(HSTREEITEM hItem) const
{
	if (hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if (pItem)
			return pItem->dwData;
	}
	return 0;
}

BOOL CDuiTreeCtrl::SetItemData(HSTREEITEM hItem, DWORD_PTR dwData)
{
	if (hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if (pItem)
		{
			pItem->dwData = dwData;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDuiTreeCtrl::ItemHasChildren(HSTREEITEM hItem) 
{
	if (!hItem) return FALSE;

	return  GetChildItem(hItem)!=NULL;
}

BOOL CDuiTreeCtrl::GetCheckState(HSTREEITEM hItem) const
{
	if (!m_bCheckBox) return FALSE;

	LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
	if (pItem->nCheckBoxValue == DuiTVICheckBox_Checked)
		return TRUE;

	return FALSE;
}

BOOL CDuiTreeCtrl::SetCheckState(HSTREEITEM hItem, BOOL bCheck)
{
	if (!m_bCheckBox) return FALSE;

	int nCheck = bCheck ? DuiTVICheckBox_Checked : DuiTVICheckBox_UnChecked;   

	LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
	pItem->nCheckBoxValue = nCheck;

	//��������
	if (CSTree<LPTVITEM>::GetChildItem(hItem))
		SetChildrenState(hItem, nCheck);

	//��鸸���״̬
	CheckState(GetParentItem(hItem), bCheck);

	NotifyInvalidate();

	return TRUE;
}

BOOL CDuiTreeCtrl::Expand(HSTREEITEM hItem , UINT nCode)
{
	BOOL bRet=FALSE;
	if(CSTree<LPTVITEM>::GetChildItem(hItem))
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if(nCode==TVE_COLLAPSE && !pItem->bCollapsed)
		{
			pItem->bCollapsed=TRUE;
			SetChildrenVisible(hItem,FALSE);
			bRet=TRUE;
		}
		if(nCode==TVE_EXPAND && pItem->bCollapsed)
		{
			pItem->bCollapsed=FALSE;
			SetChildrenVisible(hItem,TRUE);
			bRet=TRUE;
		}
		if(nCode==TVE_TOGGLE)
		{
			pItem->bCollapsed=!pItem->bCollapsed;
			SetChildrenVisible(hItem,!pItem->bCollapsed);
			bRet=TRUE;
		}
		if(bRet)
		{
			GetMaxItemWidth();
			CSize szView(m_nMaxItemWidth,m_nVisibleItems*m_nItemHei);
			SetViewSize(szView);
			NotifyInvalidate();
		}
	}
	return bRet;
}

BOOL CDuiTreeCtrl::EnsureVisible(HSTREEITEM hItem)
{
	LPTVITEM pItem=GetItem(hItem);
	if(!pItem->bVisible)
	{
		HSTREEITEM hParent=GetParentItem(hItem);
		while(hParent)
		{
			LPTVITEM pParent=GetItem(hParent);
			if(pParent->bCollapsed) Expand(hParent,TVE_EXPAND);
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

void CDuiTreeCtrl::PageUp()
{
	OnScroll(TRUE,SB_PAGEUP,0);
}

void CDuiTreeCtrl::PageDown()
{
	OnScroll(TRUE,SB_PAGEDOWN,0);
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOL CDuiTreeCtrl::LoadChildren(TiXmlElement* pTiXmlChildElem)
{
	if(!pTiXmlChildElem) return FALSE;

	RemoveAllItems();
	ItemLayout();

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
		HSTREEITEM hItem=InsertItem(pItem,hParent);

		TiXmlElement *pChildItem=pItem->FirstChildElement("item");
		if(pChildItem) LoadBranch(hItem,pChildItem);

		pItem=pItem->NextSiblingElement("item");
	}
}

void CDuiTreeCtrl::LoadItemAttribute(TiXmlElement *pTiXmlItem, LPTVITEM pItem)
{
	for (TiXmlAttribute *pAttrib = pTiXmlItem->FirstAttribute(); NULL != pAttrib; pAttrib = pAttrib->Next())
	{
		if ( !_stricmp(pAttrib->Name(), "text"))
			pItem->strText = CA2T(pAttrib->Value(), CP_UTF8); 
		else if ( !_stricmp(pAttrib->Name(), "img"))
			pItem->nImage = atoi(pAttrib->Value());
		else if ( !_stricmp(pAttrib->Name(), "selimg"))
			pItem->nSelectedImage = atoi(pAttrib->Value());
		else if ( !_stricmp(pAttrib->Name(), "data"))
			pItem->dwData = atol(pAttrib->Value());
	}
}

HSTREEITEM CDuiTreeCtrl::InsertItem(LPTVITEM pItemObj,HSTREEITEM hParent,HSTREEITEM hInsertAfter,BOOL bEnsureVisible)
{
	DUIASSERT(pItemObj);

	int nViewWidth;
	CRect rcClient;
	GetClient(rcClient);

	pItemObj->nLevel = GetItemLevel(hParent)+1;

	if(hParent!=STVI_ROOT)
	{		
		LPTVITEM pParentItem= GetItem(hParent);
		if(pParentItem->bCollapsed || !pParentItem->bVisible) 
			pItemObj->bVisible=FALSE;

		if (!GetChildItem(hParent) && !pParentItem->bHasChildren)
		{
			pParentItem->bHasChildren = TRUE;
			CalaItemWidth(pParentItem);
		}
	}	

	CalaItemWidth(pItemObj);

	HSTREEITEM hRet= CSTree<LPTVITEM>::InsertItem(pItemObj,hParent,hInsertAfter);
	pItemObj->hItem = hRet;

	if(pItemObj->bVisible)
	{
		m_nVisibleItems++;

		nViewWidth = max(rcClient.Width(), pItemObj->nItemWidth + pItemObj->nLevel*m_nIndent);
		if ( nViewWidth > m_nMaxItemWidth ) 
			m_nMaxItemWidth = nViewWidth;

		CSize szView(m_nMaxItemWidth, m_nVisibleItems*m_nItemHei);
		SetViewSize(szView);
		NotifyInvalidate();
	}

	if(bEnsureVisible) EnsureVisible(hRet);
	return hRet;
}

HSTREEITEM CDuiTreeCtrl::InsertItem(TiXmlElement *pTiXmlItem,HSTREEITEM hParent/*=STVI_ROOT*/, HSTREEITEM hInsertAfter/*=STVI_LAST*/,BOOL bEnsureVisible/*=FALSE*/)
{
	LPTVITEM pItemObj = new TVITEM();

	LoadItemAttribute(pTiXmlItem, pItemObj);
	return InsertItem(pItemObj, hParent, hInsertAfter, bEnsureVisible);
}

int CDuiTreeCtrl::GetScrollLineSize(BOOL bVertical)
{
	return m_nItemHei;
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

void CDuiTreeCtrl::SetChildrenVisible(HSTREEITEM hItem,BOOL bVisible)
{
	HSTREEITEM hChild=GetChildItem(hItem);
	while(hChild)
	{
		LPTVITEM pItem=GetItem(hChild);
		pItem->bVisible=bVisible;
		m_nVisibleItems += bVisible?1:-1;
		if(!pItem->bCollapsed) SetChildrenVisible(hChild,bVisible);
		hChild=GetNextSiblingItem(hChild);
	}
}

void CDuiTreeCtrl::SetChildrenState(HSTREEITEM hItem, int nCheckValue)
{
	HSTREEITEM hChildItem = CSTree<LPTVITEM>::GetChildItem(hItem);
	while(hChildItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hChildItem);
		pItem->nCheckBoxValue = nCheckValue;	
		SetChildrenState(hChildItem, nCheckValue);
		hChildItem = CSTree<LPTVITEM>::GetNextSiblingItem(hChildItem);
	}		
}

//������״̬һ�·���TRUE, ���򷵻�FALSE
BOOL CDuiTreeCtrl::CheckChildrenState(HSTREEITEM hItem, BOOL bCheck)
{
	HSTREEITEM hChildItem = CSTree<LPTVITEM>::GetChildItem(hItem);
	while(hChildItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hChildItem);

		int nCheckValue = bCheck ? DuiTVICheckBox_Checked : DuiTVICheckBox_UnChecked;
		//��ǰ��㲻һ����������
		if (pItem->nCheckBoxValue != nCheckValue) 
			return FALSE;
		//����ӽ�㲻һ����������
		else if (CheckChildrenState(hChildItem, bCheck) == FALSE)
			return FALSE;
		
		//����ӽ���ֵܽ��
		hChildItem = CSTree<LPTVITEM>::GetNextSiblingItem(hChildItem);
	}
	return TRUE;
}

void CDuiTreeCtrl::CheckState(HSTREEITEM hItem, BOOL bCheck, BOOL bCheckChild)
{		
	if(hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);

		if (bCheckChild && CheckChildrenState(hItem, bCheck))
		{
			int nCheckValue = bCheck ? DuiTVICheckBox_Checked : DuiTVICheckBox_UnChecked;
			pItem->nCheckBoxValue = nCheckValue;
			CheckState(GetParentItem(hItem), bCheck, TRUE);
		}
		else
		{
			pItem->nCheckBoxValue = DuiTVICheckBox_PartChecked;
			CheckState(GetParentItem(hItem),bCheck,  FALSE);
		}
	}		
}

void CDuiTreeCtrl::ItemLayout()
{
	int nOffset = 0;
	CSize sizeSkin;

	m_uItemMask = 0;
	m_rcToggle.SetRect(0, 0, 0, 0);
	m_rcCheckBox.SetRect(0, 0, 0, 0);
	m_rcIcon.SetRect(0, 0, 0, 0);

	//����λ��	
	if (m_pToggleSkin)
	{
		m_uItemMask |= DuiTVIMask_Toggle;
		sizeSkin = m_pToggleSkin->GetSkinSize();		
		m_rcToggle.SetRect( 
			nOffset, 
			(m_nItemHei - sizeSkin.cy) / 2,
			nOffset + sizeSkin.cx,
			m_nItemHei - (m_nItemHei - sizeSkin.cy) / 2);
		nOffset += sizeSkin.cx;
	}

	if (m_bCheckBox && m_pCheckSkin)
	{
		m_uItemMask |= DuiTVIMask_CheckBox;
		sizeSkin = m_pCheckSkin->GetSkinSize();	
		m_rcCheckBox.SetRect( 
			nOffset, 
			(m_nItemHei - sizeSkin.cy) / 2,
			nOffset + sizeSkin.cx,
			m_nItemHei - (m_nItemHei - sizeSkin.cy) / 2);
		nOffset += sizeSkin.cx;
	}

	if (m_pIconSkin)
	{
		m_uItemMask |= DuiTVIMask_Icon;
		sizeSkin = m_pIconSkin->GetSkinSize();
		m_rcIcon.SetRect( 
			nOffset, 
			(m_nItemHei - sizeSkin.cy) / 2,
			nOffset + sizeSkin.cx,
			m_nItemHei - (m_nItemHei - sizeSkin.cy) / 2);
		nOffset += sizeSkin.cx;
	}

	m_nItemOffset = nOffset;
}

void CDuiTreeCtrl::CalaItemWidth(LPTVITEM pItem)
{
	int nTextWidth = 0, nOffset = 0;
	CSize sizeSkin;
	CDCHandle dcDesktop = ::GetDC(::GetDesktopWindow());
	CDC dcTest;
	HFONT hftOld = NULL, hftTest = NULL;
	CRect rcTest;

	dcTest.CreateCompatibleDC(dcDesktop);

	HMONITOR hMonitor = ::MonitorFromWindow(GetContainer()->GetHostHwnd(), MONITOR_DEFAULTTONULL);
	if (hMonitor)
	{
		MONITORINFO mi = {sizeof(MONITORINFO)};
		::GetMonitorInfo(hMonitor, &mi);
		rcTest = mi.rcWork;
	}
	else
		GetClientRect(GetContainer()->GetHostHwnd(), rcTest);

	if (m_style.m_ftText)
		hftTest = m_style.m_ftText;
	else
	{
		CDuiWindow* pWnd = this;

		while (pWnd = pWnd->GetParent())
		{
			DuiStyle& style = pWnd->GetStyle();
			if (style.m_ftText)
			{
				hftTest = style.m_ftText;
				break;
			}
		}
	}
	
	if (NULL == hftTest)
		hftTest = DuiFontPool::getSingleton().GetFont(DUIF_DEFAULTFONT);

	hftOld = dcTest.SelectFont(hftTest);

	int nTestDrawMode = m_style.m_nTextAlign & ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_BOTTOM);

	DuiDrawText(dcTest,pItem->strText, pItem->strText.GetLength(), rcTest, nTestDrawMode | DT_CALCRECT);

	nTextWidth = rcTest.Width();

	dcTest.SelectFont(hftOld);
	::ReleaseDC(::GetDesktopWindow(), dcDesktop);

	pItem->nItemWidth = nTextWidth + m_nItemOffset + 2*m_nItemMargin;

	if (m_nItemOffset > 0)
		pItem->nItemWidth += m_nItemMargin;	
}

int CDuiTreeCtrl::GetMaxItemWidth(HSTREEITEM hItem)
{
	int nItemWidth = 0, nChildrenWidth = 0;

	LPTVITEM pItem=GetItem(hItem);
	if (pItem->bVisible)
		nItemWidth = pItem->nItemWidth + pItem->nLevel*m_nIndent;
	else
		return 0;

	HSTREEITEM hChild=GetChildItem(hItem);
	while(hChild)
	{
		nChildrenWidth = GetMaxItemWidth(hChild);
		if (nChildrenWidth > nItemWidth)
			nItemWidth = nChildrenWidth;

		hChild=GetNextSiblingItem(hChild);
	}

	return nItemWidth;
}

int  CDuiTreeCtrl::GetMaxItemWidth()
{
	int nItemWidth = 0;
	m_nMaxItemWidth = 0;
	HSTREEITEM hItem=CSTree<LPTVITEM>::GetNextItem(STVI_ROOT);
	
	while(hItem)
	{
		nItemWidth = GetMaxItemWidth(hItem);
		if (nItemWidth > m_nMaxItemWidth)
			m_nMaxItemWidth = nItemWidth;
		hItem=GetNextSiblingItem(hItem);
	}

	return m_nMaxItemWidth;
}

int CDuiTreeCtrl::GetItemShowIndex(HSTREEITEM hItemObj)
{
	int iVisible=-1;
	HSTREEITEM hItem=GetNextItem(STVI_ROOT);
	while(hItem)
	{
		LPTVITEM pItem=GetItem(hItem);
		if(pItem->bVisible) iVisible++;
		if(hItem==hItemObj)
		{
			return iVisible;
		}
		if(pItem->bCollapsed)
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

BOOL CDuiTreeCtrl::GetItemRect( LPTVITEM pItemObj,CRect &rcItem )
{
	if(pItemObj->bVisible==FALSE) return FALSE;

	CRect rcClient;
	GetClient(rcClient);
	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;

	int iVisible=-1;
	HSTREEITEM hItem=CSTree<LPTVITEM>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if(pItem->bVisible) iVisible++;
		if(iVisible > iFirstVisible+nPageItems) break;
		if(iVisible>=iFirstVisible && pItem==pItemObj)
		{
			CRect rcRet(m_nIndent*pItemObj->nLevel,0,rcClient.Width(),m_nItemHei);
			rcRet.OffsetRect(rcClient.left-m_ptOrgin.x,rcClient.top-m_ptOrgin.y+iVisible*m_nItemHei);
			rcItem=rcRet;
			return TRUE;
		}
		if(pItem->bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<LPTVITEM>::GetNextItem(hItem);
	}
	return FALSE;
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
	HSTREEITEM hItem=CSTree<LPTVITEM>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if(pItem->bVisible) iVisible++;
		if(iVisible == iItem)
		{
			CRect rcItem(m_nIndent*pItem->nLevel,0,rcClient.Width(),m_nItemHei);
			rcItem.OffsetRect(rcClient.left-m_ptOrgin.x,rcClient.top-m_ptOrgin.y+iVisible*m_nItemHei);
			pt-=rcItem.TopLeft();
			hRet=hItem;
			break;
		}
		if(pItem->bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<LPTVITEM>::GetNextItem(hItem);
	}		
	return hRet;
}

void CDuiTreeCtrl::RedrawItem(HSTREEITEM hItem)
{
	if(!IsVisible(TRUE)) return;
	CRect rcClient;
	GetClient(rcClient);

	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;
	int iItem=GetItemShowIndex(hItem);
	if(iItem!=-1 && iItem>=iFirstVisible && iItem<iFirstVisible+nPageItems)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		
		CRect rcItem(m_nIndent*pItem->nLevel,0,m_nIndent*pItem->nLevel+pItem->nItemWidth ,m_nItemHei);
		rcItem.OffsetRect(rcClient.left-m_ptOrgin.x,
				rcClient.top+m_nItemHei*iItem-m_ptOrgin.y);
		if (rcItem.bottom > rcClient.bottom) rcItem.bottom = rcClient.bottom;
		if (rcItem.right > rcClient.right) rcItem.right = rcClient.right;

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
	BOOL     bTextColorChanged = FALSE;;
	COLORREF crOldText;
	CRect rcItemBg;
	LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);

	dc.OffsetViewportOrg(rc.left,rc.top);

	rcItemBg.SetRect( m_nItemOffset + m_nItemMargin, 0, pItem->nItemWidth, m_nItemHei);

	//���Ʊ���
	if (hItem == m_hSelItem)
	{
		if (m_pItemSelSkin != NULL)
			m_pItemSelSkin->Draw(dc, rcItemBg, 0);
		else if (CLR_INVALID != m_crItemSelBg)
			CGdiAlpha::FillSolidRect(dc, rcItemBg, m_crItemSelBg);

		if (CLR_INVALID != m_crItemSelText)
		{
			bTextColorChanged = TRUE;
			crOldText = dc.SetTextColor(m_crItemSelText);
		}
	}
	else
	{
		if (CLR_INVALID != m_crItemText)
		{
			bTextColorChanged = TRUE;
			crOldText = dc.SetTextColor(m_crItemText);
		}
	}

	if (pItem->bHasChildren &&
		DuiTVIMask_Toggle == (m_uItemMask & DuiTVIMask_Toggle))
	{
		int nImage = IIF_STATE3(pItem->dwToggleState, 0, 1, 2);
		if (!pItem->bCollapsed) nImage += 3;
		m_pToggleSkin->Draw(dc, m_rcToggle, nImage);
	}
	
	if (DuiTVIMask_CheckBox == (m_uItemMask & DuiTVIMask_CheckBox))
	{
		int nImage = IIF_STATE3(pItem->dwCheckBoxState, 0, 1, 2);
		if (pItem->nCheckBoxValue == DuiTVICheckBox_Checked)
			nImage += 3;
		else if (pItem->nCheckBoxValue == DuiTVICheckBox_PartChecked)
			nImage += 6;
		m_pCheckSkin->Draw(dc, m_rcCheckBox, nImage);
	}
	
	if (DuiTVIMask_Icon == (m_uItemMask & DuiTVIMask_Icon) &&
		(pItem->nSelectedImage != -1 || pItem->nImage != -1))
	{
		if (pItem->nSelectedImage != -1 && hItem == m_hSelItem)
			m_pIconSkin->Draw(dc, m_rcIcon, pItem->nSelectedImage);
		else 
			m_pIconSkin->Draw(dc, m_rcIcon, pItem->nImage);		
	}
		
	UINT align=DT_VCENTER|DT_SINGLELINE;
	rcItemBg.OffsetRect(m_nItemMargin, 0);
	CGdiAlpha::DrawText(dc, pItem->strText,-1,rcItemBg,align);	

	if (bTextColorChanged)
		dc.SetTextColor(crOldText);

	dc.OffsetViewportOrg(-rc.left,-rc.top);
}

int CDuiTreeCtrl::ItemHitTest(HSTREEITEM hItem,CPoint &pt) const
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);
	int nHitTestBtn = DuiTVIBtn_None;

	if (DuiTVIMask_Toggle == (m_uItemMask & DuiTVIMask_Toggle)
		&& pItem->bHasChildren
		&& m_rcToggle.PtInRect(pt))
		nHitTestBtn = DuiTVIBtn_Toggle;
	else if (DuiTVIMask_CheckBox == (m_uItemMask & DuiTVIMask_CheckBox)
		&& m_rcCheckBox.PtInRect(pt))
		nHitTestBtn = DuiTVIBtn_CheckBox;
	
	return nHitTestBtn;
}


void CDuiTreeCtrl::ModifyToggleState(HSTREEITEM hItem, DWORD dwStateAdd, DWORD dwStateRemove)
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);

	pItem->dwToggleState |= dwStateAdd;
	pItem->dwToggleState &= ~dwStateRemove;
	
	CRect rcItem, rcUpdate = m_rcToggle;
	if (GetItemRect(pItem, rcItem))
	{		
		rcUpdate.OffsetRect(rcItem.left, rcItem.top);
		NotifyInvalidateRect(rcUpdate);
	}
}

void CDuiTreeCtrl::ModifyChekcBoxState(HSTREEITEM hItem, DWORD dwStateAdd, DWORD dwStateRemove)
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);

	pItem->dwCheckBoxState |= dwStateAdd;
	pItem->dwCheckBoxState &= ~dwStateRemove;

	CRect rcItem, rcUpdate = m_rcCheckBox;
	if (GetItemRect(pItem, rcItem))
	{
		rcUpdate.OffsetRect(rcItem.left, rcItem.top);
		NotifyInvalidateRect(rcUpdate);
	}
}

void CDuiTreeCtrl::ItemLButtonDown(HSTREEITEM hItem, UINT nFlags,CPoint pt)
{
	int nHitTestBtn = ItemHitTest(hItem, pt);	
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);	

	//���ԭ��pushdown��ť
	if (m_nItemPushDownBtn != nHitTestBtn)
	{
		if (m_nItemPushDownBtn == DuiTVIBtn_Toggle && 
			DuiWndState_PushDown == (pItem->dwToggleState & DuiWndState_PushDown))
		{
			ModifyToggleState(hItem, 0, DuiWndState_PushDown);
		}

		if (m_nItemPushDownBtn == DuiTVIBtn_CheckBox && 
			DuiWndState_PushDown == (pItem->dwCheckBoxState & DuiWndState_PushDown))
		{
			ModifyChekcBoxState(hItem, 0, DuiWndState_PushDown);
		}

		m_nItemPushDownBtn = nHitTestBtn;
	}

	//����pushdown��ť
	if (m_nItemPushDownBtn != DuiTVIBtn_None)
	{
		if (m_nItemPushDownBtn == DuiTVIBtn_Toggle && 
			DuiWndState_PushDown != (pItem->dwToggleState & DuiWndState_PushDown))
		{
			ModifyToggleState(hItem, DuiWndState_PushDown, 0);
			Expand(pItem->hItem, TVE_TOGGLE);			
		}

		if (m_nItemPushDownBtn == DuiTVIBtn_CheckBox && 
			DuiWndState_PushDown != (pItem->dwCheckBoxState & DuiWndState_PushDown))
		{
			BOOL bCheck = 
				pItem->nCheckBoxValue == DuiTVICheckBox_Checked ? FALSE : TRUE;
			ModifyChekcBoxState(hItem, DuiWndState_PushDown, 0);
			SetCheckState(pItem->hItem, bCheck);
		}
	}
}

void CDuiTreeCtrl::ItemLButtonUp(HSTREEITEM hItem, UINT nFlags,CPoint pt)
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);

	if (m_nItemPushDownBtn != DuiTVIBtn_None)
	{
		if (m_nItemPushDownBtn == DuiTVIBtn_Toggle && 
			DuiWndState_PushDown == (pItem->dwToggleState & DuiWndState_PushDown))
		{
			ModifyToggleState(hItem, 0, DuiWndState_PushDown);
		}

		if (m_nItemPushDownBtn == DuiTVIBtn_CheckBox && 
			DuiWndState_PushDown == (pItem->dwCheckBoxState & DuiWndState_PushDown))
		{
			ModifyChekcBoxState(hItem, 0, DuiWndState_PushDown);
		}

		m_nItemPushDownBtn = DuiTVIBtn_None;
	}
}

void CDuiTreeCtrl::ItemLButtonDbClick(HSTREEITEM hItem, UINT nFlags,CPoint pt)
{
	int nHitTestBtn = ItemHitTest(hItem, pt);
	if (nHitTestBtn == DuiTVIBtn_CheckBox)
		ItemLButtonDown(hItem, nFlags, pt);
}

void CDuiTreeCtrl::ItemMouseMove(HSTREEITEM hItem, UINT nFlags,CPoint pt)
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);

	int nHitTestBtn = ItemHitTest(hItem, pt);
	
	if (nHitTestBtn != m_nItemHoverBtn)
	{
		if (m_nItemHoverBtn == DuiTVIBtn_Toggle && 
			DuiWndState_Hover == (pItem->dwToggleState & DuiWndState_Hover))
		{
			ModifyToggleState(hItem, 0, DuiWndState_Hover);
		}

		if (m_nItemHoverBtn == DuiTVIBtn_CheckBox && 
			DuiWndState_Hover == (pItem->dwCheckBoxState & DuiWndState_Hover))
		{
			ModifyChekcBoxState(hItem, 0, DuiWndState_Hover);
		}

		m_nItemHoverBtn = nHitTestBtn;
	}

	if (m_nItemHoverBtn != DuiTVIBtn_None)
	{
		if (m_nItemHoverBtn == DuiTVIBtn_Toggle && 
			DuiWndState_Hover != (pItem->dwToggleState & DuiWndState_Hover))
		{
			ModifyToggleState(hItem, DuiWndState_Hover, 0);
		}

		if (m_nItemHoverBtn == DuiTVIBtn_CheckBox && 
			DuiWndState_Hover != (pItem->dwCheckBoxState & DuiWndState_Hover))
		{
			ModifyChekcBoxState(hItem, DuiWndState_Hover, 0);
		}
	}
}

void CDuiTreeCtrl::ItemMouseLeave(HSTREEITEM hItem)
{
	LPTVITEM pItem = CSTree<LPTVITEM>::GetItem(hItem);

	if (m_nItemHoverBtn != DuiTVIBtn_None)
	{
		if (m_nItemHoverBtn == DuiTVIBtn_Toggle &&
			DuiWndState_Hover == (pItem->dwToggleState & DuiWndState_Hover))
		{
			ModifyToggleState(hItem, 0, DuiWndState_Hover);
		}

		if (m_nItemHoverBtn == DuiTVIBtn_CheckBox &&
			DuiWndState_Hover == (pItem->dwCheckBoxState & DuiWndState_Hover))
		{
			ModifyChekcBoxState(hItem, 0, DuiWndState_Hover);
		}

		m_nItemHoverBtn = DuiTVIBtn_None;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////

BOOL CDuiTreeCtrl::RedrawRegion(CDCHandle& dc, CRgn& rgn)
{
	return __super::RedrawRegion(dc, rgn);
}

void CDuiTreeCtrl::OnDestroy()
{
	DeleteAllItems();
}

void CDuiTreeCtrl::OnPaint(CDCHandle dc)
{
	if(IsUpdateLocked()) return;

	CRect rcClient;
	DuiDCPaint duiDC;
	BeforePaint(dc,duiDC);

	GetClient(rcClient);
	int iFirstVisible=m_ptOrgin.y/m_nItemHei;
	int nPageItems=(m_rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;

	int iVisible=-1;
	HSTREEITEM hItem=CSTree<LPTVITEM>::GetNextItem(STVI_ROOT);
	while(hItem)
	{
		LPTVITEM pItem=CSTree<LPTVITEM>::GetItem(hItem);
		if(pItem->bVisible) iVisible++;
		if(iVisible > iFirstVisible+nPageItems) break;
		if(iVisible>=iFirstVisible)
		{
			CRect rcItem(m_nIndent*pItem->nLevel,0,m_nIndent*pItem->nLevel+pItem->nItemWidth,m_nItemHei);
			rcItem.OffsetRect(rcClient.left-m_ptOrgin.x,
				rcClient.top-m_ptOrgin.y+iVisible*m_nItemHei);
			DrawItem(dc,rcItem,hItem);
		}
		if(pItem->bCollapsed)
		{//�������۵�����
			HSTREEITEM hChild= GetChildItem(hItem,FALSE);
			while(hChild)
			{
				hItem=hChild;
				hChild= GetChildItem(hItem,FALSE);
			}
		}
		hItem=CSTree<LPTVITEM>::GetNextItem(hItem);
	}
	AfterPaint(dc,duiDC);
}

void CDuiTreeCtrl::OnLButtonDown(UINT nFlags,CPoint pt)
{
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
			HSTREEITEM hOldSelItem = m_hSelItem;
			m_hSelItem = m_hHoverItem;		
			RedrawItem(hOldSelItem);
		}
		else
			m_hSelItem = m_hHoverItem;

		if(m_hSelItem)
			RedrawItem(m_hSelItem);

		DuiNotify((LPNMHDR)&nms);			
	}

	//pt �Ѿ���HitTest�б��޸Ĺ�
	if(m_hHoverItem)
	{	
		m_hCaptureItem = m_hHoverItem;
		ItemLButtonDown(m_hHoverItem, nFlags, pt);
	}
}

void CDuiTreeCtrl::OnLButtonUp(UINT nFlags,CPoint pt)
{
	m_hHoverItem=HitTest(pt);

	if (m_hCaptureItem)
	{
		ItemLButtonUp(m_hCaptureItem, nFlags, pt);
		m_hCaptureItem = NULL;
		return;
	}

	if(m_hHoverItem)
		ItemLButtonUp(m_hHoverItem, nFlags, pt);
}

void CDuiTreeCtrl::OnLButtonDbClick(UINT nFlags,CPoint pt)
{	
	m_hHoverItem=HitTest(pt);
	if(m_hHoverItem)
	{
		Expand(m_hHoverItem,TVE_TOGGLE);
		ItemLButtonDbClick(m_hHoverItem, nFlags, pt);
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
	HSTREEITEM hHitTest=HitTest(pt);
	
	if(hHitTest!=m_hHoverItem)
	{
		if(m_hHoverItem)		
			ItemMouseLeave(m_hHoverItem);
	
		m_hHoverItem=hHitTest;
	}
	if(m_hHoverItem)
		ItemMouseMove(m_hHoverItem, nFlags, pt);
}

void CDuiTreeCtrl::OnMouseLeave()
{
	if(m_hHoverItem)
	{
		ItemMouseLeave(m_hHoverItem);
		m_hHoverItem=NULL;
	}
}

}//namespace DuiEngine