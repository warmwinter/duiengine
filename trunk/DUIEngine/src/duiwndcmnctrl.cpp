//////////////////////////////////////////////////////////////////////////
//   File Name: bkwndcmnctrl.h
// Description: DuiWindow Definition
//     Creator: Zhang Xiaoxuan
//     Version: 2009.04.28 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "duistd.h"
#include "duiwndcmnctrl.h"

#include "duiimage.h"
#include "duitheme.h"
#include "duiwndnotify.h"
#include "duisystem.h"

namespace DuiEngine{


//////////////////////////////////////////////////////////////////////////
// Static Control
// 
void CDuiStatic::DuiDrawText(HDC hdc,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
	if(!m_nMultiLines) 
	{
		__super::DuiDrawText(hdc,pszBuf,cchText,pRect,uFormat);
	}else
	{
		SIZE szChar;
		int i=0, nLine=1;
		if(cchText==-1) cchText=_tcslen(pszBuf);
		LPCTSTR p1=pszBuf;
		POINT pt={pRect->left,pRect->top};
		GetTextExtentPoint(hdc,_T("A"),1,&szChar);
		int nLineHei=szChar.cy;
		const TCHAR *ELLIPSIS_DOTS = _T("...");
		while(i<cchText)
		{
			LPTSTR p2=CharNext(p1);
			if(*p1==_T('\\') && p2 && *p2==_T('n'))
			{
				pt.y+=nLineHei+m_nLineInter;
				pt.x=pRect->left;
				nLine++;
				i+=p2-p1;
				p1=CharNext(p2);
				i+=p1-p2;
				continue;
			}
			GetTextExtentPoint(hdc,p1,p2-p1,&szChar);
			if(pt.x+szChar.cx > pRect->right)
			{
				pt.y+=nLineHei+m_nLineInter;
				pt.x=pRect->left;
				nLine++;
				continue;
			}
			if(!(uFormat & DT_CALCRECT))
			{
				if (nLine > 0 && nLine == m_nMultiLines && (pt.x+szChar.cx) > (pRect->right-30))
				{
					CGdiAlpha::TextOut(hdc,pt.x,pt.y,ELLIPSIS_DOTS,lstrlen(ELLIPSIS_DOTS));
					return;
				}
				CGdiAlpha::TextOut(hdc,pt.x,pt.y,p1,p2-p1);
			}
			pt.x+=szChar.cx;
			i+=p2-p1;
			p1=p2;
		}
		if(!(uFormat & DT_CALCRECT))
		{
			pRect->bottom=pt.y+nLineHei;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Link Control
// Only For Header Drag Test
// Usage: <link>inner text example</link>
//

void CDuiLink::DuiDrawText(HDC hdc,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
	if(!(uFormat&DT_CALCRECT))
	{
		CRect rc(pRect);
		rc.bottom=rc.top;
		DrawText(hdc,pszBuf,cchText,&rc,DT_LEFT|DT_CALCRECT);
		m_rcText=rc;
		m_rcText.right=min(rc.right,pRect->right);
		if(m_style.m_nTextAlign&DT_VCENTER)
		{
			m_rcText.top=pRect->top+ (pRect->bottom-pRect->top-rc.Height())/2;
			m_rcText.bottom=m_rcText.top+rc.Height();
		}else if(m_style.m_nTextAlign&DT_BOTTOM)
		{
			m_rcText.bottom=m_rcText.bottom;
			m_rcText.top=m_rcText.bottom-rc.Height();
		}else
		{
			m_rcText.top=m_rcText.top;
			m_rcText.bottom=m_rcText.top+rc.Height();
		}
	}
	__super::DuiDrawText(hdc,pszBuf,cchText,pRect,uFormat);
}

void CDuiLink::OnAttributeFinish( TiXmlElement* pXmlElem )
{
	if(m_strToolTipText.IsEmpty()) m_strToolTipText=m_strLinkUrl;
}

BOOL CDuiLink::OnDuiSetCursor(const CPoint &pt)
{
	if(!m_rcText.PtInRect(pt)) return FALSE;
	HCURSOR hCur = ::LoadCursor(NULL, m_style.m_lpCursorName);
	::SetCursor(hCur);
	return TRUE;
}

void CDuiLink::OnLButtonDown( UINT nFlags,CPoint pt )
{
	if(!m_rcText.PtInRect(pt)) return;
	__super::OnLButtonDown(nFlags,pt);
}

void CDuiLink::OnLButtonUp( UINT nFlags,CPoint pt )
{
	if(!m_rcText.PtInRect(pt))
	{
		ReleaseDuiCapture();
		return;
	}
	__super::OnLButtonUp(nFlags,pt);
}

void CDuiLink::OnMouseMove( UINT nFlags,CPoint pt )
{
	if(!m_rcText.PtInRect(pt))
	{
		if(m_dwState&DuiWndState_Hover) OnMouseLeave();
	}else
	{
		if(!(m_dwState&DuiWndState_Hover)) OnMouseHover(nFlags,pt);
	}
}

void CDuiLink::OnMouseHover( WPARAM wParam, CPoint pt )
{
	if(!m_rcText.PtInRect(pt)) return;
	__super::OnMouseHover(wParam,pt);
}
//////////////////////////////////////////////////////////////////////////
// Button Control
// Use id attribute to process click event
// 
// Usage: <button id=xx>inner text example</button>
//

void CDuiButton::OnPaint(CDCHandle dc)
{	
	if (m_pBgSkin)
	{
		m_pBgSkin->Draw(dc, m_rcWindow, IIF_STATE4(GetState(), 0, 1, 2, 3));			
	}

	__super::OnPaint(dc);
}

void CDuiButton::OnLButtonDown(UINT uFlag,CPoint pt)
{
	__super::OnLButtonDown(uFlag,pt);
	SetDuiFocus();
}

void CDuiButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_SPACE && GetCmdID())
	{
		DUINMCOMMAND nms;
		nms.hdr.code = DUINM_COMMAND;
		nms.hdr.hwndFrom = NULL;
		nms.hdr.idFrom = GetCmdID();
		nms.uItemID = GetCmdID();
		nms.szItemClass = GetObjectClass();
		nms.uItemData = GetCmdData(); 
		DuiNotify((LPNMHDR)&nms);
	}
}

//////////////////////////////////////////////////////////////////////////
// Image Control
// Use src attribute specify a resource id
// 
// Usage: <img skin="skin" sub="0"/>
//
CDuiImageWnd::CDuiImageWnd()
: m_nSubImageID(0)
, m_pSkin(NULL)	
, m_bManaged(FALSE)
{
	m_bMsgTransparent=TRUE;
}

CDuiImageWnd::~CDuiImageWnd()
{
	if(m_bManaged && m_pSkin)
	{
		m_pSkin->Release();
	}
	m_pSkin=NULL;
}

void CDuiImageWnd::OnPaint(CDCHandle dc)
{
	if (m_pSkin)
		m_pSkin->Draw(dc, m_rcWindow, m_nSubImageID);
}

LRESULT CDuiImageWnd::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);


