#ifndef DuiTime_h__
#define DuiTime_h__

#pragma  once

#include <time.h>

namespace DuiEngine
{
	class DUI_EXP CTimeSpan
	{
	public:
		CTimeSpan() throw();
		CTimeSpan( __time64_t time ) throw();
		CTimeSpan( LONG lDays, int nHours, int nMins, int nSecs ) throw();

		LONGLONG GetDays() const throw();
		LONGLONG GetTotalHours() const throw();
		LONG GetHours() const throw();
		LONGLONG GetTotalMinutes() const throw();
		LONG GetMinutes() const throw();
		LONGLONG GetTotalSeconds() const throw();
		LONG GetSeconds() const throw();

		__time64_t GetTimeSpan() const throw();

		CTimeSpan operator+( CTimeSpan span ) const throw();
		CTimeSpan operator-( CTimeSpan span ) const throw();
		CTimeSpan& operator+=( CTimeSpan span ) throw();
		CTimeSpan& operator-=( CTimeSpan span ) throw();
		bool operator==( CTimeSpan span ) const throw();
		bool operator!=( CTimeSpan span ) const throw();
		bool operator<( CTimeSpan span ) const throw();
		bool operator>( CTimeSpan span ) const throw();
		bool operator<=( CTimeSpan span ) const throw();
		bool operator>=( CTimeSpan span ) const throw();

	private:
		__time64_t m_timeSpan;
	};


	inline CTimeSpan::CTimeSpan() throw() :
	m_timeSpan(0)
	{
	}

	inline CTimeSpan::CTimeSpan( __time64_t time ) throw() :
	m_timeSpan( time )
	{
	}

	inline CTimeSpan::CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs) throw()
	{
		m_timeSpan = nSecs + 60* (nMins + 60* (nHours + __int64(24) * lDays));
	}

	inline LONGLONG CTimeSpan::GetDays() const throw()
	{
		return( m_timeSpan/(24*3600) );
	}

	inline LONGLONG CTimeSpan::GetTotalHours() const throw()
	{
		return( m_timeSpan/3600 );
	}

	inline LONG CTimeSpan::GetHours() const throw()
	{
		return( LONG( GetTotalHours()-(GetDays()*24) ) );
	}

	inline LONGLONG CTimeSpan::GetTotalMinutes() const throw()
	{
		return( m_timeSpan/60 );
	}

	inline LONG CTimeSpan::GetMinutes() const throw()
	{
		return( LONG( GetTotalMinutes()-(GetTotalHours()*60) ) );
	}

	inline LONGLONG CTimeSpan::GetTotalSeconds() const throw()
	{
		return( m_timeSpan );
	}

	inline LONG CTimeSpan::GetSeconds() const throw()
	{
		return( LONG( GetTotalSeconds()-(GetTotalMinutes()*60) ) );
	}

	inline __time64_t CTimeSpan::GetTimeSpan() const throw()
	{
		return( m_timeSpan );
	}

	inline CTimeSpan CTimeSpan::operator+( CTimeSpan span ) const throw()
	{
		return( CTimeSpan( m_timeSpan+span.m_timeSpan ) );
	}

	inline CTimeSpan CTimeSpan::operator-( CTimeSpan span ) const throw()
	{
		return( CTimeSpan( m_timeSpan-span.m_timeSpan ) );
	}

	inline CTimeSpan& CTimeSpan::operator+=( CTimeSpan span ) throw()
	{
		m_timeSpan += span.m_timeSpan;
		return( *this );
	}

	inline CTimeSpan& CTimeSpan::operator-=( CTimeSpan span ) throw()
	{
		m_timeSpan -= span.m_timeSpan;
		return( *this );
	}

	inline bool CTimeSpan::operator==( CTimeSpan span ) const throw()
	{
		return( m_timeSpan == span.m_timeSpan );
	}

	inline bool CTimeSpan::operator!=( CTimeSpan span ) const throw()
	{
		return( m_timeSpan != span.m_timeSpan );
	}

	inline bool CTimeSpan::operator<( CTimeSpan span ) const throw()
	{
		return( m_timeSpan < span.m_timeSpan );
	}

	inline bool CTimeSpan::operator>( CTimeSpan span ) const throw()
	{
		return( m_timeSpan > span.m_timeSpan );
	}

	inline bool CTimeSpan::operator<=( CTimeSpan span ) const throw()
	{
		return( m_timeSpan <= span.m_timeSpan );
	}

	inline bool CTimeSpan::operator>=( CTimeSpan span ) const throw()
	{
		return( m_timeSpan >= span.m_timeSpan );
	}

