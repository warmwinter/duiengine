#pragma once
// DuiCalendar.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "duiwnd.h"

namespace DuiEngine
{
#define START_YEAR	1901
#define END_YEAR	2050

	class CCalendarCore{
	public:
		//�ж�iYear�ǲ�������
		static BOOL IsLeapYear(WORD iYear)
		{return !(iYear%4)&&(iYear%100) || !(iYear%400);}

		//����iYear,iMonth,iDay��Ӧ�����ڼ� 1��1��1�� --- 65535��12��31��
		static WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay);

		//�����ָ���·ݵ�����
		static WORD MonthWeeks(WORD iYear, WORD iMonth);

		//����ָ�����Ǹ��µĵڼ���
		static WORD DayWeek(WORD iYear, WORD iMonth, WORD iDay);

		//����iYear��iMonth�µ����� 1��1�� --- 65535��12��
		static WORD MonthDays(WORD iYear, WORD iMonth);

		//��������iLunarYer������iLunarMonth�µ����������iLunarMonthΪ���£�
		//����Ϊ�ڶ���iLunarMonth�µ��������������Ϊ0 
		// 1901��1��---2050��12��
		static LONG LunarMonthDays(WORD iLunarYear, WORD iLunarMonth);

		//��������iLunarYear���������
		// 1901��1��---2050��12��
		static WORD LunarYearDays(WORD iLunarYear);

		//��������iLunarYear��������·ݣ���û�з���0
		// 1901��1��---2050��12��
		static WORD GetLeapMonth(WORD iLunarYear);

		//��iYear���ʽ������ɼ��귨��ʾ���ַ���
		static void FormatLunarYear(WORD  iYear, TCHAR *pBuffer);

		//��iMonth��ʽ���������ַ���
		static void FormatMonth(WORD iMonth, TCHAR *pBuffer, BOOL bLunar = TRUE);

		//��iDay��ʽ���������ַ���
		static void FormatLunarDay(WORD  iDay, TCHAR *pBuffer);

		//���㹫���������ڼ���������  1��1��1�� --- 65535��12��31��
		static LONG CalcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay,
			WORD iStartYear = START_YEAR, 
			WORD iStartMonth =1, WORD iStartDay =1);

		//���㹫��iYear��iMonth��iDay�ն�Ӧ����������,���ض�Ӧ���������� 0-24
		//1901��1��1��---2050��12��31��
		static WORD GetLunarDate(WORD iYear, WORD iMonth, WORD iDay,
			WORD &iLunarYear, WORD &iLunarMonth, WORD &iLunarDay);

	protected:
		//�����1901��1��1�չ�iSpanDays������������
		static void   l_CalcLunarDate(WORD &iYear, WORD &iMonth ,WORD &iDay, LONG iSpanDays);
		//���㹫��iYear��iMonth��iDay�ն�Ӧ�Ľ��� 0-24��0���ǽ���
		static WORD   l_GetLunarHolDay(WORD iYear, WORD iMonth, WORD iDay);
	};

	class CDuiCalendar : public CDuiWindow
	{
	public:
		DUIOBJ_DECLARE_CLASS_NAME(CDuiCalendar, "calendar")
		CDuiCalendar(WORD iYear, WORD iMonth, WORD iDay);
		CDuiCalendar();

	public:
		WORD GetYear(){return m_iYear;}
		WORD GetMonth(){return m_iMonth;}
		WORD GetDay(){return m_iDay;}
		void GetDate(WORD &iYear, WORD &iMonth, WORD &iDay);
		BOOL SetDate(WORD iYear, WORD iMonth, WORD iDay);

	protected:
		void Init();

		void OnPaint(CDCHandle dc);
		void OnLButtonDown(UINT nFlags, CPoint point);

		void OnMouseMove(UINT nFlags,CPoint pt);
		void OnMouseLeave();
		virtual BOOL Load(pugi::xml_node xmlNode);
	protected:
		CRect GetDayRect(WORD iDay);
		WORD HitTest(CPoint  pt);
		void DrawTitle(CDCHandle &dc);
		void DrawDate(CDCHandle &dc);
		void DrawDay(CDCHandle &dc,CRect & rcDay,WORD iDay );
		void RedrawDay(WORD iDay);

		bool OnTodayClick(CDuiWindow * pSender, LPNMHDR pNmhdr);

		DUIWIN_DECLARE_ATTRIBUTES_BEGIN()
			DUIWIN_INT_ATTRIBUTE("titleHeight", m_nTitleHei, FALSE)
			DUIWIN_INT_ATTRIBUTE("footerHeight", m_nFooterHei, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("crWeekend", m_crWeekend, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("crTitleBack", m_crTitleBack, FALSE)
			DUIWIN_COLOR_ATTRIBUTE("crDay", m_crDay, FALSE)
			DUIWIN_SKIN_ATTRIBUTE("daySkin", m_pDaySkin, FALSE)
			DUIWIN_SKIN_ATTRIBUTE("titleSkin", m_pTitleSkin, FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-1", m_strTitle[0], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-2", m_strTitle[1], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-3", m_strTitle[2], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-4", m_strTitle[3], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-5", m_strTitle[4], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-6", m_strTitle[5], FALSE)
			DUIWIN_TSTRING_ATTRIBUTE("title-7", m_strTitle[6], FALSE)
		DUIWIN_DECLARE_ATTRIBUTES_END()

		DUIWIN_BEGIN_MSG_MAP()
			MSG_WM_PAINT(OnPaint)
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSELEAVE(OnMouseLeave)
		DUIWIN_END_MSG_MAP()
	protected:
		int				m_nTitleHei;	//��ͷ�߶�
		int				m_nFooterHei;	//��β�߶�
		CDuiSkinBase	*m_pDaySkin;	//������Ƥ��
		CDuiSkinBase	*m_pTitleSkin;	//��ͷƤ��

		COLORREF		m_crWeekend;	//��ĩ������ɫ
		COLORREF		m_crTitleBack;	//��ͷ����ɫ
		COLORREF		m_crDay;		//ѡ��������ɫ
		COLORREF		m_crDayBack;	//ѡ�����ڱ�����ɫ
		CDuiStringT		m_strTitle[7];	//��ͷ�ı�

		WORD    m_iYear, m_iMonth, m_iDay;
		int		m_iHoverDay;

	};


}//end of namespace