	if (m_pSkin && (m_dlgpos.nCount==2 || m_dlgpos.nCount==0))	// ������õ�pos������2�������¼�����δ�С
	{
		SIZE sizeImage;
		sizeImage = m_pSkin->GetSkinSize();

		if (sizeImage.cx)
			pSize->cx = sizeImage.cx;
		if (sizeImage.cy)
			pSize->cy = sizeImage.cy;

		return TRUE;
	}
	return 0;
}

BOOL CDuiImageWnd::SetSkin(CDuiSkinBase *pSkin,int nSubID/*=0*/)
{
	if(IsVisible(TRUE)) NotifyInvalidate();
	if(m_bManaged && m_pSkin)
	{
		m_pSkin->Release();
		m_bManaged=FALSE;
	}
	if(!pSkin) return FALSE;
	m_pSkin=pSkin;
	m_pSkin->AddRef();
	m_bManaged=TRUE;
	m_nSubImageID=nSubID;
	if(m_dlgpos.nCount==2 && m_pParent)
	{//���¼�������
		m_pParent->DuiSendMessage(WM_CALCWNDPOS,0,(LPARAM)this);
	}
	if(IsVisible(TRUE)) NotifyInvalidate();
	return TRUE;
}

BOOL CDuiImageWnd::SetIcon( int nSubID )
{
	if(!m_pSkin) return FALSE;
	if(nSubID<0 || nSubID>m_pSkin->GetStates()-1) return FALSE;
	m_nSubImageID=nSubID;
	return TRUE;
}

