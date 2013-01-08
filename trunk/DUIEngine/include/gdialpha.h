#pragma once

namespace DuiEngine
{

#define  MAX_ALPHABUF	1<<16

typedef struct tagALPHAINFO
{
    LPBYTE lpBuf;
    RECT	rc;
    tagALPHAINFO()
    {
        lpBuf=NULL;
        rc.left=rc.top=rc.right=rc.bottom=0;
    }
} ALPHAINFO,* LPALPHAINFO;

class DUI_EXP CGdiAlpha
{
private:
    static BYTE s_byAlphaBack[MAX_ALPHABUF];

    static BOOL s_bAlphaEnable;

    static LPBYTE ALPHABACKUP(BITMAP *pBitmap,int x,int y,int cx,int cy);
    //�ָ�λͼ��Alphaͨ��
    static void ALPHARESTORE(BITMAP *pBitmap,int x,int y,int cx,int cy,LPBYTE lpAlpha);

    static void _swap(int &a,int &b);
public:
    static void SetAlphaEnable(BOOL bEnableAlpha)
    {
        s_bAlphaEnable=bEnableAlpha;
    }
    static BOOL IsAlphaEnable()
    {
        return s_bAlphaEnable;
    }

    static BOOL AlphaBackup(HDC hdc,LPCRECT pRect,ALPHAINFO &alphaInfo);

    static void AlphaRestore(HDC hdc,const ALPHAINFO &alphaInfo);
    static BOOL RoundRect(HDC hdc,LPCRECT pRc, POINT point);

    static int FillSolidRect(HDC hdc,LPCRECT pRect,COLORREF cr);

    static int FillRect(HDC hdc,LPCRECT pRc,HBRUSH hbrh);

    static SIZE TextOut(HDC hdc,int x, int y, LPCTSTR pszText,int nCount=-1);

    static int DrawText(HDC hdc,LPCTSTR pszText,int nCount,LPRECT pRect,UINT uFormat);

    static void DrawLine(HDC hdc,int x1,int y1,int x2,int y2,COLORREF cr,UINT style);

    static HBITMAP CreateBitmap32(HDC hdc,int nWid,int nHei,LPVOID * ppBits=NULL,BYTE byAlpha=0);

};

}//namespace DuiEngine