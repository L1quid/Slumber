//////////////////////////////////////////////////////////////////////
//
// HighTime.cpp: implementation of the CHighTime and the CHighTimeSpan classes.
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

#include "HighTime.h"

#define LLABS(i) (((i)<0)?-(i):(i))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_TIME_BUFFER_SIZE 128

// Constant array with months # of days of year
int CHighTime::anMonthDayInYear[13] = {0, 31, 59, 90, 120, 151, 181, 
                                       212, 243, 273, 304, 334, 365};

// Static member for getting the current time
const CHighTime CHighTime::GetPresentTime()
{
    SYSTEMTIME systime;
    ::GetLocalTime(&systime);
    return CHighTime(systime);
} // CHighTime::GetPresentTime()

CHighTime::CHighTime(int nYear, int nMonth, int nDay,
                     int nHour, int nMinute, int nSecond, 
                     int nMilli , int nMicro ,int nNano)
                       // nMilli, nMicro & nNano default = 0
{
    _HighTimeFormat  SrcTime;
    SrcTime.nYear   = nYear;
    SrcTime.nMonth  = nMonth;
    SrcTime.nDay    = nDay;
    SrcTime.nHour   = nHour;
    SrcTime.nMinute = nMinute;
    SrcTime.nSecond = nSecond;
    SrcTime.nMilli  = nMilli;
    SrcTime.nMicro  = nMicro;
    SrcTime.nNano   = nNano;
    m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
} // CHighTime::CHighTime()

// CHighTime operators

#if defined(USE_MFC)
const CHighTime& CHighTime::operator=(const COleDateTime& dateSrc) 
{
    _HighTimeFormat  SrcTime;
    SrcTime.nYear   = dateSrc.GetYear();
    SrcTime.nMonth  = dateSrc.GetMonth();
    SrcTime.nDay    = dateSrc.GetDay();
    SrcTime.nHour   = dateSrc.GetHour();
    SrcTime.nMinute = dateSrc.GetMinute();
    SrcTime.nSecond = dateSrc.GetSecond();
    SrcTime.nMilli  = 0;
    SrcTime.nMicro  = 0;
    SrcTime.nNano  = 0;
    m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
    return *this;
} // CHighTime::operator=()

const CHighTime& CHighTime::operator=(const CTime& dateSrc) 
{
    _HighTimeFormat  SrcTime;
    SrcTime.nYear   = dateSrc.GetYear();
    SrcTime.nMonth  = dateSrc.GetMonth();
    SrcTime.nDay    = dateSrc.GetDay();
    SrcTime.nHour   = dateSrc.GetHour();
    SrcTime.nMinute = dateSrc.GetMinute();
    SrcTime.nSecond = dateSrc.GetSecond();
    SrcTime.nMilli  = 0;
    SrcTime.nMicro  = 0;
    SrcTime.nNano  = 0;
    m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
    return *this;
} // CHighTime::operator=()
#endif

const CHighTime& CHighTime::operator=(const SYSTEMTIME& systimeSrc)
{
    _HighTimeFormat  SrcTime;
    SrcTime.nYear   = systimeSrc.wYear;
    SrcTime.nMonth  = systimeSrc.wMonth;
    SrcTime.nDay    = systimeSrc.wDay;
    SrcTime.nHour   = systimeSrc.wHour;
    SrcTime.nMinute = systimeSrc.wMinute;
    SrcTime.nSecond = systimeSrc.wSecond;
    SrcTime.nMilli  = systimeSrc.wMilliseconds;
    SrcTime.nMicro  = 0;
    SrcTime.nNano  = 0;
    m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
    return *this;
} // CHighTime::operator=()

const CHighTime& CHighTime::operator=(const FILETIME& filetimeSrc)
{
    SYSTEMTIME systime;

    if (FileTimeToSystemTime(&filetimeSrc, &systime)) {
        _HighTimeFormat  SrcTime;
        SrcTime.nYear   = systime.wYear;
        SrcTime.nMonth  = systime.wMonth;
        SrcTime.nDay    = systime.wDay;
        SrcTime.nHour   = systime.wHour;
        SrcTime.nMinute = systime.wMinute;
        SrcTime.nSecond = systime.wSecond;
        SrcTime.nMilli  = systime.wMilliseconds;
        SrcTime.nMicro  = 0;
        SrcTime.nNano  = 0;
        m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
    }
    else {
        SetStatus(invalid);
    }
    return *this;
} // CHighTime::operator=()

