�����ṩ�����ϴ�����չ�ؼ�.
ʹ��ʱ��ע��.
�ٶ�һ���ؼ���ΪCTestCtrl
�ؼ�ע��:
	TplDuiWindowFactory<CTestCtrl> *pFacTestCtrl= new TplDuiWindowFactory<CTestCtrl>;
	DuiWindowFactoryManager::getSingleton().RegisterFactory(pFacTestCtrl);
��ע��:
	DuiWindowFactoryManager::getSingleton().UnregisterFactory(pFacTestCtrl);
	delete pFacTestCtrl;
