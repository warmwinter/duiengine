//////////////////////////////////////////////////////////////////////////
//   File Name: bkwndcmnctrl.h
// Description: DuiWindow Definition
//     Creator: Zhang Xiaoxuan
//     Version: 2009.04.28 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "duiwnd.h"
#include "duiwndpanel.h"
#include "duiwndnotify.h"
#include "Accelerator.h"

namespace DuiEngine
{


//////////////////////////////////////////////////////////////////////////
// Spacing Control
// Only leave a space
//
// Usage: <spacing width=xx />
//
class DUI_EXP CDuiSpacing : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiSpacing, "spacing")

public:

    // Do nothing
    void OnPaint(CDCHandle dc)
    {
    }

protected:
    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()
};


//////////////////////////////////////////////////////////////////////////
// Static Control
//
// Usage: <text>inner text example</text>
// �ж�������ʱ��\n����ǿ�ƻ���
//
class DUI_EXP CDuiStatic : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiStatic, "text")
public:
    CDuiStatic():m_bMultiLines(0),m_nLineInter(5)
    {
        m_bMsgTransparent=TRUE;
    }
    virtual void DuiDrawText(HDC hdc,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);

protected:
    int m_bMultiLines;
    int m_nLineInter;

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_INT_ATTRIBUTE("multilines", m_bMultiLines, FALSE)
    DUIWIN_INT_ATTRIBUTE("interhei", m_nLineInter, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()
};

//////////////////////////////////////////////////////////////////////////
// Link Control
// Only For Header Drag Test
// Usage: <link>inner text example</link>
//
class DUI_EXP CDuiLink : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiLink, "link")

public:

protected:
    virtual void OnAttributeFinish(pugi::xml_node xmlNode);
    virtual void DuiDrawText(HDC hdc,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);
    virtual BOOL OnDuiSetCursor(const CPoint &pt);

    void OnLButtonDown(UINT nFlags,CPoint pt);
    void OnLButtonUp(UINT nFlags,CPoint pt);
    void OnMouseMove(UINT nFlags,CPoint pt);
    void OnMouseHover(WPARAM wParam, CPoint ptPos);

    DUIWIN_BEGIN_MSG_MAP()
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSEHOVER(OnMouseHover)
    DUIWIN_END_MSG_MAP()

    CRect m_rcText;
};

//////////////////////////////////////////////////////////////////////////
// Button Control
// Use id attribute to process click event
//
// Usage: <button id=xx>inner text example</button>
//
class DUI_EXP CDuiButton : public CDuiWindow, public CAcceleratorTarget
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiButton, "button")
public:
    CDuiButton();
protected:

    virtual BOOL NeedRedrawWhenStateChange()
    {
        return TRUE;
    }

    virtual UINT OnGetDuiCode()
    {
        return DUIC_WANTCHARS;
    }

	virtual bool OnAcceleratorPressed(const CAccelerator& accelerator);
protected:
    void OnPaint(CDCHandle dc);

    void OnLButtonDown(UINT uFlag,CPoint pt);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	CDuiSkinBase *m_pSkin;
public:
    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
	DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_KEYDOWN(OnKeyDown)
    DUIWIN_END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
// Image Control
// Use src attribute specify a resource id
//
// Usage: <img skin="skin" sub="0"/>
//
class DUI_EXP CDuiImageWnd : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiImageWnd, "img")
public:
    CDuiImageWnd();

    virtual ~CDuiImageWnd();

    void OnPaint(CDCHandle dc);

    BOOL SetSkin(CDuiSkinBase *pSkin,int nSubID=0);

    BOOL SetIcon(int nSubID);

	CDuiSkinBase * GetSkin(){return m_pSkin;}
protected:
	virtual CSize GetDesiredSize(LPRECT pRcContainer);

    BOOL m_bManaged;
    int m_nSubImageID;
    CDuiSkinBase *m_pSkin;
    //BOOL m_bCalc;

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, TRUE)
    DUIWIN_INT_ATTRIBUTE("sub", m_nSubImageID, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()
};


class DUI_EXP CDuiAnimateImgWnd : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiAnimateImgWnd, "animateimg")
public:
    CDuiAnimateImgWnd();
    virtual ~CDuiAnimateImgWnd() {}

    void Start();

    void Stop();

	BOOL IsPlaying(){return m_bPlaying;}
