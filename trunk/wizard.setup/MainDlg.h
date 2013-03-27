// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define NAME_DUIWIZARD	_T("DuiEngineWizard")

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP_EX(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BROWSE, OnBrowse)
		COMMAND_ID_HANDLER(IDC_BROWSE2, OnBrowse2)
		COMMAND_ID_HANDLER(IDC_INSTALL, OnInstall)
		COMMAND_ID_HANDLER(IDC_UNINSTALL, OnUninstall)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		return TRUE;
	}

	LRESULT OnBrowse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog folderDlg;
		if(folderDlg.DoModal()==IDOK)
		{
			SetDlgItemText(IDC_VSDIR,folderDlg.GetFolderPath());
		}
		return 0;
	}

	LRESULT OnBrowse2(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog folderDlg;
		if(folderDlg.DoModal()==IDOK)
		{
			SetDlgItemText(IDC_DUIENGINEDIR,folderDlg.GetFolderPath());
		}
		return 0;
	}

	BOOL VarifyVsDir(TCHAR *pszBuf,int nSize)
	{
		int nLen=GetDlgItemText(IDC_VSDIR,pszBuf,nSize);
		if(pszBuf[nLen-1]==_T('\\')) pszBuf[nLen-1]=0;
		_tcscat(pszBuf,_T("\\VC\\vcprojects"));
		DWORD dwAttr=GetFileAttributes(pszBuf);
		if(dwAttr==INVALID_FILE_ATTRIBUTES) return FALSE;
		return dwAttr&FILE_ATTRIBUTE_DIRECTORY;
	}


	LRESULT OnInstall(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		TCHAR szVsDir[MAX_PATH]={0};
		if(!VarifyVsDir(szVsDir,MAX_PATH))
		{
			MessageBox(_T("����ָ��VS��װĿ¼"),_T("����"),MB_OK|MB_ICONSTOP);
		}else
		{
			TCHAR szBuf[1024]={0};
			TCHAR *p=szBuf;
			CString str;
			str=CString(NAME_DUIWIZARD)+_T(".ico");
			if(GetFileAttributes(str)==INVALID_FILE_ATTRIBUTES)
			{
				MessageBox(_T("��ǰĿ¼��û���ҵ�DuiEngine��������"),_T("����"),MB_OK|MB_ICONSTOP);
				return 0;
			}
			TCHAR szDuiEngineDir[MAX_PATH]={0};
			int nLen=GetDlgItemText(IDC_DUIENGINEDIR,szDuiEngineDir,MAX_PATH);
			if(szDuiEngineDir[nLen-1]==_T('\\')) szDuiEngineDir[--nLen]=0;

			_tcscat(szDuiEngineDir,_T("\\DuiEngine"));
			if(GetFileAttributes(szDuiEngineDir)==INVALID_FILE_ATTRIBUTES)
			{
				MessageBox(_T("��ǰĿ¼��û���ҵ�DuiEngine��Դ����"),_T("����"),MB_OK|MB_ICONSTOP);
				return 0;
			}
			szDuiEngineDir[nLen]=0;
			//���û�������

			CRegKeyEx reg;
			if(ERROR_SUCCESS==reg.Open(HKEY_LOCAL_MACHINE,_T("System\\CurrentControlSet\\Control\\Session Manager\\Environment"),KEY_SET_VALUE))
			{
				reg.SetStringValue(_T("DUIENGINEPATH"),szDuiEngineDir);
				reg.Close();
			}else
			{
				MessageBox(_T("��ӻ�������ʧ��"),_T("����"),MB_OK|MB_ICONSTOP);
				return 0;
			}

			_tcscpy(p,str);
			p+=str.GetLength()+1;
			str=CString(NAME_DUIWIZARD)+_T(".vsdir");
			_tcscpy(p,str);
			p+=str.GetLength()+1;
			str=CString(NAME_DUIWIZARD)+_T(".vsz");
			_tcscpy(p,str);
			p+=str.GetLength()+1;

			str=CString(NAME_DUIWIZARD);
			_tcscpy(p,str);
			p+=str.GetLength()+1;

			*p=0;

			SHFILEOPSTRUCT shfo;
			shfo.pFrom=szBuf;
			shfo.pTo=szVsDir;
			shfo.wFunc=FO_COPY;
			shfo.fFlags=FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;
			
			if(0==SHFileOperation(&shfo))
			{
				MessageBox(_T("��װ�ɹ�����ӭʹ��DuiEngine"),_T("��ϲ"),MB_OK);
			}
		}
		return 0;
	}

	LRESULT OnUninstall(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		TCHAR szVsDir[MAX_PATH]={0};
		if(!VarifyVsDir(szVsDir,MAX_PATH))
		{
			MessageBox(_T("����ָ��VS��װĿ¼"),_T("����"),MB_OK|MB_ICONSTOP);
		}else
		{
			TCHAR szCurDir[MAX_PATH];
			GetCurrentDirectory(MAX_PATH,szCurDir);
			SetCurrentDirectory(szVsDir);

			TCHAR szBuf[1024]={0};
			TCHAR *p=szBuf;
			CString str;
			str=CString(NAME_DUIWIZARD)+_T(".ico");
			_tcscpy(p,str);
			p+=str.GetLength()+1;
			str=CString(NAME_DUIWIZARD)+_T(".vsdir");
			_tcscpy(p,str);
			p+=str.GetLength()+1;
			str=CString(NAME_DUIWIZARD)+_T(".vsz");
			_tcscpy(p,str);
			p+=str.GetLength()+1;

			str=CString(NAME_DUIWIZARD);
			_tcscpy(p,str);
			p+=str.GetLength()+1;

			*p=0;

			SHFILEOPSTRUCT shfo;
			shfo.pFrom=szBuf;
			shfo.pTo=NULL;
			shfo.wFunc=FO_DELETE;
			shfo.fFlags=FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;

			if(0==SHFileOperation(&shfo))
			{
				MessageBox(_T("��ӭ��������������http://www.setoutsoft.cn"),_T("ж����ɣ�"),MB_OK);
			}
			SetCurrentDirectory(szCurDir);
		}
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		EndDialog(wID);
		return 0;
	}
};
