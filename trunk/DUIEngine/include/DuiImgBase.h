#pragma once

#include "duiref.h"

namespace DuiEngine
{

class DUI_EXP CDuiImgBase : public CDuiRef
{
public:
    virtual BOOL LoadFromResource(HINSTANCE hInst,LPCSTR pszType,UINT uID)=NULL;
    virtual BOOL LoadFromFile(LPCSTR pszPath)=NULL;
    virtual BOOL LoadFromMemory(LPVOID pBuf,DWORD dwSize)=NULL;

    virtual BOOL GetImageSize(SIZE & sz)=NULL;
    virtual int GetWidth()=NULL;
    virtual int GetHeight()=NULL;
    virtual BOOL IsEmpty()
    {
        return TRUE;
    }
    virtual BOOL BitBlt(HDC hdc,int x,int y,int nWid,int nHei,int xSrc,int ySrc,BYTE byAlpha=0xFF)=NULL;
    virtual BOOL StretchBlt(HDC hdc,int x,int y,int nWid,int nHei,int xSrc,int ySrc,int nWidSrc,int nHeiSrc,BYTE byAlpha=0xFF)=NULL;
    virtual BOOL TileBlt(HDC hdc,int x,int y,int nWid,int nHei,int xSrc,int ySrc,int nWidSrc,int nHeiSrc,BYTE byAlpha=0xFF)=NULL;
    virtual void SetAttributes(TiXmlElement *pTiXmlEle) {}
};


class DUI_EXP CDuiImgDecoder
{
public:
    virtual CDuiImgBase * CreateDuiImage(LPCSTR pszType)=NULL;

    virtual void DestoryDuiImage(CDuiImgBase* pImg)=NULL;

    virtual LPCSTR GetSupportTypes()= NULL;
};
}