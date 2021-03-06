#include "duistd.h"
#include "DuiComboBox.h"


namespace DuiEngine
{

#define DEF_ITEMHEI	20

// CComboEdit
CComboEdit::CComboEdit( CDuiComboBox *pOwner )
{
	SetOwner(pOwner);
}

void CComboEdit::OnMouseHover( WPARAM wParam, CPoint ptPos )
{
	__super::OnMouseHover(wParam,ptPos);
	GetOwner()->DuiSendMessage(WM_MOUSEHOVER,wParam,MAKELPARAM(ptPos.x,ptPos.y));
}

void CComboEdit::OnMouseLeave()
{
	__super::OnMouseLeave();
	GetOwner()->DuiSendMessage(WM_MOUSELEAVE);
}

void CComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDuiWindow *pOwner = GetOwner();
	if (pOwner && (nChar == VK_DOWN || nChar == VK_ESCAPE))
	{
		pOwner->DuiSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
		return;
	}

	SetMsgHandled(FALSE);
}

LRESULT CComboEdit::DuiNotify( LPNMHDR pnms )
{
	//转发richedit的txNotify消息
	if(pnms->code==DUINM_RICHEDIT_NOTIFY)
	{
		pnms->idFrom=GetOwner()->GetCmdID();
	}
	return __super::DuiNotify(pnms);
}

//////////////////////////////////////////////////////////////////////////
// CDuiComboBox
CDuiComboBox::CDuiComboBox(void)
:m_pSkinBtn(DuiSkinPool::getSingleton().GetSkin("comboboxbtn"))
,m_pEdit(NULL)
,m_iCurSel(-1)
,m_bDropdown(TRUE)
,m_nDropHeight(200)
,m_dwBtnState(DuiWndState_Normal)
,m_pListBox(NULL)
,m_bItemPanel(FALSE)
,m_iAnimTime(200)
{
	m_bTabStop=TRUE;
	m_style.SetAttribute("align","left",TRUE);
	m_style.SetAttribute("valign","middle",TRUE);

	addEvent(DUINM_LBSELCHANGED);
	addEvent(DUINM_LBSELCHANGING);
	addEvent(DUINM_RICHEDIT_NOTIFY);
}

CDuiComboBox::~CDuiComboBox(void)
{
}


BOOL CDuiComboBox::LoadChildren( pugi::xml_node xmlNode )
{
	BOOL bRet=FALSE;
	pugi::xml_node xmlParent = xmlNode.parent();

	DUIASSERT(xmlParent);
	//初始化列表数据
	pugi::xml_node xmlNode_Items=xmlParent.child("items");
	if(xmlNode_Items)
	{
		pugi::xml_node xmlNode_Item=xmlNode_Items.child("item");
		while(xmlNode_Item)
		{
			CBITEM cbi={_T(""),0,0};
			cbi.strText=DUI_CA2T(xmlNode_Item.attribute("text").value(),CP_UTF8);
			cbi.iIcon=xmlNode_Item.attribute("icon").as_int(0);
			cbi.lParam=xmlNode_Item.attribute("data").as_int(0);
            m_arrCbItem.Add(cbi);
			xmlNode_Item=xmlNode_Item.next_sibling("item");
		}
		bRet=TRUE;
	}

	//获取列表模板
	pugi::xml_node xmlNode_lstStyle=xmlParent.child("liststyle");
	if(xmlNode_lstStyle)
	{
		m_xmlListStyle.append_copy(xmlNode_lstStyle);
		m_xmlListStyle.attribute("virtual").set_value(0);
	}

	DUIASSERT(m_pSkinBtn);
	//创建edit对象
	if(!m_bDropdown)
	{
		SIZE szBtn=m_pSkinBtn->GetSkinSize();
		m_pEdit=new CComboEdit(this);
		InsertChild(m_pEdit);
		pugi::xml_node xmlEditStyle=xmlParent.child("editstyle");
		if(xmlEditStyle)
			m_pEdit->Load(xmlEditStyle);
		else
			m_pEdit->DuiSendMessage(WM_CREATE);
		CDuiStringA strPos;
		strPos.Format("0,0,-%d,-0",szBtn.cx);
		m_pEdit->SetAttribute("pos",strPos,TRUE);
		m_pEdit->SetCmdID(IDC_CB_EDIT);
	}
    if(m_iCurSel!=-1 && m_iCurSel>=m_arrCbItem.GetCount())
		m_iCurSel=-1;
	int iSel=m_iCurSel;
	m_iCurSel=-1;
	SetCurSel(iSel);
	return bRet;
}


void CDuiComboBox::GetDropBtnRect(LPRECT prc)
{
	SIZE szBtn=m_pSkinBtn->GetSkinSize();
	GetClient(prc);
	prc->left=prc->right-szBtn.cx;
}


