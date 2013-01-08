#pragma once

// DuiWindow Message Map Define
// Use WTL Message Map Ex (include atlcrack.h)
#define DUIWIN_BEGIN_MSG_MAP()                                       \
protected:                                                          \
	virtual BOOL ProcessDuiWndMessage(                              \
	UINT uMsg, WPARAM wParam,                        \
	LPARAM lParam, LRESULT& lResult)                            \
	{

#define DUIWIN_END_MSG_MAP()                                        \
	if (!IsMsgHandled())                                        \
	return __super::ProcessDuiWndMessage(                   \
	uMsg, wParam, lParam, lResult);                     \
	return TRUE;                                                \
	}

#define DUIWIN_END_MSG_MAP_BASE()                                    \
	return DuiWndProc(uMsg,wParam,lParam,lResult);               \
	}


#define WM_CALCWNDPOS	WM_GETMINMAXINFO

#define MSG_WM_CALCWNDPOS(func) \
	if (uMsg == WM_CALCWNDPOS) \
{ \
	SetMsgHandled(TRUE); \
	func((CDuiWindow*)lParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

#define WM_CALCSIZE		WM_SIZING

#define MSG_WM_CALCSIZE(func) \
	if (uMsg == WM_CALCSIZE) \
{ \
	SetMsgHandled(TRUE); \
	lResult = func((BOOL)wParam, (LPSIZE)lParam); \
	if(IsMsgHandled()) \
	return TRUE; \
}

#define MSG_WM_DUIWINPOSCHANGED(func) \
	if (uMsg == WM_WINDOWPOSCHANGED) \
{ \
	SetMsgHandled(TRUE); \
	func((LPDUIWNDPOS)lParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}


// void OnNcPaint(CDCHandle dc)
#define MSG_WM_NCPAINT_EX(func) \
	if (uMsg == WM_NCPAINT) \
{ \
	SetMsgHandled(TRUE); \
	func((HDC)wParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

// void OnSetDuiFocus()
#define MSG_WM_SETFOCUS_EX(func) \
	if (uMsg == WM_SETFOCUS) \
{ \
	SetMsgHandled(TRUE); \
	func(); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

// void OnKillFocus()
#define MSG_WM_KILLFOCUS_EX(func) \
	if (uMsg == WM_KILLFOCUS) \
{ \
	SetMsgHandled(TRUE); \
	func(); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

// void OnNcMouseHover(int nFlag,CPoint pt)
#define MSG_WM_NCMOUSEHOVER(func) \
	if(uMsg==WM_NCMOUSEHOVER)\
{\
	SetMsgHandled(TRUE); \
	func(wParam,CPoint(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam))); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

// void OnNcMouseLeave()
#define MSG_WM_NCMOUSELEAVE(func) \
	if(uMsg==WM_NCMOUSELEAVE)\
{\
	SetMsgHandled(TRUE); \
	func(); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}


// void OnDuiTimer(char cTimerID)
#define MSG_WM_DUITIMER(func) \
	if (uMsg == WM_TIMER) \
{ \
	SetMsgHandled(TRUE); \
	func((char)wParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}

#define UM_DUI_TIMEREX	(WM_USER+5432)	//����һ����HWND��ʱ�����ݵ�DUI��ʱ��

#define MSG_UM_TIMEREX(func) \
	if (uMsg == UM_DUI_TIMEREX) \
{ \
	SetMsgHandled(TRUE); \
	func(wParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}