const CHighTime& CHighTime::operator=(const time_t& timeSrc)
{
    tm *pTimeSrc = localtime(&timeSrc);

    if (pTimeSrc) {
        _HighTimeFormat  SrcTime;
        SrcTime.nYear   = pTimeSrc->tm_year+1900;
        SrcTime.nMonth  = pTimeSrc->tm_mon+1;
        SrcTime.nDay    = pTimeSrc->tm_mday;
        SrcTime.nHour   = pTimeSrc->tm_hour;
        SrcTime.nMinute = pTimeSrc->tm_min;
        SrcTime.nSecond = pTimeSrc->tm_sec;
        SrcTime.nMilli  = 0;
        SrcTime.nMicro  = 0;
        SrcTime.nNano  = 0;
        m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid;
    }
    else {
		// Local time must have failed (timsSrc before 1/1/70 12am)
        SetStatus(invalid);
        ASSERT(FALSE);
    }
    return *this;
} // CHighTime::operator=(time_t)


int CHighTime::SetDateTime(int nYear, int nMonth, int nDay,
                           int nHour, int nMinute, int nSecond, 
                           int nMilli, int nMicro, int nNano)  
                            // nMilli, nMicro & nHundredsNano default = 0
{
    _HighTimeFormat  SrcTime;
    SrcTime.nYear   = nYear;
    SrcTime.nMonth  = nMonth;
    SrcTime.nDay    = nDay;
    SrcTime.nHour   = nHour;
    SrcTime.nMinute = nMinute;
    SrcTime.nSecond = nSecond;
    SrcTime.nMilli  = nMilli;
    SrcTime.nMicro  = nMicro;
    SrcTime.nNano   = nNano;
    return (m_status = ConvertTimeToLongLong(SrcTime, m_liTime) ? valid : invalid) == valid;
} // CHighTime::SetDateTime()

// HighTime helper function, static function
BOOL CHighTime::ConvertTimeToLongLong(const _HighTimeFormat  &SrcTime,
                                      LARGE_INTEGER &liDestTime)
{
	LARGE_INTEGER nDate;
    int iDays = SrcTime.nDay;
    UINT nHour = SrcTime.nHour;
    UINT nMinute = SrcTime.nMinute;
    UINT nSecond = SrcTime.nSecond;
    UINT nMilliSecond = SrcTime.nMilli;
    UINT nMicroSecond = SrcTime.nMicro;
    UINT nHundredsNano = (SrcTime.nNano +50) / 100;

    // Validate year and month
	if (SrcTime.nYear > 29000 || SrcTime.nYear < -29000 ||
        SrcTime.nMonth < 1 || SrcTime.nMonth > 12)
		return FALSE;

	//  Check for leap year
	BOOL bIsLeapYear = ((SrcTime.nYear & 3) == 0) &&
		                ((SrcTime.nYear % 100) != 0 || (SrcTime.nYear % 400) == 0);

    /*int nDaysInMonth = 
		anMonthDayInYear[SrcTime.nMonth] - anMonthDayInYear[SrcTime.nMonth-1] +
		((bIsLeapYear && SrcTime.nDay == 29 && SrcTime.nMonth == 2) ? 1 : 0);*/

    // Adjust time and frac time
    nMicroSecond += nHundredsNano / 10;
    nHundredsNano %= 10;
    nMilliSecond += nMicroSecond / 1000;
    nMicroSecond %= 1000;
    nSecond +=nMilliSecond / 1000;
    nMilliSecond %= 1000;
    nMinute += nSecond / 60;
    nSecond %= 60;
    nHour += nMinute / 60;
    nMinute %= 60;
    iDays += nHour / 24;
    nHour %= 24;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate.QuadPart = SrcTime.nYear*365L + SrcTime.nYear/4 - SrcTime.nYear/100 + SrcTime.nYear/400 +
	                 anMonthDayInYear[SrcTime.nMonth-1] + iDays;

	//  If leap year and it's before March, subtract 1:
	if (SrcTime.nMonth <= 2 && bIsLeapYear)
		--nDate.QuadPart;

	//  Offset so that 01/01/1601 is 0
	nDate.QuadPart -= 584754L;

    // Change nDate to seconds
    nDate.QuadPart *= 86400L;
    nDate.QuadPart += (nHour * 3600L) + (nMinute * 60L) + nSecond;

    // Change nDate to hundreds of nanoseconds
    nDate.QuadPart *= 10000000L;
    nDate.QuadPart += (nMilliSecond * 10000L) + (nMicroSecond * 10L) + nHundredsNano;

	liDestTime = nDate;

    return TRUE;
} // CHighTime::ConvertTimeToLongLong()

