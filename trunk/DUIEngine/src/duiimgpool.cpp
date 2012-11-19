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

namespace DuiEngine{

#define IDC_AUTOID_START		65535	//�Զ���ŵ���ʼID�������ڳ����ж�̬����ͼƬ��ͼƬ����

template<> DuiImgPool * Singleton<DuiImgPool>::ms_Singleton =0;

DuiImgPool::DuiImgPool()
{
	CDuiImgX::InitGdiplus();
	m_pFunOnKeyRemoved=OnKeyRemoved;
	m_uAutoID=IDC_AUTOID_START;
}

DuiImgPool::~DuiImgPool()
{
	RemoveAll();//��Ҫ������ͼƬ�����ͷ�gdi+����������ͷ��ڴ�ʱ�����
	CDuiImgX::UninitGdiplus();
}

CDuiImgBase * DuiImgPool::GetImage(UINT uResID)
{
	if(HasKey(uResID)) 
	{
		return GetKeyObject(uResID);
	}else
	{
		CDuiImgBase *pImg=new CDuiImgX;
		pImg->LoadImg(uResID,DUIRES_IMGX_TYPE);
		if(pImg->IsEmpty())
		{
			delete pImg;
			pImg=new CDuiBitmap;
			pImg->LoadImg(uResID,DUIRES_BMP_TYPE);
			if(pImg->IsEmpty())
			{
				delete pImg;
				return NULL;
			}
		}
		AddKeyObject(uResID,pImg);
		return pImg;
	}
}

//����һ��ͼƬ�ļ���ͼƬ����
//LPCTSTR pszFileName:ͼƬ�ļ���
//return:ͼƬ��ID,ͼƬ����ʧ�ܷ���-1
UINT DuiImgPool::AddImage(LPCWSTR pszFileName,BOOL bBmp)
{
	CDuiImgBase *pImg=NULL;
	if(bBmp)
	{
		pImg=new CDuiBitmap;
	}else
	{
		pImg=new CDuiImgX;
	}
	pImg->LoadImg(pszFileName);
	if(pImg->IsEmpty())
	{
		delete pImg;
		return -1;
	}
	AddKeyObject(m_uAutoID,pImg);
	return m_uAutoID++;
}

}//namespace DuiEngine