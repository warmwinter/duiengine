#include "duistd.h"
#include "DuiHeaderCtrl.h"
#include "MemDC.h"
#pragma comment(lib,"comctl32.lib")

namespace DuiEngine
{
#define CX_HDITEM_MARGIN	4

	CDuiHeaderCtrl::CDuiHeaderCtrl(void)
		:m_bFixWidth(FALSE)
		,m_bItemSwapEnable(TRUE)
		,m_bSortHeader(TRUE)
		,m_pSkinItem(NULL)
		,m_pSkinSort(NULL)
		,m_dwHitTest(-1)
		,m_bDragging(FALSE)
		,m_hDragImglst(NULL)
	{
		m_bClipClient=TRUE;
		addEvent(DUINM_HDCLICK);
		addEvent(DUINM_HDSIZECHANGING);
		addEvent(DUINM_HDSIZECHANGED);
		addEvent(DUINM_HDSWAP);
	}

	CDuiHeaderCtrl::~CDuiHeaderCtrl(void)
	{
	}

	int CDuiHeaderCtrl::InsertItem( int iItem,LPCTSTR pszText,int nWidth, DUIHDSORTFLAG stFlag,LPARAM lParam )
	{
		DUIASSERT(pszText);
		DUIASSERT(nWidth>=0);
		if(iItem==-1) iItem=m_arrItems.GetCount();
		DUIHDITEM item;
		item.mask=0xFFFFFFFF;
		item.cx=nWidth;
		item.pszText=_tcsdup(pszText);
		item.cchTextMax=_tcslen(pszText);
		item.stFlag=stFlag;
		item.state=0;
		item.iOrder=iItem;
		item.lParam=lParam;
		m_arrItems.InsertAt(iItem,item);
		//��Ҫ�����е����
		for(int i=0;i<GetItemCount();i++)
		{
			if(i==iItem) continue;
			if(m_arrItems[i].iOrder>=iItem)
				m_arrItems[i].iOrder++;
		}
		NotifyInvalidate();
		return iItem;
	}

	BOOL CDuiHeaderCtrl::GetItem( int iItem,DUIHDITEM *pItem )
	{
		if(iItem>=m_arrItems.GetCount()) return FALSE;
		if(pItem->mask & DUIHDI_TEXT)
		{
			 if(pItem->cchTextMax<m_arrItems[iItem].cchTextMax) return FALSE;
			 _tcscpy(pItem->pszText,m_arrItems[iItem].pszText);
		}
		if(pItem->mask & DUIHDI_WIDTH) pItem->cx=m_arrItems[iItem].cx;
		if(pItem->mask & DUIHDI_LPARAM) pItem->lParam=m_arrItems[iItem].lParam;
		if(pItem->mask & DUIHDI_SORTFLAG) pItem->stFlag=m_arrItems[iItem].stFlag;
		if(pItem->mask & DUIHDI_ORDER) pItem->iOrder=m_arrItems[iItem].iOrder;
		return TRUE;
	}

