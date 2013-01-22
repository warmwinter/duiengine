#pragma once

#include <Richedit.h>
#include <TextServ.h>
#include "duiwndpanel.h"

namespace DuiEngine
{

class CDuiRichEdit;

class DUI_EXP CDuiTextHost : public ITextHost
{
    friend class CDuiRichEdit;
public:
    CDuiTextHost(void);
    ~CDuiTextHost(void);

    BOOL Init(CDuiRichEdit* pDuiRichEdit,LPCWSTR pszText);

    ITextServices * GetTextService()
    {
        return pserv;
    }
protected:

    // -----------------------------
    //	IUnknown interface
    // -----------------------------
    virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG _stdcall AddRef(void);
    virtual ULONG _stdcall Release(void);

    // -----------------------------
    //	ITextHost interface
    // -----------------------------

    //@cmember Get the DC for the host
    virtual HDC 		TxGetDC();

    //@cmember Release the DC gotten from the host
    virtual INT			TxReleaseDC(HDC hdc);

    //@cmember Show the scroll bar
    virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);

    //@cmember Enable the scroll bar
    virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);

    //@cmember Set the scroll range
    virtual BOOL 		TxSetScrollRange(
        INT fnBar,
        LONG nMinPos,
        INT nMaxPos,
        BOOL fRedraw);

    //@cmember Set the scroll position
    virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);

    //@cmember InvalidateRect
    virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);

    //@cmember Send a WM_PAINT to the window
    virtual void 		TxViewChange(BOOL fUpdate);

    //@cmember Create the caret
    virtual BOOL		TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);

    //@cmember Show the caret
    virtual BOOL		TxShowCaret(BOOL fShow);

    //@cmember Set the caret position
    virtual BOOL		TxSetCaretPos(INT x, INT y);

    //@cmember Create a timer with the specified timeout
    virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);

    //@cmember Destroy a timer
    virtual void 		TxKillTimer(UINT idTimer);

    //@cmember Scroll the content of the specified window's client area
    virtual void		TxScrollWindowEx (
        INT dx,
        INT dy,
        LPCRECT lprcScroll,
        LPCRECT lprcClip,
        HRGN hrgnUpdate,
        LPRECT lprcUpdate,
        UINT fuScroll);

    //@cmember Get mouse capture
    virtual void		TxSetCapture(BOOL fCapture);

    //@cmember Set the focus to the text window
    virtual void		TxSetFocus();

    //@cmember Establish a new cursor shape
    virtual void 	TxSetCursor(HCURSOR hcur, BOOL fText);

    //@cmember Converts screen coordinates of a specified point to the client coordinates
    virtual BOOL 		TxScreenToClient (LPPOINT lppt);

    //@cmember Converts the client coordinates of a specified point to screen coordinates
    virtual BOOL		TxClientToScreen (LPPOINT lppt);

    //@cmember Request host to activate text services
    virtual HRESULT		TxActivate( LONG * plOldState );

    //@cmember Request host to deactivate text services
    virtual HRESULT		TxDeactivate( LONG lNewState );

    //@cmember Retrieves the coordinates of a window's client area
    virtual HRESULT		TxGetClientRect(LPRECT prc);

    //@cmember Get the view rectangle relative to the inset
    virtual HRESULT		TxGetViewInset(LPRECT prc);

    //@cmember Get the default character format for the text
    virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );

    //@cmember Get the default paragraph format for the text
    virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);

    //@cmember Get the background color for the window
    virtual COLORREF	TxGetSysColor(int nIndex);

    //@cmember Get the background (either opaque or transparent)
    virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);

    //@cmember Get the maximum length for the text
    virtual HRESULT		TxGetMaxLength(DWORD *plength);

    //@cmember Get the bits representing requested scroll bars for the window
    virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar);

    //@cmember Get the character to display for password input
    virtual HRESULT		TxGetPasswordChar(TCHAR *pch);

    //@cmember Get the accelerator character
    virtual HRESULT		TxGetAcceleratorPos(LONG *pcp);

    //@cmember Get the native size
    virtual HRESULT		TxGetExtent(LPSIZEL lpExtent);

    //@cmember Notify host that default character format has changed
    virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf);

    //@cmember Notify host that default paragraph format has changed
    virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf);

    //@cmember Bulk access to bit properties
    virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);

    //@cmember Notify host of events
    virtual HRESULT		TxNotify(DWORD iNotify, void *pv);

    // Far East Methods for getting the Input Context
    //#ifdef WIN95_IME
    virtual HIMC		TxImmGetContext();
    virtual void		TxImmReleaseContext( HIMC himc );
    //#endif

    //@cmember Returns HIMETRIC size of the control bar.
    virtual HRESULT		TxGetSelectionBarWidth (LONG *plSelBarWidth);
