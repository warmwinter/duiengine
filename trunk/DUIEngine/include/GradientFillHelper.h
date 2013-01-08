#pragma once

namespace DuiEngine
{

typedef struct _FRG_PARAM
{
    LONG lOffset;
    COLORREF crColor;
} FRG_PARAM;

DUI_EXP BOOL GradientFill2(HDC hDC, PTRIVERTEX pVertices, DWORD nVertices, PVOID pMeshElements, ULONG nMeshElements, ULONG dwMode);

DUI_EXP void GradientFillRectV(HDC hdc, CRect &rcFill, FRG_PARAM params[], int nCount);
DUI_EXP void GradientFillRectH(HDC hdc, CRect &rcFill, FRG_PARAM params[], int nCount);

DUI_EXP void GradientFillRectV(HDC hdc, CRect &rcFill, COLORREF crTop, COLORREF crBottom);

DUI_EXP void GradientFillRectH(HDC hdc, CRect &rcFill, COLORREF crLeft, COLORREF crRight);

}//namespace DuiEngine