BOOL CHighTime::ConvertLongLongToTime(const LARGE_INTEGER &liSrcTime,
                                       _HighTimeFormat  &DestTime)
{
    LARGE_INTEGER nTempTime;
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
    long nMinutesInDay;     // Minutes in day

	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)
	BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year
    long nHNanosThisDay;
    long nMillisThisDay;

    nTempTime = liSrcTime;
    nHNanosThisDay      = (long)(nTempTime.QuadPart % 10000000L);
    nTempTime.QuadPart /= 10000000L;
    nSecsInDay          = (long)(nTempTime.QuadPart % 86400L);
    nTempTime.QuadPart /= 86400L;
    nDaysAbsolute       = (long)(nTempTime.QuadPart);
    nDaysAbsolute      += 584754L;	//  Add days from 1/1/0 to 01/01/1601

    // Calculate the day of week (sun=1, mon=2...)
    //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
    DestTime.nDayOfWeek = (int)((nDaysAbsolute - 1) % 7L) + 1;


	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = FALSE;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	DestTime.nDayOfYear = (int)n4Day + 1;
	DestTime.nYear = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			DestTime.nMonth  = 2;
			DestTime.nDay = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (DestTime.nMonth = (n4Day >> 5) + 1;
		n4Day > anMonthDayInYear[DestTime.nMonth]; DestTime.nMonth++);

	DestTime.nDay = (int)(n4Day - anMonthDayInYear[DestTime.nMonth-1]);

DoTime:
	if (nSecsInDay == 0)
		DestTime.nHour = DestTime.nMinute = DestTime.nSecond = 0;
	else
	{
		DestTime.nSecond = (UINT)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		DestTime.nMinute = (UINT)nMinutesInDay % 60;
		DestTime.nHour   = (UINT)nMinutesInDay / 60;
	}

    if (nHNanosThisDay == 0)
		DestTime.nMilli = DestTime.nMicro = DestTime.nNano = 0;
	else
	{
		DestTime.nNano = (UINT)((nHNanosThisDay % 10L) * 100L);
		nMillisThisDay = nHNanosThisDay / 10L;
		DestTime.nMicro = (UINT)nMillisThisDay % 1000;
		DestTime.nMilli   = (UINT)nMillisThisDay / 1000;
	}

    return TRUE;
} // CHighTime::ConvertLongLongToTime()

void CHighTime::ConvertToStandardFormat(_HighTimeFormat &tmTempHigh, tm &tmSrc)
{
    // Convert internal tm to format expected by runtimes (sfrtime, etc)
	tmSrc.tm_year = tmTempHigh.nYear-1900;  // year is based on 1900
	tmSrc.tm_mon  = tmTempHigh.nMonth-1;      // month of year is 0-based
	tmSrc.tm_wday = tmTempHigh.nDayOfWeek-1;     // day of week is 0-based
	tmSrc.tm_yday = tmTempHigh.nDayOfYear-1;     // day of year is 0-based
    tmSrc.tm_mday = tmTempHigh.nDay;
    tmSrc.tm_hour = tmTempHigh.nHour;
    tmSrc.tm_min = tmTempHigh.nMinute;
    tmSrc.tm_sec = tmTempHigh.nSecond;
    tmSrc.tm_isdst = 0;
} // CHighTime::ConvertToStandardFormat()

int CHighTime::GetYear() const
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nYear;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetYear()

int CHighTime::GetMonth() const       // month of year (1 = Jan)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nMonth;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetMonth()

int CHighTime::GetDay() const         // day of month (0-31)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nDay;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetDay()

int CHighTime::GetHour() const        // hour in day (0-23)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nHour;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetHour()

int CHighTime::GetMinute() const      // minute in hour (0-59)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nMinute;
	else
		return HIGH_DATETIME_ERROR;

} // CHighTime::GetMinute()

int CHighTime::GetSecond() const      // second in minute (0-59)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nSecond;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetSecond()

int CHighTime::GetMilliSecond() const // millisecond in minute (0-999)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nMilli;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetMilliSecond()

int CHighTime::GetMicroSecond() const // microsecond in minute (0-999)
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nMicro;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetMicroSecond()

int CHighTime::GetNanoSecond() const  // nanosecond in minute (0-999), step of 100ns
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nNano;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetNanoSecond()

int CHighTime::GetDayOfWeek() const   // 1=Sun, 2=Mon, ..., 7=Sat
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nDayOfWeek;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetDayOfWeek()

int CHighTime::GetDayOfYear() const   // days since start of year, Jan 1 = 1
{
	_HighTimeFormat tmTemp;
	if (GetStatus() == valid && ConvertLongLongToTime(m_liTime, tmTemp))
		return tmTemp.nDayOfYear;
	else
		return HIGH_DATETIME_ERROR;
} // CHighTime::GetDayOfYear()

