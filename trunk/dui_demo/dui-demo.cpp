// magicbox2.cpp : main source file for magicbox2.exe
//

#include "stdafx.h"
#include "menuwndhook.h"
#include "DuiSystem.h"

#ifdef _DEBUG
#include "..\memleakdetect\MemLeakDetect.h"
static CMemLeakDetect memLeakDetect;
#endif
 
#include "MainDlg.h"
 

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
 
	DuiSystem duiSystem(hInstance);
     
#ifdef __DUIFILE_RC 
    char szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
    GetModuleFileNameA( NULL, szCurrentDir, sizeof(szCurrentDir) );
    LPSTR lpInsertPos = strrchr( szCurrentDir, L'\\' );
    *lpInsertPos = '\0';   
    lstrcatA( szCurrentDir, "\\..\\dui_demo" );
	DuiResManager::getSingleton().SetResourcePath( szCurrentDir, "layout\\idmap.xml");
	DuiResManager::getSingleton().SetResourcePath( szCurrentDir, "layout\\def_skin_idmap.xml");
#else
	DuiResManager::getSingleton().SetResourceDLL(hInstance);
#endif
	// ���� Load xx ������Ǳ���ģ�����Ƥ����������ʾ
	DuiName2ID::getSingleton().Init(IDR_DUI_NAME2ID);		//����ID���ƶ��ձ�
	DuiString::getSingleton().Init(IDR_DUI_STRING_DEF); // �����ַ���
	DuiFontPool::getSingleton().SetDefaultFont(DuiString::getSingleton().Get(IDS_APP_FONT), -12); // ��������
 
	DuiSkinPool::getSingleton().Init(IDR_DUI_SKIN_DEF); // ����Ƥ��
	DuiStylePool::getSingleton().Init(IDR_DUI_STYLE_DEF); // ���ط��
	DuiCSS::getSingleton().Init(IDR_DUI_OBJATTR_DEF);//������Ĭ������

	CMenuWndHook::InstallHook(hInstance,"skin_menuborder");
	int nRet = 0; 
	// BLOCK: Run application
	{
		CMainDlg dlgMain;  
		nRet = dlgMain.DoModal();  
	}
	//�ͷ���Դ 
	CMenuWndHook::UnInstallHook();

	CoUninitialize();
	return nRet;
}
