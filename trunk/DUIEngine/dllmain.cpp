// dllmain.cpp : DllMain ��ʵ�֡�

#include "duistd.h"

#if defined(USING_ATL) && defined(DLL_DUI)
//ֻ��ʹ��ATL�Ķ�̬��汾����ҪDllMain

class CDuiEngineModule : public CAtlDllModuleT< CDuiEngineModule >
{
} _AtlModule;

// DLL ��ڵ�
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

#endif