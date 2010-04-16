//////////////////////////////////////////////////////////////////////
//
// HighTime.inl: inline implementation of the CHighTime and the CHighTimeSpan classes.
//
//////////////////////////////////////////////////////////////////////
//
// CHighTime and CHighTimeSpan are two classes for high resolution time
// The range is +/-29000 years and smallest step is 0.1 microsecond
// They works like COleDateTime and COleDateTimeSpan.
// I wrote these because MS could not write working time classes
//
// Author: Hakan Trygg, h.kan@home.se
// Classes are not to be sold for profit.
// Please send me a mail if you find it useful and using it

// Revision history: 
// 1.0 : 12 Jan 2000 : /HT : Created
// 1.1 : 19 Jan 2000 : /HT : Made it possible to use classes without MFC
//////////////////////////////////////////////////////////////////////

#if !defined(_HIGHTIME_INL_)
#define _HIGHTIME_INL_

inline CHighTime::CHighTime()
{
    m_status = valid;
    m_liTime.QuadPart = 0;    // 1 Jan-1601, 00:00, 100ns clicks 
} // CHighTime::CHighTime()

inline CHighTime::CHighTime(const CHighTime &dateSrc)
{
    *this = dateSrc;
} // CHighTime::CHighTime(CHighTime)

inline CHighTime::CHighTime(const LARGE_INTEGER dateSrc)
{
    *this = dateSrc;
} // CHighTime::CHighTime(LARGE_INTEGER)

inline CHighTime::CHighTime(const LONGLONG dateSrc)
{
//    *this = dateSrc;
} // CHighTime::CHighTime(LONGLONG)

#if defined(USE_MFC)
inline CHighTime::CHighTime(const COleDateTime &dateSrc)
{
    *this = dateSrc;
} // CHighTime::CHighTime(COleDateTime)
#endif

inline CHighTime::CHighTime(const SYSTEMTIME &systimeSrc)
{
    *this = systimeSrc;
} // CHighTime::CHighTime(SYSTEMTIME)

inline CHighTime::CHighTime(const FILETIME &filetimeSrc)
{
    *this = filetimeSrc;
} // CHighTime::CHighTime(FILETIME)

//inline CHighTime::CHighTime(const time_t timeSrc)
//{
//    *this = timeSrc;
//} // CHighTime::CHighTime()

// CHighTime operators
inline CHighTime::operator SYSTEMTIME() const
{
    SYSTEMTIME systime;
    GetAsSystemTime(systime);
    return systime;
} // CHighTime::operator ()

inline CHighTime::operator FILETIME() const
{
    FILETIME filetime;
    filetime.dwLowDateTime = m_liTime.LowPart;
    filetime.dwHighDateTime = m_liTime.HighPart;
    return filetime;
} // CHighTime::operator ()

#if defined(USE_MFC)
inline CHighTime::operator COleDateTime() const
{
	_HighTimeFormat tmTemp;
    if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp)) {
        return COleDateTime((int)tmTemp.nYear,(int)tmTemp.nMonth,(int)tmTemp.nDay,
                            (int)tmTemp.nHour,(int)tmTemp.nMinute,(int)tmTemp.nSecond);
    }
	else
		return COleDateTime();
} // CHighTime::operator COleDateTime()

inline CHighTime::operator CTime() const
{
	_HighTimeFormat tmTemp;
    if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp)) {
        return CTime((int)tmTemp.nYear,(int)tmTemp.nMonth,(int)tmTemp.nDay,
                            (int)tmTemp.nHour,(int)tmTemp.nMinute,(int)tmTemp.nSecond);
    }
	else
		return CTime();
} // CHighTime::operator COleDateTime()

inline const COleDateTime CHighTime::GetCOleDateTime(bool bAdjusted) const // default bAdjusted=false
{
    int iMilli = 0;
	_HighTimeFormat tmTemp;
    if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp)) {
        if (bAdjusted) iMilli = (tmTemp.nMilli + 500) / 1000; // 
        return COleDateTime((int)tmTemp.nYear,(int)tmTemp.nMonth,(int)tmTemp.nDay,
                            (int)tmTemp.nHour,(int)tmTemp.nMinute,(int)tmTemp.nSecond+iMilli);
    }
	else
		return COleDateTime();
} // CHighTime::GetCOleDateTime()

#endif //defined(USE_MFC)

inline const CHighTime& CHighTime::operator=(const CHighTime& dateSrc) 
{
    m_liTime = dateSrc.m_liTime;
    m_status = dateSrc.m_status;
    return *this;
} // CHighTime::operator=()

inline const CHighTime& CHighTime::operator=(const LARGE_INTEGER dateSrc)
{
    m_liTime = dateSrc;
    m_status = valid;
    return *this;
} // CHighTime::operator=()