	void CDuiHeaderCtrl::OnPaint( CDCHandle dc )
	{
		DuiDCPaint duiDC;
		BeforePaint(dc,duiDC);
		CRect rcClient;
		GetClient(&rcClient);
		CRect rcItem(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
		for(int i=0;i<m_arrItems.GetCount();i++)
		{
			rcItem.left=rcItem.right;
			rcItem.right=rcItem.left+m_arrItems[i].cx;
			DrawItem(dc,rcItem,m_arrItems.GetData()+i);
			if(rcItem.right>=rcClient.right) break;
		}
		if(rcItem.right<rcClient.right)
		{
			rcItem.left=rcItem.right;
			rcItem.right=rcClient.right;
			m_pSkinItem->Draw(dc,rcItem,0);
		}
		AfterPaint(dc,duiDC);
	}

	void CDuiHeaderCtrl::DrawItem( CDCHandle dc,CRect rcItem,const LPDUIHDITEM pItem )
	{
		if(m_pSkinItem) m_pSkinItem->Draw(dc,rcItem,pItem->state);
		CGdiAlpha::DrawText(dc,pItem->pszText,pItem->cchTextMax,rcItem,m_style.GetTextAlign());
		if(pItem->stFlag==ST_NULL || !m_pSkinSort) return;
		CSize szSort=m_pSkinSort->GetSkinSize();
		CPoint ptSort;
		ptSort.y=rcItem.top+(rcItem.Height()-szSort.cy)/2;

		if(m_style.GetTextAlign()&DT_RIGHT)
			ptSort.x=rcItem.left+2;
		else
			ptSort.x=rcItem.right-szSort.cx-2;

		m_pSkinSort->Draw(dc,CRect(ptSort,szSort),pItem->stFlag==ST_UP?0:1);
	}

	BOOL CDuiHeaderCtrl::DeleteItem( int iItem )
	{
		if(iItem<0 || iItem>=m_arrItems.GetCount()) return FALSE;

		int iOrder=m_arrItems[iItem].iOrder;
		if(m_arrItems[iItem].pszText) free(m_arrItems[iItem].pszText);
		m_arrItems.RemoveAt(iItem);
		//��������
		for(int i=0;i<m_arrItems.GetCount();i++)
		{
			if(m_arrItems[i].iOrder>iOrder)
				m_arrItems[i].iOrder--;
		}
		NotifyInvalidate();
		return TRUE;
	}

	void CDuiHeaderCtrl::DeleteAllItems()
	{
		for(int i=0;i<m_arrItems.GetCount();i++)
		{
			if(m_arrItems[i].pszText) free(m_arrItems[i].pszText);
		}
		m_arrItems.RemoveAll();
		NotifyInvalidate();
	}

	void CDuiHeaderCtrl::OnDestroy()
	{
		DeleteAllItems();
		__super::OnDestroy();
	}

	CRect CDuiHeaderCtrl::GetItemRect( int iItem )
	{
		CRect	rcClient;
		GetClient(&rcClient);
		CRect rcItem(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
		for(int i=0;i<=iItem && i<m_arrItems.GetCount();i++)
		{
			rcItem.left=rcItem.right;
			rcItem.right=rcItem.left+m_arrItems[i].cx;
		}
		return rcItem;
	}

	void CDuiHeaderCtrl::RedrawItem( int iItem )
	{
		CRect rcItem=GetItemRect(iItem);
		CDCHandle dc=GetDuiDC(rcItem,OLEDC_PAINTBKGND);
		DuiDCPaint duiDC;
		BeforePaint(dc,duiDC);
		DrawItem(dc,rcItem,m_arrItems.GetData()+iItem);
		AfterPaint(dc,duiDC);
		ReleaseDuiDC(dc);
	}

	void CDuiHeaderCtrl::OnLButtonDown( UINT nFlags,CPoint pt )
	{
		SetDuiCapture();
		m_ptClick=pt;
		m_dwHitTest=HitTest(pt);
		if(IsItemHover(m_dwHitTest))
		{
			if(m_bSortHeader)
			{
				m_arrItems[LOWORD(m_dwHitTest)].state=2;//pushdown
				RedrawItem(LOWORD(m_dwHitTest));
			}
		}else if(m_dwHitTest!=-1)
		{
			m_nAdjItemOldWidth=m_arrItems[LOWORD(m_dwHitTest)].cx;
		}
	}

	void CDuiHeaderCtrl::OnLButtonUp( UINT nFlags,CPoint pt )
	{
		if(IsItemHover(m_dwHitTest))
		{
			if(m_bDragging)
			{//�϶���ͷ��
				if(m_bItemSwapEnable)
				{
					ImageList_DragLeave(NULL);
					ImageList_EndDrag();
// 					ShowCursor(TRUE);

					ImageList_Destroy(m_hDragImglst);
					m_hDragImglst=NULL;
					if(m_dwDragTo!=m_dwHitTest && IsItemHover(m_dwDragTo))
					{
						DUIHDITEM t=m_arrItems[LOWORD(m_dwHitTest)];
						m_arrItems.RemoveAt(LOWORD(m_dwHitTest));
						int nPos=LOWORD(m_dwDragTo);
						if(nPos>LOWORD(m_dwHitTest)) nPos--;//Ҫ���ǽ��Լ��Ƴ���Ӱ��
						m_arrItems.InsertAt(LOWORD(m_dwDragTo),t);
						//����Ϣ֪ͨ��������λ�÷����仯
						DUINMHDSWAP	nm;
						nm.hdr.code=DUINM_HDSWAP;
						nm.hdr.idFrom=GetCmdID();
						nm.hdr.hwndFrom=0;
						nm.pSender=this;
						nm.iOldIndex=LOWORD(m_dwHitTest);
						nm.iNewIndex=nPos;
						DuiNotify((LPNMHDR)&nm);
					}
					m_dwHitTest=HitTest(pt);
					m_dwDragTo=-1;
					NotifyInvalidate();
				}
			}else
			{//�����ͷ��
				if(m_bSortHeader)
				{
					m_arrItems[LOWORD(m_dwHitTest)].state=1;//hover
					RedrawItem(LOWORD(m_dwHitTest));
					DUINMHDCLICK	nm;
					nm.hdr.code=DUINM_HDCLICK;
					nm.hdr.idFrom=GetCmdID();
					nm.hdr.hwndFrom=0;
					nm.pSender=this;
					nm.iItem=LOWORD(m_dwHitTest);
					DuiNotify((LPNMHDR)&nm);
				}
			}
		}else if(m_dwHitTest!=-1)
		{//������ͷ��ȣ�����һ�����������Ϣ
			DUINMHDSIZECHANGED	nm;
			nm.hdr.code=DUINM_HDSIZECHANGED;
			nm.hdr.idFrom=GetCmdID();
			nm.hdr.hwndFrom=0;
			nm.pSender=this;
			nm.iItem=LOWORD(m_dwHitTest);
			nm.nWidth=m_arrItems[nm.iItem].cx;
			DuiNotify((LPNMHDR)&nm);
		}
		m_bDragging=FALSE;
		ReleaseDuiCapture();
	}

	void CDuiHeaderCtrl::OnMouseMove( UINT nFlags,CPoint pt )
	{
		if(m_bDragging || nFlags&MK_LBUTTON)
		{
			if(!m_bDragging)
			{
				m_bDragging=TRUE;
				if(IsItemHover(m_dwHitTest) && m_bItemSwapEnable)
				{
					m_dwDragTo=m_dwHitTest;
					CRect rcItem=GetItemRect(LOWORD(m_dwHitTest));
					DrawDraggingState(m_dwDragTo);
					m_hDragImglst=CreateDragImage(LOWORD(m_dwHitTest));
// 					ShowCursor(FALSE);
// 					ImageList_SetDragCursorImage(m_hDragImglst,0,(m_ptClick.x-rcItem.left),(m_ptClick.y-rcItem.top));
					ImageList_BeginDrag(m_hDragImglst,0,(m_ptClick.x-rcItem.left),(m_ptClick.y-rcItem.top));
					ImageList_DragEnter(NULL,pt.x,pt.y);
				}
			}
			if(IsItemHover(m_dwHitTest))
			{
				if(m_bItemSwapEnable)
				{
					DWORD dwDragTo=HitTest(pt);
					if(IsItemHover(dwDragTo) && m_dwDragTo!=dwDragTo)
					{
						m_dwDragTo=dwDragTo;
						DUITRACE(_T("\n!!! dragto %d"),LOWORD(dwDragTo));
						ImageList_DragShowNolock(FALSE);
						DrawDraggingState(dwDragTo);
						ImageList_DragShowNolock(TRUE);
					}
					CPoint pt2(pt.x,m_ptClick.y);
					ClientToScreen(GetContainer()->GetHostHwnd(),&pt2);
					ImageList_DragMove(pt2.x,pt2.y);
				}
			}else if(m_dwHitTest!=-1)
			{//���ڿ��
				int cxNew=m_nAdjItemOldWidth+pt.x-m_ptClick.x;
				if(cxNew<0) cxNew=0;
				m_arrItems[LOWORD(m_dwHitTest)].cx=cxNew;
				NotifyInvalidate();
				GetContainer()->DuiUpdateWindow();//�������´���
				//�������ڿ����Ϣ
				DUINMHDSIZECHANGING	nm;
				nm.hdr.code=DUINM_HDSIZECHANGING;
				nm.hdr.idFrom=GetCmdID();
				nm.hdr.hwndFrom=0;
				nm.pSender=this;
				nm.nWidth=cxNew;
				DuiNotify((LPNMHDR)&nm);
			}
		}else
		{
			DWORD dwHitTest=HitTest(pt);
			if(dwHitTest!=m_dwHitTest)
			{
				if(m_bSortHeader)
				{
					if(IsItemHover(m_dwHitTest))
					{
						WORD iHover=LOWORD(m_dwHitTest);
						m_arrItems[iHover].state=0;
						RedrawItem(iHover);
					}
					if(IsItemHover(dwHitTest))
					{
						WORD iHover=LOWORD(dwHitTest);
						m_arrItems[iHover].state=1;//hover
						RedrawItem(iHover);
					}
				}
				m_dwHitTest=dwHitTest;
			}
		}
		
	}

	void CDuiHeaderCtrl::OnMouseLeave()
	{
		if(!m_bDragging)
		{
			if(IsItemHover(m_dwHitTest) && m_bSortHeader)
			{
				m_arrItems[LOWORD(m_dwHitTest)].state=0;
				RedrawItem(LOWORD(m_dwHitTest));
			}
			m_dwHitTest=-1;
		}
	}

	BOOL CDuiHeaderCtrl::LoadChildren( pugi::xml_node xmlNode )
	{
		if(!xmlNode || strcmp(xmlNode.name(),"items")!=0)
			return TRUE;
		pugi::xml_node xmlItem=xmlNode.child("item");
		int iOrder=0;
		while(xmlItem)
		{
			DUIHDITEM item={0};
			item.mask=0xFFFFFFFF;
			item.iOrder=iOrder++;
			CDuiStringT strTxt=DUI_CA2T(xmlItem.text().get(),CP_UTF8);
			item.pszText=_tcsdup(strTxt);
			item.cchTextMax=strTxt.GetLength();
			item.cx=xmlItem.attribute("width").as_int(50);
			item.lParam=xmlItem.attribute("userata").as_uint(0);
			item.stFlag=(DUIHDSORTFLAG)xmlItem.attribute("sortFlag").as_uint(ST_NULL);
			m_arrItems.InsertAt(m_arrItems.GetCount(),item);
			xmlItem=xmlItem.next_sibling("item");
		}
		return TRUE;
	}

	BOOL CDuiHeaderCtrl::OnDuiSetCursor( const CPoint &pt )
	{
		if(m_bFixWidth) return FALSE;
		DWORD dwHit=HitTest(pt);
		if(HIWORD(dwHit)==LOWORD(dwHit)) return FALSE;
		SetCursor(::LoadCursor(NULL,IDC_SIZEWE));
		return TRUE;
	}

	DWORD CDuiHeaderCtrl::HitTest( CPoint pt )
	{
		CRect	rcClient;
		GetClient(&rcClient);
		CRect rcItem(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
		int nMargin=m_bSortHeader?CX_HDITEM_MARGIN:0;
		for(int i=0;i<m_arrItems.GetCount();i++)
		{
			if(m_arrItems[i].cx==0) continue;	//Խ�����Ϊ0����

			rcItem.left=rcItem.right;
			rcItem.right=rcItem.left+m_arrItems[i].cx;
			if(pt.x<rcItem.left+nMargin)
			{
				int nLeft=i>0?i-1:0;
				return MAKELONG(nLeft,i);	
			}else if(pt.x<rcItem.right-nMargin)
			{
				return MAKELONG(i,i);
			}else if(pt.x<rcItem.right)
			{
				WORD nRight=(WORD)i+1;
				if(nRight>=m_arrItems.GetCount()) nRight=-1;//����-1����ĩβ
				return MAKELONG(i,nRight);
			}
		}
		return -1;
	}

	HIMAGELIST CDuiHeaderCtrl::CreateDragImage( int iItem )
	{
		if(iItem<0 || iItem>=m_arrItems.GetCount()) return NULL;
		CRect rcClient;
		GetClient(rcClient);
		CRect rcItem(0,0,m_arrItems[iItem].cx,rcClient.Height());
		
		CDCHandle dc=GetDuiDC(NULL,OLEDC_NODRAW);
		CMemDC memdc(dc,rcItem);
		CDCHandle hmemdc=memdc;
		BeforePaintEx(hmemdc);
		DrawItem(hmemdc,rcItem,m_arrItems.GetData()+iItem);
		HIMAGELIST hImglst=ImageList_Create(rcItem.Width(),rcItem.Height(),ILC_COLOR32,0,1);
		HBITMAP hItemBmp=memdc.SelectBitmap(NULL);
		ImageList_Add(hImglst,hItemBmp,NULL);
		DeleteObject(hItemBmp);
		ReleaseDuiDC(dc);
		return hImglst;
	}

	void CDuiHeaderCtrl::DrawDraggingState(DWORD dwDragTo)
	{
		CRect rcClient;
		GetClient(&rcClient);
		CDCHandle dc=GetDuiDC(rcClient,OLEDC_PAINTBKGND);
		DuiDCPaint duidc;
		BeforePaint(dc,duidc);
		CRect rcItem(rcClient.left,rcClient.top,rcClient.left,rcClient.bottom);
		int iDragTo=LOWORD(dwDragTo);
		int iDragFrom=LOWORD(m_dwHitTest);

		CDuiArray<int> items;
		for(int i=0;i<m_arrItems.GetCount();i++)
		{
			if(i!=iDragFrom) items.Add(i);
		}
		items.InsertAt(iDragTo,iDragFrom);
		
		for(int i=0;i<items.GetCount();i++)
		{
			rcItem.left=rcItem.right;
			rcItem.right=rcItem.left+m_arrItems[items[i]].cx;
			if(items[i]!=iDragFrom)
				DrawItem(dc,rcItem,m_arrItems.GetData()+items[i]);
			else//draw background
				m_pSkinItem->Draw(dc,rcItem,0);
		}
		AfterPaint(dc,duidc);
		ReleaseDuiDC(dc);
	}

	int CDuiHeaderCtrl::GetTotalWidth()
	{
		int nRet=0;
		for(int i=0;i<m_arrItems.GetCount();i++)
			nRet+=m_arrItems[i].cx;
		return nRet;
	}

	int CDuiHeaderCtrl::GetItemWidth( int iItem )
	{
		if(iItem<0 || iItem>= m_arrItems.GetCount()) return -1;
		return m_arrItems[iItem].cx;
	}


}//end of namespace DuiEngine