CDuiAnimateImgWnd::CDuiAnimateImgWnd():m_pSkin(NULL),m_iCurFrame(0),m_nSpeed(50),m_bAutoStart(TRUE)
{
	m_bMsgTransparent=TRUE;
}

void CDuiAnimateImgWnd::OnPaint(CDCHandle dc)
{
	if (m_pSkin)
		m_pSkin->Draw(dc, m_rcWindow, m_iCurFrame);
}

LRESULT CDuiAnimateImgWnd::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);

	if (m_pSkin && m_dlgpos.nCount==2)	// ������õ�pos������2�������¼�����δ�С
	{
		SIZE sizeImage;
		sizeImage = m_pSkin->GetSkinSize();

		if (sizeImage.cx)
			pSize->cx = sizeImage.cx;
		if (sizeImage.cy)
			pSize->cy = sizeImage.cy;

		return TRUE;
	}
	return 0;
}

void CDuiAnimateImgWnd::OnDuiTimer(char cID)
{
	if(!IsVisible(TRUE)) return;
	if(!m_pSkin) KillDuiTimer(cID);
	else
	{
		int nStates=m_pSkin->GetStates();
		m_iCurFrame++;
		m_iCurFrame%=nStates;
		NotifyInvalidate();
	}
}

void CDuiAnimateImgWnd::Start(){ SetDuiTimer(1,m_nSpeed);}
void CDuiAnimateImgWnd::Stop(){KillDuiTimer(1);}

void CDuiAnimateImgWnd::OnDestroy()
{
	Stop();
}

BOOL CDuiAnimateImgWnd::Load(TiXmlElement* pTiXmlElem)
{
	BOOL bRet=__super::Load(pTiXmlElem);
	if(m_bAutoStart) Start();
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// Progress Control
// Use id attribute to process click event
// 
// Usage: <progress bgskin=xx posskin=xx min=0 max=100 value=10,showpercent=0/>
//

CDuiProgress::CDuiProgress()
: m_dwMinValue(0)
, m_dwMaxValue(0)
, m_dwValue(0)
, m_dwRight(0)
, m_bShowPercent(FALSE)
, m_pSkinBg(NULL)
, m_pSkinPos(NULL)
{

}


LRESULT CDuiProgress::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);

	if (m_pSkinBg)
	{
		SIZE sizeBg = m_pSkinBg->GetSkinSize();

		pSize->cy = sizeBg.cy;

		return TRUE;
	}

	return TRUE;
}

