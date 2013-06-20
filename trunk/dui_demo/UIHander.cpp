#include "StdAfx.h"
#include "UIHander.h"

#include "MainDlg.h"
#include "skinole/ImageOle.h"

class CTestDropTarget:public IDropTarget
{
public:
	CTestDropTarget()
	{
		nRef=0;
	}

	//////////////////////////////////////////////////////////////////////////
	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		HRESULT hr=S_FALSE;
		if(riid==__uuidof(IUnknown))
			*ppvObject=(IUnknown*) this,hr=S_OK;
		else if(riid==__uuidof(IDropTarget))
			*ppvObject=(IDropTarget*)this,hr=S_OK;
		return hr;

	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void){return ++nRef;}

	virtual ULONG STDMETHODCALLTYPE Release( void) { 
		ULONG uRet= -- nRef;
		if(uRet==0) delete this;
		return uRet;
	}

	//////////////////////////////////////////////////////////////////////////
	// IDropTarget

	virtual HRESULT STDMETHODCALLTYPE DragEnter( 
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{
		*pdwEffect=DROPEFFECT_LINK;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DragOver( 
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{
		*pdwEffect=DROPEFFECT_LINK;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE DragLeave( void)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Drop( 
		/* [unique][in] */ __RPC__in_opt IDataObject *pDataObj,
		/* [in] */ DWORD grfKeyState,
		/* [in] */ POINTL pt,
		/* [out][in] */ __RPC__inout DWORD *pdwEffect)
	{
		FORMATETC format =
		{
			CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
		};
		STGMEDIUM medium;
		if(FAILED(pDataObj->GetData(&format, &medium)))
		{
			return S_FALSE;
		}

		HDROP hdrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));

		if(!hdrop)
		{
			return S_FALSE;
		}

		bool success = false;
		TCHAR filename[MAX_PATH];
		success=DragQueryFile(hdrop, 0, filename, MAX_PATH);
		DragFinish(hdrop);
		GlobalUnlock(medium.hGlobal);

		if(success) DuiMessageBox(NULL,filename,NULL,MB_OK);

		*pdwEffect=DROPEFFECT_LINK;
		return S_OK;
	}
protected:
	int nRef;
};

CUIHander::CUIHander(CMainDlg * pMainDlg) : m_pMainDlg(pMainDlg)
{
}

CUIHander::~CUIHander(void)
{
}

bool Evt_Test2(CDuiWindow * pSender, LPNMHDR pNmhdr)
{
	pSender->GetUserData();
// 	CUIHander * p=(CUIHander *)pSender->GetUserData();
// 	pSender->unsubscribeEvent(DUINM_COMMAND,Subscriber(Evt_Test2));
// 	pSender->subscribeEvent(DUINM_COMMAND,Subscriber(&CUIHander::Evt_Test,p));
// 	DuiMessageBox(NULL,_T("���msgbox����ȫ�ֺ�����ʹ��event��obsever��ʾ��"),_T("�¼�����"),MB_OK|MB_ICONWARNING);
	return true;
}

bool CUIHander::Evt_Test(CDuiWindow * pSender, LPNMHDR pNmhdr)
{
// 	pSender->subscribeEvent(DUINM_COMMAND,Subscriber(Evt_Test2));
// 	pSender->unsubscribeEvent(DUINM_COMMAND,Subscriber(&CUIHander::Evt_Test,this));
// 	pSender->SetUserData((ULONG_PTR)this);
// 	DuiMessageBox(NULL,_T("���msgbox�������Ա������ʹ��event��obsever��ʾ��"),_T("�¼�����"),MB_OK|MB_ICONWARNING);
	return true;
}

LRESULT CUIHander::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	HRESULT hr=::RegisterDragDrop(hWnd,m_pMainDlg->GetDropTarget());

	CDuiWindow *pSlider=m_pMainDlg->FindChildByName("IDC_SLIDERTEST");
	m_pMainDlg->RegisterDragDrop(pSlider->GetDuiHwnd(),new CTestDropTarget);

	//��ʼ�������б�
	CDuiListBoxEx *pList2=(CDuiListBoxEx*)m_pMainDlg->FindChildByName("mylist2");
	if(pList2)
	{
		pList2->SetItemCount(100);
	}
	CDuiButton *pBtn=(CDuiButton *)m_pMainDlg->FindChildByCmdID(IDC_REPSEL);
	m_pMainDlg->GetFocusManager()->RegisterAccelerator(CAccelerator(VK_RETURN,true),pBtn);//��repsel��ťע��һ���ȼ�Ctrl+ENTER��
#if defined(DLL_DUI) && !defined(_WIN64)
	CDuiWindow *pTst=m_pMainDlg->FindChildByCmdID(btn_tstevt);
	DuiSystem::getSingleton().GetScriptModule()->subscribeEvent(pTst,DUINM_COMMAND,"onEvtTstClick");
#endif

//	m_pMainDlg->SetTimer(100,2000,NULL);
// 	SetMsgHandled(FALSE); 
	//��ʾ�ڳ����ʼ����ʱ��ͨ�����û������ļ�����PANE�Ĵ�С.
// 	CDuiSplitWnd *pSplit=(CDuiSplitWnd*)m_pMainDlg->FindChildByCmdID(1180);
// 	pSplit->SetPaneInfo(0,100,-1,-1);
// 	CDuiRichEdit *pEdit=(CDuiRichEdit*)m_pMainDlg->FindChildByCmdID(1140);
// 	pEdit->DuiSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);

    return 0; 
}

