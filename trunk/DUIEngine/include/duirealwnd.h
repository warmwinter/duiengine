#pragma once

#include "duiwnd.h"

//////////////////////////////////////////////////////////////////////////
// Real Window Control
// Binding a real window
//
// Usage: <realwnd id=xx wndclass="edit" wndname="name" style="00000001" exstyle="00000000"/>
//

namespace DuiEngine
{

class DUI_EXP CDuiRealWndParam
{
public:
    CDuiRealWndParam();
    ~CDuiRealWndParam();

    CString m_strClassName,m_strWindowName;
    DWORD	 m_dwStyle,m_dwExStyle;
    TiXmlElement *m_pXmlParams;
};


class DUI_EXP CDuiRealWnd : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiRealWnd, "realwnd")
public:
    CDuiRealWnd();

    virtual ~CDuiRealWnd();


    const HWND GetRealHwnd(BOOL bAutoCreate=TRUE);

    const CDuiRealWndParam & GetRealWndParam()
    {
        return m_realwndParam;
    }

    void SetData(LPVOID lpData)
    {
        m_lpData=lpData;
    }
    const LPVOID GetData()
    {
        return m_lpData;
    }

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_TSTRING_ATTRIBUTE("wndclass", m_realwndParam.m_strClassName, FALSE)
    DUIWIN_TSTRING_ATTRIBUTE("wndname", m_realwndParam.m_strWindowName, FALSE)
    DUIWIN_HEX_ATTRIBUTE("style", m_realwndParam.m_dwStyle, FALSE)
    DUIWIN_HEX_ATTRIBUTE("exstyle", m_realwndParam.m_dwExStyle, FALSE)
    DUIWIN_INT_ATTRIBUTE("init",m_bInit,FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()
protected:
    virtual BOOL NeedRedrawWhenStateChange();
    virtual BOOL Load(TiXmlElement* pXmlElem);


    void OnWindowPosChanged(LPDUIWNDPOS lpWndPos);

    void OnShowWindow(BOOL bShow, UINT nStatus);
    void OnDestroy();

    // Do nothing
    void OnPaint(CDCHandle dc) {}

    void ShowRealWindow();

    BOOL InitRealWnd();


    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_DUIWINPOSCHANGED(OnWindowPosChanged)
    MSG_WM_SHOWWINDOW(OnShowWindow)
    DUIWIN_END_MSG_MAP()

    CDuiRealWndParam	m_realwndParam;
    BOOL	m_bInit;

    HWND	 m_hRealWnd;
    LPVOID	m_lpData;
};

interface DUI_EXP IDuiRealWndHandler
{
    virtual HWND OnRealWndCreate(CDuiRealWnd *pRealWnd)=NULL;
    virtual BOOL OnRealWndInit(CDuiRealWnd *pRealWnd)=NULL;
    virtual void OnRealWndDestroy(CDuiRealWnd *pRealWnd)=NULL;
    virtual void OnRealWndSize(CDuiRealWnd *pRealWnd)=NULL;
};

}//namespace DuiEngine