/*------------------------------------------------------------------
// oexlib_warnings.h
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

#ifdef OEXLIB_VERBOSE_COMPILE

//#if ( WINVER < 0x401 )
//#	pragma message( "OEXLIB: WARNING! You are compiling for a version of NT before 4.0." )
//#	pragma message( "OEXLIB: To get rid of this warning, define WINVER=0x401 or greater." )
//#endif // WINVER

#	ifdef WIN32_LEAN_AND_MEAN
#		pragma message( "OEXLIB: WIN32_LEAN_AND_MEAN is defined" )
#	endif

#	ifdef VC_EXTRALEAN
#		pragma message( "OEXLIB: VC_EXTRALEAN is defined" )
#	endif

#endif

// Disable Hidden variable warning
#ifdef __INTEL_COMPILER
#	pragma warning ( disable : 1125 ) 
#endif

// Disable VC6 truncated identifier warning
#if defined( _MSC_VER ) && ( _MSC_VER <= 1200 )
#	pragma warning( disable : 4786 )
#endif
