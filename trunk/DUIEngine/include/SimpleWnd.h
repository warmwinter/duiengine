#pragma once

//////////////////////////////////////////////////////////////////////////
// thunk ����ʵ�ֲο�http://www.cppblog.com/proguru/archive/2008/08/24/59831.html
//////////////////////////////////////////////////////////////////////////

#include "DUISingleton.h"

namespace DuiEngine
{

#if defined(_M_IX86)
// ��һ�ֽڶ���
#pragma pack(push, 1)
struct tagThunk
{
    DWORD m_mov;  // 4���ֽ�
    DWORD m_this;
    BYTE  m_jmp;
    DWORD m_relproc;
    //�ؼ�����   //////////////////////////////////////
    BOOL Init(DWORD proc, void* pThis)
    {
        m_mov = 0x042444C7;
        m_this = (DWORD)pThis;  // mov [esp+4], pThis;��esp+4�����Ƿ�hWnd,���ڱ�͵�ŷŶ���ָ����.
        m_jmp = 0xe9;
        // ��ת��procָ������ں���
        m_relproc = (DWORD)((INT_PTR)proc - ((INT_PTR)this + sizeof(tagThunk)));
        // ����CPU������������䲻�����ݣ���ָ��,��������GetCodeAddress��õ�ָ��ͻ����д�ָ��
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(tagThunk));
        return TRUE;
    }
    void* GetCodeAddress()
    {
        return this; // ָ��this,��ô��GetCodeAddress��õĺ���pProc�Ǵ�DWORD m_mov;��ʼִ�е�
    }
};
#pragma pack(pop)
#elif defined(_M_AMD64)
#pragma pack(push,2)
struct tagThunk
{
    USHORT  RcxMov;         // mov rcx, pThis
    ULONG64 RcxImm;         //
    USHORT  RaxMov;         // mov rax, target
    ULONG64 RaxImm;         //
    USHORT  RaxJmp;         // jmp target
    BOOL Init(DWORD_PTR proc, void *pThis)
    {
        RcxMov = 0xb948;          // mov rcx, pThis
        RcxImm = (ULONG64)pThis;  //
        RaxMov = 0xb848;          // mov rax, target
        RaxImm = (ULONG64)proc;   //
        RaxJmp = 0xe0ff;          // jmp rax
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
        return TRUE;
    }
    //some thunks will dynamically allocate the memory for the code
    void* GetCodeAddress()
    {
        return this;
    }
};
#pragma pack(pop)
#else
#error Only AMD64 and X86 supported
#endif

class CMessageMap
{
public:
    virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                      LRESULT& lResult, DWORD dwMsgMapID) = 0;
};

class DUI_EXP CSimpleWnd : public CMessageMap
{
public:
    CSimpleWnd(HWND hWnd=0);
    virtual ~CSimpleWnd(void);

    static ATOM RegisterSimpleWnd(HINSTANCE hInst,LPCTSTR pszSimpleWndName);

    HWND Create( LPCTSTR lpWindowName, DWORD dwStyle,DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,LPVOID lpParam );

    BOOL SubclassWindow(HWND hWnd);

    HWND UnsubclassWindow(BOOL bForce /*= FALSE*/);

    const MSG * GetCurrentMessage() const
    {
        return m_pCurrentMsg;
    }

