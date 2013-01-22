// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0601
#define _RICHEDIT_VER	0x0200

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#ifdef DLL_DUI
# ifdef DUIENGINE_EXPORTS
# pragma message("dll export")
#  define DUI_EXP __declspec(dllexport)
# else
#  define DUI_EXP __declspec(dllimport)
# endif // DUIENGINE_EXPORTS
#else
#define DUI_EXP
#endif

# pragma warning(disable:4661)
# pragma warning(disable:4251)

#include <Windows.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <OleCtl.h>
#include <assert.h>
#include <tchar.h>


#define DUIASSERT(x) assert(x)
void DUI_EXP DuiTrace(LPCTSTR pstrFormat, ...);
#define DUITRACE DuiTrace

#include "..\dependencies\tinyxml\tinyxml.h"

#ifdef DEBUG
#pragma comment(lib,"tinyxml_d.lib")
#else
#pragma comment(lib,"tinyxml.lib")
#endif//DEBUG


#ifndef NO_DUITYPES
#define _WTYPES_NS DuiEngine
#include "wtl.mini/duicrack.h"
#include "wtl.mini/duimisc.h"
#include "wtl.mini/duigdi.h"
#endif

#include "wtl.mini/duistr.h" //注意：如果CDuiStringT已经定义，可以定义NO_DUISTR来防止命名冲突
#include "wtl.mini/duicoll.h"

#include "DuiAttrCrack.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