BOOL CHighTime::GetAsSystemTime(SYSTEMTIME& sysTime) const
{
	BOOL bRetVal = FALSE;
	if (GetStatus() == valid)
	{
		_HighTimeFormat tmTemp;
		if (ConvertLongLongToTime(m_liTime, tmTemp))
		{
			sysTime.wYear = (WORD) tmTemp.nYear;
			sysTime.wMonth = (WORD) tmTemp.nMonth;
			sysTime.wDayOfWeek = (WORD) (tmTemp.nDayOfWeek - 1);
			sysTime.wDay = (WORD) tmTemp.nDay;
			sysTime.wHour = (WORD) tmTemp.nHour;
			sysTime.wMinute = (WORD) tmTemp.nMinute;
			sysTime.wSecond = (WORD) tmTemp.nSecond;
			sysTime.wMilliseconds = (WORD)tmTemp.nMilli;

			bRetVal = TRUE;
		}
	}

	return bRetVal;
} // CHighTime::GetAsSystemTime()

// CHighTime math
CHighTime CHighTime::operator+(const CHighTimeSpan &dateSpan) const
{
	CHighTime dateResult;    // Initializes m_status to valid

	// If either operand NULL, result NULL
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateResult.SetStatus(null);
		return dateResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateResult.SetStatus(invalid);
		return dateResult;
	}

	// Compute the actual date difference by adding underlying dates
	dateResult.m_liTime.QuadPart = m_liTime.QuadPart + dateSpan.m_liSpan.QuadPart;

	// Validate within range
	//dateResult.CheckRange();

	return dateResult;
} // CHighTime::operator+()

CHighTime CHighTime::operator-(const CHighTimeSpan &dateSpan) const
{
	CHighTime dateResult;    // Initializes m_status to valid

	// If either operand NULL, result NULL
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateResult.SetStatus(null);
		return dateResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateResult.SetStatus(invalid);
		return dateResult;
	}

	// Compute the actual date difference by adding underlying dates
	dateResult.m_liTime.QuadPart = m_liTime.QuadPart - dateSpan.m_liSpan.QuadPart;

	// Validate within range
	//dateResult.CheckRange();

	return dateResult;
} // CHighTime::operator-()


// CHighTimeSpan math
CHighTimeSpan CHighTime::operator-(const CHighTime& date) const
{
	CHighTimeSpan spanResult;

	// If either operand NULL, result NULL
	if (GetStatus() == null || date.GetStatus() == null)
	{
		spanResult.SetStatus(CHighTimeSpan::null);
		return spanResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || date.GetStatus() == invalid)
	{
		spanResult.SetStatus(CHighTimeSpan::invalid);
		return spanResult;
	}

    spanResult.m_liSpan.QuadPart = m_liTime.QuadPart - date.m_liTime.QuadPart;


	return spanResult;
} // CHighTime::operator-()

// String helper functions
int CHighTime::FindStr(LPCTSTR pszSource, LPCTSTR pszSub, int nStart)
{
	int nLength = strlen(pszSource);
	if (nStart > nLength)
		return -1;

	// find first
	const char *pszSearch = strstr(pszSource + nStart, pszSub);

	// return -1 for not found
	return (pszSearch == NULL) ? -1 : (int)(pszSearch - pszSource);
} // CHighTime::FindStr()

int CHighTime::ReplaceStr(LPSTR pszDest, int nDestSize, LPCTSTR pszMask, LPCTSTR pszSource)
{
    int nCount, iInsertLen, iReplaceLen, iMoveLen, iDestLen;
    LPSTR pSearch, pStartPos;

    nCount = 0;
    pStartPos = pszDest;
    while ( (pSearch = strstr(pStartPos, pszMask)) != NULL) {
        pStartPos = pSearch + 1;
        ++nCount;
    }

    if (nCount > 0) {

        iReplaceLen = strlen(pszMask);
        iInsertLen = strlen(pszSource);

        // Can destination take the target?
        if ((int)strlen(pszDest) + nCount*(iInsertLen - iReplaceLen) > nDestSize) 
            return 0; // no => do nothing

        iDestLen = strlen(pszDest);
        pStartPos = pszDest;
        while ( (pSearch = strstr(pStartPos, pszMask)) != NULL) {
           
            iMoveLen = iDestLen - (pSearch - pszDest);
            if (iReplaceLen != iInsertLen ) {
                memmove(pSearch + (iInsertLen - iReplaceLen), pSearch, iMoveLen);
                iDestLen += iInsertLen - iReplaceLen;
                pszDest[iDestLen] = '\0';
            }
            memcpy(pSearch, pszSource, iInsertLen);

            pStartPos = pSearch + 1;
        }
    }
    return nCount;
} // CHighTime::ReplaceStr()