void CDuiProgress::OnPaint(CDCHandle dc)
{
	DuiDCPaint DuiDC;
	CPen penFrame;
	CString strPercent;
	double dPos = 0;

	BeforePaint(dc, DuiDC);

	penFrame.CreatePen(
		PS_SOLID, 
		1, 
		RGB(0x70, 0x70, 0x70)
		);

	HPEN hpenOld = dc.SelectPen(penFrame);

	if (m_pSkinBg)
	{
		m_pSkinBg->Draw(dc, m_rcWindow, DuiWndState_Normal);
	}
	else
	{
		CGdiAlpha::RoundRect(dc,m_rcWindow, CPoint(2, 2));
	}

	if (m_dwMaxValue == m_dwMinValue)
	{
		dPos = 0;
	}
	else
	{
		if (m_dwMaxValue < m_dwMinValue)
		{
			DWORD dwTemp = m_dwMaxValue;
			m_dwMaxValue = m_dwMinValue;
			m_dwMinValue = dwTemp;
		}

		if (m_dwValue < m_dwMinValue)
		{
			m_dwValue = m_dwMinValue;
		}
		else if (m_dwValue > m_dwMaxValue)
		{
			m_dwValue = m_dwMaxValue;
		}

		dPos = (double)(m_dwValue - m_dwMinValue) / (double)(m_dwMaxValue - m_dwMinValue);
	}

	if (m_bShowPercent)
	{
		strPercent.Format(_T("%d%%"), (int)(dPos * 100));
		CGdiAlpha::DrawText(dc,strPercent, strPercent.GetLength(), m_rcWindow, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	CRect rcPosBar = m_rcWindow;

	if (m_pSkinPos)
	{
		if (dPos > 0)
		{			
			SIZE sizePosBar = m_pSkinPos->GetSkinSize();

			rcPosBar.DeflateRect(2, (m_rcWindow.Height() - sizePosBar.cy) / 2);
			m_pSkinPos->SetWidth(rcPosBar.Width());			
			LONG lOldWidth = rcPosBar.Width();
			rcPosBar.right = rcPosBar.left + (LONG)(rcPosBar.Width() * dPos);

			if (m_dwRight > 0 && (DWORD)rcPosBar.Width() < (DWORD)(lOldWidth - m_dwRight))
			{
				m_pSkinPos->SetAttribute("part", "left", TRUE);
			}
			else
			{
				m_pSkinPos->SetAttribute("part", "all", TRUE);
			}

			m_pSkinPos->Draw(dc, rcPosBar, DuiWndState_Normal);
		}
	}
	else
	{
		rcPosBar.DeflateRect(2, 2);
		rcPosBar.right = rcPosBar.left + (LONG)(rcPosBar.Width() * dPos);

		CGdiAlpha::RoundRect(dc,rcPosBar, CPoint(2, 2));

		CGdiAlpha::FillSolidRect(dc,rcPosBar, RGB(66, 97, 144));
	}

	dc.SelectPen(hpenOld);

	if (m_bShowPercent)
	{
		CRgn rgnClipOld, rgnClip;
		rgnClip.CreateRectRgnIndirect(rcPosBar);
		dc.GetClipRgn(rgnClipOld);
		dc.SelectClipRgn(rgnClip);

		if (CLR_INVALID == m_style.m_crBg)
			dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
		else
			dc.SetTextColor(m_style.m_crBg);

		CGdiAlpha::DrawText(dc,strPercent, strPercent.GetLength(), m_rcWindow, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		dc.SetTextColor(m_style.m_crText);
		dc.SelectClipRgn(rgnClipOld);
	}

	AfterPaint(dc, DuiDC);
}

BOOL CDuiProgress::SetValue(DWORD dwValue)
{
	if(dwValue>=m_dwMinValue && dwValue<=m_dwMaxValue)
	{
		m_dwValue=dwValue;
		NotifyInvalidate();
		return TRUE;
	}
	return FALSE;
}

DWORD CDuiProgress::GetValue(){return m_dwValue;}

//////////////////////////////////////////////////////////////////////////
// Image Button Control
// 
// Usage: <imgbtn src=xx />
//
CDuiImageBtnWnd::CDuiImageBtnWnd():m_bAnimate(FALSE),m_byAlpha(0xFF)
{
	m_bMsgTransparent=FALSE;
}

BOOL CDuiImageBtnWnd::NeedRedrawWhenStateChange()
{
	return TRUE;
}

void CDuiImageBtnWnd::OnPaint(CDCHandle dc)
{
	if (!m_pSkin) return;
	if(m_byAlpha==0xFF || !m_bAnimate)
	{
		m_pSkin->Draw(
			dc, m_rcWindow, 
			IIF_STATE4(GetState(), 0, 1, 2, 3),0xFF
			);
	}else
	{
		if(GetState()&DuiWndState_Hover)
		{//get hover
			/*m_pSkin->Draw(
			dc, m_rcWindow, 
			0,0xFF-m_byAlpha
			);*/
			m_pSkin->Draw(
				dc, m_rcWindow, 
				1,m_byAlpha
				);
		}else
		{//lose hover
			m_pSkin->Draw(
				dc, m_rcWindow, 
				0,m_byAlpha
				);
		}
	}
}

void CDuiImageBtnWnd::OnStateChanged(DWORD dwOldState,DWORD dwNewState)
{
	m_byAlpha=0xFF;
	KillDuiTimer(1);
	if(GetDuiCapture()==m_hDuiWnd) return;
	if(m_bAnimate && 
		((dwOldState==DuiWndState_Normal && dwNewState==DuiWndState_Hover)
		||(dwOldState==DuiWndState_Hover && dwNewState==DuiWndState_Normal)))
	{
		m_byAlpha=5;
		SetDuiTimer(1,10);
	}
}
void CDuiImageBtnWnd::OnDuiTimer(char cTimerID)
{
	m_byAlpha+=25;
	if(m_byAlpha==0xFF) KillDuiTimer(cTimerID);
	NotifyInvalidate();
}

//////////////////////////////////////////////////////////////////////////
// Line Control
// Simple HTML "HR" tag
//
// Usage: <hr style=solid size=1 crbg=.../>
//

CDuiLine::CDuiLine()
: m_nPenStyle(PS_SOLID)
, m_nLineSize(1)
, m_bLineShadow(FALSE)
, m_crShadow(RGB(0xFF, 0xFF, 0xFF))
{
}

// Do nothing
void CDuiLine::OnPaint(CDCHandle dc)
{
	CGdiAlpha::DrawLine(dc,m_rcWindow.left,m_rcWindow.top,m_rcWindow.right,m_rcWindow.bottom,m_style.m_crBg,m_nPenStyle);

	// ����Ӱ��
	if (m_bLineShadow)
	{		
		if ((m_rcWindow.left+m_nLineSize-1) == m_rcWindow.right)
		{
			CGdiAlpha::DrawLine(dc,m_rcWindow.right+1,m_rcWindow.top,m_rcWindow.right+1,m_rcWindow.bottom,m_crShadow,m_nPenStyle);
		}else
		{
			CGdiAlpha::DrawLine(dc,m_rcWindow.left,m_rcWindow.bottom+1,m_rcWindow.right,m_rcWindow.bottom+1,m_crShadow,m_nPenStyle);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Check Box
//
// Usage: <check state=1>This is a check-box</check>
//

CDuiCheckBox::CDuiCheckBox()
: m_pSkin(DuiSkinPool::getSingleton().GetSkin("btncheckbox"))
, m_pFocusSkin(DuiSkinPool::getSingleton().GetSkin("focuscheckbox"))
{
}

void CDuiCheckBox::GetClient(LPRECT pRect)
{
	*pRect=m_rcWindow;
	pRect->left+=CheckBoxSize+CheckBoxSpacing;
}

void CDuiCheckBox::OnPaint(CDCHandle dc)
{
	CRect rcCheckBox(0,0,CheckBoxSize,CheckBoxSize);
	rcCheckBox.MoveToXY(m_rcWindow.left,m_rcWindow.top+(m_rcWindow.Height()-CheckBoxSize)/2);

	m_pSkin->Draw(dc, rcCheckBox, _GetDrawState());

	__super::OnPaint(dc);
}

void CDuiCheckBox::DuiDrawFocus( HDC hdc )
{
	if(!m_pFocusSkin) return;
	CRect rcCheckBox(0,0,CheckBoxSize,CheckBoxSize);
	rcCheckBox.MoveToXY(m_rcWindow.left,m_rcWindow.top+(m_rcWindow.Height()-CheckBoxSize)/2);
	m_pFocusSkin->Draw(hdc,rcCheckBox,0);
}

LRESULT CDuiCheckBox::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);

	pSize->cx += CheckBoxSize + CheckBoxSpacing;
	pSize->cy = max(pSize->cy, CheckBoxSize);

	return FALSE;
}


UINT CDuiCheckBox::_GetDrawState()
{
	DWORD dwState = GetState();
	UINT uState = 0;

	if (m_pSkin)
	{
		if (dwState & DuiWndState_Check)
		{
			if (dwState & DuiWndState_Disable)
				uState = CBS_CHECKEDDISABLED;
			else if (dwState & DuiWndState_PushDown)
				uState = CBS_CHECKEDPRESSED;
			else if (dwState & DuiWndState_Hover)
				uState = CBS_CHECKEDHOT;
			else
				uState = CBS_CHECKEDNORMAL;
		}
		else
		{
			if (dwState & DuiWndState_Disable)
				uState = CBS_UNCHECKEDDISABLED;
			else if (dwState & DuiWndState_PushDown)
				uState = CBS_UNCHECKEDPRESSED;
			else if (dwState & DuiWndState_Hover)
				uState = CBS_UNCHECKEDHOT;
			else
				uState = CBS_UNCHECKEDNORMAL;
		}
	}

	--uState;	// ��1

	return uState;
}

void CDuiCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetDuiFocus();
	__super::OnLButtonDown(nFlags,point);
}

void CDuiCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsChecked())
		ModifyState(0, DuiWndState_Check,TRUE);
	else
		ModifyState(DuiWndState_Check, 0,TRUE);
	__super::OnLButtonUp(nFlags,point);
}

void CDuiCheckBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_SPACE)
	{
		if (IsChecked())
			ModifyState(0, DuiWndState_Check,TRUE);
		else
			ModifyState(DuiWndState_Check, 0,TRUE);

		if (GetCmdID())
		{
			DUINMCOMMAND nms;
			nms.hdr.code = DUINM_COMMAND;
			nms.hdr.hwndFrom = NULL;
			nms.hdr.idFrom = GetCmdID();
			nms.uItemID = GetCmdID();
			nms.szItemClass = GetObjectClass();
			nms.uItemData = GetCmdData(); 
			DuiNotify((LPNMHDR)&nms);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Icon Control
// Use src attribute specify a resource id
// 
// Usage: <icon src=xx oem="0" size="16"/>
//
CDuiIconWnd::CDuiIconWnd()
: m_uResID(-1)
, m_nSize(16)
{

}

BOOL CDuiIconWnd::Load(TiXmlElement* pTiXmlElem)
{
	if (!CDuiWindow::Load(pTiXmlElem))
		return FALSE;

	_ReloadIcon();

	return TRUE;
}

void CDuiIconWnd::OnPaint(CDCHandle dc)
{
	if (m_uResIDCurrent != m_uResID)
		_ReloadIcon();

	CRect rcClient;
	GetClient(&rcClient);
	if(GetContainer()->IsTranslucent())
	{
		Gdiplus::Bitmap * pIcon=Gdiplus::Bitmap::FromHICON(m_theIcon);
		Gdiplus::Graphics g(dc);
		g.DrawImage(pIcon,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height());
		delete pIcon;
	}else
	{
		DrawIconEx(dc, rcClient.left,rcClient.top,m_theIcon,rcClient.Width(),rcClient.Height(),0,NULL,DI_NORMAL);
	}
}

LRESULT CDuiIconWnd::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	pSize->cx = m_nSize;
	pSize->cy = m_nSize;

	return TRUE;
}

HICON CDuiIconWnd::AttachIcon(HICON hIcon)
{
	HICON tmp = m_theIcon; 
	m_theIcon = hIcon;
	return tmp; 
}

void CDuiIconWnd::LoadIconFile( LPCWSTR lpFIleName )
{
	if( m_theIcon ) DestroyIcon(m_theIcon);
	HICON hIcon = (HICON)LoadImage(NULL, lpFIleName, IMAGE_ICON, m_nSize, m_nSize, LR_LOADFROMFILE );
	m_theIcon = hIcon;
}

void CDuiIconWnd::_ReloadIcon()
{
	if (m_theIcon)		DestroyIcon(m_theIcon);
	m_theIcon=DuiSystem::getSingleton().GetResProvider()->LoadIcon(DUIRES_ICON_TYPE,m_uResID,m_nSize,m_nSize);
	if(m_theIcon) m_uResIDCurrent = m_uResID;
}

//////////////////////////////////////////////////////////////////////////
// Radio Box
//
// Usage: <radio state=1>This is a check-box</radio>
//

CDuiRadioBox::CDuiRadioBox()
: m_pSkin(DuiSkinPool::getSingleton().GetSkin("btnRadio"))
, m_pFocusSkin(DuiSkinPool::getSingleton().GetSkin("focusRadio"))
{
}

void CDuiRadioBox::GetClient(LPRECT pRect)
{
	*pRect=m_rcWindow;
	pRect->left+=RadioBoxSize+RadioBoxSpacing;
}

void CDuiRadioBox::OnPaint(CDCHandle dc)
{
	CRect rcRadioBox(0, 0, RadioBoxSize, RadioBoxSize);

	rcRadioBox.MoveToXY(m_rcWindow.left, m_rcWindow.top + (m_rcWindow.Height() - RadioBoxSize) / 2);

	if (m_pSkin)
	{
		m_pSkin->Draw(dc, rcRadioBox, _GetDrawState());
	}
	__super::OnPaint(dc);
}

void CDuiRadioBox::DuiDrawFocus(HDC hdc)
{
	if(!m_pFocusSkin) return;
	CRect rcCheckBox(0,0,RadioBoxSize,RadioBoxSize);
	rcCheckBox.MoveToXY(m_rcWindow.left,m_rcWindow.top+(m_rcWindow.Height()-RadioBoxSize)/2);
	m_pFocusSkin->Draw(hdc,rcCheckBox,0);
}

LRESULT CDuiRadioBox::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);

	pSize->cx += RadioBoxSize + RadioBoxSpacing;
	pSize->cy = max(pSize->cy, RadioBoxSize);

	return FALSE;
}


