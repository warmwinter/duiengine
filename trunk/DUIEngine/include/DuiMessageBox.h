/********************************************************************
	created:	2013/02/19
	created:	19:2:2013   10:11
	filename: 	DuiMessageBox.h
	author:		Huang Jianxiong
	
	purpose:	ģ��һ��MessageBox
*********************************************************************/
#pragma once

#include "duihostwnd.h"

namespace DuiEngine
{
	//�����Ǽ�����msgboxģ���б���ָ����ID��
	#define IDC_MSGBOX_BTN1PANEL	65400	//����������ť��panel
	#define IDC_MSGBOX_BTN2PANEL	65401	//����2����ť��panel
	#define IDC_MSGBOX_BTN3PANEL	65402	//����3����ť��panel
	#define IDC_MSGBOX_TEXT			65403	//�ı��ؼ���ֻ��Ҫָ����������
	#define IDC_MSGBOX_TITLE		65404	//����ID
	#define IDC_MSGBOX_ICON			65405	//ͼ����ʾ�ؼ�
	#define IDC_MSGBOX_BTN1			65410	//��1����ťID����ťID����ʾʱ���Զ��޸�Ϊ��IDOK,IDCANCEL������ID��
	#define IDC_MSGBOX_BTN2			65411	//��2����ťID
	#define IDC_MSGBOX_BTN3			65412	//��3����ťID

	//msgbox����Ϣ������������Ҫ���Ӹ��Ի���msgbox�������������ࡣ
	class DUI_EXP CDuiMessageBox:public CDuiHostWnd
	{
	public:
		int MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );

	protected:
		//�������ظ÷�������ʾ�Զ����ͼ��
		virtual BOOL SetIcon(UINT uType);

		void OnBtnClick(UINT uID)
		{
			EndDialog(uID);
		}


		DUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
			DUI_NOTIFY_ID_COMMAND_EX(IDOK,IDNO, OnBtnClick)
		DUI_NOTIFY_MAP_END()	

		BEGIN_MSG_MAP_EX(CDuiMessageBox)
			MSG_DUI_NOTIFY(IDC_RICHVIEW_WIN)
			CHAIN_MSG_MAP(CDuiHostWnd)
			REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
	};

	int DUI_EXP DuiMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

}//end of namespace 

