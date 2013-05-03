#pragma once
#include "duiwnd.h"
#include "Accelerator.h"

namespace DuiEngine
{

class DUI_EXP CDuiHotKeyCtrl 
	: public CDuiWindow
	, public CAccelerator
{
    DUIOBJ_DECLARE_CLASS_NAME(CDuiHotKeyCtrl, "hotkey")
public:
    CDuiHotKeyCtrl(void);
    virtual ~CDuiHotKeyCtrl(void);

    void SetRule(WORD wInvalidComp,WORD wModifier);

    void SetHotKey(WORD vKey,WORD wModifiers);

    void GetHotKey(WORD & vKey,WORD &wModifers);

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_WORD_ATTRIBUTE("invalidcomb",m_wInvalidComb,FALSE)
    DUIWIN_WORD_ATTRIBUTE("defcombkey",m_wInvalidModifier,FALSE)
    DUIWIN_WORD_ATTRIBUTE("combkey",m_wModifier,FALSE)
    DUIWIN_WORD_ATTRIBUTE("hotkey",m_wVK,FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()

protected:
    virtual BOOL IsTabStop()
    {
        return TRUE;
    }
    virtual UINT OnGetDuiCode()
    {
        return DUIC_WANTALLKEYS|DUIC_WANTSYSKEY & (~DUIC_WANTTAB);
    }

    int OnCreate(LPVOID);

    void OnLButtonDown(UINT nFlags,CPoint pt);

    void OnPaint(CDCHandle dc);

    void OnSetDuiFocus();

    void OnKillDuiFocus();

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

    void UpdateModifier();

    void UpdateCaret();

    DUIWIN_BEGIN_MSG_MAP()
    MSG_WM_CREATE(OnCreate)
    MSG_WM_PAINT(OnPaint)
    MSG_WM_LBUTTONDOWN(OnLButtonDown)
    MSG_WM_SETFOCUS_EX(OnSetDuiFocus)
    MSG_WM_KILLFOCUS_EX(OnKillDuiFocus)
    MSG_WM_KEYDOWN(OnKeyDown)
    MSG_WM_KEYUP(OnKeyUp)
    MSG_WM_SYSKEYDOWN(OnSysKeyDown)
    MSG_WM_SYSKEYUP(OnSysKeyUp)
    DUIWIN_END_MSG_MAP()
    WORD 	m_wInvalidComb;		//��Ч����ϼ�
    WORD 	m_wInvalidModifier; //����Ч��ϼ����滻����,Ĭ�Ϸ���

    HFONT	m_hTxtFont;
    BOOL	m_bInSetting;		//����������
};

}//namespace DuiEngine
