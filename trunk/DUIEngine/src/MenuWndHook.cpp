/*########################################################################
	Filename: 	MenuWndHook.cpp
	----------------------------------------------------
	Remarks:	...
	----------------------------------------------------
	Author:		����
	Email:		anyou@sina.com
				anyou@msn.com
	Created:	7/4/2003 17:38
				6/20/2012 �ƽ��ۣ��޸�Ϊʹ��DUIƤ�����Ʒǿͻ����߿�֧�ֱ߿��С����
  ########################################################################*/

#include "duistd.h"
#include "MenuWndHook.h"
#include "duiskinpool.h"
#include "duiskin.h"

namespace DuiEngine{

const TCHAR CoolMenu_oldProc[] = _T("CoolMenu_oldProc");


#define SM_CXMENUBORDER	3		//Ĭ�ϲ˵��߿��С


/*########################################################################
			  ------------------------------------------------
								class CMenuWndHook
			  ------------------------------------------------
  ########################################################################*/
std::map <HWND,CMenuWndHook*> CMenuWndHook::m_WndMenuMap;

HHOOK CMenuWndHook::m_hMenuHook = NULL;
CStringA CMenuWndHook::m_strSkinName = "";

CMenuWndHook::CMenuWndHook (HWND hWnd)
: m_hWnd(hWnd)
{

}

CMenuWndHook::~CMenuWndHook ()
{
    WNDPROC oldWndProc = (WNDPROC)::GetProp(m_hWnd, CoolMenu_oldProc);
    if (oldWndProc != NULL)
    {
        ::SetWindowLong(m_hWnd, GWL_WNDPROC, (DWORD)(ULONG)oldWndProc);
        ::RemoveProp(m_hWnd, CoolMenu_oldProc);
    }
    m_WndMenuMap.erase(m_hWnd);
}

void CMenuWndHook::InstallHook(HINSTANCE hInst,LPCSTR pszSkinName)
{
    if (m_hMenuHook == NULL )
    {
        m_hMenuHook = ::SetWindowsHookEx(WH_CALLWNDPROC, WindowHook, 
						hInst, ::GetCurrentThreadId());
		m_strSkinName=pszSkinName;
    }
}

void CMenuWndHook::UnInstallHook()
{
	std::map<HWND,CMenuWndHook*>::iterator it=m_WndMenuMap.begin();

    while (it!=m_WndMenuMap.end())
    {
		delete it->second;
		it++;
    }
    m_WndMenuMap.clear();
	
    if (m_hMenuHook != NULL)
    {
        ::UnhookWindowsHookEx(m_hMenuHook);
    }
}

CMenuWndHook* CMenuWndHook::GetWndHook(HWND hwnd)
{
	std::map<HWND,CMenuWndHook*>::iterator it=m_WndMenuMap.find(hwnd);
	if(it==m_WndMenuMap.end()) return NULL;
	return it->second;
}

CMenuWndHook* CMenuWndHook::AddWndHook(HWND hwnd)
{
	CMenuWndHook* pWnd = GetWndHook(hwnd);
	if(pWnd) return pWnd;

	pWnd = new CMenuWndHook(hwnd);
	if (pWnd != NULL)
	{
		m_WndMenuMap[hwnd]=pWnd;
	}
	return pWnd;
}

/*########################################################################
			  ------------------------------------------------
								  ��Ϣ����
			  ------------------------------------------------
  ########################################################################*/
LRESULT CALLBACK CMenuWndHook::WindowHook(int code, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;
	
    while (code == HC_ACTION)
    {
        HWND hWnd = pStruct->hwnd;
		
        if (pStruct->message != WM_CREATE && pStruct->message != 0x01E2)
        {
            break;
        }

        // �Ƿ�Ϊ�˵��� ----------------------------------------
        TCHAR strClassName[10];
        int Count = ::GetClassName (hWnd, strClassName, sizeof(strClassName) / sizeof(strClassName[0]));
        if (Count != 6 || _tcscmp(strClassName, _T("#32768")) != 0 )
        {
            break;
        }
		
		// �Ƿ��Ѿ������໯ ------------------------------------
        if (::GetProp(hWnd, CoolMenu_oldProc) != NULL )
        {
            break;
        }

        // ȡ��ԭ���Ĵ��ڹ��� ----------------------------------
        WNDPROC oldWndProc = (WNDPROC)(long)::GetWindowLong(hWnd, GWL_WNDPROC);
        if (oldWndProc == NULL)
        {
            break;
        }
		
        DUIASSERT(oldWndProc != CoolMenuProc);
        // ���浽���ڵ������� ----------------------------------
        if (!SetProp(hWnd, CoolMenu_oldProc, oldWndProc) )
        {
            break;
        }

        // ���໯ ----------------------------------------------
        if (!SetWindowLong(hWnd, GWL_WNDPROC,(DWORD)(ULONG)CoolMenuProc) )
        {
            ::RemoveProp(hWnd, CoolMenu_oldProc);
            break;
        }

        break;
    }
    return CallNextHookEx (m_hMenuHook, code, wParam, lParam);
}

LRESULT CALLBACK CMenuWndHook::CoolMenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldWndProc = (WNDPROC)::GetProp(hWnd, CoolMenu_oldProc);
    CMenuWndHook* pWnd = NULL;
	
