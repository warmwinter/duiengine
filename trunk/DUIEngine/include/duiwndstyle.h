//////////////////////////////////////////////////////////////////////////
//   File Name: DuiWndStyle.h
// Description: DuiWindow Styles Definition
//     Creator: Zhang Xiaoxuan
//     Version: 2009.04.28 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "duiskin.h"

namespace DuiEngine
{

class DUI_EXP DuiStyle : public CDuiObject
{
    DUIOBJ_DECLARE_CLASS_NAME(DuiStyle, "style")

    enum
    {
        // Specify by "cursor" attribute
        Cursor_Mask     = 0xF00UL,
        Cursor_Arrow    = 0x000UL,   // cursor = "arrow"
        Cursor_Hand     = 0x100UL,   // cursor = "hand"


		Align_Left				= 0x000UL, // valign = top
		Align_Center			= 0x100UL, // valign = middle
		Align_Right				= 0x200UL, // valign = bottom

		VAlign_Top				= 0x0000UL, // valign = top
		VAlign_Middle			= 0x1000UL, // valign = middle
		VAlign_Bottom			= 0x2000UL, // valign = bottom

    };
public:
    DuiStyle();

    CDuiStringA m_strClassName;

    COLORREF m_crBg;
    COLORREF m_crBgHover;
    COLORREF m_crText;
    COLORREF m_crHoverText;
    COLORREF m_crDisabledText;
    COLORREF m_crPushText;
    HFONT m_ftText;
    HFONT m_ftHover;
    HFONT m_ftPush;
    int m_nMarginX;
    int m_nMarginY;
    int m_nSpacing;
    int m_nLineSpacing;
	BOOL m_bDotted;

    LPCTSTR m_lpCursorName;
    CDuiStringA m_strSkinName,m_strNcSkinName;

    // Get class name
    LPCSTR GetName();

	UINT GetTextAlign();
protected:
	UINT m_nTextAlign;
	UINT m_uAlign,m_uVAlign;

    // Load style-pool from xml tree
    static void _LoadStylePool(TiXmlElement *pXmlStyleRootElem);

    DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
    DUIWIN_STRING_ATTRIBUTE("name", m_strClassName, TRUE)
    DUIWIN_STRING_ATTRIBUTE("skin", m_strSkinName, TRUE)
    DUIWIN_STRING_ATTRIBUTE("ncskin", m_strNcSkinName, TRUE)
    DUIWIN_HEX_ATTRIBUTE("textmode", m_nTextAlign, TRUE)

	DUIWIN_ENUM_ATTRIBUTE("align", UINT, TRUE)
	DUIWIN_ENUM_VALUE("left", Align_Left)
	DUIWIN_ENUM_VALUE("center", Align_Center)
	DUIWIN_ENUM_VALUE("right", Align_Right)
	DUIWIN_ENUM_END(m_uAlign)
	DUIWIN_ENUM_ATTRIBUTE("valign", UINT, TRUE)
	DUIWIN_ENUM_VALUE("top", VAlign_Top)
	DUIWIN_ENUM_VALUE("middle", VAlign_Middle)
	DUIWIN_ENUM_VALUE("bottom", VAlign_Bottom)
	DUIWIN_ENUM_END(m_uVAlign)

    DUIWIN_COLOR_ATTRIBUTE("crbg", m_crBg, TRUE)
    DUIWIN_COLOR_ATTRIBUTE("crbghover", m_crBgHover, TRUE)
    DUIWIN_COLOR_ATTRIBUTE("crtext", m_crText, TRUE)
    DUIWIN_COLOR_ATTRIBUTE("crhover", m_crHoverText, TRUE)
    DUIWIN_COLOR_ATTRIBUTE("crpush", m_crPushText, TRUE)
    DUIWIN_COLOR_ATTRIBUTE("crdisabled", m_crDisabledText, TRUE)
    DUIWIN_FONT_ATTRIBUTE("font", m_ftText, TRUE)
    DUIWIN_FONT_ATTRIBUTE("hoverfont", m_ftHover, TRUE)
    DUIWIN_FONT_ATTRIBUTE("pushfont", m_ftPush, TRUE)
    DUIWIN_INT_ATTRIBUTE("x-margin", m_nMarginX, TRUE)
    DUIWIN_INT_ATTRIBUTE("y-margin", m_nMarginY, TRUE)
    DUIWIN_INT_ATTRIBUTE("margin", m_nMarginX = m_nMarginY, TRUE) // �����Ƚ�bt������.....�պ��ð�
    DUIWIN_INT_ATTRIBUTE("spacing", m_nSpacing, TRUE)
    DUIWIN_INT_ATTRIBUTE("linespacing", m_nLineSpacing, TRUE)
    DUIWIN_ENUM_ATTRIBUTE("cursor", LPCTSTR, FALSE)
    DUIWIN_ENUM_VALUE("hand", IDC_HAND)
    DUIWIN_ENUM_VALUE("arrow", IDC_ARROW)
    DUIWIN_ENUM_END(m_lpCursorName)
    DUIWIN_INT_ATTRIBUTE("dotted",m_bDotted,FALSE)
    DUIWIN_DECLARE_ATTRIBUTES_END()
};

#include "DuiSingletonMap.h"

class DUI_EXP DuiStylePool :public DuiSingletonMap<DuiStylePool,DuiStyle,CDuiStringA>
{
public:
    // Get style object from pool by class name
    static BOOL GetStyle(LPCSTR lpszName,DuiStyle& style);

    static BOOL Init(UINT uResID);

    static BOOL Init(LPCSTR lpszXml);
protected:
    void _LoadStylePool(TiXmlElement *pXmlStyleRootElem);
};

}//namespace DuiEngine