#include "StdAfx.h"
#include "UIHander.h"

#include "MainDlg.h"

CUIHander::CUIHander(CMainDlg * pMainDlg) : m_pMainDlg(pMainDlg)
{
}

CUIHander::~CUIHander(void)
{
}

LRESULT CUIHander::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SetMsgHandled(FALSE); 
	//��ʾ�ڳ����ʼ����ʱ��ͨ�����û������ļ�����PANE�Ĵ�С.
// 	CDuiSplitWnd *pSplit=(CDuiSplitWnd*)m_pMainDlg->FindChildByCmdID(1180);
// 	pSplit->SetPaneInfo(0,100,-1,-1);
	CDuiRichEdit *pEdit=(CDuiRichEdit*)m_pMainDlg->FindChildByCmdID(1140);
	pEdit->DuiSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);

	m_menu.LoadMenu(IDR_MENU_TEST); //load menu

// 	CDuiWebkit *pwebkit=(CDuiWebkit*)m_pMainDlg->FindChildByCmdID(1205);
// 	pwebkit->SetCookieUsage(L"e:\\testcookie.txt",100*1024,1024,100);
    return 0; 
}

void CUIHander::OnAttrReposition()
{
	m_pMainDlg->FindChildByCmdID(����)->SetAttribute("pos","|-100,|-15,|100,|15");
}


void CUIHander::OnRepEditSel()
{
	CDuiRichEdit *pEdit=(CDuiRichEdit *)m_pMainDlg->FindChildByCmdID(1140);
	//pEdit->SetDefaultTextColor(RGB(0,128,128));
// 	pEdit->SetAttribute("crtext","00CCCC");
	pEdit->SetSel(MAKELONG(5,5));
	pEdit->ReplaceSel(L"repsel");
	TCHAR szBuf[100]={0};
	pEdit->GetWindowText(szBuf,100);
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
		CDuiListBox *pListBox=(CDuiListBox*)pItemNHdr->pHostDuiWin;
		int iItem=pListBox->GetItemObjIndex(pItemNHdr->pItem);
		pCombobox->DeleteItem(iItem); 
	}
	return S_OK;
}


void CUIHander::OnIECtrl()
{
}


void CUIHander::OnDuiMenu()
{
	CPoint pt; 
	GetCursorPos(&pt);
	CDuiMenu subMenu=m_menu.GetSubMenu(5);
	CheckMenuRadioItem(subMenu.m_hMenu,51,53,52,MF_BYCOMMAND);
// 	CDuiMenu subMenu2; 
// 	subMenu2.CreatePopupMenu();
// 	subMenu2.InsertMenu(1,MF_STRING|MF_BYPOSITION,59,_T("�������1"),3);
// 	subMenu2.InsertMenu(2,MF_STRING|MF_BYPOSITION|MF_GRAYED,58,_T("�������2"),3);
// 	subMenu.InsertMenu(1,MF_POPUP|MF_BYPOSITION,(UINT_PTR)&subMenu2,_T("�����Ӳ˵�"),5);
	UINT uRet=m_menu.TrackPopupMenu(0,pt.x,pt.y,m_pMainDlg->m_hWnd);
// 	ATLTRACE(L"\nmenu ret=%d",uRet); 
}

void CUIHander::OnCommand( UINT uNotifyCode, int nID, HWND wndCtl )
{
// 	ATLTRACE(L"\nOnCommand nID=%d",nID);  

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
// 	CStringA strURI=CW2A(szURI,CP_UTF8);
// 	pWebkitCtrl->SetURI(strURI);
}

//��ʾ�б��еİ�ť�ؼ�����Ӧ
LRESULT CUIHander::OnListBtnClick( LPNMHDR pNHdr )
{
	DUINMITEMNOTIFY *pNHdrEx=(DUINMITEMNOTIFY*)pNHdr;

	return S_OK;
}