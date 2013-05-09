//////////////////////////////////////////////////////////////////////////
//  CDuiSliderBar
//	Author: Huang Jianxiong
//	date: 2013/2/17
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "duiwndcmnctrl.h"

namespace DuiEngine
{

class DUI_EXP CDuiSliderBar : public CDuiProgress
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiSliderBar, "sliderbar")

public:
    CDuiSliderBar();
    ~CDuiSliderBar();

    enum
    {
        SC_RAIL,
        SC_SELECT,
        SC_THUMB,
    };

public:
    int             HitTest(CPoint pt);

protected:
    BOOL            m_bDrag;
    CPoint          m_ptDrag;
	int				m_nDragValue;
    int             m_uHtPrev;

	CDuiSkinBase *  m_pSkinThumb;

protected:
	LRESULT         NotifySbCode(UINT uCode, int nPos);

	virtual CSize	GetDesiredSize(LPRECT pRcContainer);

	CRect           GetPartRect(UINT uSBCode);

	void            OnPaint(CDCHandle dc);
	void            OnLButtonUp(UINT nFlags, CPoint point);
	void            OnLButtonDown(UINT nFlags, CPoint point);
	void            OnMouseMove(UINT nFlags, CPoint point);
	void            OnMouseLeave();


    DUIWIN_BEGIN_MSG_MAP()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_PAINT(OnPaint)
    DUIWIN_END_MSG_MAP()

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
		DUIWIN_SKIN_ATTRIBUTE("thumbskin", m_pSkinThumb, FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()
};

template<typename T>
void DUI_SWAP(T &a,T &b)
{
	T t=a;
	a=b;
	b=t;
}

}
