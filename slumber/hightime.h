//////////////////////////////////////////////////////////////////////
//
// HighTime.h: interface for the CHighTime class.
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

#if !defined(_HIGHTIME_H_)
#define _HIGHTIME_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _AFXDLL
#define USE_MFC
#endif

#ifndef USE_MFC

#include <windows.h>
#include <time.h>
#include <assert.h>

#ifndef ASSERT
    #ifdef _DEBUG
    #define ASSERT assert
    #else
    #define ASSERT
    #endif // _DEBUG
#endif // !ASSERT

#endif //!USE_MFC

#define HIGH_DATETIME_ERROR (-1)
#define INVALID_DATETIME "Invalid datetime"
#define INVALID_SPANTIME "Invalid datetimespan"

class CHighTime;
class CHighTimeSpan;

class CHighTime
{
public:
    static const CHighTime GetPresentTime();
public:
    enum HighTimeStatus {
       valid = 0,
       invalid = 1,
       null = 2,
    };
	CHighTime();
	CHighTime(const CHighTime &dateSrc);
    CHighTime(const LARGE_INTEGER dateSrc);
    CHighTime(const LONGLONG dateSrc);
#if defined(USE_MFC)
    CHighTime(const COleDateTime &dateSrc);
    CHighTime(const CTime &dateSrc);
#endif
	CHighTime(const SYSTEMTIME &systimeSrc);
	CHighTime(const FILETIME &filetimeSrc);
	//CHighTime(const time_t timeSrc);
	CHighTime(int nYear, int nMonth, int nDay,
              int nHour, int nMinute, int nSecond, 
              int nMilli = 0, int nMicro = 0, int nNano = 0);

// operators
public:
	const CHighTime& operator=(const CHighTime& dateSrc);
	const CHighTime& operator=(const LARGE_INTEGER dateSrc);
	const CHighTime& operator=(const LONGLONG dateSrc);
#if defined(USE_MFC)
	const CHighTime& operator=(const COleDateTime& dateSrc);
	const CHighTime& operator=(const CTime& dateSrc);
#endif

	const CHighTime& operator=(const SYSTEMTIME& systimeSrc);
	const CHighTime& operator=(const FILETIME& filetimeSrc);
	const CHighTime& operator=(const time_t& timeSrc);

    bool operator==(const CHighTime& date) const;
    bool operator!=(const CHighTime& date) const;
    bool operator<(const CHighTime& date) const;
    bool operator>(const CHighTime& date) const;
    bool operator<=(const CHighTime& date) const;
    bool operator>=(const CHighTime& date) const;

    // CHighTime math
    CHighTime operator+(const CHighTimeSpan &dateSpan) const;
    CHighTime operator-(const CHighTimeSpan &dateSpan) const;
    const CHighTime& operator+=(const CHighTimeSpan dateSpan);
    const CHighTime& operator-=(const CHighTimeSpan dateSpan);

    // CHighTimeSpan math
    CHighTimeSpan operator-(const CHighTime& date) const;

    operator SYSTEMTIME() const;
    operator FILETIME() const;
    operator LARGE_INTEGER() const {return m_liTime;}
    operator LONGLONG() const {return m_liTime.QuadPart;}
#if defined(USE_MFC)
    operator COleDateTime() const;
    operator CTime() const;
    const COleDateTime GetCOleDateTime(bool bAdjusted=false) const;
#endif //defined(USE_MFC)

    // Operations
public:
    int SetDateTime(int nYear, int nMonth, int nDay,
                    int nHour, int nMinute, int nSecond, 
                    int nMilli = 0, int nMicro = 0, int nNano = 0);
	int SetDate(int nYear, int nMonth, int nDay);
	int SetTime(int nHour, int nMin, int nSec);
	int SetFracTime(int nMilli, int nMicro, int nNano);

    HighTimeStatus GetStatus() const {return m_status;}
    void SetStatus(HighTimeStatus status) { m_status = status; }
	BOOL GetAsSystemTime(SYSTEMTIME& sysTime) const;

