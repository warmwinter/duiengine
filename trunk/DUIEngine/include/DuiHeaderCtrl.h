#pragma once

#include "duiwnd.h"
#include "wtl.mini/duicoll.h"
namespace DuiEngine
{
#define DUIHDI_WIDTH               0x0001
#define DUIHDI_TEXT                0x0002
#define DUIHDI_SORTFLAG			   0x0004
#define DUIHDI_LPARAM              0x0008

	typedef enum _DUIHDSORTFLAG{
		ST_NULL=0,
		ST_UP,
		ST_DOWN,
	}DUIHDSORTFLAG;

	typedef struct _DUIHDITEM {
		UINT    mask; 
		int     cx; 
		LPTSTR  pszText; 
		int     cchTextMax; 
		DUIHDSORTFLAG stFlag;
		LPARAM  lParam; 
		UINT   state;
	} DUIHDITEM,  *LPDUIHDITEM;


	class CDuiHeaderCtrl: public CDuiWindow
	{
		DUIOBJ_DECLARE_CLASS_NAME(CDuiHeaderCtrl, "header")
	public:
		CDuiHeaderCtrl(void);
		~CDuiHeaderCtrl(void);

		int InsertItem( int iItem,LPCTSTR pszText,int nWidth, DUIHDSORTFLAG stFlag,LPARAM lParam );
		BOOL GetItem(int iItem,DUIHDITEM *pItem);

		BOOL DeleteItem(int iItem);
		void DeleteAllItems();

		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_SKIN_ATTRIBUTE("itemSkin",m_pSkinItem,FALSE)
			DUIWIN_SKIN_ATTRIBUTE("sortSkin",m_pSkinSort,FALSE)
			DUIWIN_INT_ATTRIBUTE("fixWidth",m_bFixWidth,FALSE)
			DUIWIN_INT_ATTRIBUTE("itemSwapEnable",m_bItemSwapEnable,FALSE)
			DUIWIN_INT_ATTRIBUTE("sortHeader",m_bSortHeader,FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()
	protected:
		virtual BOOL LoadChildren(pugi::xml_node xmlNode);
		virtual BOOL OnDuiSetCursor(const CPoint &pt);

		void OnPaint(CDCHandle dc);
		virtual void DrawItem(CDCHandle dc,CRect rcItem,const LPDUIHDITEM pItem);
		CRect	GetItemRect(int iItem);
		//���λ�÷���������һ��DWORD��LOWORDΪǰ����HIWORDΪ��������ͬʱ������һ������м�
		DWORD  HitTest(CPoint pt);
		void RedrawItem(int iItem);

		void DrawDraggingState(DWORD dwDragTo);

		BOOL IsItemHover(DWORD dwHitTest)
		{
			return dwHitTest!=-1 && LOWORD(dwHitTest)==HIWORD(dwHitTest);
		}

		HIMAGELIST CreateDragImage(int iItem);

		void OnLButtonDown(UINT nFlags,CPoint pt);
		void OnLButtonUp(UINT nFlags,CPoint pt);
		void OnMouseMove(UINT nFlags,CPoint pt);
		void OnMouseLeave();
		void OnDestroy();
		
		DUIWIN_BEGIN_MSG_MAP()
			MSG_WM_PAINT(OnPaint)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
			MSG_WM_DESTROY(OnDestroy)
		DUIWIN_END_MSG_MAP()

		CDuiSkinBase *	m_pSkinItem;
		CDuiSkinBase *	m_pSkinSort;
		BOOL			m_bSortHeader;		//��ͷ���Ե������
		BOOL			m_bFixWidth;		//�����ȹ̶�����
		BOOL			m_bItemSwapEnable;//�����϶�����λ�ÿ���

		BOOL			m_bDragging;
		HIMAGELIST		m_hDragImglst;
		CPoint			m_ptClick;
		DWORD			m_dwHitTest;
		DWORD			m_dwDragTo;
		
		int				m_nAdjItemOldWidth;//���汻�϶����ԭʼ���
		CDuiArray<DUIHDITEM> m_arrItems;
	};
}//end of namespace DuiEngine
