// dllmain.cpp : DllMain ��ʵ�֡�

#include "duistd.h"

class CDuiEngineModule : public CAtlDllModuleT< CDuiEngineModule >
{
} _AtlModule;

// DLL ��ڵ�
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
