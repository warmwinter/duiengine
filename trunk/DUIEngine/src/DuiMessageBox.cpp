#include "duistd.h"
#include "DuiMessageBox.h"
#include "DuiSystem.h"
#include "duiwndcmnctrl.h"

namespace DuiEngine
{


	int CDuiMessageBox::MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
	{
		TiXmlElement *pXmlTempl=DuiSystem::getSingleton().GetMsgBoxTemplate();
		if(!pXmlTempl) return ::MessageBox(hWnd,lpText,lpCaption,uType);

		TiXmlElement * pXmlBody=pXmlTempl->FirstChildElement("body");
		DUIASSERT(pXmlBody);

		Create(NULL,NULL,WS_POPUPWINDOW,0,0,0,10,10,NULL);
		SetXml(pXmlTempl);


		switch(uType&0x0F)
		{
		case MB_ABORTRETRYIGNORE:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN3PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��ֹ"));pBtn->SetCmdID(IDABORT);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("����"));pBtn->SetCmdID(IDRETRY);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN3);
				pBtn->SetInnerText(_T("����"));pBtn->SetCmdID(IDIGNORE);
			}
			break;
		case MB_YESNOCANCEL:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN3PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��"));pBtn->SetCmdID(IDYES);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("��"));pBtn->SetCmdID(IDNO);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN3);
				pBtn->SetInnerText(_T("ȡ��"));pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_OKCANCEL:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("ȷ��"));	pBtn->SetCmdID(IDOK);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("ȡ��"));	pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_YESNO:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��"));	pBtn->SetCmdID(IDYES);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("��"));	pBtn->SetCmdID(IDNO);
			}
			break;
		case MB_RETRYCANCEL:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("����"));	pBtn->SetCmdID(IDRETRY);
				pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("ȡ��"));	pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_OK:
			{
				FindChildByCmdID(IDC_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				FindChildByCmdID(IDC_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByCmdID(IDC_MSGBOX_BTN1PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByCmdID(IDC_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("ȷ��"));	pBtn->SetCmdID(IDOK);
			}
			break;
		default:
			DUIASSERT(FALSE);
			break;
		}
		const char *pszFrameAttr=pXmlTempl->Attribute("frame_size");
		CRect rcFrame;
		sscanf(pszFrameAttr,"%d,%d,%d,%d",&rcFrame.left,&rcFrame.top,&rcFrame.right,&rcFrame.bottom);
		CSize szMin;
		const char *pszMinAttr=pXmlTempl->Attribute("minsize");
		sscanf(pszMinAttr,"%d,%d",&szMin.cx,&szMin.cy);

		CDuiWindow * pTitle= FindChildByCmdID(IDC_MSGBOX_TITLE);
		DUIASSERT(pTitle);
		pTitle->SetInnerText(lpCaption?lpCaption:_T("��ʾ"));

		CDuiWindow * pMsg= FindChildByCmdID(IDC_MSGBOX_TEXT);
		DUIASSERT(pMsg);
		pMsg->SetInnerText(lpText);

		SetIcon(uType);

		CRect rcText;
		pMsg->GetRect(&rcText);

		CSize szWnd;
		szWnd.cx=max(szMin.cx,rcText.Width()+rcFrame.left+rcFrame.right);
		szWnd.cy=max(szMin.cy,rcText.Height()+rcFrame.top+rcFrame.bottom);

		SetWindowPos(0,0,0,szWnd.cx,szWnd.cy,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSENDCHANGING|SWP_NOACTIVATE);

		return DoModal(hWnd);
	}

	BOOL CDuiMessageBox::SetIcon( UINT uType )
	{
		CDuiIconWnd *pIcon=(CDuiIconWnd *)FindChildByCmdID(IDC_MSGBOX_ICON);
		if(!pIcon) return FALSE;
		switch(uType&0xF0)
		{
		case MB_ICONEXCLAMATION:
			pIcon->AttachIcon(LoadIcon(NULL,IDI_EXCLAMATION));
			break;
		case MB_ICONINFORMATION:
			pIcon->AttachIcon(LoadIcon(NULL,IDI_INFORMATION));
			break;
		case MB_ICONQUESTION:
			pIcon->AttachIcon(LoadIcon(NULL,IDI_QUESTION));
			break;
		case MB_ICONHAND:
			pIcon->AttachIcon(LoadIcon(NULL,IDI_HAND));
			break;
		}
		return TRUE;
	}

	int DuiMessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
	{
		CDuiMessageBox duiMsgBox;
		return duiMsgBox.MessageBox(hWnd,lpText,lpCaption,uType);
	}



}//end of namespace 
