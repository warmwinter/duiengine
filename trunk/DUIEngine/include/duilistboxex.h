//////////////////////////////////////////////////////////////////////////
//  Class Name: CDuiListBoxEx
// Description: A DuiWindow Based ListBox Control. Can contain control as an item
//     Creator: Huang Jianxiong
//     Version: 2011.8.27 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma  once

#include "duiitempanel.h"

namespace DuiEngine
{

class DUI_EXP CDuiListBoxEx :public CDuiScrollView
    ,public CDuiItemContainer
{
public:

    DUIOBJ_DECLARE_CLASS_NAME(CDuiListBoxEx, "listboxex")

    CDuiListBoxEx();

    virtual ~CDuiListBoxEx();


    void DeleteAllItems(BOOL bUpdate=TRUE);

    void DeleteItem(int iItem);

    int InsertItem(int iItem,TiXmlElement *pXmlItem,DWORD dwData=0);

    int InsertItem(int iItem,LPCWSTR pszXml,DWORD dwData=0);

    void SetCurSel(int iItem);

    void EnsureVisible(int iItem);

    int GetCurSel();

    int GetItemObjIndex(CDuiPanel *pItemObj);

    CDuiPanel * GetItemPanel(int iItem);

    LPARAM GetItemData(int iItem);


    //************************************
    // Method:    SetItemCount
    // FullName:  DuiEngine::CDuiListBoxEx::SetItemCount
    // Access:    public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: int nItems ��Ŀ����
    // Parameter: LPCTSTR pszXmlTemplate ��ʾʱʹ�õ�XMLģ�壬Ϊ��ʱʹ��XML�ű���ָ����template�ӽڵ�����
    //************************************
    BOOL SetItemCount(int nItems,LPCTSTR pszXmlTemplate=NULL);

    int GetItemCount() ;

    int GetItemHeight()
    {
        return m_nItemHei;
    }

    void RedrawItem(int iItem);

    //�Զ��޸�pt��λ��Ϊ��Ե�ǰ���ƫ����
    int HitTest(CPoint &pt);

    virtual BOOL Load(TiXmlElement* pTiXmlElem);

	BOOL IsVirtual(){return m_bVirtual;}
protected:
	void UpdatePanelsIndex(UINT nFirst=0,UINT nLast=-1);

	CRect	GetItemRect(int iItem);

    virtual void OnItemSetCapture(CDuiItemPanel *pItem,BOOL bCapture);
    virtual BOOL OnItemGetRect(CDuiItemPanel *pItem,CRect &rcItem);

    virtual int GetScrollLineSize(BOOL bVertical);

    void OnPaint(CDCHandle dc);

	void OnSize(UINT nType, CSize size);

    virtual void OnDrawItem(CDCHandle & dc, CRect & rc, int iItem);

    virtual BOOL LoadChildren(TiXmlElement* pTiXmlChildElem);
    // Get tooltip Info
    virtual BOOL OnUpdateToolTip(HDUIWND hCurTipHost,HDUIWND &hNewTipHost,CRect &rcTip,CDuiStringT &strTip);

    void NotifySelChange(int nOldSel,int nNewSel,UINT uMsg);

	LRESULT OnMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

    void OnMouseLeave();

    void OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags );

    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    UINT OnGetDuiCode();

    virtual BOOL OnDuiSetCursor(const CPoint &pt);

    void OnDestroy();
protected:
    CDuiArray<CDuiItemPanel *> m_arrItems;

    int		m_iSelItem;
    int		m_iHoverItem;
    int		m_iScrollSpeed;

	CDuiItemPanel	* m_pTemplPanel;	//�����б�ʹ�õ�DUIģ��
	int		m_nItems;					//�����б��м�¼�б���
	TiXmlElement	* m_pXmlTempl;		//�б�ģ��XML
    CDuiItemPanel	*	m_pCapturedFrame;
    CDuiSkinBase * m_pItemSkin;
	int		m_nItemHei;
	BOOL	m_bVirtual;
public:
    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
		DUIWIN_INT_ATTRIBUTE("scrollspeed", m_iScrollSpeed, FALSE)
		DUIWIN_INT_ATTRIBUTE("itemheight", m_nItemHei, FALSE)
		DUIWIN_INT_ATTRIBUTE("virtual", m_bVirtual, TRUE)
		DUIWIN_SKIN_ATTRIBUTE("itemskin", m_pItemSkin, TRUE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_PAINT(OnPaint)
    MSG_WM_MOUSELEAVE(OnMouseLeave)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_CHAR(OnChar)
	MSG_WM_SIZE(OnSize)
	MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MBUTTONDBLCLK,OnMouseEvent)
    DUIWIN_END_MSG_MAP()
};

}//namespace DuiEngine