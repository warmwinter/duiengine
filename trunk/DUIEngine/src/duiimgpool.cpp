//////////////////////////////////////////////////////////////////////////
//  Class Name: DuiImgPool
// Description: Image Pool
//     Creator: Huang jianxiong
//     Version: 2012.8.30 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "duistd.h"
#include "duiobject.h"
#include "duiimgpool.h"
#include "duiresprovider.h"
#include "DuiSystem.h"

namespace DuiEngine
{

template<> DuiImgPool * Singleton<DuiImgPool>::ms_Singleton =0;

DuiImgPool::DuiImgPool()
{
    m_pFunOnKeyRemoved=OnImageRemoved;
}

DuiImgPool::~DuiImgPool()
{
    RemoveAll();//��Ҫ������ͼƬ�����ͷ�gdi+����������ͷ��ڴ�ʱ�����
}

CDuiImgBase * DuiImgPool::GetImage(UINT uResID,LPCSTR pszType)
{
    DuiResID resid(pszType,uResID);
    if(HasKey(resid))
    {
        return GetKeyObject(resid);
    }
    else
    {
        DuiResProviderBase * pResProvider=DuiSystem::getSingleton().GetResProvider();
        DUIASSERT(pResProvider);
        CDuiImgBase *pImg=NULL;
        if(pszType)
        {
            pImg=pResProvider->LoadImage(pszType,uResID);
        }
        else
        {
            //ö������֧�ֵ�ͼƬ��Դ�����Զ�ƥ��
            CDuiImgDecoder *pImgDecoder=pResProvider->GetImageDecoder();
            DUIASSERT(pImgDecoder);
            LPCSTR pszTypes=pImgDecoder->GetSupportTypes();
            while(*pszTypes)
            {
                if(pResProvider->HasResource(pszTypes,uResID))
                {
                    pImg=pResProvider->LoadImage(pszTypes,uResID);
                    if(pImg) break;
                }
                pszTypes+=strlen(pszTypes)+1;
            }
        }
        if(pImg)
        {
            AddKeyObject(resid,pImg);
            if(pszType!=NULL) AddKeyObject(DuiResID(NULL,uResID),pImg);//IDΨһʱ��֤��ʹ������Ҳ���ҵ���ͼƬ��Դ
        }
        return pImg;
    }
}

}//namespace DuiEngine