UINT CDuiRadioBox::_GetDrawState()
{
	DWORD dwState = GetState();
	UINT uState = 0;

	if (dwState & DuiWndState_Check)
	{
		if (dwState & DuiWndState_Disable)
			uState = RBS_CHECKEDDISABLED;
		else if (dwState & DuiWndState_PushDown)
			uState = RBS_CHECKEDPRESSED;
		else if (dwState & DuiWndState_Hover)
			uState = RBS_CHECKEDHOT;
		else
			uState = RBS_CHECKEDNORMAL;
	}
	else
	{
		if (dwState & DuiWndState_Disable)
			uState = RBS_UNCHECKEDDISABLED;
		else if (dwState & DuiWndState_PushDown)
			uState = RBS_UNCHECKEDPRESSED;
		else if (dwState & DuiWndState_Hover)
			uState = RBS_UNCHECKEDHOT;
		else
			uState = RBS_UNCHECKEDNORMAL;
	}

	--uState;

	return uState;
}

BOOL CDuiRadioBox::NeedRedrawWhenStateChange()
{
	return TRUE;
}

void CDuiRadioBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetDuiFocus();
	__super::OnLButtonDown(nFlags,point);
}


void CDuiRadioBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags,point);
	CDuiWindow *pParent=GetParent();
	if(pParent->IsClass("div"))
	{
		pParent->CheckRadioButton(this);
	}else
	{
		DUIASSERT(FALSE);
	}
}

