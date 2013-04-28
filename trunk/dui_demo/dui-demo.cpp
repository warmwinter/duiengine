// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "menuwndhook.h"
#include "DuiSystem.h" 
#include "DuiDefaultLogger.h"

#include "DuiSkinGif.h"	//Ӧ�ò㶨���Ƥ������

//��ZIP�ļ�����Ƥ��ģ��
#include "zipskin/DuiResProviderZip.h"


#ifdef _DEBUG
// #include <vld.h>//ʹ��Vitural Leaker Detector������ڴ�й©�����Դ�http://vld.codeplex.com/ ����
#endif

#include "MainDlg.h"
#include "ResModeSelDlg.h"

//��ʾ���ʹ�������ⲿʵ�ֵ�DUI�ؼ�
class  CDuiListBox2 :public CDuiListBoxEx
{
public:
	DUIOBJ_DECLARE_CLASS_NAME(CDuiListBox2, "listboxex")

};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	DUIASSERT(SUCCEEDED(hRes));
 
	//////////////////////////////////////////////////////////////////////////
	//<--��Դ����ѡ�� 
	DuiSystem * pDuiModeSel = new DuiSystem(hInstance);
	DuiSystem::getSingletonPtr();

	DuiResProviderZip *pResModeSel= new DuiResProviderZip;
	pResModeSel->Init(hInstance,IDR_ZIP_MODESEL,_T("ZIP"));
	pDuiModeSel->SetResProvider(pResModeSel);
 	DuiSkinPool::getSingleton().Init(100); // ����Ƥ��,Ϊ�˲������ڳ����еĺ궨�壬�˴���ID����Ӳ����
 	DuiStylePool::getSingleton().Init(101); // ���ط��
 	DuiCSS::getSingleton().Init(104);//������Ĭ������
	int nMode=-1;
 	{
 		CResModeSelDlg dlgModeSel;  
 		if(IDOK==dlgModeSel.DoModal())
 		{
 			nMode=dlgModeSel.m_nMode;
 		}
 	}
	delete pResModeSel;
	delete pDuiModeSel;

	if(nMode==-1) return -1;

	//��Դ����ѡ����� -->
	
	DuiSystem *pDuiSystem=new DuiSystem(hInstance);

	//���ɿؼ��೧��ע�ᵽϵͳ
	TplDuiWindowFactory<CDuiListBox2> *pFacListCtrl= new TplDuiWindowFactory<CDuiListBox2>;
	DuiWindowFactoryManager::getSingleton().RegisterFactory(pFacListCtrl,true);

	//����Ƥ���೧��ע�ᵽϵͳ
	TplSkinFactory<CDuiSkinGif> * pFacSkinGif = new TplSkinFactory<CDuiSkinGif>;
	DuiSkinFactoryManager::getSingleton().RegisterFactory(pFacSkinGif);


	char szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
	GetModuleFileNameA( NULL, szCurrentDir, sizeof(szCurrentDir) );
	LPSTR lpInsertPos = strrchr( szCurrentDir, L'\\' );
	*lpInsertPos = '\0';   

	DefaultLogger loger;
	loger.setLogFilename(DUI_CA2T(CDuiStringA(szCurrentDir)+"\\dui-demo.log")); 
	pDuiSystem->SetLogger(&loger);
	pDuiSystem->InitName2ID(IDR_NAME2ID,"XML2");//����ID���ƶ��ձ�,����Դ����APP��������Ƥ��Ӧ�ù�������ֱ����ֱ����Ǵӳ�����Դ����

	//����ѡ�����Դ����ģʽ����resprovider 
	switch(nMode)
	{
	case 0://load from files
		{
			lstrcatA( szCurrentDir, "\\..\\dui_demo" );
			DuiResProviderFiles *pResFiles=new DuiResProviderFiles;
			if(!pResFiles->Init(szCurrentDir))
			{
				DUIASSERT(0);
				return 1;
			}
			pDuiSystem->SetResProvider(pResFiles);
			pDuiSystem->logEvent(_T("load resource from files"));
		}
		break;
	case 1://load from PE
		{
			pDuiSystem->SetResProvider(new DuiResProviderPE(hInstance));
			pDuiSystem->logEvent(_T("load resource from exe"));
		}
		break;
	default://load form ZIP
		{
			//��ZIP�ļ�����Ƥ��
			DuiResProviderZip *zipSkin=new DuiResProviderZip;
			CDuiStringT strZip=DUI_CA2T(szCurrentDir)+_T("\\def_skin.zip");
			if(!zipSkin->Init(strZip))
			{ 
				DUIASSERT(0);
				return 1;
			}
			pDuiSystem->SetResProvider(zipSkin); 
			pDuiSystem->logEvent(_T("load resource from zip"));
		}
		break;
	}

// 2013ǰ�汾ʹ�õ�ϵͳ��ʼ�����룬�°汾ʹ��DuiSystem::Init������
//	���� Load xx ������Ǳ���ģ�����Ƥ����������ʾ���ⲿ����Դ����Ƥ�����𣬲�ͬ��Ƥ�������в�ͬ�Ķ���
// 	BOOL bOK=DuiStringPool::getSingleton().Init(IDR_DUI_STRING_DEF); // �����ַ���
// 	DuiFontPool::getSingleton().SetDefaultFont(DuiStringPool::getSingleton().Get(IDS_APP_FONT), -12); // ��������,������style����ǰ���á�
// 	bOK=DuiSkinPool::getSingleton().Init(IDR_DUI_SKIN_DEF); // ����Ƥ��
// 	bOK=DuiStylePool::getSingleton().Init(IDR_DUI_STYLE_DEF); // ���ط��
// 	bOK=DuiCSS::getSingleton().Init(IDR_DUI_OBJATTR_DEF);//������Ĭ������

	BOOL bOK=pDuiSystem->Init(IDR_DUI_INIT); //��ʼ��DUIϵͳ,ԭ����ϵͳ��ʼ����ʽ��Ȼ����ʹ�á�
	pDuiSystem->SetMsgBoxTemplate(IDR_DUI_MSGBOX);

#ifdef DLL_DUI
	CLuaScriptModule scriptLua;
	scriptLua.executeScriptFile("..\\dui_demo\\lua\\test.lua");
	pDuiSystem->SetScriptModule(&scriptLua);
#endif

	CMenuWndHook::InstallHook(hInstance,"skin_menuborder");
	int nRet = 0; 
	// BLOCK: Run application
	{
		pDuiSystem->logEvent(_T("demo started"));
		CMainDlg dlgMain;  
		nRet = dlgMain.DoModal();  
		pDuiSystem->logEvent(_T("demo end"));
	}


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

	OleUninitialize();
	return nRet;
}
