//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiListBox
// Description: A DuiWindow Based ListBox Control.
//     Creator: JinHui
//     Version: 2012.12.18 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma  once
#include "duistd.h"
#include "duilistbox.h"
#include "DuiSystem.h"
#include "mybuffer.h"

#pragma warning(disable:4018)
#pragma warning(disable:4267)

namespace DuiEngine
{


CDuiListBox::CDuiListBox()
    : m_nItemHei(20)
    , m_iScrollSpeed(-1)
    , m_iSelItem(-1)
    , m_iHoverItem(-1)
    , m_crItemBg(CLR_INVALID)
    , m_crItemBg2(CLR_INVALID)
    , m_crItemSelBg(CLR_INVALID)
    , m_crText(CLR_INVALID)
    , m_crSelText(CLR_INVALID)
    , m_pItemSkin(NULL)
    , m_pIconSkin(NULL)
    , m_ptIcon(-1,-1)
    , m_ptText(-1,-1)
    , m_bHotTrack(FALSE)
{
}

CDuiListBox::~CDuiListBox()
{
}

int CDuiListBox::GetCount() const
{
    return m_arrItems.GetCount();
}

int CDuiListBox::GetCurSel() const
{
    return m_iSelItem;
}

BOOL CDuiListBox::SetCurSel(int nIndex)
{
    if(m_iSelItem == nIndex) return 0;

    if(nIndex < 0 && nIndex >= GetCount())
        return FALSE;

    int nOldSelItem = m_iSelItem;
    m_iSelItem = nIndex;

    if(IsVisible(TRUE))
    {
        if(nOldSelItem != -1)
            RedrawItem(nOldSelItem);

        RedrawItem(nIndex);
    }
    return TRUE;
}

int CDuiListBox::GetTopIndex() const
{
    return m_ptOrigin.y / m_nItemHei;
}

BOOL CDuiListBox::SetTopIndex(int nIndex)
{
    if (nIndex < 0 || nIndex >= GetCount())
        return FALSE;

    OnScroll(TRUE,SB_THUMBPOSITION, nIndex*m_nItemHei);
    return TRUE;
}

LPARAM CDuiListBox::GetItemData(int nIndex) const
{
    if (nIndex < 0 || nIndex >= GetCount())
        return 0;

    return m_arrItems[nIndex]->lParam;
}

BOOL CDuiListBox::SetItemData(int nIndex, LPARAM lParam)
{
    if (nIndex < 0 || nIndex >= GetCount())
        return FALSE;

    m_arrItems[nIndex]->lParam = lParam;
    return TRUE;
}

int CDuiListBox::GetText(int nIndex, LPTSTR lpszBuffer) const
{
    int nRet = GetTextLen(nIndex);

    if(nRet != LB_ERR)
        _tcscpy(lpszBuffer, m_arrItems[nIndex]->strText);

    return nRet;
}

int CDuiListBox::GetText(int nIndex, CDuiStringT& strText) const
{
    int nRet = GetTextLen(nIndex);

    if(nRet != LB_ERR)
        strText = m_arrItems[nIndex]->strText;

    return nRet;
}

int CDuiListBox::GetTextLen(int nIndex) const
{
    if (nIndex < 0 || nIndex >= GetCount())
        return LB_ERR;

    return m_arrItems[nIndex]->strText.GetLength();
}

int CDuiListBox::GetItemHeight(int nIndex) const
{
    return m_nItemHei;
}

BOOL CDuiListBox::SetItemHeight(int nIndex, int cyItemHeight)
{
    if (cyItemHeight < 0 || nIndex < 0 || nIndex >= GetCount())
        return FALSE;

    m_nItemHei = cyItemHeight;
    return TRUE;
}

void CDuiListBox::DeleteAll()
{
    for(int i=0; i < GetCount(); i++)
    {
        if (m_arrItems[i])
            delete m_arrItems[i];
    }
    m_arrItems.RemoveAll();

    m_iSelItem=-1;
    m_iHoverItem=-1;

    SetViewSize(CSize(0,0));
    NotifyInvalidate();
}

BOOL CDuiListBox::DeleteString(int nIndex)
{
    if(nIndex<0 || nIndex>=GetCount()) return FALSE;


    if (m_arrItems[nIndex])
        delete m_arrItems[nIndex];
    m_arrItems.RemoveAt(nIndex);

    if(m_iSelItem==nIndex) m_iSelItem=-1;
    else if(m_iSelItem>nIndex) m_iSelItem--;
    if(m_iHoverItem==nIndex) m_iHoverItem=-1;
    else if(m_iHoverItem>nIndex) m_iHoverItem--;

    CRect rcClient;
    CDuiWindow::GetClient(&rcClient);
    CSize szView(rcClient.Width(),GetCount()*m_nItemHei);
    if(szView.cy>rcClient.Height()) szView.cx-=m_nSbWid;
    SetViewSize(szView);

    return TRUE;
}

int CDuiListBox::AddString(LPCTSTR lpszItem, int nImage, LPARAM lParam)
{
    return InsertString(-1, lpszItem, nImage,  lParam);
}

int CDuiListBox::InsertString(int nIndex, LPCTSTR lpszItem, int nImage,  LPARAM lParam)
{
    DUIASSERT(lpszItem);

    LPLBITEM pItem = new LBITEM;
    pItem->strText = lpszItem;
    pItem->nImage = nImage;
    pItem->lParam = lParam;

    return InsertItem(nIndex, pItem);
}

void CDuiListBox::EnsureVisible(int nIndex)
{
    if(nIndex < 0 || nIndex >= GetCount()) return;

    CRect rcClient;
    GetClient(&rcClient);

    int iFirstVisible = (m_ptOrigin.y + m_nItemHei -1) / m_nItemHei;
    int nVisibleItems = rcClient.Height() / m_nItemHei;
    if(nIndex < iFirstVisible || nIndex > iFirstVisible+nVisibleItems-1)
    {
        int nOffset = GetScrollPos(TRUE);
        if(nIndex < iFirstVisible) nOffset = (nIndex-iFirstVisible)*m_nItemHei;
        else nOffset=(nIndex - iFirstVisible-nVisibleItems +1)*m_nItemHei;
        nOffset-=nOffset%m_nItemHei;//�õ�ǰ�иպ���ʾ
        OnScroll(TRUE,SB_THUMBPOSITION,nOffset + GetScrollPos(TRUE));
    }
}

//�Զ��޸�pt��λ��Ϊ��Ե�ǰ���ƫ����
int CDuiListBox::HitTest(CPoint &pt)
{
    CRect rcClient;
    GetClient(&rcClient);
    CPoint pt2=pt;
    pt2.y -= rcClient.top - m_ptOrigin.y;
    int nRet=pt2.y/m_nItemHei;
    if(nRet >= GetCount()) nRet=-1;
    else
    {
        pt.x-=rcClient.left;
        pt.y=pt2.y%m_nItemHei;
    }

    return nRet;
}

BOOL CDuiListBox::Load(pugi::xml_node xmlNode)
{
    if (!__super::Load(xmlNode))
        return FALSE;

    int	nChildSrc= xmlNode.attribute("itemsrc").as_int(-1);

    if (nChildSrc == -1)
        return TRUE;

	pugi::xml_document xmlDoc;
	if(!DuiSystem::getSingleton().LoadXmlDocment(xmlDoc,nChildSrc,DUIRES_XML_TYPE)) return FALSE;

    return LoadChildren(xmlDoc.first_child());
}

BOOL CDuiListBox::LoadChildren(pugi::xml_node xmlNode)
{
    if(!xmlNode) return TRUE;

	pugi::xml_node xmlParent=xmlNode.parent();
	pugi::xml_node xmlItem=xmlParent.child("items");
    while(xmlItem)
    {
        LPLBITEM pItemObj = new LBITEM;
        LoadItemAttribute(xmlItem, pItemObj);
        InsertItem(-1, pItemObj);
        xmlItem = xmlItem.next_sibling();
    }

    int nSelItem=xmlParent.attribute("cursel").as_int(-1);
	SetCurSel(nSelItem);

    return TRUE;
}

void CDuiListBox::LoadItemAttribute(pugi::xml_node xmlNode, LPLBITEM pItem)
{
	pItem->nImage=xmlNode.attribute("img").as_int(pItem->nImage);
	pItem->lParam=xmlNode.attribute("data").as_uint(pItem->lParam);

    pItem->strText =  DUI_CA2T(xmlNode.text().get(), CP_UTF8);
    DuiStringPool::getSingleton().BuildString(pItem->strText);
}

int CDuiListBox::InsertItem(int nIndex, LPLBITEM pItem)
{
    DUIASSERT(pItem);

    if(nIndex==-1 || nIndex>=GetCount())
    {
        nIndex = GetCount();
    }

    m_arrItems.InsertAt(nIndex, pItem);

    if(m_iSelItem >= nIndex) m_iSelItem++;
    if(m_iHoverItem >= nIndex) m_iHoverItem++;

    CRect rcClient;
    CDuiWindow::GetClient(&rcClient);
    CSize szView(rcClient.Width(),GetCount()*m_nItemHei);
    if(szView.cy>rcClient.Height()) szView.cx-=m_nSbWid;
    SetViewSize(szView);

    return nIndex;
}

int CDuiListBox::GetScrollLineSize(BOOL bVertical)
{
    return m_iScrollSpeed >0 ? m_iScrollSpeed : m_nItemHei;
}

void CDuiListBox::RedrawItem(int iItem)
{
    if(!IsVisible(TRUE)) return;

    CRect rcClient;
    GetClient(&rcClient);
    int iFirstVisible = GetTopIndex();
    int nPageItems=(rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;

    if(iItem>=iFirstVisible && iItem<GetCount() && iItem<iFirstVisible+nPageItems)
    {
        CRect rcItem(0,0,rcClient.Width(),m_nItemHei);
        rcItem.OffsetRect(0,m_nItemHei*iItem-m_ptOrigin.y);
        rcItem.OffsetRect(rcClient.TopLeft());
        CDCHandle dc=GetDuiDC(&rcItem,OLEDC_PAINTBKGND);
        DuiDCPaint duiDC;
        BeforePaint(dc,duiDC);

        DuiSendMessage(WM_ERASEBKGND,(WPARAM)(HDC)dc);
        DrawItem(dc,rcItem,iItem);

        AfterPaint(dc,duiDC);
        ReleaseDuiDC(dc);
    }
}

void CDuiListBox::DrawItem(CDCHandle & dc, CRect & rc, int iItem)
{
    if (iItem < 0 || iItem >= GetCount()) return;

    BOOL bTextColorChanged = FALSE;
    int nBgImg = 0;
    COLORREF crOldText;
    COLORREF crItemBg = m_crItemBg;
    COLORREF crText = m_crText;
    LPLBITEM pItem = m_arrItems[iItem];
    CRect rcIcon, rcText;

    if (iItem % 2)
    {
        if (m_pItemSkin != NULL)
            nBgImg = 1;
        else if (CLR_INVALID != m_crItemBg2)
            crItemBg = m_crItemBg2;
    }

    if (iItem == m_iSelItem)
    {
        if (m_pItemSkin != NULL)
            nBgImg = 2;
        else if (CLR_INVALID != m_crItemSelBg)
            crItemBg = m_crItemSelBg;

        if (CLR_INVALID != m_crSelText)
            crText = m_crSelText;
    }

    //���Ʊ���
    if (m_pItemSkin != NULL)
        m_pItemSkin->Draw(dc, rc, nBgImg);
    else if (CLR_INVALID != crItemBg)
        CGdiAlpha::FillSolidRect(dc, rc, crItemBg);

    if (CLR_INVALID != crText)
    {
        bTextColorChanged = TRUE;
        crOldText = dc.SetTextColor(crText);
    }

    if (pItem->nImage != -1 && m_pIconSkin)
    {
        int nOffsetX =m_ptIcon.x, nOffsetY = m_ptIcon.y;
        CSize sizeSkin = m_pIconSkin->GetSkinSize();
        rcIcon.SetRect(0, 0, sizeSkin.cx, sizeSkin.cy);

        if (m_ptIcon.x == -1)
            nOffsetX =  m_nItemHei / 6;

        if (m_ptIcon.y == -1)
            nOffsetY = (m_nItemHei - sizeSkin.cy) / 2;	//y Ĭ�Ͼ���

        rcIcon.OffsetRect(rc.left + nOffsetX, rc.top + nOffsetY);
        m_pIconSkin->Draw(dc, rcIcon, pItem->nImage);
    }

    UINT align = DT_SINGLELINE;
    rcText = rc;

    if (m_ptText.x == -1)
        rcText.left = rcIcon.Width() > 0 ? rcIcon.right + m_nItemHei / 6 : rc.left;
    else
        rcText.left = rc.left + m_ptText.x;

    if (m_ptText.y == -1)
        align |= DT_VCENTER;
    else
        rcText.top = rc.top + m_ptText.y;

    CGdiAlpha::DrawText(dc, pItem->strText,-1,rcText,align);

    if (bTextColorChanged)
        dc.SetTextColor(crOldText);
}


void CDuiListBox::NotifySelChange( int nOldSel,int nNewSel ,UINT uMsg)
{
    DUINMLBSELCHANGE nms;
    nms.hdr.hwndFrom=NULL;
    nms.hdr.idFrom=GetCmdID();
    nms.nOldSel=nOldSel;
    nms.nNewSel=nNewSel;
    nms.uMsg=uMsg;
    nms.uHoverID=0;

    nms.hdr.code=DUINM_LBSELCHANGING;
    if(S_OK!=DuiNotify((LPNMHDR)&nms)) return ;

    m_iSelItem=nNewSel;
    if(nOldSel!=-1)
        RedrawItem(nOldSel);

    if(m_iSelItem!=-1)
        RedrawItem(m_iSelItem);

    nms.hdr.idFrom=GetCmdID();
    nms.hdr.code=DUINM_LBSELCHANGED;
    DuiNotify((LPNMHDR)&nms);
}

void CDuiListBox::OnPaint(CDCHandle dc)
{
    DuiDCPaint duiDC;
    BeforePaint(dc,duiDC);

    int iFirstVisible = GetTopIndex();
    int nPageItems = (m_rcClient.Height()+m_nItemHei-1)/m_nItemHei+1;

    for(int iItem = iFirstVisible; iItem<GetCount() && iItem <iFirstVisible+nPageItems; iItem++)
    {
        CRect rcItem(0,0,m_rcClient.Width(),m_nItemHei);
        rcItem.OffsetRect(0,m_nItemHei*iItem-m_ptOrigin.y);
        rcItem.OffsetRect(m_rcClient.TopLeft());
        DrawItem(dc,rcItem,iItem);
    }

    AfterPaint(dc,duiDC);
}

void CDuiListBox::OnSize(UINT nType,CSize size)
{
    CRect rcClient;
    CDuiWindow::GetClient(&rcClient);
    CSize szView(rcClient.Width(),GetCount()*m_nItemHei);
    if(szView.cy>rcClient.Height()) szView.cx-=m_nSbWid;
    SetViewSize(szView);
}

void CDuiListBox::OnLButtonDown(UINT nFlags,CPoint pt)
{
    m_iHoverItem = HitTest(pt);
	if(m_iHoverItem!=m_iSelItem && !m_bHotTrack)
		NotifySelChange(m_iSelItem,m_iHoverItem,WM_LBUTTONDOWN);
}

void CDuiListBox::OnLButtonUp(UINT nFlags,CPoint pt)
{
    m_iHoverItem = HitTest(pt);

    if(m_bHotTrack || m_iHoverItem!=m_iSelItem)
        NotifySelChange(m_iSelItem,m_iHoverItem,WM_LBUTTONUP);
}

void CDuiListBox::OnLButtonDbClick(UINT nFlags,CPoint pt)
{
}

void CDuiListBox::OnMouseMove(UINT nFlags,CPoint pt)
{
    m_iHoverItem = HitTest(pt);

    if(m_bHotTrack && m_iHoverItem!=m_iSelItem)
        NotifySelChange(m_iSelItem,m_iHoverItem,WM_MOUSEMOVE);
}

void CDuiListBox::OnMouseLeave()
{
    if(m_iHoverItem!=-1)
        m_iHoverItem = -1;
}

void CDuiListBox::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
{
    int  nNewSelItem = -1;
    CDuiWindow *pOwner = GetOwner();
    if (pOwner && (nChar == VK_ESCAPE))
    {
        pOwner->DuiSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
        return;
    }

    if (nChar == VK_DOWN && m_iSelItem < GetCount() - 1)
        nNewSelItem = m_iSelItem+1;
    else if (nChar == VK_UP && m_iSelItem > 0)
        nNewSelItem = m_iSelItem-1;
    else if (pOwner && nChar == VK_RETURN)
        nNewSelItem = m_iSelItem;

    if(nNewSelItem!=-1)
    {
        EnsureVisible(nNewSelItem);
        NotifySelChange(m_iSelItem,nNewSelItem,
                        m_bHotTrack&&nChar!=VK_RETURN ? WM_MOUSEMOVE : WM_LBUTTONUP);
    }
}

void CDuiListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDuiWindow *pOwner = GetOwner();
    if (pOwner)
        pOwner->DuiSendMessage(WM_CHAR, nChar, MAKELONG(nFlags, nRepCnt));
}

UINT CDuiListBox::OnGetDuiCode()
{
    return DUIC_WANTALLKEYS;
}

void CDuiListBox::OnDestroy()
{
    DeleteAll();
    __super::OnDestroy();
}


}//namespace DuiEngine