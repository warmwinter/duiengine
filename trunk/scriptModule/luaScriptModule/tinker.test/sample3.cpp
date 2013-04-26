// sample3.cpp : LuaTinker �� Ŭ���� ����� �˾ƺ���.
//
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

#include "lua_tinker.h"

namespace NS{
struct A
{
	A(int v) : value(v) {}
	int value;
};

struct base
{
	base() {}

	const char* is_base(){ return "this is base"; }
};

class test : public base
{
public:
	test(int val) : _test(val) {}
	~test() {}

	const char* is_test(){ return "this is test"; }

	void ret_void() {}
	int ret_int()				{ return _test;			}
	int ret_mul(int m) const	{ return _test * m;		}
	A get()						{ return A(_test);		}
	void set(A a)				{ _test = a.value;		}

	HWND GetWnd(){
		HWND hRet=GetActiveWindow();
		return (HWND)0x12345678;
	}

	void SetWnd(HWND hWnd)
	{
		HWND hNewWnd=hWnd;
	}
	int _test;
};

test g_test(11);
}

int main()
{
	// Lua �� �ʱ�ȭ �Ѵ�.
	lua_State* L = lua_open();

	// Lua �⺻ �Լ����� �ε��Ѵ�.- print() ���
	luaopen_base(L);
	// Lua ���ڿ� �Լ����� �ε��Ѵ�.- string ���
	luaopen_string(L);

	// base Ŭ������ Lua �� �߰��Ѵ�.
	lua_tinker::class_add<NS::base>(L, "base");
	// base �� �Լ��� ����Ѵ�.
	lua_tinker::class_def<NS::base>(L, "is_base", &NS::base::is_base);

	// test Ŭ������ Lua �� �߰��Ѵ�.
	lua_tinker::class_add<NS::test>(L, "test");
	// test �� base �� ��� �޾����� �˷��ش�.
	lua_tinker::class_inh<NS::test, NS::base>(L);
	// test Ŭ���� �����ڸ� ����Ѵ�.
	lua_tinker::class_con<NS::test>(L, lua_tinker::constructor<NS::test,int>);
	// test �� �Լ����� ����Ѵ�.
	lua_tinker::class_def<NS::test>(L, "is_test", &NS::test::is_test);
	lua_tinker::class_def<NS::test>(L, "ret_void", &NS::test::ret_void);
	lua_tinker::class_def<NS::test>(L, "ret_int", &NS::test::ret_int);
	lua_tinker::class_def<NS::test>(L, "ret_mul", &NS::test::ret_mul);
	lua_tinker::class_def<NS::test>(L, "get", &NS::test::get);
	lua_tinker::class_def<NS::test>(L, "set", &NS::test::set);
	lua_tinker::class_mem<NS::test>(L, "_test", &NS::test::_test);
	lua_tinker::class_def<NS::test>(L, "getwnd", &NS::test::GetWnd);
	lua_tinker::class_def<NS::test>(L, "setwnd", &NS::test::SetWnd);

	// Lua ���� ����ȣ g_test �� �����͸� ����Ѵ�.
	lua_tinker::set(L, "g_test", &NS::g_test);

	// sample3.lua ������ �ε�/�����Ѵ�.
	lua_tinker::dofile(L, "sample3.lua");

	HWND hWnd=lua_tinker::call<HWND>(L,"lua_getwnd");

	// ���α׷� ����
	lua_close(L);

	return 0;
}