    DWORD GetStyle() const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
    }

    DWORD GetExStyle() const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return (DWORD)::GetWindowLong(m_hWnd, GWL_EXSTYLE);
    }

    LONG_PTR GetWindowLongPtr(int nIndex) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetWindowLongPtr(m_hWnd, nIndex);
    }

    LONG_PTR SetWindowLongPtr(int nIndex, LONG_PTR dwNewLong) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetWindowLongPtr(m_hWnd, nIndex, dwNewLong);
    }

    BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));

        DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
        DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
        if(dwStyle == dwNewStyle)
            return FALSE;

        ::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
        if(nFlags != 0)
        {
            ::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
                           SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
        }

        return TRUE;
    }

    BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));

        DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
        if(dwStyle == dwNewStyle)
            return FALSE;

        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
        if(nFlags != 0)
        {
            ::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
                           SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
        }

        return TRUE;
    }
    BOOL SetWindowPos(HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetWindowPos(m_hWnd, hWndInsertAfter, x, y, cx, cy, nFlags);
    }
    BOOL CenterWindow(HWND hWndCenter = NULL) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));

        // determine owner window to center against
        DWORD dwStyle = GetStyle();
        if(hWndCenter == NULL)
        {
            if(dwStyle & WS_CHILD)
                hWndCenter = ::GetParent(m_hWnd);
            else
                hWndCenter = ::GetWindow(m_hWnd, GW_OWNER);
        }

        // get coordinates of the window relative to its parent
        RECT rcDlg;
        ::GetWindowRect(m_hWnd, &rcDlg);
        RECT rcArea;
        RECT rcCenter;
        HWND hWndParent;
        if(!(dwStyle & WS_CHILD))
        {
            // don't center against invisible or minimized windows
            if(hWndCenter != NULL)
            {
                DWORD dwStyleCenter = ::GetWindowLong(hWndCenter, GWL_STYLE);
                if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
                    hWndCenter = NULL;
            }

            // center within screen coordinates
#if WINVER < 0x0500
            ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
#else
            HMONITOR hMonitor = NULL;
            if(hWndCenter != NULL)
            {
                hMonitor = ::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST);
            }
            else
            {
                hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
            }

            MONITORINFO minfo;
            minfo.cbSize = sizeof(MONITORINFO);
            BOOL bResult = ::GetMonitorInfo(hMonitor, &minfo);

            rcArea = minfo.rcWork;