protected:
	virtual CSize GetDesiredSize(LPRECT pRcContainer);
	void OnPaint(CDCHandle dc);

	void OnDuiTimer(char cID);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnDestroy();

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_DUITIMER(OnDuiTimer)
    MSG_WM_DESTROY(OnDestroy)
	MSG_WM_SHOWWINDOW(OnShowWindow)
    DUIWIN_END_MSG_MAP()

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, TRUE)
    DUIWIN_UINT_ATTRIBUTE("speed", m_nSpeed, FALSE)
    DUIWIN_UINT_ATTRIBUTE("autostart", m_bAutoStart, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

protected:
    CDuiSkinBase *m_pSkin;
    int			  m_nSpeed;
    int			  m_iCurFrame;
    BOOL		  m_bAutoStart;
	BOOL		  m_bPlaying;
};
//////////////////////////////////////////////////////////////////////////
// Progress Control
// Use id attribute to process click event
//
// Usage: <progress bgskin=xx posskin=xx min=0 max=100 value=10,showpercent=0/>
//

class DUI_EXP CDuiProgress : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, "progress")
public:
    CDuiProgress();

    BOOL SetValue(int nValue);

	int GetValue(){return m_nValue;}

	void SetRange(int nMin,int nMax);

	void GetRange(int *pMin,int *pMax);

	BOOL IsVertical(){return m_bVertical;}
protected:

	virtual CSize GetDesiredSize(LPRECT pRcContainer);

	void OnPaint(CDCHandle dc);

    int m_nMinValue;
    int m_nMaxValue;
    int m_nValue;

    BOOL m_bShowPercent;
	BOOL	m_bVertical;

    CDuiSkinBase *m_pSkinBg;
    CDuiSkinBase *m_pSkinPos;

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_SKIN_ATTRIBUTE("bgskin", m_pSkinBg, TRUE)
    DUIWIN_SKIN_ATTRIBUTE("posskin", m_pSkinPos, TRUE)
    DUIWIN_INT_ATTRIBUTE("min", m_nMinValue, FALSE)
    DUIWIN_INT_ATTRIBUTE("max", m_nMaxValue, FALSE)
    DUIWIN_INT_ATTRIBUTE("value", m_nValue, FALSE)
    DUIWIN_UINT_ATTRIBUTE("vertical", m_bVertical, FALSE)
    DUIWIN_UINT_ATTRIBUTE("showpercent", m_bShowPercent, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()
};

//////////////////////////////////////////////////////////////////////////
// Image Button Control
//
// Usage: <imgbtn src=xx />
//
class DUI_EXP CDuiImageBtnWnd : public CDuiImageWnd
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiImageBtnWnd, "imgbtn")
public:
    CDuiImageBtnWnd();
    virtual ~CDuiImageBtnWnd() {}

    virtual BOOL NeedRedrawWhenStateChange();

    void OnPaint(CDCHandle dc);

    virtual void OnStateChanged(DWORD dwOldState,DWORD dwNewState);
    void OnDuiTimer(char cTimerID);
protected:
    BOOL m_bAnimate;
    BYTE m_byAlpha;

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_INT_ATTRIBUTE("animate", m_bAnimate, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_DUITIMER(OnDuiTimer)
    DUIWIN_END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
// Line Control
// Simple HTML "HR" tag
//
// Usage: <hr style=solid size=1 crbg=.../>
//
class DUI_EXP CDuiLine : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiLine, "hr")

public:

    CDuiLine();

    // Do nothing
    void OnPaint(CDCHandle dc);
protected:
    int m_nPenStyle;
    int m_nLineSize;
    BOOL m_bLineShadow;
    COLORREF m_crShadow;

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_INT_ATTRIBUTE("size", m_nLineSize, FALSE)
    DUIWIN_UINT_ATTRIBUTE("shadow", m_bLineShadow, FALSE)
    DUIWIN_COLOR_ATTRIBUTE("crshadow", m_crShadow, FALSE)
    DUIWIN_ENUM_ATTRIBUTE("style", int, FALSE)
    DUIWIN_ENUM_VALUE("solid", PS_SOLID)             // default
    DUIWIN_ENUM_VALUE("dash", PS_DASH)               /* -------  */
    DUIWIN_ENUM_VALUE("dot", PS_DOT)                 /* .......  */
    DUIWIN_ENUM_VALUE("dashdot", PS_DASHDOT)         /* _._._._  */
    DUIWIN_ENUM_VALUE("dashdotdot", PS_DASHDOTDOT)   /* _.._.._  */
    DUIWIN_ENUM_END(m_nPenStyle)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
// Check Box
//
// Usage: <check state=4>This is a check-box</check>
//
class DUI_EXP CDuiCheckBox : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiCheckBox, "check")

    enum
    {
        CheckBoxSpacing = 4,
    };