void CUIHander::OnDestory()
{
	::RevokeDragDrop(m_pMainDlg->m_hWnd);
	SetMsgHandled(FALSE); 
}

void CUIHander::OnAttrReposition()
{
	m_pMainDlg->FindChildByCmdID(����)->SetAttribute("pos","|-100,|-15,|100,|15");
	CDuiWindow *pHost=m_pMainDlg->FindChildByCmdID(116);

// 	DuiSystem::getSingleton().GetScriptModule()->executeScriptFile("..\\dui_demo\\lua\\test.lua");
// 	lua_State *L=(lua_State *)DuiSystem::getSingleton().GetScriptModule()->GetScriptEngine();
// 	lua_function<void> funTst(L,"test");
// 	funTst(pHost);
}


// #define  PATH_GIF _T("E:\\dui.work\\RichEditDemo\\RichEditDemo\\Face\\1.gif")

void CUIHander::OnRepEditSel()
{
	CDuiRichEdit *pEdit=(CDuiRichEdit *)m_pMainDlg->FindChildByCmdID(1140);
	if(pEdit)
	{
		for(int i=0;i<5;i++)
		{
			CDuiSkinBase *pSkinGif=DuiSkinPool::getSingleton().GetSkin("face0");
// 			RichEdit_InsertImage(pEdit,PATH_GIF);//���ļ����� 
			RichEdit_InsertSkin(pEdit,pSkinGif);
			CDuiSkinBase *pSkin=DuiSkinPool::getSingleton().GetSkin("bmpmask");
			RichEdit_InsertSkin(pEdit,pSkin);
// 			RichEdit_InsertSkin(pEdit,pSkin);
// 			RichEdit_InsertSkin(pEdit,pSkin);
		}
	}

}

LRESULT CUIHander::OnEditNotify( LPNMHDR pNHdr )
{
	LPDUIRICHEDITNOTIFY pEditNotify=(LPDUIRICHEDITNOTIFY)pNHdr;
	if(pEditNotify->iNotify==EN_CHANGE)
	{
	}
	return S_OK;
}

//��ʾ��ֹcombobox��������selchanged��Ϣ��
LRESULT CUIHander::OnComboListSelChanging( LPNMHDR pNHdr )
{
	LPDUINMLBSELCHANGE pLbSelChange=(LPDUINMLBSELCHANGE)pNHdr;
	return pLbSelChange->uHoverID==3?S_FALSE:S_OK;//���ɾ����ťʱ�������ڲ��ر�
}
  
//��ʾcomboboxѡ��ı���¼���Ӧ
LRESULT CUIHander::OnComboListSelChanged( LPNMHDR pNHdr )
{
	LPDUINMLBSELCHANGE pLbSelChange=(LPDUINMLBSELCHANGE)pNHdr;
	CDuiComboBox *pCombobox=(CDuiComboBox*)m_pMainDlg->FindChildByCmdID(1310);
	return S_OK;
}

//��ʾ��Ӧcomboboxѡ���а�ť�¼�����Ӧ
LRESULT CUIHander::OnComboListItemNotify( LPNMHDR pNHdr )
{
	LPDUINMITEMNOTIFY pItemNHdr=(LPDUINMITEMNOTIFY)pNHdr;
	if(pItemNHdr->pOriginHdr->idFrom==3) 
	{//delete button 
		CDuiComboBox *pCombobox=(CDuiComboBox*)m_pMainDlg->FindChildByCmdID(1310);
		CDuiListBoxEx *pListBox=(CDuiListBoxEx*)pItemNHdr->pHostDuiWin;
		int iItem=pListBox->GetItemObjIndex(pItemNHdr->pItem);
		pCombobox->DeleteItem(iItem);
		pCombobox->GetDropDownList()->UpdateItems(); 
	}
	return S_OK;
}


