// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "menuwndhook.h"
#include "DuiSystem.h" 
#include "DuiDefaultLogger.h"

#include "DuiSkinGif.h"	//Ӧ�ò㶨���Ƥ������

//��ZIP�ļ�����Ƥ��ģ��
#if !defined(_WIN64)
#include "zipskin/DuiResProviderZip.h"
#endif

#ifdef _DEBUG
#include <vld.h>//ʹ��Vitural Leaker Detector������ڴ�й©�����Դ�http://vld.codeplex.com/ ����
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

#if !defined(_WIN64)
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
#else
	int nMode=0;	//64λʱֱ�Ӵ��ļ�������Դ
#endif
	//��Դ����ѡ����� -->
	
	DuiSystem *pDuiSystem=new DuiSystem(hInstance);

	//���ɿؼ��೧��ע�ᵽϵͳ
	DuiWindowFactoryManager::getSingleton().RegisterFactory(TplDuiWindowFactory<CDuiListBox2>(),true);

	//����Ƥ���೧��ע�ᵽϵͳ
	DuiSkinFactoryManager::getSingleton().RegisterFactory(TplSkinFactory<CDuiSkinGif>());


	TCHAR szCurrentDir[MAX_PATH]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
	GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
	LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
	*lpInsertPos = _T('\0');   

	DefaultLogger loger;
	loger.setLogFilename(CDuiStringT(szCurrentDir)+_T("\\dui-demo.log")); 
	pDuiSystem->SetLogger(&loger);
	pDuiSystem->InitName2ID(IDR_NAME2ID,_T("XML2"));//����ID���ƶ��ձ�,����Դ����APP��������Ƥ��Ӧ�ù�������ֱ����ֱ����Ǵӳ�����Դ����

	//����ѡ�����Դ����ģʽ����resprovider 
	switch(nMode)
	{
	case 0://load from files
		{
			_tcscat( szCurrentDir, _T("\\..\\dui_demo") );
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
#if !defined(_WIN64)
			//��ZIP�ļ�����Ƥ��
			DuiResProviderZip *zipSkin=new DuiResProviderZip;
			CDuiStringT strZip=CDuiStringT(szCurrentDir)+_T("\\def_skin.zip");
			if(!zipSkin->Init(strZip))
			{ 
				DUIASSERT(0);
				return 1;
			}
			pDuiSystem->SetResProvider(zipSkin); 
			pDuiSystem->logEvent(_T("load resource from zip"));
#else
			return -1;
#endif;
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

#if defined(DLL_DUI) && !defined(_WIN64)
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
	DuiWindowFactoryManager::getSingleton().UnregisterFactory(CDuiListBox2::GetClassName());
	DuiSkinFactoryManager::getSingleton().UnregisterFactory(CDuiSkinGif::GetClassName());

	delete pDuiSystem;

	OleUninitialize();
	return nRet;
}