public:

    CDuiCheckBox();

    void OnPaint(CDCHandle dc);
protected:

    CDuiSkinBase *m_pSkin;
    CDuiSkinBase *m_pFocusSkin;

    UINT _GetDrawState();
	CRect GetCheckRect();

	virtual CSize GetDesiredSize(LPRECT pRcContainer);

	virtual void GetTextRect(LPRECT pRect);

    virtual BOOL NeedRedrawWhenStateChange()
    {
        return TRUE;
    }

    virtual UINT OnGetDuiCode()
    {
        return DUIC_WANTCHARS;
    }

    virtual void DuiDrawFocus(HDC dc);

    void OnLButtonDown(UINT nFlags, CPoint point);

    void OnLButtonUp(UINT nFlags, CPoint point);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
		DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, FALSE)
		DUIWIN_SKIN_ATTRIBUTE("focusskin", m_pFocusSkin, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
		MSG_WM_PAINT(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_KEYDOWN(OnKeyDown)
    DUIWIN_END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
// Icon Control
// Use src attribute specify a resource id
//
// Usage: <icon src=xx size="16"/>
//
class DUI_EXP CDuiIconWnd : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiIconWnd, "icon")
public:
    CDuiIconWnd();

    virtual BOOL Load(pugi::xml_node xmlNode);

    void OnPaint(CDCHandle dc);

    HICON AttachIcon(HICON hIcon);

    void LoadIconFile( LPCWSTR lpFIleName );
protected:
	virtual CSize GetDesiredSize(LPRECT pRcContainer);

    void _ReloadIcon();

    HICON m_theIcon;
    UINT m_uResID;
    UINT m_uResIDCurrent;
    int m_nSize;

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_UINT_ATTRIBUTE("src", m_uResID, FALSE)
    DUIWIN_INT_ATTRIBUTE("size", m_nSize, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
// Radio Box
//
// Usage: <radio state=1>This is a check-box</radio>
//
class DUI_EXP CDuiRadioBox : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiRadioBox, "radio")

    enum
    {
        RadioBoxSpacing = 4,
    };

public:

    CDuiRadioBox();


    void OnPaint(CDCHandle dc);

protected:

    // CDuiRadioBoxTheme m_theme;
    CDuiSkinBase *m_pSkin;
    CDuiSkinBase *m_pFocusSkin;

    UINT _GetDrawState();
	CRect GetRadioRect();
	virtual void GetTextRect(LPRECT pRect);

	virtual CSize GetDesiredSize(LPRECT pRcContainer);

    virtual BOOL NeedRedrawWhenStateChange();

    virtual void DuiDrawFocus(HDC dc);

    virtual UINT OnGetDuiCode()
    {
        return 0;
    }

	virtual BOOL IsSiblingsAutoGroupped() {return TRUE;}

    void OnLButtonDown(UINT nFlags, CPoint point);

	void OnSetDuiFocus();


    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, FALSE)
    DUIWIN_SKIN_ATTRIBUTE("focusskin", m_pFocusSkin, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
	MSG_WM_SETFOCUS_EX(OnSetDuiFocus)
    DUIWIN_END_MSG_MAP()
};


class DUI_EXP CDuiToggle : public CDuiWindow
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiToggle, "togglectrl")
public:
    CDuiToggle();

    void SetToggle(BOOL bToggle,BOOL bUpdate=TRUE);

    BOOL GetToggle();
protected:
    void OnPaint(CDCHandle dc);
    void OnLButtonUp(UINT nFlags,CPoint pt);
	virtual CSize GetDesiredSize(LPRECT pRcContainer);

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_INT_ATTRIBUTE("toggled", m_bToggled, TRUE)
    DUIWIN_SKIN_ATTRIBUTE("skin", m_pSkin, TRUE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    DUIWIN_END_MSG_MAP()
protected:
    BOOL m_bToggled;
	CDuiSkinBase *m_pSkin;
};

//<group crline1="b8d5e2" crline2="999999">group text</>
class DUI_EXP CDuiGroup : public CDuiPanel
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiGroup, "group")
public:
    CDuiGroup();

protected:
    void OnPaint(CDCHandle dc);
    COLORREF m_crLine1,m_crLine2;
    int		 m_nRound;
public:
    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_COLOR_ATTRIBUTE("crline1", m_crLine1, FALSE)
    DUIWIN_COLOR_ATTRIBUTE("crline2", m_crLine2, FALSE)
    DUIWIN_INT_ATTRIBUTE("round",m_nRound,FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()
};

}//namespace DuiEngine