protected:
    void RegisterDragDrop(void);
    void RevokeDragDrop(void);

    BOOL fRegisteredForDrop;

    ULONG	cRefs;					// Reference Count
    ITextServices	*pserv;		    // pointer to Text Services object
    CDuiRichEdit	*m_pDuiRichEdit;// duiwindow for text host
};

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

class DUI_EXP CDuiRichEdit :public CDuiPanelEx
{
    friend class CDuiTextHost;
public:
    DUIOBJ_DECLARE_CLASS_NAME(CDuiRichEdit, "richedit")

    CDuiRichEdit();
    virtual ~CDuiRichEdit() {}

public://richedit interface
    int GetWindowText(LPTSTR lpString, int nMaxCount );
    int GetWindowTextLength();
    BOOL SetWindowText(LPCTSTR lpszText);
    void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE);
    void ReplaceSel(LPWSTR pszText,BOOL bCanUndo=TRUE);

    BOOL GetWordWrap(void);
    void SetWordWrap(BOOL fWordWrap);

    BOOL GetReadOnly();
    BOOL SetReadOnly(BOOL bReadOnly);

    LONG GetLimitText();
    BOOL SetLimitText(int nLength);

    WORD GetDefaultAlign();
    void SetDefaultAlign(WORD wNewAlign);

    BOOL GetRichTextFlag();
    void SetRichTextFlag(BOOL fRich);

    LONG GetDefaultLeftIndent();
    void SetDefaultLeftIndent(LONG lNewIndent);

    BOOL SetSaveSelection(BOOL fSaveSelection);

    COLORREF SetDefaultTextColor(COLORREF cr);