void CDuiComboBox::DropDown()
{
	if(m_dwBtnState==DuiWndState_PushDown) return;
	m_dwBtnState=DuiWndState_PushDown;
	CRect rcBtn;
	GetDropBtnRect(&rcBtn);
	NotifyInvalidateRect(rcBtn);
	//todo:show list window
	CRect rc;
	GetRect(&rc);
	CSimpleWnd wnd(GetContainer()->GetHostHwnd());
	wnd.ClientToScreen(&rc);

	if (!m_bItemPanel)
		m_pListBox=new CDuiDropDownList(this,m_nDropHeight);
	else
		m_pListBox=new CDuiDropDownListEx(this,m_nDropHeight);

	m_pListBox->Create(GetContainer()->GetHostHwnd(),NULL,WS_POPUP,WS_EX_TOOLWINDOW,0,0,0,0,&m_xmlListStyle);
 	m_pListBox->UpdateItems(&rc);
}

void CDuiComboBox::CloseUp()
{
	m_pListBox->PostMessage(WM_CLOSE);
	m_pListBox = NULL;
}


void CDuiComboBox::GetTextRect( LPRECT pRect )
{
	GetClient(pRect);
	SIZE szBtn=m_pSkinBtn->GetSkinSize();
	pRect->right-=szBtn.cx;
}

void CDuiComboBox::OnPaint( CDCHandle dc )
{
	if(m_iCurSel != -1 && m_pEdit==NULL)
	{
		__super::OnPaint(dc);
	}
	CRect rcBtn;
	GetDropBtnRect(&rcBtn);
	m_pSkinBtn->Draw(dc,rcBtn,IIF_STATE3(m_dwBtnState,DuiWndState_Normal,DuiWndState_Hover,DuiWndState_PushDown));
}

void CDuiComboBox::OnLButtonDown( UINT nFlags,CPoint pt )
{
	DropDown();
}

void CDuiComboBox::OnLButtonUp(UINT nFlags,CPoint pt)
{
}

void CDuiComboBox::OnLButtonDbClick(UINT nFlags,CPoint pt)
{	
	DuiSendMessage( WM_LBUTTONDOWN, 0, MAKELPARAM( pt.x, pt.y ) );
}

void CDuiComboBox::OnMouseMove( UINT nFlags,CPoint pt )
{
	if(m_dwBtnState==DuiWndState_PushDown) return;

	__super::OnMouseHover(nFlags,pt);
	CRect rcBtn;
	GetDropBtnRect(&rcBtn);
	if(rcBtn.PtInRect(pt))
	{
		m_dwBtnState=DuiWndState_Hover;
		NotifyInvalidateRect(rcBtn);
	}else if(m_dwBtnState==DuiWndState_Hover)
	{
		m_dwBtnState=DuiWndState_Normal;
		NotifyInvalidateRect(rcBtn);
	}
}

void CDuiComboBox::OnMouseLeave()
{
	if(m_dwBtnState==DuiWndState_PushDown) return;

	if(GetState()&DuiWndState_Hover) 
		__super::OnMouseLeave();
	if(m_dwBtnState==DuiWndState_Hover)
	{
		m_dwBtnState=DuiWndState_Normal;
		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
		NotifyInvalidateRect(rcBtn);
	}
}

void CDuiComboBox::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
{	
	if ( nChar == VK_DOWN)
		DropDown();
	else if ( nChar == VK_ESCAPE)
		CloseUp();
}

void CDuiComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_bDropdown)
	{
		CComboEdit *pEdit = static_cast<CComboEdit *>(FindChildByCmdID(IDC_CB_EDIT));
		if (pEdit)
			pEdit->DuiSendMessage(WM_CHAR, nChar, MAKELONG(nFlags, nRepCnt));
		return;
	}
}

UINT CDuiComboBox::OnGetDuiCode()
{		
	return DUIC_WANTALLKEYS;
}

BOOL CDuiComboBox::IsTabStop()
{
	if (m_bDropdown && m_bTabStop)
		return TRUE;
	return FALSE;
}

LRESULT CDuiComboBox::DuiNotify(LPNMHDR pnms)
{
	LRESULT lRet=0;
	if(pnms->idFrom == IDC_DROPDOWN_LIST)
	{
		pnms->idFrom=GetCmdID();
		if(pnms->code==DUINM_LBSELCHANGED && m_pListBox)
		{//select item changed
			DUINMLBSELCHANGE *pnmclk = (DUINMLBSELCHANGE *)pnms;
			if(pnmclk->uMsg==WM_LBUTTONDOWN || pnmclk->uMsg==WM_LBUTTONUP)
			{
				CloseUp();
				SetCurSel(pnmclk->nNewSel);
				if (!m_bDropdown && m_pEdit)
				{
					m_pEdit->SetDuiFocus();
					m_pEdit->SetSel(MAKELONG(0,-1));
				}
				lRet=__super::DuiNotify(pnms);
			}
		}else
		{
			lRet=__super::DuiNotify(pnms);
		}
		pnms->idFrom=IDC_DROPDOWN_LIST;
	}else
	{
		lRet=__super::DuiNotify(pnms);
	}
	return lRet;
}