	int GetYear() const;
	int GetMonth() const;       // month of year (1 = Jan)
	int GetDay() const;         // day of month (0-31)
	int GetHour() const;        // hour in day (0-23)
	int GetMinute() const;      // minute in hour (0-59)
	int GetSecond() const;      // second in minute (0-59)
	int GetMilliSecond() const; // millisecond in minute (0-999)
	int GetMicroSecond() const; // microsecond in minute (0-999)
	int GetNanoSecond() const;  // nanosecond in minute (0-999), step of 100ns
    int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
	int GetDayOfYear() const;   // days since start of year, Jan 1 = 1

    // formatting

#if defined(USE_MFC)
	CString Format(LPCTSTR lpszFormat) const;
	CString Format(UINT nFormatID) const;
#endif // defined(USE_MFC)
    // This function is not complete yet.. it can not show milli.micro or nanoseconds
    // but the rest works
	LPCTSTR Format(LPSTR pszBuffer, int iBufferLen, LPCTSTR pszFormat) const;

public:
	HighTimeStatus   m_status;
    LARGE_INTEGER    m_liTime;

public:
#if defined(USE_MFC)
#ifdef _DEBUG
    friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,CHighTime dateSrc);
#endif
    friend CArchive& AFXAPI operator<<(CArchive& ar, CHighTime dateSrc);
    friend CArchive& AFXAPI operator>>(CArchive& ar, CHighTime& dateSrc);
#endif

private:
    struct _HighTimeFormat {
        int  nYear;
        UINT nMonth;
        UINT nDay;
        UINT nHour;
        UINT nMinute;
        UINT nSecond;
        UINT nMilli;
        UINT nMicro;
        UINT nNano;
        UINT nDayOfYear;
        UINT nDayOfWeek;
    };
    static int anMonthDayInYear[13];
    static BOOL ConvertTimeToLongLong(const _HighTimeFormat  &SrcTime,
                                      LARGE_INTEGER &liDestTime);
    static BOOL ConvertLongLongToTime(const LARGE_INTEGER &liSrcTime,
                                       _HighTimeFormat  &DestTime);
    static void ConvertToStandardFormat(_HighTimeFormat &tmTempHigh, tm &tmTemp);

    // String helper functions
    static int FindStr(LPCTSTR pszSource, LPCTSTR pszSub, int nStart);
    static int ReplaceStr(LPSTR pszDest, int nDestSize, LPCTSTR pszMask, LPCTSTR pszSource);

    friend class CHighTimeSpan;
}; //class CHighTime

#if defined(USE_MFC)
// CHighTime diagnostics and serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc,CHighTime dateSrc);
#endif
CArchive& AFXAPI operator<<(CArchive& ar, CHighTime dateSrc);
CArchive& AFXAPI operator>>(CArchive& ar, CHighTime& dateSrc);
#endif //defined(USE_MFC)



class CHighTimeSpan
{
public:
    enum HighTimeSpanStatus {
       valid = 0,
       invalid = 1,
       null = 2,
    };

    CHighTimeSpan();
    CHighTimeSpan(const LONGLONG dateSrcSpan);
    CHighTimeSpan(const LARGE_INTEGER dateSrcSpan);
#if defined(USE_MFC)
    CHighTimeSpan(const COleDateTimeSpan &dateSrcSpan);
    CHighTimeSpan(const CTimeSpan &dateSrcSpan);
#endif
    CHighTimeSpan(const CHighTimeSpan &dateSrcSpan);
	CHighTimeSpan(long lDays, int nHours, int nMins, int nSecs,
                  int nMilli = 0, int nMicro = 0, int nNano = 0);

    // Operations
public:
    void SetStatus(HighTimeSpanStatus status) { m_status = status; }
    HighTimeSpanStatus GetStatus() const {return m_status;}

	void SetHighTimeSpan(long lDays, int nHours, int nMins, int nSecs,
                                     int nMillis = 0, int nMicros = 0, int nNanos = 0);