inline const CHighTime& CHighTime::operator=(const LONGLONG dateSrc)
{
    m_liTime.QuadPart = dateSrc;
    m_status = valid;
    return *this;
} // CHighTime::operator=()

inline int CHighTime::SetDate(int nYear, int nMonth, int nDay)
{
    return SetDateTime(nYear, nMonth, nDay, 0, 0, 0, 0, 0, 0);
} // CHighTime::SetDate()

inline int CHighTime::SetTime(int nHour, int nMin, int nSec)
{
    return SetDateTime(1601, 1, 1, nHour, nMin, nSec, 0, 0, 0);
} // CHighTime::SetTime()

inline int CHighTime::SetFracTime(int nMilli, int nMicro, int nNano)
{
    return SetDateTime(1601, 1, 1, 0, 0, 0, nMilli, nMicro, nNano);
} // CHighTime::SetFracTime()

inline bool CHighTime::operator==(const CHighTime& date) const
{ 
    return (m_status == date.m_status && m_liTime.QuadPart == date.m_liTime.QuadPart); 
} // CHighTime::operator==()

inline bool CHighTime::operator!=(const CHighTime& date) const
{
    return (m_status != date.m_status || m_liTime.QuadPart != date.m_liTime.QuadPart); 
} // CHighTime::operator!=()

inline bool CHighTime::operator<(const CHighTime& date) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(date.GetStatus() == valid);
    return (m_liTime.QuadPart < date.m_liTime.QuadPart); 
} // CHighTime::operator<()

inline bool CHighTime::operator>(const CHighTime& date) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(date.GetStatus() == valid);
    return (m_liTime.QuadPart > date.m_liTime.QuadPart); 
} // CHighTime::operator>()

inline bool CHighTime::operator<=(const CHighTime& date) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(date.GetStatus() == valid);
    return (m_liTime.QuadPart <= date.m_liTime.QuadPart); 
} // CHighTime::operator<=()

inline bool CHighTime::operator>=(const CHighTime& date) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(date.GetStatus() == valid);
    return (m_liTime.QuadPart >= date.m_liTime.QuadPart); 
} // CHighTime::operator>=()

inline const CHighTime& CHighTime::operator+=(const CHighTimeSpan dateSpan)
{
	*this = *this + dateSpan; 
    return *this;
} // CHighTime::operator+=()

inline const CHighTime& CHighTime::operator-=(const CHighTimeSpan dateSpan)
{
	*this = *this - dateSpan; 
    return *this;
} // CHighTime::operator-=()



// ******************************
// HighTimeSpan class
// ******************************
inline CHighTimeSpan::CHighTimeSpan()
{
    m_liSpan.QuadPart = 0;
	m_status = valid;
} // CHighTimeSpan::CHighTimeSpan()

inline CHighTimeSpan::CHighTimeSpan(const LARGE_INTEGER dateSrcSpan)
{
    *this = dateSrcSpan;
} // CHighTimeSpan::CHighTimeSpan(LARGE_INTEGER)

inline CHighTimeSpan::CHighTimeSpan(const LONGLONG dateSrcSpan)
{
    *this = dateSrcSpan;
} // CHighTimeSpan::CHighTimeSpan(LONGLONG )

#if defined(USE_MFC)
inline CHighTimeSpan::CHighTimeSpan(const COleDateTimeSpan &dateSrcSpan)
{
    *this = dateSrcSpan;
}
inline CHighTimeSpan::CHighTimeSpan(const CTimeSpan &dateSrcSpan)
{
    *this = dateSrcSpan;
}
#endif

inline CHighTimeSpan::CHighTimeSpan(const CHighTimeSpan &dateSrcSpan)
{
    *this = dateSrcSpan;
} // CHighTimeSpan::CHighTimeSpan()

inline CHighTimeSpan::CHighTimeSpan(long lDays, int nHours, int nMins, int nSecs,
                            int nMilli, int nMicro, int nNano)   // Milli, Micro & nano, default = 0
{
    SetHighTimeSpan(lDays, nHours, nMins, nSecs,
                           nMilli, nMicro, nNano);
} // CHighTimeSpan::CHighTimeSpan()

inline bool CHighTimeSpan::IsNegative() const
{
    return (m_liSpan.QuadPart < 0);
} // CHighTimeSpan::IsNegative()

#if defined(USE_MFC)
inline CHighTimeSpan::operator COleDateTimeSpan() const
{
    return COleDateTimeSpan(GetDays(), GetHours(), GetMinutes(), GetSeconds());
} // CHighTimeSpan::operator COleDateTimeSpan()