	char szBuf[100];
	sprintf(szBuf,"\nHOOK MSG=0x%04X",uMsg);
	OutputDebugStringA(szBuf);
    switch (uMsg)
    {
		case WM_NCCALCSIZE:
			{
				LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
				if ((pWnd = GetWndHook(hWnd)) != NULL)
				{
					pWnd->OnNcCalcsize((BOOL)wParam,(NCCALCSIZE_PARAMS*)lParam);
				}
				return lResult;
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if ((pWnd = GetWndHook(hWnd)) != NULL)
				{
					pWnd->OnWindowPosChanging((LPWINDOWPOS)lParam);
				}
			}
			break;
// 		case WM_WINDOWPOSCHANGED:
// 			{
// 				LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
// 				if ((pWnd = GetWndHook(hWnd)) != NULL)
// 				{
//  					pWnd->SetLayeredAttr();
// 				}
// 				return lResult;
// 			}
// 			break;
		case WM_PRINT:
			{
				LRESULT lResult = CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
				if ((pWnd = GetWndHook(hWnd)) != NULL)
				{
					pWnd->OnPrint((HDC)wParam);
				}
				return lResult;
			}
			break;
		case WM_NCPAINT:
			{
				if ((pWnd = GetWndHook(hWnd)) != NULL)
				{
					pWnd->OnNcPaint();
					return 0;
				}
			}
			break;
		case WM_NCDESTROY:
			{
				if ((pWnd = GetWndHook(hWnd)) != NULL)
				{
					pWnd->OnNcDestroy();
				}
			}
			break;
    }
    return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

/*########################################################################
			  ------------------------------------------------
								��Ϣ������	
			  ------------------------------------------------
  ########################################################################*/
void CMenuWndHook::OnWindowPosChanging(WINDOWPOS *pWindowPos)
{
	if(m_strSkinName.IsEmpty()) return;
	CDuiSkinBase *pSkin=DuiSkinPool::getSingleton().GetSkin(m_strSkinName);
	if(!pSkin) return;
	CDuiMenuBorderSkin *pBorderSkin=static_cast<CDuiMenuBorderSkin*>(pSkin);
	if(!pBorderSkin) return;
	pWindowPos->cx += pBorderSkin->GetMarginRect().left+pBorderSkin->GetMarginRect().right-SM_CXMENUBORDER*2;
	pWindowPos->cy -= pBorderSkin->GetMarginRect().top+pBorderSkin->GetMarginRect().bottom-SM_CXMENUBORDER*2;
}

void CMenuWndHook::OnNcCalcsize(BOOL bValidCalc,NCCALCSIZE_PARAMS* lpncsp)
{
	if(m_strSkinName.IsEmpty()) return;
	CDuiSkinBase *pSkin=DuiSkinPool::getSingleton().GetSkin(m_strSkinName);
	if(!pSkin) return;
	CDuiMenuBorderSkin *pBorderSkin=static_cast<CDuiMenuBorderSkin*>(pSkin);
	if(!pBorderSkin) return;
	
	lpncsp->rgrc[0].left=lpncsp->lppos->x+pBorderSkin->GetMarginRect().left;
	lpncsp->rgrc[0].top=lpncsp->lppos->y+pBorderSkin->GetMarginRect().top;
	lpncsp->rgrc[0].right=lpncsp->lppos->x+lpncsp->lppos->cx-pBorderSkin->GetMarginRect().right;
	lpncsp->rgrc[0].bottom=lpncsp->lppos->y+lpncsp->lppos->cy-pBorderSkin->GetMarginRect().bottom;
}

void CMenuWndHook::OnNcPaint()
{
	CDCHandle dc=GetDC(m_hWnd);
 	OnPrint(dc);
	ReleaseDC(m_hWnd,dc);
}

void CMenuWndHook::OnPrint(CDCHandle dc)
{
	if(m_strSkinName.IsEmpty()) return;
	CDuiSkinBase *pSkin=DuiSkinPool::getSingleton().GetSkin(m_strSkinName);
	if(!pSkin) return;
	CDuiMenuBorderSkin *pBorderSkin=static_cast<CDuiMenuBorderSkin*>(pSkin);
	if(!pBorderSkin) return;

	CRect rcClient;
	GetClientRect(m_hWnd,&rcClient);
	ClientToScreen(m_hWnd,(LPPOINT)&rcClient);
	ClientToScreen(m_hWnd,((LPPOINT)&rcClient)+1);
	CRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	rcClient.OffsetRect(-rcWnd.TopLeft());
	dc.ExcludeClipRect(rcClient);
	rcWnd.MoveToXY(0,0);

	pBorderSkin->Draw(dc,rcWnd,0,0xFF);
}

void CMenuWndHook::OnNcDestroy()
{
	delete this;
}

//������ƴ��ڰ�͸������������󣬷ǿͻ���λ�÷����ı䣬������ԭ�� 
void CMenuWndHook::SetLayeredAttr()
{
	CRect rcWnd;
	GetWindowRect(m_hWnd,&rcWnd);
	rcWnd.MoveToXY(0,0);
	CRgn rgn;
	rgn.CreateEllipticRgnIndirect(&rcWnd);
	//rgn.CreateRoundRectRgn(rcWnd.left,rcWnd.top,rcWnd.right,rcWnd.bottom,5,5);
	SetWindowRgn(m_hWnd,rgn,TRUE);
}

}//namespace DuiEngine