	LONGLONG GetTotalDays() const;    // span in days (about -3.65e6 to 3.65e6)
	LONGLONG GetTotalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
	LONGLONG GetTotalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
	LONGLONG GetTotalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)
	LONGLONG GetTotalMilliSeconds() const; // span in milliseconds
	LONGLONG GetTotalMicroSeconds() const; // span in microseconds
	LONGLONG GetTotalNanoSeconds() const; // span in nanoseconds

	int GetDays() const;       // component days in span
	int GetHours() const;      // component hours in span (-23 to 23)
	int GetMinutes() const;    // component minutes in span (-59 to 59)
	int GetSeconds() const;    // component seconds in span (-59 to 59)
	int GetMilliSeconds() const; // component Milliseconds in span (-999 to 999)
	int GetMicroSeconds() const; // component Microseconds in span (-999 to 999)
	int GetNanoSeconds() const;  // component Nanoseconds in span (-900 to 900)

    bool IsNegative() const;

    // formatting
    LPSTR Format(LPSTR pszBuffer, int iBufferLen, LPCTSTR pFormat) const;
#if defined(USE_MFC)
    CString Format(LPCTSTR pFormat) const;
	CString Format(UINT nID) const;
#endif

    // operators
public:
	const CHighTimeSpan& operator=(LARGE_INTEGER dateSpanSrc);
	const CHighTimeSpan& operator=(LONGLONG dateSpanSrc);
	const CHighTimeSpan& operator=(const CHighTimeSpan& dateSpanSrc);
#if defined(USE_MFC)
	const CHighTimeSpan& operator=(const COleDateTimeSpan& dateSpanSrc);
	const CHighTimeSpan& operator=(const CTimeSpan& dateSpanSrc);
#endif

    bool operator==(const CHighTimeSpan& dateSpan) const;
	bool operator!=(const CHighTimeSpan& dateSpan) const;
	bool operator<(const CHighTimeSpan& dateSpan) const;
	bool operator>(const CHighTimeSpan& dateSpan) const;
	bool operator<=(const CHighTimeSpan& dateSpan) const;
	bool operator>=(const CHighTimeSpan& dateSpan) const;

	// CHighTimeSpan math
	CHighTimeSpan operator+(const CHighTimeSpan& dateSpan) const;
	CHighTimeSpan operator-(const CHighTimeSpan& dateSpan) const;
	const CHighTimeSpan& operator+=(const CHighTimeSpan dateSpan);
	const CHighTimeSpan& operator-=(const CHighTimeSpan dateSpan);
	CHighTimeSpan operator-() const;

    operator LARGE_INTEGER() const {return m_liSpan;}
    operator LONGLONG() const {return m_liSpan.QuadPart;}
#if defined(USE_MFC)
    operator COleDateTimeSpan() const;
    operator CTimeSpan() const;
    const COleDateTimeSpan GetCOleDateTimeSpan(bool bAdjusted=false) const;
#endif //defined(USE_MFC)

public:
	HighTimeSpanStatus m_status;
    LARGE_INTEGER      m_liSpan;

public:
#if defined(USE_MFC)
#ifdef _DEBUG
    friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,CHighTimeSpan dateSpanSrc);
#endif
    friend CArchive& AFXAPI operator<<(CArchive& ar, CHighTimeSpan dateSpanSrc);
    friend CArchive& AFXAPI operator>>(CArchive& ar, CHighTimeSpan& dateSpanSrc);
#endif

private:
    friend class CHighTime;
}; // class CHighTimeSpan

#if defined(USE_MFC)
// CHighTimeSpan diagnostics and serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc,CHighTimeSpan dateSpanSrc);
#endif
CArchive& AFXAPI operator<<(CArchive& ar, CHighTimeSpan dateSpanSrc);
CArchive& AFXAPI operator>>(CArchive& ar, CHighTimeSpan& dateSpanSrc);
#endif //defined(USE_MFC)

#include "hightime.inl"

#endif // !defined(_HIGHTIME_H_)