void CUIHander::OnIECtrl()
{
	CDuiImageWnd *pImgWnd=(CDuiImageWnd*)m_pMainDlg->FindChildByCmdID(1139);
	if(pImgWnd)
	{
// 		CDuiImgX *pImg=new CDuiImgX;
// 		pImg->LoadImg(L"e:\\test.png");
// 		CDuiImageList *pSkin=new CDuiImageList;
// 		pSkin->SetImage(pImg);
// 		pImgWnd->SetSkin(pSkin);
// 		pSkin->Release();
	}
}


void CUIHander::OnDuiMenu()
{
	CPoint pt; 
	GetCursorPos(&pt);
	CDuiMenu menu;  
	menu.LoadMenu(IDR_MENU_TEST); //load menu
	CDuiMenu subMenu=menu.GetSubMenu(5);
	CheckMenuRadioItem(subMenu.m_hMenu,51,53,52,MF_BYCOMMAND);
// 	CDuiMenu subMenu2; 
// 	subMenu2.CreatePopupMenu();
// 	subMenu2.InsertMenu(1,MF_STRING|MF_BYPOSITION,59,_T("�������1"),3);
// 	subMenu2.InsertMenu(2,MF_STRING|MF_BYPOSITION|MF_GRAYED,58,_T("�������2"),3);
// 	subMenu.InsertMenu(1,MF_POPUP|MF_BYPOSITION,(UINT_PTR)&subMenu2,_T("�����Ӳ˵�"),5);
	UINT uRet=menu.TrackPopupMenu(0,pt.x,pt.y,m_pMainDlg->m_hWnd);
// 	ATLTRACE(L"\nmenu ret=%d",uRet); 
}

void CUIHander::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
{
// 	ATLTRACE(L"\nOnCommand nID=%d",nID);  
	TCHAR szBuf[200];
// 	_stprintf(szBuf,_T("Menu Command ID=%d\\nSecond Line\\nSecond Line\\nSecond Line\\nSecond Line"),nID);
	_stprintf(szBuf,_T("Menu Command ID=%d\\nSecond Line \\na long long line :��Ϣ���������Զ����в��ԣ�hahahaha haha haha"),nID);
	DuiMessageBox(NULL,szBuf,_T("tip"),MB_YESNOCANCEL|MB_ICONWARNING);
}


void CUIHander::OnWebkit_GoBack()
{
// 	CDuiWebkit *pWebkitCtrl=static_cast<CDuiWebkit*>(m_pMainDlg->FindChildByCmdID(1205));
// 	pWebkitCtrl->GoBack();
}

void CUIHander::OnWebkit_GoForward()
{
// 	CDuiWebkit *pWebkitCtrl=static_cast<CDuiWebkit*>(m_pMainDlg->FindChildByCmdID(1205));
// 	pWebkitCtrl->GoForward();
}

void CUIHander::OnWebkit_Refresh()
{
// 	CDuiWebkit *pWebkitCtrl=static_cast<CDuiWebkit*>(m_pMainDlg->FindChildByCmdID(1205));
// 	pWebkitCtrl->Refresh();
}

void CUIHander::OnWebkit_Go()
{
// 	CDuiWebkit *pWebkitCtrl=static_cast<CDuiWebkit*>(m_pMainDlg->FindChildByCmdID(1205));
// 	CDuiRichEdit *pEdit=static_cast<CDuiRichEdit*>(m_pMainDlg->FindChildByCmdID(1203));
// 	WCHAR szURI[200];
// 	pEdit->GetWindowText(szURI,200);
// 	CDuiStringA strURI=CW2A(szURI,CP_UTF8);
// 	pWebkitCtrl->SetURI(strURI);
}

//��ʾ�б��еİ�ť�ؼ�����Ӧ
LRESULT CUIHander::OnListBtnClick( LPNMHDR pNHdr )
{
	DUINMITEMNOTIFY *pNHdrEx=(DUINMITEMNOTIFY*)pNHdr;

	return S_OK;
}

LRESULT CUIHander::OnListPredraw( LPNMHDR pNHdr )
{
	LPDUINMGETLBDISPINFO lpNHdrEx=(LPDUINMGETLBDISPINFO)pNHdr;
	CDuiStringT str;
	str.Format(_T("item:%d"),lpNHdrEx->nListItemID);
	lpNHdrEx->pItem->FindChildByName("idx")->SetInnerText(str);
	return S_OK;
}

void CUIHander::OnTimer( UINT_PTR nIDEvent )
{
	if(nIDEvent!=100) return;
	static int nCount=0;
	if(nCount>5) m_pMainDlg->KillTimer(nIDEvent);
	TCHAR szMsg[100];
	_stprintf(szMsg,_T("Msg box No. %d"),nCount);
	nCount++;
	DuiMessageBox(0,szMsg,_T("tip"),MB_OK);
}
