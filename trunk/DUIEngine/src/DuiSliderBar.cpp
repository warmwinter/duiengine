#include "duistd.h"
#include "DuiSliderBar.h"

namespace DuiEngine
{

#define TIMERID_NOTIFY1     1
#define TIMERID_DELAY1      2

//////////////////////////////////////////////////////////////////////////
//  CDuiSliderBar
CDuiSliderBar::CDuiSliderBar()
    : m_bDrag(FALSE)
    , m_uHtPrev(-1)
	, m_pSkinThumb(NULL)
{
}

CDuiSliderBar::~CDuiSliderBar()
{
}

int CDuiSliderBar::HitTest(CPoint pt)
{
    CRect rc;

    rc = GetPartRect(SC_THUMB);
    if (rc.PtInRect(pt))
        return SC_THUMB;

    rc = GetPartRect(SC_SELECT);
    if (rc.PtInRect(pt))
        return SC_SELECT;

    rc = GetPartRect(SC_RAIL);
    if (rc.PtInRect(pt))
        return SC_RAIL;

    return -1;
}


CRect CDuiSliderBar::GetPartRect(UINT uSBCode)
{
    DUIASSERT(m_pSkinThumb);

	CRect rcClient;
	GetClient(&rcClient);


    SIZE szThumb = m_pSkinThumb->GetSkinSize();

	int nLength = IsVertical()? rcClient.Height():rcClient.Width();
	int nHei=IsVertical()? rcClient.Width():rcClient.Height();
	int nRailWid=IsVertical()? m_pSkinBg->GetSkinSize().cx:m_pSkinBg->GetSkinSize().cy;
	int nRailLength=nLength-(IsVertical()?szThumb.cy:szThumb.cx);
	if(nRailWid>nHei) nRailWid=nHei;

	CRect rcRet(0,0,nLength,nHei);
    switch (uSBCode)
    {
    case SC_RAIL:
        {
            //  ���Ҫƫ�ƻ����һ��(+1)
            rcRet.left   += szThumb.cx/2;
            //  �ұ�Ҫƫ�ƻ����һ��(-1)
            rcRet.right  -= szThumb.cx/2;
            //  ��ֱ����
            rcRet.top    += (nHei-nRailWid)/2;
            rcRet.bottom -= (nHei-nRailWid)/2;
        }
        break;

    case SC_SELECT:
        {
			rcRet.left   += szThumb.cx/2;
			rcRet.right  = rcRet.left+(m_nValue-m_nMinValue)*nRailLength/(m_nMaxValue-m_nMinValue);
			//  ��ֱ����
			rcRet.top    += (nHei-nRailWid)/2;
			rcRet.bottom -= (nHei-nRailWid)/2;
        }
        break;

    case SC_THUMB:
        {
			int nPos=(m_nValue-m_nMinValue)*nRailLength/(m_nMaxValue-m_nMinValue);
			CSize szThumb=m_pSkinThumb->GetSkinSize();
			if(!IsVertical())
			{
				rcRet.left += nPos;
				//rcRet.left-=szThumb.cx/2-szThumb.cx/2;left����Ҫ�仯��
				rcRet.right=rcRet.left+szThumb.cx;
				int nMargin=(rcClient.Height()-szThumb.cy)/2;
				if(nMargin>0)
				{
					rcRet.top+=nMargin;
					rcRet.bottom=rcRet.top+szThumb.cy;
				}else
				{
					rcRet.top=0;
					rcRet.bottom=rcClient.Height();
				}
			}
			else
			{
				rcRet.top += nPos;
				//rcRet.top-=szThumb.cy/2-szThumb.cy/2;
				rcRet.bottom=rcRet.top+szThumb.cy;
				int nMargin=(rcClient.Width()-szThumb.cx)/2;
				if(nMargin>0)
				{
					rcRet.left+=nMargin;
					rcRet.right=rcRet.left+szThumb.cx;
				}else
				{
					rcRet.left=0;
					rcRet.right=rcClient.Width();
				}
			}
			
        }
        break;
    }

	if(uSBCode!=SC_THUMB && IsVertical())
	{//�������꣬�����꽻��
		
		DUI_SWAP(rcRet.left,rcRet.top);
		DUI_SWAP(rcRet.right,rcRet.bottom);
	}
	rcRet.OffsetRect(rcClient.TopLeft());
    return rcRet;
}

void CDuiSliderBar::OnPaint(CDCHandle dc)
{
	DUIASSERT(m_pSkinThumb && m_pSkinBg && m_pSkinPos);

    DuiDCPaint dxDC;

    BeforePaint(dc, dxDC);

	CRect rcRail=GetPartRect(SC_RAIL);
	m_pSkinBg->Draw(dc,rcRail,0);
	if(m_nValue!=m_nMinValue)
	{
		CRect rcSel=GetPartRect(SC_SELECT);
		m_pSkinPos->Draw(dc,rcSel,0);
	}
    CRect rcThumb = GetPartRect(SC_THUMB);
	int nState=0;//normal
	if(m_bDrag) nState=1;//pushback
	else if(m_uHtPrev==SC_THUMB) nState=2;//hover
	m_pSkinThumb->Draw(dc, rcThumb, nState);

    AfterPaint(dc, dxDC);
}

void CDuiSliderBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseDuiCapture();

