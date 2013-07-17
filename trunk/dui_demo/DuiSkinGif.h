/********************************************************************
	created:	2012/12/27
	created:	27:12:2012   14:55
	filename: 	DuiSkinGif.h
	file base:	DuiSkinGif
	file ext:	h
	author:		huangjianxiong
	
	purpose:	�Զ���Ƥ������
*********************************************************************/
#pragma once
#include "../DUIEngine/dependencies/tinyxml/tinyxml.h"
#include "../DUIEngine/include/DuiSkinBase.h"
#include "../DUIEngine/include/duiimage.h"

 class CDuiSkinGif : public CDuiSkinBase
 {
 	DUIOBJ_DECLARE_CLASS_NAME(CDuiSkinGif, "gif")
 public:
 	CDuiSkinGif():m_nFrames(0),m_iFrame(0),m_pFrameDelay(NULL)
 	{
 
 	}
 	virtual ~CDuiSkinGif()
 	{
 		if(m_pFrameDelay) delete [] m_pFrameDelay;
 	}
 	virtual void Draw(CDCHandle dc, CRect rcDraw, DWORD dwState,BYTE byAlpha=0xFF);
 
 	virtual int GetStates(){return m_nFrames;}
 	virtual void OnAttributeChanged(const CDuiStringA & strAttrName,BOOL bLoading,HRESULT hRet);
 	virtual SIZE GetSkinSize()
 	{
 		SIZE sz={0};
 		if(m_pDuiImg)
 		{
 			m_pDuiImg->GetImageSize(sz);
 		}
 		return sz;
 	}
 
 	virtual void SetImage(CDuiImgBase *pImg)
 	{
 		__super::SetImage(pImg);
 		OnSetImage();
 	}
 
 	long GetFrameDelay(int iFrame=-1);
 	void ActiveNextFrame();
 	void SelectActiveFrame(int iFrame);
 protected:
 	void OnSetImage();
 	int m_nFrames;
 	int m_iFrame;
 	long *m_pFrameDelay;
 	static GUID ms_Guid;
 
 };