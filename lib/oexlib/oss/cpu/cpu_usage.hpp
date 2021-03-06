

#include "cpu_usage.h"

#pragma pack(push,8)
#include "perf_counters.h"
#pragma pack(pop)

#define CPU_WEIGHT							0

#define SYSTEM_OBJECT_INDEX					2		// 'System' object
#define PROCESS_OBJECT_INDEX				230		// 'Process' object
#define PROCESSOR_OBJECT_INDEX				238		// 'Processor' object
#define TOTAL_PROCESSOR_TIME_COUNTER_INDEX	240		// '% Total processor time' counter (valid in WinNT under 'System' object)
#define PROCESSOR_TIME_COUNTER_INDEX		6		// '% processor time' counter (for Win2K/XP)

///////////////////////////////////////////////////////////////////
//
//		GetCpuUsage uses the performance counters to retrieve the
//		system cpu usage.
//		The cpu usage counter is of type PERF_100NSEC_TIMER_INV
//		which as the following calculation:
//
//		Element		Value
//		=======		===========
//		X			CounterData
//		Y			100NsTime
//		Data Size	8 Bytes
//		Time base	100Ns
//		Calculation 100*(1-(X1-X0)/(Y1-Y0))
//
//      where the denominator (Y) represents the total elapsed time of the
//      sample interval and the numerator (X) represents the time during
//      the interval when the monitored components were inactive.
//
//
//		Note:
//		====
//		On windows NT, cpu usage counter is '% Total processor time'
//		under 'System' object. However, in Win2K/XP Microsoft moved
//		that counter to '% processor time' under '_Total' instance
//		of 'Processor' object.
//		Read 'INFO: Percent Total Performance Counter Changes on Windows 2000'
//		Q259390 in MSDN.
//
///////////////////////////////////////////////////////////////////

typedef enum
{
	PF_WINNT, PF_WIN2K_XP, PF_WIN9X, PF_UNKNOWN
}PLATFORM;

PLATFORM GetPlatform()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&osvi))
		return PF_UNKNOWN;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		return PF_WIN9X;
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion == 4)
			return PF_WINNT;
		else
			return PF_WIN2K_XP;
	}
	return PF_UNKNOWN;
}

CCpuUsage::CCpuUsage()
{
	m_lAvg = 0;
	m_bFirstTime = true;
	m_lnOldValue = 0;
	memset(&m_OldPerfTime100nSec, 0, sizeof(m_OldPerfTime100nSec));

	// Call once to initialize
	GetCpuUsage();
}

CCpuUsage::~CCpuUsage()
{
}

BOOL CCpuUsage::EnablePerformaceCounters(BOOL bEnable)
{
	if (GetPlatform() != PF_WIN2K_XP)
		return TRUE;

	HKEY hKey = NULL;
	LONG lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, oexT( "SYSTEM\\CurrentControlSet\\Services\\PerfOS\\Performance" ),
								0, NULL, 0, KEY_WRITE | KEY_SET_VALUE, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS == lRes )
	{	DWORD dwEnable = bEnable ? 0 : 1;
		RegSetValueEx( hKey, oexT( "Disable Performance Counters" ), 0, REG_DWORD, (LPBYTE)&dwEnable, sizeof( dwEnable ) );
		RegCloseKey( hKey );
	} // end else

	hKey = NULL;
	lRes = RegCreateKeyEx( HKEY_LOCAL_MACHINE, oexT( "SYSTEM\\CurrentControlSet\\Services\\PerfProc\\Performance" ),
								0, NULL, 0, KEY_WRITE | KEY_SET_VALUE, NULL, &hKey, NULL );
	if ( ERROR_SUCCESS == lRes )
	{	DWORD dwEnable = bEnable ? 0 : 1;
		RegSetValueEx( hKey, oexT( "Disable Performance Counters" ), 0, REG_DWORD, (LPBYTE)&dwEnable, sizeof( dwEnable ) );
		RegCloseKey( hKey );
	} // end else

	return TRUE;
}