    if (m_bDrag)
    {
        m_bDrag   = FALSE;
		CRect rcThumb = GetPartRect(SC_THUMB);
		HDC   hdc     = GetDuiDC(&rcThumb, OLEDC_PAINTBKGND);
		m_pSkinThumb->Draw(hdc, rcThumb, IIF_STATE4(DuiWndState_Hover, 0, 1, 2, 3), 0xFF);
		ReleaseDuiDC(hdc);
    }
	OnMouseMove(nFlags,point);
}

void CDuiSliderBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetDuiCapture();

    UINT uHit = HitTest(point);
    if (uHit == SC_THUMB)
    {
        m_bDrag    = TRUE;
        m_ptDrag   = point;
		m_nDragValue=m_nValue;
    }
    else
    {
        if (uHit == SC_SELECT || uHit == SC_RAIL)
        {
			CRect rcRail=GetPartRect(SC_RAIL);
			int nValue=0;
			if(IsVertical())
			{
				nValue= (point.y-rcRail.top)*(m_nMaxValue-m_nMinValue+1)/rcRail.Height();
			}else
			{
				nValue= (point.x-rcRail.left)*(m_nMaxValue-m_nMinValue+1)/rcRail.Width();
			}
			SetValue(nValue);
			NotifySbCode(SC_THUMB,m_nValue);
        }
    }
}

void CDuiSliderBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (m_bDrag)
    {
		CRect rcRail=GetPartRect(SC_RAIL);

		int nInterHei=(IsVertical()?rcRail.Height():rcRail.Width());
		int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
		int nSlide=nDragLen*(m_nMaxValue-m_nMinValue+1)/nInterHei;

		int nNewTrackPos=m_nDragValue+nSlide;
		if(nNewTrackPos<m_nMinValue)
		{
			nNewTrackPos=m_nMinValue;
		}
		else if(nNewTrackPos>m_nMaxValue)
		{
			nNewTrackPos=m_nMaxValue;
		}
		if(nNewTrackPos!=m_nValue)
		{
			m_nValue=nNewTrackPos;
			NotifyInvalidate();
			NotifySbCode(SC_THUMB,m_nValue);
		}
    }
    else
    {
        int uHit = HitTest(point);
        if (uHit != m_uHtPrev && (m_uHtPrev==SC_THUMB || uHit==SC_THUMB))
        {
			CRect rcThumb = GetPartRect(SC_THUMB);
			HDC   hdc     = GetDuiDC(&rcThumb, OLEDC_PAINTBKGND);
			m_pSkinThumb->Draw(hdc, rcThumb, IIF_STATE4(uHit==SC_THUMB?DuiWndState_Hover:DuiWndState_Normal, 0, 1, 2, 3), 0xFF);
			ReleaseDuiDC(hdc);
            m_uHtPrev = uHit;
        }
    }
}

void CDuiSliderBar::OnMouseLeave()
{
    if (!m_bDrag && m_uHtPrev==SC_THUMB)
    {
		CRect rcThumb = GetPartRect(SC_THUMB);
		HDC   hdc     = GetDuiDC(&rcThumb, OLEDC_PAINTBKGND);
		m_pSkinThumb->Draw(hdc, rcThumb, IIF_STATE4(DuiWndState_Normal, 0, 1, 2, 3), 0xFF);
		ReleaseDuiDC(hdc);
		m_uHtPrev=-1;
    }
}

LRESULT CDuiSliderBar::NotifySbCode(UINT uCode, int nPos)
{
    DUINMSLIDER nms;
    nms.hdr.code     = DUINM_SLIDER;
    nms.hdr.idFrom   = GetCmdID();
    nms.hdr.hwndFrom = NULL;
    nms.uSbCode      = uCode;
    nms.pSliderBar   = this;
    nms.nPos         = nPos;
    nms.bVertical    = IsVertical();

    return DuiNotify((LPNMHDR)&nms);
}

LRESULT CDuiSliderBar::OnCalcSize( BOOL bCalcValidRects, LPSIZE pSize )
{
	__super::OnCalcSize(bCalcValidRects, pSize);
	
	if(m_dlgpos.nCount!=4)
	{
		SIZE sizeBg = m_pSkinBg->GetSkinSize();
		SIZE sizeThumb= m_pSkinThumb->GetSkinSize();

		if(IsVertical())
		{
			if(sizeThumb.cx>sizeBg.cx) pSize->cx+=sizeThumb.cx-sizeBg.cx;
		}else
		{
			if(sizeThumb.cy>sizeBg.cy) pSize->cy+=sizeThumb.cy-sizeBg.cy;
		}
	}
	return 0;
}

}//end of namespace