#pragma once

#include "duiobject.h"
#include "duiskinpool.h"
#include "SimpleWnd.h"

namespace DuiEngine
{

#define CX_ICON	16		//֧�ֵ�ͼ��Ŀ��
#define CY_ICON	16		//֧�ֵ�ͼ��ĸ߶�

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

typedef DuiMenuItemData * PDuiMenuItemData;

template <class T>
class CDuiOwnerDraw
{
public:
    // Message map and handlers
    BEGIN_MSG_MAP_EX(CDuiOwnerDraw< T >)
    MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
    MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
    MESSAGE_HANDLER(WM_COMPAREITEM, OnCompareItem)
    MESSAGE_HANDLER(WM_DELETEITEM, OnDeleteItem)
    ALT_MSG_MAP(1)
    MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
    MESSAGE_HANDLER(OCM_MEASUREITEM, OnMeasureItem)
    MESSAGE_HANDLER(OCM_COMPAREITEM, OnCompareItem)
    MESSAGE_HANDLER(OCM_DELETEITEM, OnDeleteItem)
    END_MSG_MAP()

    LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->DrawItem((LPDRAWITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    LRESULT OnMeasureItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->MeasureItem((LPMEASUREITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    LRESULT OnCompareItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)pT->CompareItem((LPCOMPAREITEMSTRUCT)lParam);
    }

    LRESULT OnDeleteItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
    {
        T* pT = static_cast<T*>(this);
        pT->SetMsgHandled(TRUE);
        pT->DeleteItem((LPDELETEITEMSTRUCT)lParam);
        bHandled = pT->IsMsgHandled();
        return (LRESULT)TRUE;
    }

    // Overrideables
    void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
    {
        // must be implemented
        DUIASSERT(FALSE);
    }

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        if(lpMeasureItemStruct->CtlType != ODT_MENU)
        {
            // return default height for a system font
            T* pT = static_cast<T*>(this);
            HWND hWnd = pT->GetDlgItem(lpMeasureItemStruct->CtlID);
            CClientDC dc(hWnd);
            TEXTMETRIC tm = { 0 };
            dc.GetTextMetrics(&tm);

            lpMeasureItemStruct->itemHeight = tm.tmHeight;
        }
        else
            lpMeasureItemStruct->itemHeight = ::GetSystemMetrics(SM_CYMENU);
    }

    int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/)
    {
        // all items are equal
        return 0;
    }

    void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
    {
        // default - nothing
    }
};

class CDuiMenuODWnd : public CSimpleWnd
    ,public CDuiOwnerDraw<CDuiMenuODWnd>
    ,public CDuiMenuAttr
{
    friend class CDuiOwnerDraw<CDuiMenuODWnd>;
public:
    CDuiMenuODWnd(HWND hMenuOwner);

protected:
    void OnInitMenu(HMENU menu);
    void OnInitMenuPopup(HMENU menuPopup, UINT nIndex, BOOL bSysMenu);

    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

    void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU menu);

    BEGIN_MSG_MAP_EX(CDuiMenuODWnd)
    MSG_WM_INITMENU(OnInitMenu)
    MSG_WM_INITMENUPOPUP(OnInitMenuPopup)
    MSG_WM_MENUSELECT(OnMenuSelect)
    CHAIN_MSG_MAP(CDuiOwnerDraw<CDuiMenuODWnd>)
    REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

	HWND m_hMenuOwner;
};


class DUI_EXP CDuiMenu
{
public:
    CDuiMenu();
    ~CDuiMenu(void);

    BOOL LoadMenu(UINT uResID);

    BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem,LPCTSTR strText, int iIcon);

    UINT TrackPopupMenu( UINT uFlags, int x, int y, HWND hWnd, LPCRECT prcRect=NULL);

    void DestroyMenu();

    CDuiMenu GetSubMenu(int nPos);

    HMENU m_hMenu;

protected:
    CDuiMenu(CDuiMenu *pParent);

    void BuildMenu(HMENU menuPopup,TiXmlElement *pTiXmlMenu);

    CDuiArray<DuiMenuItemData *> m_arrDmmi;
    CDuiMenuAttr	m_menuSkin;
    CDuiMenu	*	m_pParent;
};

}//namespace DuiEngine