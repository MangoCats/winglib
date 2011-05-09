// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CPaOutput, CPaOutput )

	SQBIND_MEMBER_FUNCTION( CPaOutput, Init )
	SQBIND_MEMBER_FUNCTION( CPaOutput, Destroy )
	SQBIND_MEMBER_FUNCTION( CPaOutput, Open )
	SQBIND_MEMBER_FUNCTION( CPaOutput, Start )
	SQBIND_MEMBER_FUNCTION( CPaOutput, Stop )
	SQBIND_MEMBER_FUNCTION( CPaOutput, Write )
	SQBIND_MEMBER_FUNCTION( CPaOutput, WriteTs )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getTs )

	SQBIND_MEMBER_FUNCTION( CPaOutput, getInitError )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getLastError )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getDefaultOutputDevice )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getDeviceCount )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getDeviceInfo )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getBufferedBytes )
	SQBIND_MEMBER_FUNCTION( CPaOutput, getFrameBytes )
	SQBIND_MEMBER_FUNCTION( CPaOutput, isOpen )
	
	SQBIND_GLOBALCONST( paFloat32 )
	SQBIND_GLOBALCONST( paInt16 )
	SQBIND_GLOBALCONST( paInt32 )
	SQBIND_GLOBALCONST( paInt24 )
	SQBIND_GLOBALCONST( paInt8 )
	SQBIND_GLOBALCONST( paUInt8 )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CPaOutput );

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CPaInput, CPaInput )

	SQBIND_MEMBER_FUNCTION( CPaInput, Init )
	SQBIND_MEMBER_FUNCTION( CPaInput, Destroy )
	SQBIND_MEMBER_FUNCTION( CPaInput, Open )
	SQBIND_MEMBER_FUNCTION( CPaInput, Start )
	SQBIND_MEMBER_FUNCTION( CPaInput, Stop )
	SQBIND_MEMBER_FUNCTION( CPaInput, Read )

	SQBIND_MEMBER_FUNCTION( CPaInput, getInitError )
	SQBIND_MEMBER_FUNCTION( CPaInput, getLastError )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDefaultInputDevice )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDeviceCount )
	SQBIND_MEMBER_FUNCTION( CPaInput, getDeviceInfo )
	SQBIND_MEMBER_FUNCTION( CPaInput, getBufferedBytes )
	SQBIND_MEMBER_FUNCTION( CPaInput, getFrameBytes )
	SQBIND_MEMBER_FUNCTION( CPaInput, isOpen )
	
	SQBIND_GLOBALCONST( paFloat32 )
	SQBIND_GLOBALCONST( paInt16 )
	SQBIND_GLOBALCONST( paInt32 )
	SQBIND_GLOBALCONST( paInt24 )
	SQBIND_GLOBALCONST( paInt8 )
	SQBIND_GLOBALCONST( paUInt8 )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CPaInput );

// Export classes
static void SQBIND_Export_portaudio( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

	SQBIND_EXPORT( x_vm, CPaInput );
	SQBIND_EXPORT( x_vm, CPaOutput );
}

#if defined( SQBIND_STATIC )
	#include "pa_input.cpp"
	#include "pa_output.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_portaudio( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