class DUI_EXP CTime
{
public:
	static CTime GetCurrentTime() throw();
	CTime(__time64_t tm=0):m_time(tm){}
	CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
		int nDST = -1 );

	__time64_t	GetTime() const throw();
	struct tm* GetLocalTm(struct tm* ptm) const;

	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetDayOfWeek() const;

	CTime& operator=( __time64_t time ) throw();

	CTime& operator+=( CTimeSpan span ) throw();
	CTime& operator-=( CTimeSpan span ) throw();

	CTimeSpan operator-( CTime time ) const throw();
	CTime operator-( CTimeSpan span ) const throw();
	CTime operator+( CTimeSpan span ) const throw();

	bool operator==( CTime time ) const throw();
	bool operator!=( CTime time ) const throw();
	bool operator<( CTime time ) const throw();
	bool operator>( CTime time ) const throw();
	bool operator<=( CTime time ) const throw();
	bool operator>=( CTime time ) const throw();
protected:

	__time64_t m_time;
};

inline CTime::CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,
							int nDST)
{
	struct tm atm;

	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	atm.tm_mday = nDay;
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
	atm.tm_year = nYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;

	m_time = _mktime64(&atm);
}

inline struct tm* CTime::GetLocalTm(struct tm* ptm) const
{
	// Ensure ptm is valid
	DUIASSERT( ptm != NULL );

	if (ptm != NULL)
	{
		struct tm ptmTemp;
		errno_t err = _localtime64_s(&ptmTemp, &m_time);

		if (err != 0)
		{
			return NULL;    // indicates that m_time was not initialized!
		}

		*ptm = ptmTemp;
		return ptm;
	}

	return NULL;
}

inline CTime CTime::GetCurrentTime() throw()
{
	return( CTime( ::_time64( NULL ) ) );
}

inline __time64_t CTime::GetTime() const throw()
{
	return( m_time );
}

inline int CTime::GetYear() const
{ 
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? (ptm->tm_year) + 1900 : 0 ; 
}

inline int CTime::GetMonth() const
{ 
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_mon + 1 : 0;
}

inline int CTime::GetDay() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_mday : 0 ; 
}

inline int CTime::GetHour() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_hour : -1 ; 
}

inline int CTime::GetMinute() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_min : -1 ; 
}

inline int CTime::GetSecond() const
{ 
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_sec : -1 ;
}

inline int CTime::GetDayOfWeek() const
{ 
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_wday + 1 : 0 ;
}

inline CTime& CTime::operator=( __time64_t time ) throw()
{
	m_time = time;

	return( *this );
}

inline bool CTime::operator==( CTime time ) const throw()
{
	return( m_time == time.m_time );
}

inline bool CTime::operator!=( CTime time ) const throw()
{
	return( m_time != time.m_time );
}

inline bool CTime::operator<( CTime time ) const throw()
{
	return( m_time < time.m_time );
}

inline bool CTime::operator>( CTime time ) const throw()
{
	return( m_time > time.m_time );
}

inline bool CTime::operator<=( CTime time ) const throw()
{
	return( m_time <= time.m_time );
}

inline bool CTime::operator>=( CTime time ) const throw()
{
	return( m_time >= time.m_time );
}


inline CTime& CTime::operator+=( CTimeSpan span ) throw()
{
	m_time += span.GetTimeSpan();

	return( *this );
}

inline CTime& CTime::operator-=( CTimeSpan span ) throw()
{
	m_time -= span.GetTimeSpan();

	return( *this );
}

inline CTimeSpan CTime::operator-( CTime time ) const throw()
{
	return( CTimeSpan( m_time-time.m_time ) );
}

inline CTime CTime::operator-( CTimeSpan span ) const throw()
{
	return( CTime( m_time-span.GetTimeSpan() ) );
}

inline CTime CTime::operator+( CTimeSpan span ) const throw()
{
	return( CTime( m_time+span.GetTimeSpan() ) );
}


}//end of namespace DuiEngine


#endif // DuiTime_h__
