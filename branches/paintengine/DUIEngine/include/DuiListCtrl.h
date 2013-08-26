#pragma once

#include "duiwnd.h"
#include "duiwndpanel.h"
#include "DuiHeaderCtrl.h"

namespace DuiEngine
{

	enum{
		DUI_LVIF_TEXT=0x01,
		DUI_LVIF_IMAGE=0x02,
		DUI_LVIF_INDENT=0x04,
	};

	typedef int (__cdecl  *PFNLVCOMPAREEX)(void *, const void *, const void *);//ʹ�ÿ��������㷨�еıȽϺ���,�ο�qsort_s

	typedef struct _DXLVSUBITEM
	{
		_DXLVSUBITEM()
		{
			mask=0;
			nImage = 0;
			strText=NULL;
			cchTextMax=0;
			nIndent=0;
		}

		UINT mask;
		LPTSTR strText;
		int		cchTextMax;
		UINT    nImage;
		int		nIndent;
	} DXLVSUBITEM;

	typedef CDuiArray<DXLVSUBITEM>   ArrSubItem;

	typedef struct _DXLVITEM
	{
		_DXLVITEM()
		{
			dwData = 0;
			arSubItems=NULL;
		}

		ArrSubItem  *arSubItems;
		DWORD       dwData;
	} DXLVITEM;

	//////////////////////////////////////////////////////////////////////////
	//  CDuiListCtrl
	class DUI_EXP CDuiListCtrl : public CDuiPanelEx
	{
		DUIOBJ_DECLARE_CLASS_NAME(CDuiListCtrl, "listctrl")

	public:
		CDuiListCtrl();
		virtual ~CDuiListCtrl();

		int             InsertColumn(int nIndex, LPCTSTR pszText, int nWidth, LPARAM lParam=0);
		int             InsertItem(int nItem, LPCTSTR pszText, int nImage=-1);

		BOOL            SetItemData(int nItem, DWORD dwData);
		DWORD           GetItemData(int nItem);

		BOOL            SetSubItem(int nItem, int nSubItem,const DXLVSUBITEM* plv);
		BOOL            GetSubItem(int nItem, int nSubItem, DXLVSUBITEM* plv);

		BOOL            SetSubItemText(int nItem, int nSubItem, LPCTSTR pszText);

		int             GetSelectedItem();
		void            SetSelectedItem(int nItem);

		int             GetItemCount();
		BOOL			SetItemCount( int nItems ,int nGrowBy);
		int             GetColumnCount();

		int             GetCountPerPage(BOOL bPartial);

		void            DeleteItem(int nItem);
		void			DeleteColumn(int iCol);
		void            DeleteAllItems();

		virtual BOOL    LoadChildren(pugi::xml_node xmlNode);

		int             HitTest(const CPoint& pt);

		BOOL			SortItems( PFNLVCOMPAREEX pfnCompare, void * pContext );
	protected:
		int             GetTopIndex() const;

		CRect           GetItemRect(int nItem, int nSubItem=0);
		virtual void    DrawItem(CDCHandle dc, CRect rcItem, int nItem);

		void            RedrawItem(int nItem);

		void            NotifySelChange(int nOldSel, int nNewSel, UINT uMsg);

		void            OnPaint(CDCHandle dc);
		void            OnDestroy();

		bool            OnHeaderClick(CDuiWindow* pSender, LPNMHDR pNmhdr);
		bool            OnHeaderSizeChanging(CDuiWindow* pSender, LPNMHDR pNmhdr);
		bool            OnHeaderSwap(CDuiWindow* pSender, LPNMHDR pNmhdr);

		virtual BOOL    OnScroll(BOOL bVertical,UINT uCode,int nPos);
		virtual void    OnLButtonDown(UINT nFlags, CPoint pt);
		virtual void    OnLButtonUp(UINT nFlags, CPoint pt);
		virtual void    OnSize(UINT nType, CSize size);
		virtual void	UpdateChildrenPosition();

		CRect           GetListRect();
		void            UpdateScrollBar();
		void			UpdateHeaderCtrl();

	protected:
		int             m_nHeaderHeight;
		int             m_nItemHeight;

		//  ��ǰѡ�е��б���
		int             m_nSelectItem;
		int             m_nHoverItem;
		BOOL            m_bHotTrack;

		CPoint          m_ptIcon;
		CPoint          m_ptText;

		int             m_nMargin;

		COLORREF        m_crItemBg1;
		COLORREF        m_crItemBg2;
		COLORREF        m_crItemSelBg;
		COLORREF        m_crText;
		COLORREF        m_crSelText;

		CDuiSkinBase*    m_pItemSkin;
		CDuiSkinBase*    m_pIconSkin;

	protected:
		typedef CDuiArray<DXLVITEM> ArrLvItem;

		CDuiHeaderCtrl*  m_pHeader;
		ArrLvItem       m_arrItems;
		CPoint          m_ptOrigin;

	protected:
		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_INT_ATTRIBUTE("headerHeight", m_nHeaderHeight, FALSE)
			DUIWIN_INT_ATTRIBUTE("itemHeight", m_nItemHeight, FALSE)
			DUIWIN_SKIN_ATTRIBUTE("itemskin", m_pItemSkin, TRUE)
			DUIWIN_SKIN_ATTRIBUTE("iconskin", m_pIconSkin, TRUE)
			DUIWIN_COLOR_ATTRIBUTE("critembg1", m_crItemBg1, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("critembg2", m_crItemBg2, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("critemselbg", m_crItemSelBg, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("crtext", m_crText, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("crseltext", m_crSelText, FALSE)
			DUIWIN_INT_ATTRIBUTE("icon-x", m_ptIcon.x, FALSE)
			DUIWIN_INT_ATTRIBUTE("icon-y", m_ptIcon.y, FALSE)
			DUIWIN_INT_ATTRIBUTE("text-x", m_ptText.x, FALSE)
			DUIWIN_INT_ATTRIBUTE("text-y", m_ptText.y, FALSE)
			DUIWIN_INT_ATTRIBUTE("hottrack", m_bHotTrack, FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()

		DUIWIN_BEGIN_MSG_MAP()
			MSG_WM_PAINT(OnPaint)
			MSG_WM_DESTROY(OnDestroy)
			MSG_WM_SIZE(OnSize)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
		DUIWIN_END_MSG_MAP()
	};

}//end of namespace
