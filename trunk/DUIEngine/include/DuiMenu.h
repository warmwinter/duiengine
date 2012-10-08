#pragma once

#include "duiobject.h"
#include "duiskin.h"

namespace DuiEngine{

#define CX_ICON	16		//֧�ֵ�ͼ��Ŀ��
#define CY_ICON	16		//֧�ֵ�ͼ��ĸ߶�

typedef CWinTraits<WS_POPUP , WS_EX_NOACTIVATE> CDuiMenuODTraits;

class CDuiMenuAttr:public CDuiObject
{
	friend class CDuiMenu;
	DUIOBJ_DECLARE_CLASS_NAME(CDuiMenuAttr, "menuattribute")
public:
	CDuiMenuAttr();

	virtual void OnAttributeFinish(TiXmlElement* pXmlElem);

	DUIWIN_DECLARE_ATTRIBUTES_BEGIN()		
		DUIWIN_SKIN_ATTRIBUTE("itemskin",m_pItemSkin,FALSE)
		DUIWIN_SKIN_ATTRIBUTE("iconskin",m_pIconSkin,FALSE)
		DUIWIN_SKIN_ATTRIBUTE("sepskin",m_pSepSkin,FALSE)
		DUIWIN_SKIN_ATTRIBUTE("checkskin",m_pCheckSkin,FALSE)
		DUIWIN_INT_ATTRIBUTE("itemheight",m_nItemHei,FALSE)
		DUIWIN_INT_ATTRIBUTE("iconmargin",m_nIconMargin,FALSE)
		DUIWIN_INT_ATTRIBUTE("textmargin",m_nTextMargin,FALSE)
		DUIWIN_SIZE_ATTRIBUTE("iconsize",m_szIcon,FALSE)
		DUIWIN_FONT_ATTRIBUTE("font",m_hFont,FALSE)
		DUIWIN_COLOR_ATTRIBUTE("crtxt",m_crTxtNormal,FALSE);
		DUIWIN_COLOR_ATTRIBUTE("crtxtsel",m_crTxtSel,FALSE);
		DUIWIN_COLOR_ATTRIBUTE("crtxtgray",m_crTxtGray,FALSE);

	DUIWIN_DECLARE_ATTRIBUTES_END()
protected:
	CDuiSkinBase *m_pItemSkin;	//�˵���Ƥ��������2��״̬������״̬+ѡ��״̬
	CDuiSkinBase *m_pIconSkin;	//�˵�ͼ��
	CDuiSkinBase *m_pSepSkin;	//�ָ���Ƥ��
	CDuiSkinBase *m_pCheckSkin;	//ѡ��״̬,��������״̬:��ѡ+Ȧѡ
	int			  m_nItemHei;	//�˵���߶�
	int			  m_nIconMargin;//ͼ���Ե�ռ�
	int			  m_nTextMargin;//�ı���Ե�ռ�
	COLORREF	  m_crTxtNormal;//�����ı���ɫ
	COLORREF	  m_crTxtSel;	//ѡ���ı���ɫ
	COLORREF	  m_crTxtGray;	//���ı���ɫ
	CSize		  m_szIcon;		//ͼ��ߴ�
	HFONT		 m_hFont;
};

struct DuiMenuItemInfo
{
	int iIcon;
	CString strText;
};

struct DuiMenuItemData
{
	HMENU hMenu;
	UINT_PTR nID;
	DuiMenuItemInfo itemInfo;
};

class CDuiMenuODWnd : public CWindowImpl<CDuiMenuODWnd,ATL::CWindow,CDuiMenuODTraits>
	,public COwnerDraw<CDuiMenuODWnd>
	,public CDuiMenuAttr
{
	friend class COwnerDraw<CDuiMenuODWnd>;
public:
	CDuiMenuODWnd();

protected:
	void OnInitMenu(CMenuHandle menu);
	void OnInitMenuPopup(CMenuHandle menuPopup, UINT nIndex, BOOL bSysMenu);

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	void OnMenuSelect(UINT nItemID, UINT nFlags, CMenuHandle menu);

	BEGIN_MSG_MAP_EX(CDuiMenuODWnd)
		MSG_WM_INITMENU(OnInitMenu)
		MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
		MSG_WM_MENUSELECT(OnMenuSelect)
		CHAIN_MSG_MAP(COwnerDraw<CDuiMenuODWnd>)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
};


class DUI_EXP CDuiMenu
	:public CMenuHandle
{
public:
	CDuiMenu();
	~CDuiMenu(void);
	
	BOOL LoadMenu(UINT uResID);

	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem,CString strText, int iIcon);

	UINT TrackPopupMenu( UINT uFlags, int x, int y, HWND hWnd, LPCRECT prcRect=NULL);

	void DestroyMenu();

	CDuiMenu GetSubMenu(int nPos);
protected:
	CDuiMenu(CDuiMenu *pParent);


	void BuildMenu(CMenuHandle menuPopup,TiXmlElement *pTiXmlMenu);
	CSimpleArray<DuiMenuItemData *>	m_arrDmmi;

	CDuiMenuAttr	m_menuSkin;
	CDuiMenu	*	m_pParent;
};

}//namespace DuiEngine