protected:

    LRESULT OnCreate(LPVOID);

    void OnDestroy();

    void OnPaint(CDCHandle dc);

    void OnSetDuiFocus();

    void OnKillDuiFocus();

    void OnDuiTimer(char idEvent);

    void OnDuiTimerEx(UINT_PTR idEvent);

    virtual BOOL IsTabStop()
    {
        return TRUE;
    }

    virtual UINT OnGetDuiCode()
    {
        UINT uRet=DUIC_WANTCHARS|DUIC_WANTARROWS;
        if(m_fWantTab) uRet |= DLGC_WANTTAB;
        if(m_dwStyle&ES_WANTRETURN) uRet |= DUIC_WANTRETURN;
        return uRet;
    }

    virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);

    virtual BOOL OnDuiSetCursor(const CPoint &pt);

    virtual BOOL DuiWndProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT & lResult);

    HRESULT InitDefaultCharFormat(CHARFORMAT2W* pcf,HFONT hFont=NULL);

    HRESULT InitDefaultParaFormat(PARAFORMAT2* ppf);

    HRESULT OnTxInPlaceDeactivate();

    HRESULT OnTxInPlaceActivate( LPCRECT prcClient );

    HRESULT OnTxNotify(DWORD iNotify,LPVOID pv);

    virtual HRESULT DefAttributeProc(const CDuiStringA & strAttribName,const CDuiStringA & strValue, BOOL bLoading);

    void OnLButtonDown(UINT nFlags, CPoint point);

    void OnLButtonUp(UINT nFlags, CPoint point);

    void OnMouseMove(UINT nFlags, CPoint point);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnSetFont(HFONT font, BOOL bRedraw);

    LRESULT OnSetText(UINT uMsg,WPARAM wparam,LPARAM lparam);

    LRESULT OnSetCharFormat(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnSetParaFormat(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnSetReadOnly(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnSetLimitText(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);

    LRESULT OnSetTextColor(const CDuiStringA &  strValue,BOOL bLoading);
protected:
    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_CREATE(OnCreate)
    MSG_WM_DESTROY(OnDestroy)
    MSG_WM_PAINT(OnPaint)
    MSG_WM_NCCALCSIZE(OnNcCalcSize)
    MSG_WM_SETFOCUS_EX(OnSetDuiFocus)
    MSG_WM_KILLFOCUS_EX(OnKillDuiFocus)
    MSG_WM_DUITIMER(OnDuiTimer)
    MSG_UM_TIMEREX(OnDuiTimerEx)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_LBUTTONUP(OnLButtonUp)
    MSG_WM_MOUSEMOVE(OnMouseMove)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_CHAR(OnChar)
    MSG_WM_SETFONT(OnSetFont)
    MESSAGE_HANDLER_EX(WM_SETTEXT,OnSetText)
    MESSAGE_HANDLER_EX(EM_SETPARAFORMAT,OnSetParaFormat)
    MESSAGE_HANDLER_EX(EM_SETCHARFORMAT,OnSetCharFormat)
    MESSAGE_HANDLER_EX(EM_SETREADONLY,OnSetReadOnly)
    MESSAGE_HANDLER_EX(EM_EXLIMITTEXT,OnSetLimitText)
    DUIWIN_END_MSG_MAP()

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_INT_ATTRIBUTE("style",m_dwStyle,FALSE)
    DUIWIN_INT_ATTRIBUTE("maxbuf",m_cchTextMost,FALSE)
    DUIWIN_INT_ATTRIBUTE("transparent",m_fTransparent,FALSE)
    DUIWIN_INT_ATTRIBUTE("rich",m_fRich,FALSE)
    DUIWIN_INT_ATTRIBUTE("vertical",m_fVertical,FALSE)
    DUIWIN_INT_ATTRIBUTE("wordwrap",m_fWordWrap,FALSE)
    DUIWIN_INT_ATTRIBUTE("allowbeep",m_fAllowBeep,FALSE)
    DUIWIN_INT_ATTRIBUTE("autowordsel",m_fEnableAutoWordSel,FALSE)
    DUIWIN_INT_ATTRIBUTE("vcenter",m_fSingleLineVCenter,FALSE)
    DUIWIN_RECT_ATTRIBUTE("inset",m_rcInsetPixel,FALSE)
    DUIWIN_CUSTOM_ATTRIBUTE("crtext",OnSetTextColor)
    DUIWIN_DECLARE_ATTRIBUTES_END()
    //////////////////////////////////////////////////////////////////////////
    //	RichEdit Properties
    CHARFORMAT2W m_cfDef;				// Default character format
    PARAFORMAT2	m_pfDef;			    // Default paragraph format
    DWORD m_cchTextMost;					// Maximize Characters
    TCHAR m_chPasswordChar;				// Password character
    LONG		m_lAccelPos;			// Accelerator position
    SIZEL		m_sizelExtent;			// Extent array
    CRect		m_rcInset;				// inset margin
    CRect		m_rcInsetPixel;			// inset margin in pixel
    TEXTMETRIC	m_tmFont;				//
    DWORD	m_dwStyle;

    UINT	m_fEnableAutoWordSel	:1;	// enable Word style auto word selection?
    UINT	m_fWordWrap			:1;	// Whether control should word wrap
    UINT	m_fRich				:1;	// Whether control is rich text
    UINT	m_fSaveSelection		:1;	// Whether to save the selection when inactive
    UINT	m_fInplaceActive		:1; // Whether control is inplace active
    UINT	m_fTransparent		:1; // Whether control is transparent
    UINT	m_fVertical			:1;	// Whether control is layout following vertical
    UINT	m_fAllowBeep		:1;	// Whether message beep is allowed in the control
    UINT	m_fWantTab			:1;	// Whether control will deal with tab input
    UINT	m_fSingleLineVCenter:1;	// Whether control that is single line will be vertical centered
    UINT	m_fScrollPending	:1; // Whether scroll is activated by richedit or by panelex.
    CDuiTextHost	*m_pTxtHost;
};

class DUI_EXP CDuiEdit : public CDuiRichEdit
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiRichEdit, "edit")
public:
    CDuiEdit()
    {
        m_fRich=0;
    }
    virtual ~CDuiEdit() {}
};
}//namespace DuiEngine
