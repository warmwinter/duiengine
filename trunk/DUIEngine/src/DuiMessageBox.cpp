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
				FindChildByName(NAME_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN3PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��ֹ"));pBtn->SetCmdID(IDABORT);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("����"));pBtn->SetCmdID(IDRETRY);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN3);
				pBtn->SetInnerText(_T("����"));pBtn->SetCmdID(IDIGNORE);
			}
			break;
		case MB_YESNOCANCEL:
			{
				FindChildByName(NAME_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN3PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��"));pBtn->SetCmdID(IDYES);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("��"));pBtn->SetCmdID(IDNO);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN3);
				pBtn->SetInnerText(_T("ȡ��"));pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_OKCANCEL:
			{
				FindChildByName(NAME_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("ȷ��"));	pBtn->SetCmdID(IDOK);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("ȡ��"));	pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_YESNO:
			{
				FindChildByName(NAME_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("��"));	pBtn->SetCmdID(IDYES);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("��"));	pBtn->SetCmdID(IDNO);
			}
			break;
		case MB_RETRYCANCEL:
			{
				FindChildByName(NAME_MSGBOX_BTN1PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN2PANEL);
				pBtnPanel->SetVisible(TRUE);

				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
				pBtn->SetInnerText(_T("����"));	pBtn->SetCmdID(IDRETRY);
				pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN2);
				pBtn->SetInnerText(_T("ȡ��"));	pBtn->SetCmdID(IDCANCEL);
			}
			break;
		case MB_OK:
			{
				FindChildByName(NAME_MSGBOX_BTN2PANEL)->SetVisible(FALSE);
				FindChildByName(NAME_MSGBOX_BTN3PANEL)->SetVisible(FALSE);

				CDuiWindow *pBtnPanel=FindChildByName(NAME_MSGBOX_BTN1PANEL);
				pBtnPanel->SetVisible(TRUE);
				CDuiWindow *pBtn=pBtnPanel->FindChildByName(NAME_MSGBOX_BTN1);
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

		CDuiWindow * pTitle= FindChildByName(NAME_MSGBOX_TITLE);
		DUIASSERT(pTitle);
		pTitle->SetInnerText(lpCaption?lpCaption:_T("��ʾ"));

		CDuiWindow * pMsg= FindChildByName(NAME_MSGBOX_TEXT);
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
		CDuiIconWnd *pIcon=(CDuiIconWnd *)FindChildByName(NAME_MSGBOX_ICON);
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
