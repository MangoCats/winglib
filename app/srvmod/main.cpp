
#include "stdafx.h"
#include "stdio.h"
#include "string.h"

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Start );
extern "C" oex::oexRESULT SRV_Start( oex::os::SRawAllocator x_sRawAllocator, oex::oexCSTR x_pPath, oex::oexCSTR x_pCommandLine, oex::oexINT x_nCommandLine, oex::oexCPVOID x_pData )
{
	// Set our allocator
	oex::os::CMem::SetRawAllocator( x_sRawAllocator );

	if ( oexCHECK_PTR( x_pPath ) && *x_pPath )
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oex::CStr( x_pPath ).GetFileName().Ptr(), oexT( ".module.debug.log" ) );
	else
		oex::CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".module.debug.log" ) );

	// Start a log file
	oexNOTICE( 0, oexT( "Module startup" ) );

	return 0;
}

extern "C" oexDECLARE_SRV_FUNCTION( SRV_Stop );
extern "C" oex::oexRESULT SRV_Stop()
{
	// Switch back to default allocator
	oex::os::CMem::SetDefaultRawAllocator();

	return 0;
}


extern "C" oexDECLARE_SRV_FUNCTION( SRV_GetModuleInfo );
extern "C" oex::oexRESULT SRV_GetModuleInfo( oex::os::service::SSrvInfo *pDi )
{
	if ( !oexCHECK_PTR( pDi ) )
	{	oexERROR( 0, oexT( "Invalid function argument" ) );
		return -1;
	} // end if

	// Clear structure
	oexZeroMemory( pDi, sizeof( oex::os::service::SSrvInfo ) );

	// Module name
	strncpy( pDi->szName, "Test Module", sizeof( pDi->szName ) - 1 );

	// Module description
	strncpy( pDi->szDesc, "This is just a test module", sizeof( pDi->szDesc ) - 1 );

	// pDi->guidType = ;

	// pDi->guidId = ;

	// Create random guid
	oexUniqueGuid( &pDi->guidInstance );

	// Set version
	pDi->lVer = oexVERSION( 1, 0 );

	return 0;
}