inline CHighTimeSpan::operator CTimeSpan() const
{
    return CTimeSpan(GetDays(), GetHours(), GetMinutes(), GetSeconds());
} // CHighTimeSpan::operator CTimeSpan()

inline const COleDateTimeSpan CHighTimeSpan::GetCOleDateTimeSpan(bool bAdjusted) const 
                                                             // default bAdjusted=false
{
    int iMilli = 0;
    if (bAdjusted) iMilli= (GetMilliSeconds() + 500) / 1000; // 
    return COleDateTimeSpan(GetDays(), GetHours(), GetMinutes(), GetSeconds()+iMilli);
} // CHighTimeSpan::GetCOleDateTimeSpan()
#endif //defined(USE_MFC)

inline const CHighTimeSpan& CHighTimeSpan::operator=(LARGE_INTEGER dateSpanSrc)
{
    m_liSpan = dateSpanSrc;
    m_status = valid;
    return *this;
} // CHighTimeSpan::operator=()

inline const CHighTimeSpan& CHighTimeSpan::operator=(LONGLONG dateSpanSrc)
{
    m_liSpan.QuadPart = dateSpanSrc;
    m_status = valid;
    return *this;
} // CHighTimeSpan::operator=()

inline const CHighTimeSpan& CHighTimeSpan::operator=(const CHighTimeSpan& dateSpanSrc)
{
    m_liSpan = dateSpanSrc.m_liSpan;
    m_status = dateSpanSrc.m_status;
    return *this;
} // CHighTimeSpan::operator=()

#if defined(USE_MFC)
inline const CHighTimeSpan& CHighTimeSpan::operator=(const COleDateTimeSpan& dateSpanSrc)
{
    SetHighTimeSpan(dateSpanSrc.GetDays(), dateSpanSrc.GetHours(), 
                    dateSpanSrc.GetMinutes(), dateSpanSrc.GetSeconds());
    return *this;
} // CHighTimeSpan::operator=()

inline const CHighTimeSpan& CHighTimeSpan::operator=(const CTimeSpan& dateSpanSrc)
{
    SetHighTimeSpan(dateSpanSrc.GetDays(), dateSpanSrc.GetHours(), 
                    dateSpanSrc.GetMinutes(), dateSpanSrc.GetSeconds());
    return *this;
} // CHighTimeSpan::operator=()
#endif

inline bool CHighTimeSpan::operator==(const CHighTimeSpan& dateSpan) const
{
    return (m_status == dateSpan.m_status &&
		    m_liSpan.QuadPart == dateSpan.m_liSpan.QuadPart);
} // CHighTimeSpan::operator==()

inline bool CHighTimeSpan::operator!=(const CHighTimeSpan& dateSpan) const
{
    return (m_status != dateSpan.m_status ||
		    m_liSpan.QuadPart != dateSpan.m_liSpan.QuadPart);
} // CHighTimeSpan::operator!=()

inline bool CHighTimeSpan::operator<(const CHighTimeSpan& dateSpan) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(dateSpan.GetStatus() == valid);
    return m_liSpan.QuadPart < dateSpan.m_liSpan.QuadPart;
} // CHighTimeSpan::operator<()

inline bool CHighTimeSpan::operator>(const CHighTimeSpan& dateSpan) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(dateSpan.GetStatus() == valid);
    return m_liSpan.QuadPart > dateSpan.m_liSpan.QuadPart;
} // CHighTimeSpan::operator>()

inline bool CHighTimeSpan::operator<=(const CHighTimeSpan& dateSpan) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(dateSpan.GetStatus() == valid);
    return m_liSpan.QuadPart <= dateSpan.m_liSpan.QuadPart;
} // CHighTimeSpan::operator<=()

inline bool CHighTimeSpan::operator>=(const CHighTimeSpan& dateSpan) const
{
	ASSERT(GetStatus() == valid);
	ASSERT(dateSpan.GetStatus() == valid);
    return m_liSpan.QuadPart >= dateSpan.m_liSpan.QuadPart;
} // CHighTimeSpan::operator>=()

inline const CHighTimeSpan& CHighTimeSpan::operator+=(const CHighTimeSpan dateSpan)
{
	*this = *this + dateSpan; 
    return *this;
} // CHighTimeSpan::operator+=()

inline const CHighTimeSpan& CHighTimeSpan::operator-=(const CHighTimeSpan dateSpan)
{
	*this = *this - dateSpan; 
    return *this;
} // CHighTimeSpan::operator-=()

inline CHighTimeSpan CHighTimeSpan::operator-() const
{
	return -this->m_liSpan.QuadPart;
} // CHighTimeSpan::operator-()

#endif //!defined(_HIGHTIME_INL_)