// Formatting, Non MFC
LPCTSTR CHighTime::Format(LPSTR pBuffer, int iBufferLen, LPCTSTR pFormat) const
{
	_HighTimeFormat tmHighTimeTemp;
    tm tmTemp;
    LPSTR szTemp = NULL;
    int iPos;

	// If null, return empty string
	if (pBuffer == NULL || GetStatus() == null || iBufferLen <= 0)
		return NULL;

	// If invalid, return DateTime resource string
	if (GetStatus() == invalid || !ConvertLongLongToTime(m_liTime, tmHighTimeTemp))
	{
		strncpy(pBuffer, INVALID_DATETIME, iBufferLen);
		return pBuffer;
	}

    if (iBufferLen <= 10)
        szTemp = new CHAR[10];
    else
        szTemp = new CHAR[iBufferLen+1];
    // Add milli, micro & nano part!!!!!!
    //%s for the millisecond part
    //%u for the microsecond part
    //%n for the nanosecond part
    strncpy(pBuffer, pFormat, iBufferLen);

    iPos = FindStr(pBuffer, "%s", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",tmHighTimeTemp.nMilli);
        ReplaceStr(pBuffer, iBufferLen, "%s", szTemp);
    }
    iPos = FindStr(pBuffer, "%#s", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",tmHighTimeTemp.nMilli);
        ReplaceStr(pBuffer, iBufferLen, "%#s", szTemp);
    }

    iPos = FindStr(pBuffer, "%u", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",tmHighTimeTemp.nMicro);
        ReplaceStr(pBuffer, iBufferLen, "%u", szTemp);
    }
    iPos = FindStr(pBuffer, "%#u", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",tmHighTimeTemp.nMicro);
        ReplaceStr(pBuffer, iBufferLen, "%#u", szTemp);
    }

    iPos = FindStr(pBuffer, "%n", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",tmHighTimeTemp.nNano);
        ReplaceStr(pBuffer, iBufferLen, "%n", szTemp);
    }
    iPos = FindStr(pBuffer, "%#n", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",tmHighTimeTemp.nNano);
        ReplaceStr(pBuffer, iBufferLen, "%#n", szTemp);
    }

	// Convert tm from internal format to standard format
    ConvertToStandardFormat(tmHighTimeTemp, tmTemp);

	// Fill in the buffer, disregard return value as it's not necessary
    strcpy(szTemp, pBuffer);
	strftime(pBuffer, iBufferLen, szTemp, &tmTemp);

    delete [] szTemp;

	return pBuffer;
} // CHighTime::Format()

// formatting
#if defined(USE_MFC)
CString CHighTime::Format(LPCTSTR lpszFormat) const
{
    CString szParse(lpszFormat);
    CString szTemp,strDate;
	_HighTimeFormat tmHighTimeTemp;
    tm tmTemp;
    int iPos;

	// If null, return empty string
	if (GetStatus() == null)
		return strDate;

	// If invalid, return DateTime resource string
	if (GetStatus() == invalid || !ConvertLongLongToTime(m_liTime, tmHighTimeTemp))
	{
		VERIFY(strDate.LoadString(AFX_IDS_INVALID_DATETIME));
		return strDate;
	}

    // Add milli, micro & nano part!!!!!!
    //%s for the millisecond part
    //%u for the microsecond part
    //%n for the nanosecond part
    iPos = szParse.Find("%s", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",abs(tmHighTimeTemp.nMilli));
        szParse.Replace("%s",szTemp);
    }
    iPos = szParse.Find("%#s", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",abs(tmHighTimeTemp.nMilli));
        szParse.Replace("%#s",szTemp);
    }
    iPos = szParse.Find("%u", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",abs(tmHighTimeTemp.nMicro));
        szParse.Replace("%u",szTemp);
    }
    iPos = szParse.Find("%#u", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",abs(tmHighTimeTemp.nMicro));
        szParse.Replace("%#u",szTemp);
    }
    iPos = szParse.Find("%n", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",abs(tmHighTimeTemp.nNano));
        szParse.Replace("%n",szTemp);
    }
    iPos = szParse.Find("%#n", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",abs(tmHighTimeTemp.nNano));
        szParse.Replace("%#n",szTemp);
    }

    // Convert tm from internal format to standard format
    ConvertToStandardFormat(tmHighTimeTemp, tmTemp);

	// Fill in the buffer, disregard return value as it's not necessary
	strftime(strDate.GetBuffer(MAX_TIME_BUFFER_SIZE), MAX_TIME_BUFFER_SIZE, 
              szParse.GetBuffer(szParse.GetLength()), &tmTemp);
    szParse.ReleaseBuffer();
	strDate.ReleaseBuffer();

	return strDate;
} // CHighTime::Format()

CString CHighTime::Format(UINT nFormatID) const
{
	CString strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);
	return Format(strFormat);
} // CHighTime::Format()

// serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc, CHighTime dateSrc)
{
	dc << "\nCHighTime Object:";
	dc << "\n\tm_status = " << (long)dateSrc.m_status;
	dc << "\n\tdate = " << dateSrc.m_liTime.HighPart;
	return dc << "." << dateSrc.m_liTime.LowPart;
} //  operator<<(CDumpContext, CHighTime)
#endif // _DEBUG

