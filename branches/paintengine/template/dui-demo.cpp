// magicbox2.cpp : main source file for magicbox2.exe
//

#include "stdafx.h"
#include "DuiSystem.h" 
#include "DuiDefaultLogger.h"
 

#include "MainDlg.h"
 

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	HRESULT hRes = CoInitialize(NULL);
	DUIASSERT(SUCCEEDED(hRes));
 
	TCHAR szCurrentDir[MAX_PATH+1]; memset( szCurrentDir, 0, sizeof(szCurrentDir) );
	GetModuleFileName( NULL, szCurrentDir, MAX_PATH );
	LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
	*lpInsertPos = _T('\0');   

	DuiSystem duiSystem(hInstance);
	DefaultLogger loger;
	loger.setLogFilename(CDuiStringT(szCurrentDir)+_T("\\duiengine.log"));
	duiSystem.SetLogger(&loger);

	duiSystem.logEvent(_T("demo started"));
	duiSystem.InitName2ID(IDR_NAME2ID,_T("XML2"));//����ID���ƶ��ձ�,����Դ����APP��������Ƥ��Ӧ�ù�������ֱ����ֱ����Ǵӳ�����Դ����
#ifdef __DUIFILE_RC 
	//���ļ��м���Ƥ��,ָ��Ƥ��λ��
	_tcscat( szCurrentDir, _T("\\..\\template\\skin") );
	DuiResProviderFiles *pResFiles=new DuiResProviderFiles;
	if(!pResFiles->Init(szCurrentDir))
	{
		DUIASSERT(0);
		return 1;
	}
	duiSystem.SetResProvider(pResFiles);
#else 
	//����Դ����Ƥ��
	duiSystem.SetResProvider(new DuiResProviderPE(hInstance));
#endif 
	// ���� Load xx ������Ǳ���ģ�����Ƥ����������ʾ���ⲿ����Դ����Ƥ�����𣬲�ͬ��Ƥ�������в�ͬ�Ķ���
	DuiStringPool::getSingleton().Init(IDR_DUI_STRING_DEF); // �����ַ���
	DuiFontPool::getSingleton().SetDefaultFont(_T("����"), -12); // ��������
	DuiSkinPool::getSingleton().Init(IDR_DUI_SKIN_DEF); // ����Ƥ��
	DuiStylePool::getSingleton().Init(IDR_DUI_STYLE_DEF); // ���ط��
	DuiCSS::getSingleton().Init(IDR_DUI_OBJATTR_DEF);//������Ĭ������

	int nRet = 0; 
	// BLOCK: Run application
	{
		CMainDlg dlgMain;  
		nRet = dlgMain.DoModal();  
	}

	duiSystem.logEvent(_T("demo end"));

	delete duiSystem.GetResProvider();

	CoUninitialize();
	return nRet;
}