void CDuiRadioBox::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(nChar==VK_SPACE)
	{
		CDuiWindow *pParent=GetParent();
		if(pParent->IsClass("div"))
		{
			pParent->CheckRadioButton(this);

			if (GetCmdID())
			{
				DUINMCOMMAND nms;
				nms.hdr.code = DUINM_COMMAND;
				nms.hdr.hwndFrom = NULL;
				nms.hdr.idFrom = GetCmdID();
				nms.uItemID = GetCmdID();
				nms.szItemClass = GetObjectClass();
				nms.uItemData = GetCmdData(); 
				DuiNotify((LPNMHDR)&nms);
			}
		}else
		{
			DUIASSERT(FALSE);
		}

	}
}

//////////////////////////////////////////////////////////////////////////
// CDuiToggle
CDuiToggle::CDuiToggle():m_bToggled(FALSE)
{

}

void CDuiToggle::SetToggle(BOOL bToggle,BOOL bUpdate/*=TRUE*/)
{
	m_bToggled=bToggle;
	if(bUpdate) NotifyInvalidate();
}

BOOL CDuiToggle::GetToggle(){return m_bToggled;}

void CDuiToggle::OnPaint(CDCHandle dc)
{
	DUIASSERT(m_pBgSkin);
	DWORD nState=0;
	if(GetState()&DuiWndState_Hover) nState=2;
	else if(GetState()&DuiWndState_Check) nState=3;
	if(m_bToggled) nState+=3;
	m_pBgSkin->Draw(dc,m_rcWindow,nState);
}