//
//	GetCpuUsage returns the system-wide cpu usage.
//	Since we calculate the cpu usage by two samplings, the first
//	call to GetCpuUsage() returns 0 and keeps the values for the next
//	sampling.
//  Read the comment at the beginning of this file for the formula.
//
int CCpuUsage::GetCpuUsage()
{
	static PLATFORM Platform = GetPlatform();

	if (m_bFirstTime)
		EnablePerformaceCounters();

	// Cpu usage counter is 8 byte length.
	CPerfCounters<LONGLONG> PerfCounters;
	oex::CStr sInstance;

//		Note:
//		====
//		On windows NT, cpu usage counter is '% Total processor time'
//		under 'System' object. However, in Win2K/XP Microsoft moved
//		that counter to '% processor time' under '_Total' instance
//		of 'Processor' object.
//		Read 'INFO: Percent Total Performance Counter Changes on Windows 2000'
//		Q259390 in MSDN.

	DWORD dwObjectIndex;
	DWORD dwCpuUsageIndex;
	switch (Platform)
	{
	case PF_WINNT:
		dwObjectIndex = SYSTEM_OBJECT_INDEX;
		dwCpuUsageIndex = TOTAL_PROCESSOR_TIME_COUNTER_INDEX;
		break;
	case PF_WIN2K_XP:
		dwObjectIndex = PROCESSOR_OBJECT_INDEX;
		dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;
		sInstance = oexT( "_Total" );
		break;
	default:
		return -1;
	}

	int				CpuUsage = 0;
	LONGLONG		lnNewValue = 0;
	PPERF_DATA_BLOCK pPerfData = NULL;
	LARGE_INTEGER	NewPerfTime100nSec;
	NewPerfTime100nSec.QuadPart = 0;

	lnNewValue = PerfCounters.GetCounterValue(&pPerfData, dwObjectIndex, dwCpuUsageIndex, sInstance.Ptr());
	NewPerfTime100nSec = pPerfData->PerfTime100nSec;

	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		m_lnOldValue = lnNewValue;
		m_OldPerfTime100nSec = NewPerfTime100nSec;
		return 0;
	}

	LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
	double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

	m_lnOldValue = lnNewValue;
	m_OldPerfTime100nSec = NewPerfTime100nSec;

	double a = (double)lnValueDelta / DeltaPerfTime100nSec;

	double f = (1.0 - a) * 100.0;
	CpuUsage = (int)(f + 0.5);	// rounding the result
	if (CpuUsage < 0)
		return 0;

	// Calculate average
	if ( m_lAvg == 0 ) m_lAvg = CpuUsage;
	else m_lAvg = ( ( m_lAvg * CPU_WEIGHT ) + CpuUsage ) / ( CPU_WEIGHT + 1 );

	return m_lAvg;
}

int CCpuUsage::GetCpuUsage(LPCTSTR pProcessName)
{
	if (m_bFirstTime)
		EnablePerformaceCounters();

	// Cpu usage counter is 8 byte length.
	CPerfCounters<LONGLONG> PerfCounters;
	oex::CStr sInstance;

	DWORD dwObjectIndex = PROCESS_OBJECT_INDEX;
	DWORD dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;
	sInstance = pProcessName;

	int				CpuUsage = 0;
	LONGLONG		lnNewValue = 0;
	PPERF_DATA_BLOCK pPerfData = NULL;
	LARGE_INTEGER	NewPerfTime100nSec;
	NewPerfTime100nSec.QuadPart = 0;

	lnNewValue = PerfCounters.GetCounterValue(&pPerfData, dwObjectIndex, dwCpuUsageIndex, sInstance.Ptr());
	NewPerfTime100nSec = pPerfData->PerfTime100nSec;

	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		m_lnOldValue = lnNewValue;
		m_OldPerfTime100nSec = NewPerfTime100nSec;
		return 0;
	}

	LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
	double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

	m_lnOldValue = lnNewValue;
	m_OldPerfTime100nSec = NewPerfTime100nSec;

	double a = (double)lnValueDelta / DeltaPerfTime100nSec;

	CpuUsage = (int) (a*100);
	if (CpuUsage < 0)
		return 0;

#if ( 0 < CPU_WEIGHT )

	// Calculate average
	if ( m_lAvg == 0 )
		m_lAvg = CpuUsage;
	else
		m_lAvg = ( ( m_lAvg * CPU_WEIGHT ) + CpuUsage ) / ( CPU_WEIGHT + 1 );

#else
	m_lAvg = CpuUsage;
#endif

	return m_lAvg;
}


