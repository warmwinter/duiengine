#pragma once
#include "duiwnd.h"
#include "activex/flash10t.tlh"
#include "activex/wmp.tlh"


namespace DuiEngine
{

	class DUI_EXP CDuiActiveX : public CDuiWindow
	{
		friend class CDuiAxContainerImpl;
	public:
		DUIOBJ_DECLARE_CLASS_NAME(CDuiActiveX, "activex")
		explicit CDuiActiveX();
		virtual ~CDuiActiveX();

	protected:
		virtual void OnAxActivate(IUnknown *pUnknwn){}

		int OnCreate(LPVOID);
		void OnSize(UINT nType, CSize size);
		void OnPaint(CDCHandle dc);
		LRESULT OnMouseEvent(UINT uMsg,WPARAM wp,LPARAM lp);
		LRESULT OnKeyEvent(UINT uMsg,WPARAM wp,LPARAM lp);
		void OnShowWindow(BOOL bShow, UINT nStatus);

		virtual UINT OnGetDuiCode(){return DUIC_WANTALLKEYS;}

		virtual BOOL IsTabStop(){return TRUE;}

		HRESULT OnAttrClsid(const CDuiStringA & strValue,BOOL bLoading);
		DUIWIN_BEGIN_MSG_MAP()
			MSG_WM_PAINT(OnPaint)
			MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,WM_MOUSELAST,OnMouseEvent)
			MESSAGE_RANGE_HANDLER_EX(WM_KEYFIRST,WM_KEYLAST,OnKeyEvent)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_SIZE(OnSize)
			MSG_WM_SHOWWINDOW(OnShowWindow)
		DUIWIN_END_MSG_MAP()

		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_CUSTOM_ATTRIBUTE("clsid",OnAttrClsid)
			DUIWIN_UINT_ATTRIBUTE("delayinit",m_bDelayInit,FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()

		virtual void OnInitActiveXFinished(){}

		BOOL InitActiveX();
		void SetActiveXVisible(BOOL bVisible);
	protected:
		CDuiAxContainerImpl * m_axContainer;
		CLSID	m_clsid;
		BOOL		m_bDelayInit;
	};

	class DUI_EXP CDuiFlashCtrl : public CDuiActiveX
	{
	public:
		DUIOBJ_DECLARE_CLASS_NAME(CDuiFlashCtrl, "flash")
		CDuiFlashCtrl();

		ShockwaveFlashObjects::IShockwaveFlash* GetFlashInterface()  const
		{
			return flash_;
		}

	protected:
		virtual void OnAxActivate(IUnknown *pUnknwn)
		{
			flash_=pUnknwn;
			if(flash_)
			{
				flash_->put_WMode(bstr_t(_T("transparent")));
				flash_->put_Movie(bstr_t(m_strUrl));
			}

		}

		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_WSTRING_ATTRIBUTE("url",m_strUrl,FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()

		CDuiStringW m_strUrl;

		CDuiComQIPtr<ShockwaveFlashObjects::IShockwaveFlash> flash_;
	};


	class DUI_EXP CDuiMediaPlayer :public CDuiActiveX
	{
	public:
		DUIOBJ_DECLARE_CLASS_NAME(CDuiMediaPlayer, "mediaplayer")
		CDuiMediaPlayer();

		WMPLib::IWMPPlayer4* GetMediaPlayerInterface()  const
		{
			return wmp_;
		}
		bool Play(LPCWSTR pszUrl);

	protected:
		virtual void OnInitActiveXFinished(){
			if(!m_strUrl.IsEmpty() && wmp_)
			{
				Play(m_strUrl);
			}
		}

		virtual void OnAxActivate(IUnknown *pUnknwn);

		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_WSTRING_ATTRIBUTE("url",m_strUrl,FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()

		CDuiStringW m_strUrl;
		CDuiComQIPtr<WMPLib::IWMPPlayer4> wmp_;
	};

}