CArchive& AFXAPI operator<<(CArchive& ar, CHighTime dateSrc)
{
	ar << (long)dateSrc.m_status;
	ar << dateSrc.m_liTime.HighPart;
	return ar << dateSrc.m_liTime.LowPart;
} // operator<<(CArchive, CHighTime)

CArchive& AFXAPI operator>>(CArchive& ar, CHighTime& dateSrc)
{
	ar >> (long&)dateSrc.m_status;
	ar >> dateSrc.m_liTime.HighPart;
	return ar >> dateSrc.m_liTime.LowPart;
} // operator>>(CArchive, CHighTime)
#endif // defined(USE_MFC)


// ******************************
// HighTimeSpan class
// ******************************
void CHighTimeSpan::SetHighTimeSpan(long lDays, int nHours, int nMinutes, int nSeconds,
                                    int nMillis, int nMicros, int nNanos)   
                                      // Milli, Micro & nano, default = 0
{

    int nHundredsNanos;
    
    if (nNanos >= 0)
        nHundredsNanos = (nNanos+50) / 100;
    else
        nHundredsNanos = (nNanos-50) / 100;

    nMicros += nHundredsNanos / 10;
    nHundredsNanos %= 10;
    nMillis += nMicros / 1000;
    nMicros %= 1000;
    nSeconds +=nMillis / 1000;
    nMillis %= 1000;
    nMinutes += nSeconds / 60;
    nSeconds %= 60;
    nHours += nMinutes / 60;
    nMinutes %= 60;
    lDays += nHours / 24;
    nHours %= 24;

    m_liSpan.QuadPart = lDays;
    m_liSpan.QuadPart *= 86400L;
    m_liSpan.QuadPart += (nHours * 3600L) +
                         (nMinutes * 60)  +
                         nSeconds;
    m_liSpan.QuadPart *= 10000000L;
    m_liSpan.QuadPart += (nMillis * 10000L) +
                         (nMicros * 10L) +
                         nHundredsNanos;

    SetStatus(valid);
} // CHighTimeSpan::SetHighTimeSpan()

LONGLONG CHighTimeSpan::GetTotalDays() const    // span in days (about -3.65e6 to 3.65e6)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp /= 86400L;
    return liTemp;
} // CHighTimeSpan::GetTotalDays()

LONGLONG CHighTimeSpan::GetTotalHours() const   // span in hours (about -8.77e7 to 8.77e6)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp /= 3600L;
    return liTemp;
} // CHighTimeSpan::GetTotalHours()

LONGLONG CHighTimeSpan::GetTotalMinutes() const // span in minutes (about -5.26e9 to 5.26e9)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp /= 60L;
    return liTemp;
} // CHighTimeSpan::GetTotalMinutes()

LONGLONG CHighTimeSpan::GetTotalSeconds() const // span in seconds (about -3.16e11 to 3.16e11)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    return liTemp;
} // CHighTimeSpan::GetTotalSeconds()

LONGLONG CHighTimeSpan::GetTotalMilliSeconds() const // span in milliseconds
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000L;
    return liTemp;
} // CHighTimeSpan::GetTotalMilliSeconds()

LONGLONG CHighTimeSpan::GetTotalMicroSeconds() const // span in microseconds
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10L;
    return liTemp;
} // CHighTimeSpan::GetTotalMicroSeconds()

LONGLONG CHighTimeSpan::GetTotalNanoSeconds() const // span in nanoseconds
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart * 100L;
    return liTemp;
} // CHighTimeSpan::GetTotalNanoSeconds()

int CHighTimeSpan::GetDays() const       // component days in span
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp = (liTemp / 86400L);
    return (int)liTemp;
} // CHighTimeSpan::GetDays()

int CHighTimeSpan::GetHours() const      // component hours in span (-23 to 23)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp = (liTemp % 86400L) / 3600;
    return (int)liTemp;
} // CHighTimeSpan::GetHours()

int CHighTimeSpan::GetMinutes() const    // component minutes in span (-59 to 59)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp = (liTemp % 3600L) / 60;
    return (int)liTemp;
} // CHighTimeSpan::GetMinutes()

int CHighTimeSpan::GetSeconds() const    // component seconds in span (-59 to 59)
{
    LONGLONG liTemp;
    liTemp = m_liSpan.QuadPart / 10000000L;
    liTemp = (liTemp % 60L);
    return (int)liTemp;
} // CHighTimeSpan::GetSeconds()

int CHighTimeSpan::GetMilliSeconds() const // component Milliseconds in span (-999 to 999)
{
    LONGLONG liTemp;
    liTemp = (m_liSpan.QuadPart % 10000000L) / 10000L;
    return (int)liTemp;
} // CHighTimeSpan::GetMilliSeconds()