int CDuiComboBox::SetCurSel( int iSel )
{
	if(iSel!=m_iCurSel)
	{
		if(iSel==-1)
		{
			m_iCurSel=-1;
			if(m_pEdit)
			{
				m_pEdit->setMutedState(true);
				m_pEdit->SetWindowText(NULL);
				m_pEdit->setMutedState(false);
			}
			else
				SetInnerText(NULL);
		}else
		{
            if(iSel>=0 && iSel<m_arrCbItem.GetCount())
			{
				m_iCurSel=iSel;
				if(m_pEdit)
				{
					m_pEdit->setMutedState(true);
					m_pEdit->SetWindowText(m_arrCbItem[iSel].strText);
					m_pEdit->setMutedState(false);
				}
				else
					SetInnerText(m_arrCbItem[iSel].strText);
			}else
			{
				return CB_ERR;
			}
		}

		NotifyInvalidate();
	}
	return m_iCurSel;
}

int CDuiComboBox::GetCurSel()
{
	return m_iCurSel;
}

BOOL CDuiComboBox::GetItemInfo( int iItem,CBITEM *pCbItem )
{
    if(iItem<0 || iItem>=m_arrCbItem.GetCount()) return FALSE;
	*pCbItem=m_arrCbItem[iItem];
	return TRUE;
}

LPARAM CDuiComboBox::GetItemData(int iItem) const
{
    if(iItem<0 || iItem>=m_arrCbItem.GetCount()) return FALSE;
	return m_arrCbItem[iItem].lParam;
}

int CDuiComboBox::SetItemData(int iItem, LPARAM lParam)
{
    if(iItem<0 || iItem>=m_arrCbItem.GetCount()) return CB_ERR;
	m_arrCbItem[iItem].lParam = lParam;
	return 0;
}

int CDuiComboBox::InsertItem(int iPos, LPCTSTR pszText,int iIcon,LPARAM lParam )
{
	CBITEM cbi={pszText,iIcon,lParam};
    if(iPos<0 || iPos>m_arrCbItem.GetCount()) iPos=m_arrCbItem.GetCount();
    m_arrCbItem.InsertAt(iPos,cbi);
	if(m_pListBox)
	{//update list box
		m_pListBox->UpdateItems();
	}
	return iPos;
}

BOOL CDuiComboBox::DeleteItem( int iItem )
{
    if(iItem<0 || iItem>=m_arrCbItem.GetCount()) return FALSE;
	m_arrCbItem.RemoveAt(iItem);
	if(m_pListBox)
	{//update list box
		m_pListBox->DeleteItem(iItem);
	}
	return TRUE;
}

BOOL CDuiComboBox::DeleteAllItem()
{
	m_iCurSel=-1;
	if(m_pEdit)
		m_pEdit->SetWindowText(NULL);
	else
		SetInnerText(NULL);
	m_arrCbItem.RemoveAll();
	if(m_pListBox)
		CloseUp();
		
	NotifyInvalidate();
	return TRUE;	
}

int CDuiComboBox::GetWindowText( LPTSTR lpString, int nMaxCount )
{
	if(m_bDropdown)
	{
		if(m_iCurSel==-1) return 0;
		else
		{
			int nRet=m_arrCbItem[m_iCurSel].strText.GetLength();
			if(nRet>nMaxCount-1) nRet=nMaxCount-1;
			_tcsncpy(lpString,m_arrCbItem[m_iCurSel].strText,nRet);
			lpString[nRet]=0;
			return nRet;
		}
	}else
	{
		DUIASSERT(m_pEdit);
		return m_pEdit->GetWindowText(lpString,nMaxCount);
	}
}

BOOL CDuiComboBox::SetWindowText(LPCTSTR strText)
{
	if(m_bDropdown) return FALSE;
	return m_pEdit->SetWindowText(strText);
}

HWND CDuiComboBox::GetHostHwnd()
{
	return GetContainer()->GetHostHwnd();
}

CDuiWindow* CDuiComboBox::GetWindow()
{
	return this;
}

int  CDuiComboBox::GetListItemCount()
{
    return m_arrCbItem.GetCount();
}

LPCTSTR	CDuiComboBox::GetListItemText(int nItem)
{
    if (nItem < 0 || nItem >= m_arrCbItem.GetCount())
        return NULL;

    return m_arrCbItem[nItem].strText;
}

int CDuiComboBox::GetListItemIcon(int nItem)
{
    if (nItem < 0 || nItem >= m_arrCbItem.GetCount())
        return -1;

	return m_arrCbItem[nItem].iIcon;
}

int CDuiComboBox::GetListCurSel()
{
	return m_iCurSel;
}

int CDuiComboBox::GetAnimateTime()
{
	return m_iAnimTime;
}

void CDuiComboBox::OnDropDownListClose()
{
	m_dwBtnState=DuiWndState_Normal;
	HDUIWND hHover=GetContainer()->GetDuiHover();
	if(GetDuiHwnd()!=hHover && (!m_pEdit || hHover!=m_pEdit->GetDuiHwnd()))
	{
		ModifyState(0,DuiWndState_Hover);
	}
	NotifyInvalidate();
}

}//namespace DuiEngine

