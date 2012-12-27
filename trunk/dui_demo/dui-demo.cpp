// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "menuwndhook.h"
#include "DuiSystem.h" 
#include "DuiDefaultLogger.h"

#include "DuiSkinGif.h"	//Ӧ�ò㶨���Ƥ������

//��ZIP�ļ�����Ƥ��ģ��
#include "../zipresprovider/DuiResProviderZip.h"

#ifdef DEBUG
#pragma comment(lib,"zlib_d.lib")
#pragma comment(lib,"zipresprovider_d.lib")
#else
#pragma comment(lib,"zlib.lib")
#pragma comment(lib,"zipresprovider.lib")
#endif 

#ifdef _DEBUG
#include <vld.h>//ʹ��Vitural Leaker Detector������ڴ�й©�����Դ�http://vld.codeplex.com/ ����
#endif

#include "MainDlg.h"

//��ʾ���ʹ�������ⲿʵ�ֵ�DUI�ؼ�
class  CDuiListBox2 :public CDuiListBoxEx
{
public:
	DUIOBJ_DECLARE_CLASS_NAME(CDuiListBox2, "listbox2")

};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = CoInitialize(NULL);
	DUIASSERT(SUCCEEDED(hRes));
 
	char szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
	GetModuleFileNameA( NULL, szCurrentDir, sizeof(szCurrentDir) );
	LPSTR lpInsertPos = strrchr( szCurrentDir, L'\\' );
	*lpInsertPos = '\0';   


	DuiSystem *pDuiSystem=new DuiSystem(hInstance);

	//���ɿؼ��೧��ע�ᵽϵͳ
	TplDuiWindowFactory<CDuiListBox2> *pFacListCtrl= new TplDuiWindowFactory<CDuiListBox2>;
	DuiWindowFactoryManager::getSingleton().RegisterFactory(pFacListCtrl);

	//����Ƥ���೧��ע�ᵽϵͳ
	TplSkinFactory<CDuiSkinGif> * pFacSkinGif = new TplSkinFactory<CDuiSkinGif>;
	DuiSkinFactoryManager::getSingleton().RegisterFactory(pFacSkinGif);

	DefaultLogger loger;
	loger.setLogFilename(CStringA(szCurrentDir)+"\\dui-demo.log");
	pDuiSystem->SetLogger(&loger);

	//��ZIP�ļ�����Ƥ��
// 	DuiResProviderZip *zipSkin=new DuiResProviderZip;
// 	CString strZip=CA2T(szCurrentDir)+_T("\\def_skin.zip");
// 	if(!zipSkin->Init(strZip))
// 	{ 
// 		DUIASSERT(0);
// 		return 1;
// 	}
// 	duiSystem.SetResProvider(zipSkin); 

	pDuiSystem->logEvent("demo started");
	pDuiSystem->InitName2ID(IDR_NAME2ID,"XML2");//����ID���ƶ��ձ�,����Դ����APP��������Ƥ��Ӧ�ù�������ֱ����ֱ����Ǵӳ�����Դ����
#ifdef __DUIFILE_RC 
    lstrcatA( szCurrentDir, "\\..\\dui_demo" );
	DuiResProviderFiles *pResFiles=new DuiResProviderFiles;
	if(!pResFiles->Init(szCurrentDir))
	{
		DUIASSERT(0);
		return 1;
	}
	pDuiSystem->SetResProvider(pResFiles);
#else 
	pDuiSystem->SetResProvider(new DuiResProviderPE(hInstance));
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

	pDuiSystem->logEvent("demo end");

	delete pDuiSystem->GetResProvider();
	//�ͷ���Դ 
	CMenuWndHook::UnInstallHook();

	DuiSkinPool::getSingleton().RemoveAll();//��DLL��ʽʹ��DuiEngineʱ��ʹ�����Զ���Ƥ������ʱ��Ҫ��ɾ��Ƥ���ز��������ͷ�Ƥ���೧��

	//��ϵͳ�з�ע��ؼ���Ƥ���೧��ɾ���೧����
	DuiWindowFactoryManager::getSingleton().UnregisterFactory(pFacListCtrl);
	delete pFacListCtrl;
	DuiSkinFactoryManager::getSingleton().UnregisterFactory(pFacSkinGif);
	delete pFacSkinGif;

	delete pDuiSystem;

	CoUninitialize();
	return nRet;
}