int CHighTimeSpan::GetMicroSeconds() const // component Microseconds in span (-999 to 999)
{
    LONGLONG liTemp;
    liTemp = (m_liSpan.QuadPart % 10000L) / 10L;
    return (int)liTemp;
} // CHighTimeSpan::GetMicroSeconds()

int CHighTimeSpan::GetNanoSeconds() const  // component Nanoseconds in span (-900 to 900)
{
    LONGLONG liTemp;
    liTemp = (m_liSpan.QuadPart % 10) * 100L;
    return (int)liTemp;
} // CHighTimeSpan::GetNanoSeconds()


// CHighTimeSpan math
CHighTimeSpan CHighTimeSpan::operator+(const CHighTimeSpan& dateSpan) const
{
	CHighTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.m_liSpan.QuadPart = m_liSpan.QuadPart + dateSpan.m_liSpan.QuadPart;

	return dateSpanTemp;
} // CHighTimeSpan::operator+()

CHighTimeSpan CHighTimeSpan::operator-(const CHighTimeSpan& dateSpan) const
{
	CHighTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.m_liSpan.QuadPart = m_liSpan.QuadPart - dateSpan.m_liSpan.QuadPart;

	return dateSpanTemp;
} // CHighTimeSpan::operator-()

// formatting
LPSTR CHighTimeSpan::Format(LPSTR pBuffer, int iBufferLen, LPCTSTR pFormat) const
{
    CHighTime::_HighTimeFormat tmHighTimeTemp;
    tm tmTemp;
    int iPos;
    LPSTR szTemp;

    // If null, return empty string
	if (pFormat == NULL || GetStatus() == null || iBufferLen <= 0)
		return NULL;

    // If invalid, return DateTimeSpan resource string
    if (GetStatus() == invalid || !CHighTime::ConvertLongLongToTime(m_liSpan, tmHighTimeTemp))
	{
		strncpy(pBuffer, INVALID_DATETIME, iBufferLen);
		return pBuffer;
	}

    // Add milli, micro & nano part!!!!!!
    //%D for the Days part
    //%s for the millisecond part
    //%u for the microsecond part
    //%n for the nanosecond part
    if (iBufferLen <= 10)
        szTemp = new CHAR[10];
    else
        szTemp = new CHAR[iBufferLen+1];

    strncpy(pBuffer, pFormat, iBufferLen);

    iPos = CHighTime::FindStr(pBuffer, "%D", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%0d",LLABS(GetDays()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%D", szTemp);
    }
    iPos = CHighTime::FindStr(pBuffer, "%#D", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",LLABS(GetDays()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%#D", szTemp);
    }

    iPos = CHighTime::FindStr(pBuffer, "%s", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",LLABS(GetMilliSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%s", szTemp);
    }
    iPos = CHighTime::FindStr(pBuffer, "%#s", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",LLABS(GetMilliSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%#s", szTemp);
    }

    iPos = CHighTime::FindStr(pBuffer, "%u", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",LLABS(GetMicroSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%u", szTemp);
    }
    iPos = CHighTime::FindStr(pBuffer, "%#u", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",LLABS(GetMicroSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%#u", szTemp);
    }

    iPos = CHighTime::FindStr(pBuffer, "%n", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%03d",LLABS(GetNanoSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%n", szTemp);
    }
    iPos = CHighTime::FindStr(pBuffer, "%#n", 0);
    if (iPos >= 0) {
        wsprintf(szTemp,"%d",LLABS(GetNanoSeconds()));
        CHighTime::ReplaceStr(pBuffer, iBufferLen, "%#n", szTemp);
    }

	// Convert tm from internal format to standard format
	tmTemp.tm_year = tmHighTimeTemp.nYear-1601;   // year is based on 1900
	tmTemp.tm_mon  = tmHighTimeTemp.nMonth-1;     // month of year is 0-based
	tmTemp.tm_wday = tmHighTimeTemp.nDayOfWeek-1; // day of week is 0-based
	tmTemp.tm_yday = tmHighTimeTemp.nDayOfYear-1; // day of year is 0-based
    tmTemp.tm_mday = tmHighTimeTemp.nDay;
    tmTemp.tm_hour = tmHighTimeTemp.nHour;
    tmTemp.tm_min = tmHighTimeTemp.nMinute;
    tmTemp.tm_sec = tmHighTimeTemp.nSecond;
    tmTemp.tm_isdst = 0;

    // Fill in the buffer, disregard return value as it's not necessary
    strcpy(szTemp, pBuffer);
	strftime(pBuffer, iBufferLen, szTemp, &tmTemp);
    delete [] szTemp;

    return pBuffer;
} // CHighTimeSpan::Format()

