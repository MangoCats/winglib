/*------------------------------------------------------------------
// oexlib_debugging.h
// Copyright (c) 2006 
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted for commercial and 
// non-commercial purposes, provided that the following 
// conditions are met:
//
// * Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to 
//   endorse or promote products derived from this software without 
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#pragma once

// No reason to switch this off, there is no runtime hit
// http://www.codeguru.com/forum/showthread.php?t=404495
// Thanks Jeff ;)
template < const int T > class oex_static_assert{};
#define oexSTATIC_ASSERT( s )										\
	typedef oex_static_assert< sizeof( int[ ( s ) ? 1 : -1 ] ) >	\
		oex_typedef_static_assert;

#ifdef _DEBUG

#	define oexASSERT( s )		( s )
#	define oexVERIFY( s )		( s )
#	define oexTRACE			
#	define oexUNUSED( s )

// 0xABABABAB	-	LocalAlloc()
// 0xBAADF00D	-	LocalAlloc( LMEM_FIXED )
// 0xCCCCCCCC	-	Uninitialized stack memory
// 0xCDCDCDCD	-	Uninitialized heap memory ( allocated with new )
// 0xDDDDDDDD	-	Released heap memory ( after delete )
// 0xFDFDFDFD	-	Debugging gaurd bytes for heap memory ( buffer overrun? )
// 0xFEEEFEEE	-	HeapFree()
// 0xDEADDEAD	-	Windows Stop error
// 0xDEADC0DE	-	Linux / Unix
// 0xDEADBEEF	-	Linux / Unix
// 0xDEADBABE	-	Linux / Unix
// 0xA5A5A5A5	-	Embedded
#	define oexVALIDATE_PTR_NULL_OK( ptr )									\
				oexASSERT( (		( (oexTYPEOF_PTR)ptr != 0xABABABAB ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xBAADF00D ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xCCCCCCCC ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xCDCDCDCD ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xDDDDDDDD ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xFDFDFDFD ) &&	\
									( (oexTYPEOF_PTR)ptr != 0xFEEEFEEE ) && \
									( (oexTYPEOF_PTR)ptr != 0xDEADDEAD ) && \
									( (oexTYPEOF_PTR)ptr != 0xDEADC0DE ) && \
									( (oexTYPEOF_PTR)ptr != 0xDEADBEEF ) && \
									( (oexTYPEOF_PTR)ptr != 0xDEADBABE ) && \
									( (oexTYPEOF_PTR)ptr != 0xA5A5A5A5 ) ) )

#	define oexVALIDATE_PTR( ptr )											\
				(	oexASSERT( ( (oexTYPEOF_PTR)ptr != NULL ) ) &&			\
					oexVALIDATE_PTR_NULL_OK( ptr ) )

#else

#	define oexASSERT( s )
#	define oexVERIFY( s )		( s )
#	define oexTRACE
#	define oexVALIDATE_PTR( ptr )
#	define oexVALIDATE_PTR_NULL_OK( ptr )
#	define oexUNUSED( s )

#endif