void CDuiToggle::OnLButtonUp(UINT nFlags,CPoint pt)
{
	m_bToggled=!m_bToggled;
	__super::OnLButtonUp(nFlags,pt);
}

LRESULT CDuiToggle::OnCalcSize(BOOL bCalcValidRects, LPSIZE pSize)
{
	__super::OnCalcSize(bCalcValidRects, pSize);

	if (m_pBgSkin && m_dlgpos.nCount==2)	// ������õ�pos������2�������¼�����δ�С
	{
		SIZE sizeImage;
		sizeImage = m_pBgSkin->GetSkinSize();

		if (sizeImage.cx)
			pSize->cx = sizeImage.cx;
		if (sizeImage.cy)
			pSize->cy = sizeImage.cy;

		return TRUE;
	}
	return 0;
}


#define GROUP_HEADER		20
#define GROUP_ROUNDCORNOR	4

CDuiGroup::CDuiGroup():m_nRound(GROUP_ROUNDCORNOR),m_crLine1(RGB(0xF0,0xF0,0xF0)),m_crLine2(RGB(0xA0,0xA0,0xA0))
{

}
void CDuiGroup::OnPaint(CDCHandle dc)
{

	DuiDCPaint DuiDC;

	BeforePaint(dc, DuiDC);

	CSize szFnt;
	dc.GetTextExtent(m_strInnerText, m_strInnerText.GetLength(),&szFnt);

	CRect rcText=m_rcWindow;
	rcText.left+=GROUP_HEADER,rcText.right-=GROUP_HEADER;
	rcText.bottom=rcText.top+szFnt.cy+2;
	if(m_style.m_nTextAlign & DT_CENTER)
	{
		rcText.left+=(rcText.Width()-szFnt.cx)/2;
		rcText.right=rcText.left+szFnt.cx;
	}else if(m_style.m_nTextAlign & DT_RIGHT)
	{
		rcText.left=rcText.right-szFnt.cx;
	}else
	{
		rcText.right=rcText.left+szFnt.cx;
	}

	CRgnHandle hRgn;
	int nSavedDC=dc.SaveDC();
	if(!m_strInnerText.IsEmpty())
	{
		CRect rcClip=rcText;
		rcClip.InflateRect(5,5,5,5);
		hRgn.CreateRectRgnIndirect(&rcClip);
		dc.SelectClipRgn(hRgn,RGN_DIFF);
		hRgn.DeleteObject();
	}

	{
		CRect rcGroupBox = m_rcWindow;

		if(!m_strInnerText.IsEmpty()) rcGroupBox.top+=szFnt.cy/2;
		rcGroupBox.DeflateRect(1,1,1,0);
		CPenHandle pen1,pen2;
		CPen oldPen;
		pen1.CreatePen(PS_SOLID,1,m_crLine1);
		pen2.CreatePen(PS_SOLID,1,m_crLine2);
		oldPen=dc.SelectPen(pen1);
		CBrush oldBr=dc.SelectBrush((HBRUSH)GetStockObject(NULL_BRUSH));
		CGdiAlpha::RoundRect(dc,rcGroupBox,CPoint(m_nRound,m_nRound));
		dc.SelectPen(pen2);
		rcGroupBox.InflateRect(1,1,1,-1);
		CGdiAlpha::RoundRect(dc,rcGroupBox,CPoint(m_nRound,m_nRound));
		dc.SelectBrush(oldBr);

		dc.SelectPen(oldPen);
	}

	dc.RestoreDC(nSavedDC);
	if(!m_strInnerText.IsEmpty())
	{
		CGdiAlpha::DrawText(dc,m_strInnerText, m_strInnerText.GetLength(), rcText, DT_SINGLELINE|DT_VCENTER);
	}

	AfterPaint(dc, DuiDC);
}

}//namespace DuiEngine