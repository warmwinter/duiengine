// magicbox2.cpp : main source file for magicbox2.exe
//

#include "stdafx.h"
#include "menuwndhook.h"
#include "DuiSystem.h"
#include "DuiDefaultLogger.h"

#ifdef _DEBUG
#include "..\memleakdetect\MemLeakDetect.h"
// static CMemLeakDetect memLeakDetect;
#endif
 
#include "MainDlg.h"
 

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
 
	char szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
	GetModuleFileNameA( NULL, szCurrentDir, sizeof(szCurrentDir) );
	LPSTR lpInsertPos = strrchr( szCurrentDir, L'\\' );
	*lpInsertPos = '\0';   

	DuiSystem duiSystem(hInstance);
	DefaultLogger *pLoger=new DefaultLogger;
	pLoger->setLogFilename(CStringA(szCurrentDir)+"\\dui-demo.log");
	duiSystem.SetLogger(pLoger);


	duiSystem.logEvent("demo started");
	duiSystem.InitName2ID(IDR_DUI_NAME2ID);//����ID���ƶ��ձ�,����Դ����APP��������Ƥ��Ӧ�ù�������ֱ����ֱ����Ǵӳ�����Դ����
#ifdef __DUIFILE_RC 
    char szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
    GetModuleFileNameA( NULL, szCurrentDir, sizeof(szCurrentDir) );
    LPSTR lpInsertPos = strrchr( szCurrentDir, L'\\' );
    *lpInsertPos = '\0';   
    lstrcatA( szCurrentDir, "\\..\\dui_demo" );
	duiSystem.SetResProvider(new DuiResProviderFiles(szCurrentDir));

#else
	duiSystem.SetResProvider(new DuiResProviderPE(hInstance));
#endif
	// ���� Load xx ������Ǳ���ģ�����Ƥ����������ʾ���ⲿ����Դ����Ƥ�����𣬲�ͬ��Ƥ�������в�ͬ�Ķ���
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

	duiSystem.logEvent("demo end");

	delete duiSystem.GetResProvider();
	delete duiSystem.GetLogger();
	//�ͷ���Դ 
	CMenuWndHook::UnInstallHook();

	CoUninitialize();
	return nRet;
}