#if defined(USE_MFC)
CString CHighTimeSpan::Format(LPCTSTR pFormat) const
{
    CString szParse(pFormat);
    CString szTemp, strSpan;
    CHighTime::_HighTimeFormat tmHighTimeTemp;
    tm tmTemp;
    int iPos;

    // If null, return empty string
	if (pFormat == NULL || GetStatus() == null)
		return strSpan;

    // If invalid, return DateTimeSpan resource string
    if (GetStatus() == invalid || !CHighTime::ConvertLongLongToTime(m_liSpan, tmHighTimeTemp))
	{
		VERIFY(strSpan.LoadString(AFX_IDS_INVALID_DATETIMESPAN));
		return strSpan;
	}

    // Add milli, micro & nano part!!!!!!
    //%D for the Days part
    //%s for the millisecond part
    //%u for the microsecond part
    //%n for the nanosecond part
    iPos = szParse.Find("%D", 0);
    if (iPos >= 0) {
        szTemp.Format("%0d",LLABS(GetDays()));
        szParse.Replace("%D",szTemp);
    }
    iPos = szParse.Find("%#D", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",LLABS(GetDays()));
        szParse.Replace("%#D",szTemp);
    }
    iPos = szParse.Find("%s", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",LLABS(GetMilliSeconds()));
        szParse.Replace("%s",szTemp);
    }
    iPos = szParse.Find("%#s", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",LLABS(GetMilliSeconds()));
        szParse.Replace("%#s",szTemp);
    }
    iPos = szParse.Find("%u", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",LLABS(GetMicroSeconds()));
        szParse.Replace("%u",szTemp);
    }
    iPos = szParse.Find("%#u", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",LLABS(GetMicroSeconds()));
        szParse.Replace("%#u",szTemp);
    }
    iPos = szParse.Find("%n", 0);
    if (iPos >= 0) {
        szTemp.Format("%03d",LLABS(GetNanoSeconds()));
        szParse.Replace("%n",szTemp);
    }
    iPos = szParse.Find("%#n", 0);
    if (iPos >= 0) {
        szTemp.Format("%d",LLABS(GetNanoSeconds()));
        szParse.Replace("%#n",szTemp);
    }

	// Convert tm from internal format to standard format
	tmTemp.tm_year = tmHighTimeTemp.nYear-1601;   // year is based on 1900
	tmTemp.tm_mon  = tmHighTimeTemp.nMonth-1;     // month of year is 0-based
	tmTemp.tm_wday = tmHighTimeTemp.nDayOfWeek-1; // day of week is 0-based
	tmTemp.tm_yday = tmHighTimeTemp.nDayOfYear-1; // day of year is 0-based
    tmTemp.tm_mday = tmHighTimeTemp.nDay;
    tmTemp.tm_hour = tmHighTimeTemp.nHour;
    tmTemp.tm_min = tmHighTimeTemp.nMinute;
    tmTemp.tm_sec = tmHighTimeTemp.nSecond;
    tmTemp.tm_isdst = 0;
    
    // Fill in the buffer, disregard return value as it's not necessary
	strftime(strSpan.GetBuffer(MAX_TIME_BUFFER_SIZE), MAX_TIME_BUFFER_SIZE, 
              szParse.GetBuffer(szParse.GetLength()), &tmTemp);
    szParse.ReleaseBuffer();
	strSpan.ReleaseBuffer();

    return strSpan;
} // CHighTimeSpan::Format()

CString CHighTimeSpan::Format(UINT nID) const
{
    CString sBuffer;
    sBuffer.LoadString(nID);
    Format(sBuffer);
    return sBuffer;
} // CHighTimeSpan::Format()

// serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc, CHighTimeSpan dateSpanSrc)
{
	dc << "\nCHighTimeSpan Object:";
	dc << "\n\tm_status = " << (long)dateSpanSrc.m_status;
	dc << "\n\tdateSpan = " << dateSpanSrc.m_liSpan.HighPart;
	return dc << "." << dateSpanSrc.m_liSpan.LowPart;
} //  operator<<(CDumpContext, CHighTimeSpan)
#endif // _DEBUG

CArchive& AFXAPI operator<<(CArchive& ar, CHighTimeSpan dateSpanSrc)
{
	ar << (long)dateSpanSrc.m_status;
	ar << dateSpanSrc.m_liSpan.HighPart;
	return ar << dateSpanSrc.m_liSpan.LowPart;
} // operator<<(CArchive, CHighTimeSpan)

CArchive& AFXAPI operator>>(CArchive& ar, CHighTimeSpan& dateSpanSrc)
{
	ar >> (long&)dateSpanSrc.m_status;
	ar >> dateSpanSrc.m_liSpan.HighPart;
	return ar >> dateSpanSrc.m_liSpan.LowPart;
} // operator>>(CArchive, CHighTimeSpan)

#endif // defined(USE_MFC)