#endif
            if(hWndCenter == NULL)
                rcCenter = rcArea;
            else
                ::GetWindowRect(hWndCenter, &rcCenter);
        }
        else
        {
            // center within parent client coordinates
            hWndParent = ::GetParent(m_hWnd);
            DUIASSERT(::IsWindow(hWndParent));

            ::GetClientRect(hWndParent, &rcArea);
            DUIASSERT(::IsWindow(hWndCenter));
            ::GetClientRect(hWndCenter, &rcCenter);
            ::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
        }

        int DlgWidth = rcDlg.right - rcDlg.left;
        int DlgHeight = rcDlg.bottom - rcDlg.top;

        // find dialog's upper left based on rcCenter
        int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
        int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

        // if the dialog is outside the screen, move it inside
        if(xLeft + DlgWidth > rcArea.right)
            xLeft = rcArea.right - DlgWidth;
        if(xLeft < rcArea.left)
            xLeft = rcArea.left;

        if(yTop + DlgHeight > rcArea.bottom)
            yTop = rcArea.bottom - DlgHeight;
        if(yTop < rcArea.top)
            yTop = rcArea.top;

        // map screen coordinates to child coordinates
        return ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1,
                              SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    BOOL DestroyWindow()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::DestroyWindow(m_hWnd);
    }
    BOOL IsWindow()
    {
        return ::IsWindow(m_hWnd);
    }


    BOOL Invalidate(BOOL bErase = TRUE) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::InvalidateRect(m_hWnd, NULL, bErase);
    }

    BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::InvalidateRect(m_hWnd, lpRect, bErase);
    }
    BOOL GetWindowRect(LPRECT lpRect) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetWindowRect(m_hWnd, lpRect);
    }

    BOOL GetClientRect(LPRECT lpRect) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetClientRect(m_hWnd, lpRect);
    }
    BOOL ClientToScreen(LPPOINT lpPoint) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::ClientToScreen(m_hWnd, lpPoint);
    }

    BOOL ClientToScreen(LPRECT lpRect) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        if(!::ClientToScreen(m_hWnd, (LPPOINT)lpRect))
            return FALSE;
        return ::ClientToScreen(m_hWnd, ((LPPOINT)lpRect)+1);
    }

    BOOL ScreenToClient(LPPOINT lpPoint) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::ScreenToClient(m_hWnd, lpPoint);
    }

    BOOL ScreenToClient(LPRECT lpRect) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        if(!::ScreenToClient(m_hWnd, (LPPOINT)lpRect))
            return FALSE;
        return ::ScreenToClient(m_hWnd, ((LPPOINT)lpRect)+1);
    }

    int MapWindowPoints(HWND hWndTo, LPPOINT lpPoint, UINT nCount) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::MapWindowPoints(m_hWnd, hWndTo, lpPoint, nCount);
    }

    int MapWindowPoints(HWND hWndTo, LPRECT lpRect) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::MapWindowPoints(m_hWnd, hWndTo, (LPPOINT)lpRect, 2);
    }


    UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetTimer(m_hWnd, nIDEvent, nElapse, (TIMERPROC)lpfnTimer);
    }

    BOOL KillTimer(UINT_PTR nIDEvent) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::KillTimer(m_hWnd, nIDEvent);
    }

    HDC GetDC() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetDC(m_hWnd);
    }

    HDC GetWindowDC() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetWindowDC(m_hWnd);
    }

    int ReleaseDC(HDC hDC) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::ReleaseDC(m_hWnd, hDC);
    }

    BOOL CreateCaret(HBITMAP hBitmap) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::CreateCaret(m_hWnd, hBitmap, 0, 0);
    }

    BOOL CreateSolidCaret(int nWidth, int nHeight) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::CreateCaret(m_hWnd, (HBITMAP)0, nWidth, nHeight);
    }

    BOOL CreateGrayCaret(int nWidth, int nHeight) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::CreateCaret(m_hWnd, (HBITMAP)1, nWidth, nHeight);
    }

    BOOL HideCaret() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::HideCaret(m_hWnd);
    }

    BOOL ShowCaret() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::ShowCaret(m_hWnd);
    }
    HWND SetCapture() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetCapture(m_hWnd);
    }

    HWND SetFocus() throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetFocus(m_hWnd);
    }

    LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SendMessage(m_hWnd,message,wParam,lParam);
    }

    BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::PostMessage(m_hWnd,message,wParam,lParam);
    }

    BOOL SendNotifyMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SendNotifyMessage(m_hWnd, message, wParam, lParam);
    }
    BOOL SetWindowText(LPCTSTR lpszString) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::SetWindowText(m_hWnd, lpszString);
    }

    int GetWindowText(_Out_z_cap_post_count_(nMaxCount, return + 1) LPTSTR lpszStringBuf, _In_ int nMaxCount) const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::GetWindowText(m_hWnd, lpszStringBuf, nMaxCount);
    }
    BOOL IsIconic() const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::IsIconic(m_hWnd);
    }

    BOOL IsZoomed() const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::IsZoomed(m_hWnd);
    }

    BOOL IsWindowVisible() const throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::IsWindowVisible(m_hWnd);
    }

    BOOL MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::MoveWindow(m_hWnd, x, y, nWidth, nHeight, bRepaint);
    }

    BOOL MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE) throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
    }

    BOOL ShowWindow(int nCmdShow)  throw()
    {
        DUIASSERT(::IsWindow(m_hWnd));
        return ::ShowWindow(m_hWnd,nCmdShow);
    }

    LRESULT DefWindowProc()
    {
        const MSG* pMsg = m_pCurrentMsg;
        LRESULT lRes = 0;
        if (pMsg != NULL)
            lRes = DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
        return lRes;
    }
    LRESULT ForwardNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    static BOOL DefaultReflectionHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

    BEGIN_MSG_MAP_EX(CSimpleWnd)
    END_MSG_MAP()
protected:
    LRESULT DefWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual void OnFinalMessage(HWND hWnd);

    const MSG * m_pCurrentMsg;
    BOOL m_bDestoryed;
public:

    HWND m_hWnd;
protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam, LPARAM lParam);

    // ִֻ��һ��
    static LRESULT CALLBACK StartWindowProc(HWND hWnd, UINT uMsg,
                                            WPARAM wParam, LPARAM lParam);

    tagThunk *m_pThunk;
    WNDPROC	m_pfnSuperWindowProc;
};

}//namespace DuiEngine
