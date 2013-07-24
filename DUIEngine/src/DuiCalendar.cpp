// DuiCalendar.cpp : implementation file
//
#include "duistd.h"
#include "DuiCalendar.h"
#include "DuiTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendar

namespace DuiEngine{

extern WORD gLunarMonthDay[];
extern BYTE gLunarMonth[];
extern BYTE gLunarHolDay[];

#define TITLE_HEIGHT 20
#define FOOTER_HEIGHT 20

//�����ӿؼ�������
#define NAME_BTN_TODAY	"btn_today"
#define NAME_LABEL_TODAY "label_today"


WORD CCalendarCore::WeekDay(WORD iYear, WORD iMonth, WORD iDay)
{
	//����Ԫ��monthday[i]��ʾ��i������ǰ������������7������
	WORD monthday[]={0,3,3,6,1,4,6,2,5,0,3,5};
	WORD iDays = (iYear-1)%7 + (iYear-1)/4 - (iYear-1)/100 +(iYear-1)/400;
	iDays += (monthday[iMonth-1] +iDay) ;
	//���iYear������
	if(IsLeapYear(iYear) && iMonth>2)
		iDays++;
	//����:0,1,2,3,4,5,6���ա�һ�����������ġ��塢��
	return iDays%7;
}

WORD CCalendarCore::DayWeek( WORD iYear, WORD iMonth, WORD iDay )
{
	int iDayFirst=WeekDay(iYear,iMonth,1);
	int days1week=7-iDayFirst;
	if(iDay<=days1week) return 0;
	iDay-=days1week;
	return (iDay+7-1)/7;
}

WORD CCalendarCore::MonthWeeks( WORD iYear, WORD iMonth )
{
	int iDayFirst=WeekDay(iYear,iMonth,1);
	int days=MonthDays(iYear,iMonth);
	int days1week=7-iDayFirst;
	int nRet=1;
	days-=days1week;
	nRet+=days/7;
	if((days%7)>0) nRet++;
	return nRet;	
}

WORD CCalendarCore::MonthDays(WORD iYear, WORD iMonth)
{
	switch(iMonth)
	{
	case 1:case 3:case 5:case 7:case 8:case 10:case 12:
		return 31;
		break;
	case 4:case 6:case 9:case 11:
		return 30;
		break;
	case 2:
		//���������
		if(IsLeapYear(iYear))
			return 29;
		else
			return 28;
		break;
	}
	return 0;
}

WORD CCalendarCore::GetLeapMonth(WORD iLunarYear)
{
	BYTE &flag = gLunarMonth[(iLunarYear - START_YEAR)/2];
	return  (iLunarYear - START_YEAR)%2 ? flag&0x0f : flag>>4;
}


LONG CCalendarCore::CalcDateDiff(WORD iEndYear, WORD iEndMonth, WORD iEndDay,
								 WORD  iStartYear, WORD iStartMonth, WORD iStartDay)
{
	WORD monthday[]={0, 31, 59 ,90, 120, 151, 181, 212, 243, 273, 304, 334}; 

	//�����������1��1��֮����������
	LONG iDiffDays =(iEndYear - iStartYear)*365;
	iDiffDays += (iEndYear-1)/4 - (iStartYear-1)/4;
	iDiffDays -= ((iEndYear-1)/100 - (iStartYear-1)/100);
	iDiffDays += (iEndYear-1)/400 - (iStartYear-1)/400;

	//����iEndYear��1��1�յ�iEndMonth��iEndDay��֮�������
	iDiffDays += monthday[iEndMonth-1] +
		(IsLeapYear(iEndYear)&&iEndMonth>2? 1: 0);
	iDiffDays += iEndDay;

	//��ȥiStartYear��1��1�յ�iStartMonth��iStartDay��֮�������
	iDiffDays -= (monthday[iStartMonth-1] + 
		(IsLeapYear(iStartYear)&&iStartMonth>2 ? 1: 0));
	iDiffDays -= iStartDay;	
	return iDiffDays;
}

void  CCalendarCore::l_CalcLunarDate(WORD &iYear, WORD &iMonth ,WORD &iDay, LONG iSpanDays)
{
	//����1901��2��19��Ϊ����1901�����³�һ
	//����1901��1��1�յ�2��19�չ���49��
	if(iSpanDays <49)
	{
		iYear  = START_YEAR-1;
		if(iSpanDays <19)
		{ 
			iMonth = 11;  
			iDay   = 11+WORD(iSpanDays);
		}
		else
		{
			iMonth = 12;
			iDay   =  WORD(iSpanDays) -18;
		}
		return ;
	}
	//���������1901�����³�һ����
	iSpanDays -=49;
	iYear  = START_YEAR;
	iMonth = 1;
	iDay   = 1;
	//������
	LONG tmp = CCalendarCore::LunarYearDays(iYear); 
	while(iSpanDays >= tmp)
	{
		iSpanDays -= tmp;
		tmp = CCalendarCore::LunarYearDays(++iYear);
	}
	//������
	tmp = LOWORD(CCalendarCore::LunarMonthDays(iYear, iMonth));
	while(iSpanDays >= tmp)
	{
		iSpanDays -= tmp;
		if(iMonth == CCalendarCore::GetLeapMonth(iYear))
		{
			tmp  = HIWORD(CCalendarCore::LunarMonthDays(iYear, iMonth));
			if(iSpanDays < tmp)	
				break;
			iSpanDays -= tmp;
		}
		tmp = LOWORD(CCalendarCore::LunarMonthDays(iYear, ++iMonth));
	}
	//������
	iDay += WORD(iSpanDays);
}

WORD CCalendarCore::GetLunarDate(WORD iYear, WORD iMonth, WORD iDay,
								 WORD &iLunarYear, WORD &iLunarMonth, WORD &iLunarDay)
{
	l_CalcLunarDate(iLunarYear, iLunarMonth, iLunarDay, 
		CalcDateDiff(iYear, iMonth, iDay));

	return l_GetLunarHolDay(iYear, iMonth, iDay);
}

//���ݽ������ݴ洢��ʽ,��������iYear��iMonth��iDay�ն�Ӧ�Ľ���,
WORD  CCalendarCore::l_GetLunarHolDay(WORD iYear, WORD iMonth, WORD iDay)
{
	BYTE &flag = gLunarHolDay[(iYear - START_YEAR)*12+iMonth -1];
	WORD day;
	if(iDay <15)
		day= 15 - ((flag>>4)&0x0f);
	else
		day = ((flag)&0x0f)+15;
	if(iDay == day)
		return (iMonth-1) *2 + (iDay>15? 1: 0) +1; 
	else
		return 0;
}

LONG CCalendarCore::LunarMonthDays(WORD iLunarYear, WORD iLunarMonth)
{
	if(iLunarYear < START_YEAR) 
		return 30L;

	WORD height =0 ,low =29;
	int iBit = 16 - iLunarMonth;

	if(iLunarMonth > GetLeapMonth(iLunarYear) && GetLeapMonth(iLunarYear))
		iBit --;

	if(gLunarMonthDay[iLunarYear - START_YEAR] & (1<<iBit))
		low ++;

	if(iLunarMonth == GetLeapMonth(iLunarYear))
		if(gLunarMonthDay[iLunarYear - START_YEAR] & (1<< (iBit -1)))
			height =30;
		else 
			height =29;

	return MAKELONG(low, height);
}

WORD CCalendarCore::LunarYearDays(WORD iLunarYear)
{
	WORD days =0;
	for(WORD  i=1; i<=12; i++)
	{ 
		LONG  tmp = LunarMonthDays(iLunarYear ,i); 
		days += HIWORD(tmp);
		days += LOWORD(tmp);
	}
	return days;
}

void CCalendarCore::FormatLunarYear(WORD  iYear, TCHAR *pBuffer)
{	
	TCHAR szText1[][3]={_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};
	TCHAR szText2[][3]={_T("��"),_T("��"),_T("��"),_T("î"),_T("��"),_T("��"),_T("��"),_T("δ"),_T("��"),_T("��"),_T("��"),_T("��")};
	TCHAR szText3[][3]={_T("��"),_T("ţ"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};

	_stprintf(pBuffer,_T("%s%s %s"),szText1[(iYear-4)%10],szText2[(iYear-4)%12],(iYear-4)%12);
}

void CCalendarCore::FormatMonth(WORD iMonth, TCHAR *pBuffer, BOOL bLunar)
{
	if(!bLunar && iMonth==1)
	{
		_tcscpy(pBuffer, _T("��һ��"));
		return;
	}

	TCHAR szText[][3]={_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("ʮ")};
	if(iMonth<=10)
		_stprintf(pBuffer,_T(" %s��"),szText[iMonth -1]);
	else if (iMonth == 11)
		_tcscpy(pBuffer, _T("ʮһ��"));
	else
		_tcscpy(pBuffer, _T("ʮ����"));  
}

void CCalendarCore::FormatLunarDay(WORD  iDay, TCHAR *pBuffer)
{
	TCHAR szText1[][3]={_T("��"),_T("ʮ"),_T("إ"),_T("��")};
	TCHAR szText2[][3]={_T("һ"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��"),_T("ʮ")};
	if(iDay != 20 && iDay !=30)
	{
		_stprintf(pBuffer,_T("%s%s"),szText1[(iDay-1)/10],szText2[(iDay-1)%10]);
	}
	else
	{
		_stprintf(pBuffer,_T("%sʮ"),szText1[iDay/10]);
	}
}

//////////////////////////////////////////////////////////////////////////
//	CCalendar
//////////////////////////////////////////////////////////////////////////

CDuiCalendar::CDuiCalendar(WORD iYear, WORD iMonth, WORD iDay)
{
	Init();
   if(!SetDate(iYear, iMonth, iDay))
   {
	   CTime tm=CTime::GetCurrentTime();
	   SetDate(tm.GetYear(),tm.GetMonth(),tm.GetDay());
   }
}

CDuiCalendar::CDuiCalendar()
{
	Init();
	CTime tm=CTime::GetCurrentTime();
	SetDate(tm.GetYear(),tm.GetMonth(),tm.GetDay());
}

void CDuiCalendar::Init()
{
	addEvent(DUINM_CALENDAR_SELECTDAY);
	m_nTitleHei=TITLE_HEIGHT;
	m_nFooterHei=FOOTER_HEIGHT;
	m_crWeekend=RGB(255,0,0);
	m_crDay=RGB(255,0,0);
	m_crTitleBack=RGB(0,255,0);
	m_crDayBack=RGB(0,0,255);
	m_pTitleSkin=m_pDaySkin=NULL;

	m_iHoverDay=0;

	TCHAR sztext[][3]={_T("��"),_T("һ"),_T("��"),_T("��"),_T("��"),_T("��"),_T("��")};
	for(int i=0;i<7;i++) m_strTitle[i]=sztext[i];
}


/////////////////////////////////////////////////////////////////////////////
// CCalendar message handlers
void CDuiCalendar::DrawTitle(CDCHandle &dc)
{
   CRect rect ;
   GetClient(&rect);

   rect.bottom = rect.top + m_nTitleHei;

   if(m_pTitleSkin)
	   m_pTitleSkin->Draw(dc,rect,0);
   else
	   CGdiAlpha::FillRect(dc,&rect, CBrush(m_crTitleBack));

   int nWid=rect.Width()/7;
   int x=rect.left;
   CRect rcItem=rect;
   rcItem.right=rcItem.left+nWid;

   COLORREF crTxt=dc.GetTextColor();
   for(int i=0; i <7; i++)
   {
	   if(i==0 || i==6 )
		   dc.SetTextColor(m_crWeekend);
	   else
		   dc.SetTextColor(crTxt);

	   CGdiAlpha::DrawText(dc,m_strTitle[i],m_strTitle[i].GetLength(),rcItem,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	   rcItem.OffsetRect(nWid,0);
   }
   dc.SetTextColor(crTxt);
}

void CDuiCalendar::DrawDate(CDCHandle &dc)
{

	int days=CCalendarCore::MonthDays(m_iYear, m_iMonth);

	for(int i=1;i<=days;i++)
	{
		CRect rcDay=GetDayRect(i);
		DrawDay(dc,rcDay,i);
	}
}


void CDuiCalendar::DrawDay( CDCHandle &dc,CRect & rcDay,WORD iDay )
{
	TCHAR text[3];
	_stprintf(text, _T("%2d"), iDay);
	COLORREF crTxt=dc.GetTextColor();
	if(iDay==m_iDay)
	{
		if(m_pDaySkin) m_pDaySkin->Draw(dc,rcDay,2);
		else CGdiAlpha::FillSolidRect(dc,rcDay,m_crDayBack);
		dc.SetTextColor(m_crDay);
	}else
	{
		if(m_pDaySkin) m_pDaySkin->Draw(dc,rcDay,iDay==m_iHoverDay?1:0);
		int iweekday=CCalendarCore::WeekDay(m_iYear,m_iMonth,iDay);
		if(iweekday==0 || iweekday==6)
			dc.SetTextColor(m_crWeekend);
	}
	CGdiAlpha::DrawText(dc,text,-1,rcDay,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	dc.SetTextColor(crTxt);
}

void CDuiCalendar::RedrawDay(WORD iDay )
{
	CRect rcDay=GetDayRect(iDay);
	CDCHandle dc=GetDuiDC(&rcDay,OLEDC_PAINTBKGND);
	DuiDCPaint duiDC;
	BeforePaint(dc,duiDC);
	DrawDay(dc,rcDay,iDay);
	AfterPaint(dc,duiDC);
	ReleaseDuiDC(dc);
}

void CDuiCalendar::OnPaint(CDCHandle dc) 
{
	DuiDCPaint duidc;
	BeforePaint(dc,duidc);
	DrawTitle(dc);
	DrawDate(dc);
	AfterPaint(dc,duidc);
}

void CDuiCalendar::GetDate(WORD &iYear, WORD &iMonth, WORD &iDay) 
{
	iYear  = m_iYear;
	iMonth = m_iMonth;
	iDay   = m_iDay;
}

BOOL CDuiCalendar::SetDate(WORD iYear, WORD iMonth, WORD iDay)
{
  if(iYear < START_YEAR || iYear > END_YEAR || iMonth <1 || iMonth >12)
    	return FALSE;

  if(iDay <1 || iDay > CCalendarCore::MonthDays(iYear, iMonth))
	    return FALSE;

  m_iYear   = iYear;
  m_iMonth  = iMonth;
  m_iDay    = iDay;

  m_iHoverDay	= 0;
  return TRUE;
} 


CRect CDuiCalendar::GetDayRect( WORD iDay )
{
	CRect rcClient;
	GetClient(&rcClient);
	rcClient.top+=m_nTitleHei;
	rcClient.bottom-=m_nFooterHei;
	
	int weeks = CCalendarCore::MonthWeeks(m_iYear,m_iMonth);//�����iMonth�м���
	int col  = CCalendarCore::WeekDay(m_iYear, m_iMonth,iDay);//�����iday�����ڼ�
	int row	 = CCalendarCore::DayWeek(m_iYear, m_iMonth,iDay);//�����iday�ǵڼ���
	
	int nWid=rcClient.Width()/7;
	int nHei=rcClient.Height()/weeks;

	CRect rc(0,0,nWid,nHei);
	rc.OffsetRect(nWid*col,nHei*row);
	rc.OffsetRect(rcClient.TopLeft());
	return rc;
}

WORD CDuiCalendar::HitTest(CPoint pt)
{
	CRect rcClient;
	GetClient(&rcClient);
	rcClient.top+=m_nTitleHei;
	rcClient.bottom-=m_nFooterHei;

	int weeks = CCalendarCore::MonthWeeks(m_iYear,m_iMonth);//�����iMonth�м���

	int nWid=rcClient.Width()/7;
	int nHei=rcClient.Height()/weeks;

	pt-=rcClient.TopLeft();
	int iCol  = pt.x/nWid;
	int iRow  = pt.y/nHei;


	WORD startcol ,endrow, endcol;
	startcol = CCalendarCore::WeekDay(m_iYear, m_iMonth, 1);
	endcol   = CCalendarCore::WeekDay(m_iYear, m_iMonth, CCalendarCore::MonthDays(m_iYear,m_iMonth));

	endrow   = (CCalendarCore::MonthDays(m_iYear, m_iMonth) + startcol -1)/7 ;
    if(iRow == 0 && iCol < startcol || iRow == endrow && iCol > endcol ||  iRow > endrow)
		return 0;
	return iRow *7 + iCol + 1 - startcol ;
}

void CDuiCalendar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	__super::OnLButtonDown(nFlags,point);
	WORD day = HitTest(point);
	if(day !=0 && day != m_iDay)
	{
		DUINMCALENDARSELECTDAY nm;
		nm.hdr.code=DUINM_HDSIZECHANGING;
		nm.hdr.idFrom=GetCmdID();
		nm.hdr.hwndFrom=0;
		nm.pSender=this;
		nm.wOldDay=m_iDay;
		nm.wNewDay=day;

		m_iDay = day;
		NotifyInvalidate();
		DuiNotify((LPNMHDR)&nm);
    }
}

void CDuiCalendar::OnMouseMove( UINT nFlags,CPoint pt )
{
	int iDay=HitTest(pt);
	if(iDay!=m_iHoverDay)
	{
		WORD oldHover=m_iHoverDay;
		m_iHoverDay=iDay;
		if(m_pDaySkin)
		{
			if(oldHover!=0) RedrawDay(oldHover);
			if(m_iHoverDay!=0) RedrawDay(m_iHoverDay);
		}
	}
}

void CDuiCalendar::OnMouseLeave()
{
	if(m_iHoverDay!=0)
	{
		WORD oldHover=m_iHoverDay;
		m_iHoverDay=0;
		if(m_pDaySkin) RedrawDay(oldHover);
	}
}

BOOL CDuiCalendar::Load( pugi::xml_node xmlNode )
{
	BOOL bLoad=__super::Load(xmlNode);
	if(bLoad)
	{
		CDuiWindow *pBtnToday=FindChildByName("btn_today");
		if(pBtnToday)
		{
			pBtnToday->SetCmdID(100);
			pBtnToday->subscribeEvent(DUINM_COMMAND,Subscriber(&CDuiCalendar::OnTodayClick,this));
		}
		CDuiWindow *pLabelToday=FindChildByName("label_today");
		if(pLabelToday)
		{
			CTime today=CTime::GetCurrentTime();
			CDuiStringT str;
			str.Format(_T("%04d-%02d-%02d"),today.GetYear(),today.GetMonth(),today.GetDay());
			pLabelToday->SetInnerText(str);
		}
	}
	return bLoad;
}

bool CDuiCalendar::OnTodayClick( CDuiWindow * pSender, LPNMHDR pNmhdr )
{
	CTime today=CTime::GetCurrentTime();
	SetDate(today.GetYear(),today.GetMonth(),today.GetDay());
	NotifyInvalidate();
	return true;
}

/******************************************************************************
  ����Ϊ�����������������,Ϊ��ʡ�洢�ռ�,���Բ�������Ƚϱ�̬�Ĵ洢����.
   
*******************************************************************************/
//����gLunarDay��������1901�굽2100��ÿ���е���������Ϣ��
//����ÿ��ֻ����29��30�죬һ����12����13����������λ��ʾ����ӦλΪ1��30�죬����Ϊ29��
WORD gLunarMonthDay[]=
{
	//��������ֻ��1901.1.1 --2050.12.31
  0X4ae0, 0Xa570, 0X5268, 0Xd260, 0Xd950, 0X6aa8, 0X56a0, 0X9ad0, 0X4ae8, 0X4ae0,   //1910
  0Xa4d8, 0Xa4d0, 0Xd250, 0Xd548, 0Xb550, 0X56a0, 0X96d0, 0X95b0, 0X49b8, 0X49b0,   //1920
  0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada8, 0X2b60, 0X9570, 0X4978, 0X4970, 0X64b0,   //1930
  0Xd4a0, 0Xea50, 0X6d48, 0X5ad0, 0X2b60, 0X9370, 0X92e0, 0Xc968, 0Xc950, 0Xd4a0,   //1940
  0Xda50, 0Xb550, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950, 0Xb4a8, 0X6ca0,   //1950
  0Xb550, 0X55a8, 0X4da0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa950, 0Xe950, 0X6aa0, 0Xad50,   //1960
  0Xab50, 0X4b60, 0Xa570, 0Xa570, 0X5260, 0Xe930, 0Xd950, 0X5aa8, 0X56a0, 0X96d0,   //1970
  0X4ae8, 0X4ad0, 0Xa4d0, 0Xd268, 0Xd250, 0Xd528, 0Xb540, 0Xb6a0, 0X96d0, 0X95b0,   //1980
  0X49b0, 0Xa4b8, 0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada0, 0Xab60, 0X9370, 0X4978,   //1990
  0X4970, 0X64b0, 0X6a50, 0Xea50, 0X6b28, 0X5ac0, 0Xab60, 0X9368, 0X92e0, 0Xc960,   //2000
  0Xd4a8, 0Xd4a0, 0Xda50, 0X5aa8, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950,   //2010
  0Xb4a0, 0Xb550, 0Xb550, 0X55a8, 0X4ba0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa930, 0X74a8,   //2020
  0X6aa0, 0Xad50, 0X4da8, 0X4b60, 0X9570, 0Xa4e0, 0Xd260, 0Xe930, 0Xd530, 0X5aa0,   //2030
  0X6b50, 0X96d0, 0X4ae8, 0X4ad0, 0Xa4d0, 0Xd258, 0Xd250, 0Xd520, 0Xdaa0, 0Xb5a0,   //2040
  0X56d0, 0X4ad8, 0X49b0, 0Xa4b8, 0Xa4b0, 0Xaa50, 0Xb528, 0X6d20, 0Xada0, 0X55b0,   //2050
  
};

//����gLanarMonth�������1901�굽2050�����µ��·ݣ���û����Ϊ0��ÿ�ֽڴ�����
BYTE  gLunarMonth[]=
{
	0X00, 0X50, 0X04, 0X00, 0X20,   //1910
	0X60, 0X05, 0X00, 0X20, 0X70,   //1920
	0X05, 0X00, 0X40, 0X02, 0X06,   //1930
	0X00, 0X50, 0X03, 0X07, 0X00,   //1940
	0X60, 0X04, 0X00, 0X20, 0X70,   //1950
	0X05, 0X00, 0X30, 0X80, 0X06,   //1960
	0X00, 0X40, 0X03, 0X07, 0X00,   //1970
	0X50, 0X04, 0X08, 0X00, 0X60,   //1980
	0X04, 0X0a, 0X00, 0X60, 0X05,   //1990
	0X00, 0X30, 0X80, 0X05, 0X00,   //2000
	0X40, 0X02, 0X07, 0X00, 0X50,   //2010
	0X04, 0X09, 0X00, 0X60, 0X04,   //2020
	0X00, 0X20, 0X60, 0X05, 0X00,   //2030
	0X30, 0Xb0, 0X06, 0X00, 0X50,   //2040
	0X02, 0X07, 0X00, 0X50, 0X03    //2050
};

//����gLanarHoliDay���ÿ��Ķ�ʮ�Ľ�����Ӧ����������
//ÿ��Ķ�ʮ�Ľ�����Ӧ���������ڼ����̶���ƽ���ֲ���ʮ��������
//   1��          2��         3��         4��         5��         6��   
//С�� ��   ����  ��ˮ   ���� ����   ���� ����   ���� С��   â�� ����
//   7��          8��         9��         10��       11��        12��  
//С�� ����   ����  ����   ��¶ ���   ��¶ ˪��   ���� Сѩ   ��ѩ ����

/*********************************************************************************
 �������κ�ȷ������,����ֻ�ô��,Ҫ��ʡ�ռ�,����....
  �������ִ淨ʵ����̫��̬��,��ͽ����ſ���
**********************************************************************************/
//���ݸ�ʽ˵��:
//��1901��Ľ���Ϊ
//  1��     2��     3��   4��    5��   6��   7��    8��   9��    10��  11��     12��
// 6, 21, 4, 19,  6, 21, 5, 21, 6,22, 6,22, 8, 23, 8, 24, 8, 24, 8, 24, 8, 23, 8, 22
// 9, 6,  11,4,   9, 6,  10,6,  9,7,  9,7,  7, 8,  7, 9,  7,  9, 7,  9, 7,  8, 7, 15
//�����һ������Ϊÿ�½�����Ӧ����,15��ȥÿ�µ�һ������,ÿ�µڶ���������ȥ15�õڶ���
// ����ÿ������������Ӧ���ݶ�С��16,ÿ����һ���ֽڴ��,��λ��ŵ�һ����������,��λ���
//�ڶ�������������,�ɵ��±�

BYTE gLunarHolDay[]=
{
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1901
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1902
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1903
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //1904
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1905
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1906
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1907
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1908
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1909
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1910
	0X96, 0XA5, 0X87, 0X96, 0X87, 0X87, 0X79, 0X69, 0X69, 0X69, 0X78, 0X78,   //1911
	0X86, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1912
	0X95, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1913
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1914
	0X96, 0XA5, 0X97, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1915
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1916
	0X95, 0XB4, 0X96, 0XA6, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1917
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X77,   //1918
	0X96, 0XA5, 0X97, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1919
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1920
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1921
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X77,   //1922
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X69, 0X69, 0X78, 0X78,   //1923
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1924
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X87,   //1925
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1926
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1927
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1928
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1929
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1930
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X87, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1931
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1932
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1933
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1934
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1935
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1936
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1937
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1938
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1939
	0X96, 0XA5, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1940
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1941
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1942
	0X96, 0XA4, 0X96, 0X96, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1943
	0X96, 0XA5, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1944
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1945
	0X95, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1946
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1947
	0X96, 0XA5, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1948
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X79, 0X78, 0X79, 0X77, 0X87,   //1949
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1950
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X79, 0X79, 0X79, 0X69, 0X78, 0X78,   //1951
	0X96, 0XA5, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1952
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1953
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X68, 0X78, 0X87,   //1954
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1955
	0X96, 0XA5, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1956
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1957
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1958
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1959
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1960
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1961
	0X96, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1962
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1963
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1964
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1965
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1966
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1967
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1968
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1969
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1970
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X79, 0X69, 0X78, 0X77,   //1971
	0X96, 0XA4, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1972
	0XA5, 0XB5, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1973
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1974
	0X96, 0XB4, 0X96, 0XA6, 0X97, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1975
	0X96, 0XA4, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X89, 0X88, 0X78, 0X87, 0X87,   //1976
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1977
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //1978
	0X96, 0XB4, 0X96, 0XA6, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1979
	0X96, 0XA4, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1980
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X77, 0X87,   //1981
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1982
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X78, 0X79, 0X78, 0X69, 0X78, 0X77,   //1983
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //1984
	0XA5, 0XB4, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //1985
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1986
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X79, 0X78, 0X69, 0X78, 0X87,   //1987
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1988
	0XA5, 0XB4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1989
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //1990
	0X95, 0XB4, 0X96, 0XA5, 0X86, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1991
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1992
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1993
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1994
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X76, 0X78, 0X69, 0X78, 0X87,   //1995
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //1996
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //1997
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //1998
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //1999
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2000
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2001
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2002
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //2003
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2004
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2005
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2006
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X69, 0X78, 0X87,   //2007
	0X96, 0XB4, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2008
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2009
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2010
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X78, 0X87,   //2011
	0X96, 0XB4, 0XA5, 0XB5, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2012
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2013
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2014
	0X95, 0XB4, 0X96, 0XA5, 0X96, 0X97, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2015
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2016
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2017
	0XA5, 0XB4, 0XA6, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2018
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2019
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X86,   //2020
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2021
	0XA5, 0XB4, 0XA5, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2022
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X79, 0X77, 0X87,   //2023
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2024
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2025
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2026
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2027
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2028
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2029
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2030
	0XA5, 0XB4, 0X96, 0XA5, 0X96, 0X96, 0X88, 0X78, 0X78, 0X78, 0X87, 0X87,   //2031
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2032
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X86,   //2033
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X78, 0X88, 0X78, 0X87, 0X87,   //2034
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2035
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2036
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X86,   //2037
	0XA5, 0XB3, 0XA5, 0XA5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2038
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2039
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X96,   //2040
	0XA5, 0XC3, 0XA5, 0XB5, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2041
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X88, 0X88, 0X88, 0X78, 0X87, 0X87,   //2042
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2043
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X88, 0X87, 0X96,   //2044
	0XA5, 0XC3, 0XA5, 0XB4, 0XA5, 0XA6, 0X87, 0X88, 0X87, 0X78, 0X87, 0X86,   //2045
	0XA5, 0XB3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X88, 0X78, 0X87, 0X87,   //2046
	0XA5, 0XB4, 0X96, 0XA5, 0XA6, 0X96, 0X88, 0X88, 0X78, 0X78, 0X87, 0X87,   //2047
	0X95, 0XB4, 0XA5, 0XB4, 0XA5, 0XA5, 0X97, 0X87, 0X87, 0X88, 0X86, 0X96,   //2048
	0XA4, 0XC3, 0XA5, 0XA5, 0XA5, 0XA6, 0X97, 0X87, 0X87, 0X78, 0X87, 0X86,   //2049
	0XA5, 0XC3, 0XA5, 0XB5, 0XA6, 0XA6, 0X87, 0X88, 0X78, 0X78, 0X87, 0X87    //2050

};


}//end of namespace