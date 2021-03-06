/*------------------------------------------------------------------
// base_file.cpp
//
// Copyright (c) 1997
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

#include "oexlib.h"
#include "std_os.h"

#if defined( OEX_MSC )
#	include <ShlObj.h>
#	include <Shellapi.h>
#else
#	define OEX_USE_DYNAMIC_SHELL32

#define CSIDL_ALTSTARTUP              29
#define CSIDL_APPDATA                 26
#define CSIDL_BITBUCKET               10
#define CSIDL_CDBURN_AREA             59
#define CSIDL_COMMON_ADMINTOOLS       47
#define CSIDL_COMMON_ALTSTARTUP       30
#define CSIDL_COMMON_APPDATA          35
#define CSIDL_COMMON_DESKTOPDIRECTORY 25
#define CSIDL_COMMON_DOCUMENTS        46
#define CSIDL_COMMON_FAVORITES        31
#define CSIDL_COMMON_MUSIC            53
#define CSIDL_COMMON_PICTURES         54
#define CSIDL_COMMON_PROGRAMS         23
#define CSIDL_COMMON_STARTMENU        22
#define CSIDL_COMMON_STARTUP          24
#define CSIDL_COMMON_TEMPLATES        45
#define CSIDL_COMMON_VIDEO            55
#define CSIDL_CONTROLS                 3
#define CSIDL_COOKIES                 33
#define CSIDL_DESKTOP                  0
#define CSIDL_DESKTOPDIRECTORY        16
#define CSIDL_DRIVES                  17
#define CSIDL_FAVORITES                6
#define CSIDL_FONTS                   20
#define CSIDL_HISTORY                 34
#define CSIDL_INTERNET                 1
#define CSIDL_INTERNET_CACHE          32
#define CSIDL_LOCAL_APPDATA           28
#define CSIDL_MYDOCUMENTS             12
#define CSIDL_MYMUSIC                 13
#define CSIDL_MYPICTURES              39
#define CSIDL_MYVIDEO                 14
#define CSIDL_NETHOOD                 19
#define CSIDL_NETWORK                 18
#define CSIDL_PERSONAL                 5
#define CSIDL_PRINTERS                 4
#define CSIDL_PRINTHOOD               27
#define CSIDL_PROFILE                 40
#define CSIDL_PROFILES                62
#define CSIDL_PROGRAM_FILES           38
#define CSIDL_PROGRAM_FILES_COMMON    43
#define CSIDL_PROGRAMS                 2
#define CSIDL_RECENT                   8
#define CSIDL_SENDTO                   9
#define CSIDL_STARTMENU               11
#define CSIDL_STARTUP                  7
#define CSIDL_SYSTEM                  37
#define CSIDL_TEMPLATES               21
#define CSIDL_WINDOWS                 36

#endif

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFILE ) == sizeof( HANDLE ) );
oexSTATIC_ASSERT( sizeof( CBaseFile::t_HFIND ) == sizeof( HANDLE ) );

const CBaseFile::t_HFILE CBaseFile::c_Invalid = INVALID_HANDLE_VALUE;
const CBaseFile::t_HFIND CBaseFile::c_InvalidFindHandle = INVALID_HANDLE_VALUE;

oexBOOL CBaseFile::InitFileSystem()
{//_STT();
	return oexTRUE;
}

oexBOOL CBaseFile::FreeFileSystem()
{//_STT();
	return oexTRUE;
}

CBaseFile::t_HFILE CBaseFile::Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexINT *x_pnError )
{_STT();
	DWORD dwDisposition = OPEN_EXISTING;
	switch( x_eDisposition )
	{   case eDisCreateNew : dwDisposition = CREATE_NEW; break;
		case eDisCreateAlways : dwDisposition = CREATE_ALWAYS; break;
		case eDisOpenExisting : dwDisposition = OPEN_EXISTING; break;
		case eDisOpenAlways : dwDisposition = OPEN_ALWAYS; break;
	} // end switch

	DWORD dwAccess = 0;
	if ( 0 != ( x_eAccess & eAccessRead ) ) dwAccess |= GENERIC_READ;
	if ( 0 != ( x_eAccess & eAccessWrite ) ) dwAccess |= GENERIC_WRITE;
	if ( 0 != ( x_eAccess & eAccessExecute ) ) dwAccess |= GENERIC_EXECUTE;

	DWORD dwShare = 0;
	if ( 0 != ( x_eShare & eShareDelete ) ) dwShare |= FILE_SHARE_DELETE;
	if ( 0 != ( x_eShare & eShareRead ) ) dwShare |= FILE_SHARE_READ;
	if ( 0 != ( x_eShare & eShareWrite ) ) dwShare |= FILE_SHARE_WRITE;

	// Allow access
	SECURITY_ATTRIBUTES  sa, *pSa = NULL;
	sa.nLength = sizeof( SECURITY_ATTRIBUTES );
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = (SECURITY_DESCRIPTOR*)LocalAlloc(LPTR,SECURITY_DESCRIPTOR_MIN_LENGTH);
	if ( sa.lpSecurityDescriptor
		 && InitializeSecurityDescriptor( (SECURITY_DESCRIPTOR*)sa.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION )
		 && SetSecurityDescriptorDacl( (SECURITY_DESCRIPTOR*)sa.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE ) )
		pSa = &sa;

	t_HFILE hFile = ::CreateFile( x_pFile, dwAccess, dwShare, pSa, dwDisposition, x_uFlags, NULL );

	if ( pSa )
		LocalFree( pSa->lpSecurityDescriptor );

	if ( x_pnError )
	{	if ( INVALID_HANDLE_VALUE == hFile )
			*x_pnError = GetLastError();
		else *x_pnError = 0;
	} // end if
	
	// Was there an error
	if ( INVALID_HANDLE_VALUE == hFile )
		return hFile;

	// Truncate file
	if ( eDisCreateAlways == x_eDisposition  || eDisCreateNew == x_eDisposition )
	{	SetPointer( hFile, 0, os::CBaseFile::eFileOffsetBegin );
		::SetEndOfFile( hFile );
	} // end if

	return hFile;
}


oexBOOL CBaseFile::Close( CBaseFile::t_HFILE x_hFile, oexINT *x_pnErr )
{_STT();
	if ( INVALID_HANDLE_VALUE == x_hFile )
		return oexFALSE;

	oexBOOL bRet = ::CloseHandle( x_hFile ) ? oexTRUE : oexFALSE;

	if ( x_pnErr )
	{   if ( bRet ) *x_pnErr = 0;
		else *x_pnErr = GetLastError();
	} // end if

	return bRet;
}


oexBOOL CBaseFile::Write( CBaseFile::t_HFILE x_hFile, oexCPVOID x_pData, oexINT64 x_llSize, oexINT64 *x_pllWritten, oexINT *x_pnErr )
{_STT();
	DWORD dwWritten = 0;
	oexBOOL bRet = ::WriteFile( x_hFile, x_pData, x_llSize, &dwWritten, NULL ) ? oexTRUE : oexFALSE;

	if ( x_pllWritten )
		*x_pllWritten = dwWritten;

	if ( x_pnErr )
	{   if ( bRet ) *x_pnErr = 0;
		else *x_pnErr = GetLastError();
	} // end if

	return bRet;
}

oexBOOL CBaseFile::Read( CBaseFile::t_HFILE x_hFile, oexPVOID x_pData, oexINT64 x_llSize, oexINT64 *x_pllRead, oexINT *x_pnErr )
{_STT();
	DWORD dwRead = 0;
	oexBOOL bRet = ::ReadFile( x_hFile, x_pData, x_llSize, &dwRead, NULL ) ? oexTRUE : oexFALSE;

	if ( x_pllRead )
		*x_pllRead = dwRead;

	else if ( dwRead != x_llSize )
		bRet = FALSE;

	if ( x_pnErr )
	{   if ( bRet ) *x_pnErr = 0;
		else *x_pnErr = GetLastError();
	} // end if

	return bRet;
}

oexBOOL CBaseFile::Flush( t_HFILE x_hFile )
{_STT();
	return ::FlushFileBuffers( x_hFile );
}

#define CBaseFile_FT2INT64( ft ) ( (oexINT64)ft.dwLowDateTime | ( (oexINT64)ft.dwHighDateTime << 32 ) )
typedef struct { LPCTSTR pName; DWORD dwFlag; } CBaseFile_ATTDESC;

CPropertyBag CBaseFile::GetFileInfo( oexCSTR x_pFile )
{_STT();

	CPropertyBag pb;
	if ( !x_pFile || !*x_pFile )
		return pb;

	// Save path
	pb[ "full" ] = x_pFile;

	// Get file info
    WIN32_FILE_ATTRIBUTE_DATA   wfad; oexZero( wfad );	
	if ( !GetFileAttributesEx( x_pFile, GetFileExInfoStandard, (LPVOID)&wfad ) )
		return pb;

	// Save file info into structure
	pb[ "os_flags" ] = wfad.dwFileAttributes;
	
	static CBaseFile_ATTDESC ad[] = 
	{
		{ "archive",		FILE_ATTRIBUTE_ARCHIVE },
		{ "compressed",		FILE_ATTRIBUTE_COMPRESSED },
		{ "device",			FILE_ATTRIBUTE_DEVICE },
		{ "directory",		FILE_ATTRIBUTE_DIRECTORY },
		{ "encrypted",		FILE_ATTRIBUTE_ENCRYPTED },
		{ "hidden",			FILE_ATTRIBUTE_HIDDEN },
		{ "normal",			FILE_ATTRIBUTE_NORMAL },
		{ "not_indexed",	FILE_ATTRIBUTE_NOT_CONTENT_INDEXED },
		{ "offline",		FILE_ATTRIBUTE_OFFLINE },
		{ "readonly",		FILE_ATTRIBUTE_READONLY },
		{ "reparse_point",	FILE_ATTRIBUTE_REPARSE_POINT },
		{ "sparse_file",	FILE_ATTRIBUTE_SPARSE_FILE },
		{ "system",			FILE_ATTRIBUTE_SYSTEM },
		{ "temporary",		FILE_ATTRIBUTE_TEMPORARY },
//		{ "virtual",		FILE_ATTRIBUTE_VIRTUAL },
		{ 0, 0 }
	};

	for ( oexINT i = 0; ad[ i ].pName; i++ )
		if ( 0 != ( ad[ i ].dwFlag & wfad.dwFileAttributes ) )		 
			pb[ ad[ i ].pName ] = oexT( "1" );

	pb[ "ft_created" ] = CBaseFile_FT2INT64( wfad.ftCreationTime );
	pb[ "ft_last_access" ] = CBaseFile_FT2INT64( wfad.ftLastAccessTime );
	pb[ "ft_last_modified" ] = CBaseFile_FT2INT64( wfad.ftLastWriteTime );
	pb[ "size" ] = (oexINT64)wfad.nFileSizeLow | ( (oexINT64)wfad.nFileSizeHigh << 32 );

	return pb;
}

oexINT64 CBaseFile::GetFileSize( oexCSTR x_pFile )
{
	if ( !x_pFile || !*x_pFile )
		return 0;

	// Get file info
    WIN32_FILE_ATTRIBUTE_DATA   wfad; oexZero( wfad );	
	if ( !GetFileAttributesEx( x_pFile, GetFileExInfoStandard, (LPVOID)&wfad ) )
		return 0;

	// Return the size
	return (oexINT64)wfad.nFileSizeLow | ( (oexINT64)wfad.nFileSizeHigh << 32 );
}

oexINT64 CBaseFile::Size( t_HFILE hFile )
{_STT();

	// Ensure valid handle
	if ( c_Invalid == hFile ) 
		return 0;
	// Read the file size
	DWORD dwHi = 0, dwLo = ::GetFileSize( hFile, &dwHi );

	// Check for error
	if ( INVALID_FILE_SIZE == dwLo && GetLastError() )
		return 0;
	
	// Return the file size
	return (oexINT64)dwLo | ( (oexINT64)dwHi << 32 );
}

oexINT64 CBaseFile::SetPointer( t_HFILE hFile, oexINT64 llMove, oexINT nMethod )
{_STT();
	// Get method
	DWORD dwMethod = 0;
	if ( nMethod == eFileOffsetBegin ) dwMethod = FILE_BEGIN;
	else if ( nMethod == eFileOffsetCurrent ) dwMethod = FILE_CURRENT;
	else if ( nMethod == eFileOffsetEnd ) dwMethod = FILE_END;
	else return -1;

	// Do the move
	SetLastError( NO_ERROR );
	LONG lHi = (DWORD)( ( llMove >> 32 ) & 0xffffffff );
	DWORD dwRet = ::SetFilePointer( hFile, (LONG)( llMove & 0xffffffff ), &lHi, dwMethod );

	// Check for error
	if ( dwRet == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR )
		return 0;

	// Return the result
	return (oexINT64)dwRet | ( (oexINT64)lHi << 32 );
}

oexBOOL CBaseFile::Delete( oexCSTR x_pFile )
{_STT();
	::SetFileAttributes( x_pFile, FILE_ATTRIBUTE_NORMAL );
	return ::DeleteFile( x_pFile ) ? oexTRUE : oexFALSE;
}

oexBOOL CBaseFile::RemoveFolder( oexCSTR x_pDir )
{_STT();
	::SetFileAttributes( x_pDir, FILE_ATTRIBUTE_NORMAL );
	return ::RemoveDirectory( x_pDir ) ? oexTRUE : oexFALSE;
}

static oexUINT g_ConvBaseFileAttribToWinAttrib[] =
{
	CBaseFile::eFileAttribReadOnly,             FILE_ATTRIBUTE_READONLY,
	CBaseFile::eFileAttribHidden,               FILE_ATTRIBUTE_HIDDEN,
	CBaseFile::eFileAttribSystem,               FILE_ATTRIBUTE_SYSTEM,
	CBaseFile::eFileAttribDirectory,            FILE_ATTRIBUTE_DIRECTORY,
	CBaseFile::eFileAttribArchive,              FILE_ATTRIBUTE_ARCHIVE,
	CBaseFile::eFileAttribDevice,               FILE_ATTRIBUTE_DEVICE,
	CBaseFile::eFileAttribNormal,               FILE_ATTRIBUTE_NORMAL,
	CBaseFile::eFileAttribTemporary,            FILE_ATTRIBUTE_TEMPORARY,
	CBaseFile::eFileAttribSparseFile,           FILE_ATTRIBUTE_SPARSE_FILE,
	CBaseFile::eFileAttribReparsePoint,         FILE_ATTRIBUTE_REPARSE_POINT,
	CBaseFile::eFileAttribCompressed,           FILE_ATTRIBUTE_COMPRESSED,
	CBaseFile::eFileAttribOffline,              FILE_ATTRIBUTE_OFFLINE,
	CBaseFile::eFileAttribNotContentIndexed,    FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
	CBaseFile::eFileAttribEncrypted,            FILE_ATTRIBUTE_ENCRYPTED,
	(oexUINT)-1, (oexUINT)-1
};

static DWORD CBaseFile_ToFileAttributes( oexUINT uAttrib )
{_STT();
	DWORD dwAttrib = 0;
	for( oexUINT i = 0; (DWORD)-1 != g_ConvBaseFileAttribToWinAttrib[ i ]
						&& (DWORD)-1 != g_ConvBaseFileAttribToWinAttrib[ i + 1 ];
						i += 2 )
	if ( 0 != ( uAttrib & g_ConvBaseFileAttribToWinAttrib[ i ] ) )
		dwAttrib |= g_ConvBaseFileAttribToWinAttrib[ i + 1 ];

	return dwAttrib;
}

static oexUINT CBaseFile_ToCBaseFileAttributes( DWORD dwAttrib )
{_STT();
	DWORD uAttrib = 0;
	for( oexUINT i = 0; (DWORD)-1 != g_ConvBaseFileAttribToWinAttrib[ i ]
						&& (DWORD)-1 != g_ConvBaseFileAttribToWinAttrib[ i + 1 ];
						i += 2 )
	if ( 0 != ( dwAttrib & g_ConvBaseFileAttribToWinAttrib[ i + 1 ] ) )
		uAttrib |= g_ConvBaseFileAttribToWinAttrib[ i ];

	return uAttrib;
}

oexUINT CBaseFile::GetFileAttrib( oexCSTR x_pFile )
{_STT();
	return CBaseFile_ToCBaseFileAttributes( ::GetFileAttributes( x_pFile ) );
}

oexBOOL CBaseFile::SetFileAttrib( oexCSTR x_pFile, oexUINT x_uAttrib )
{_STT();
	return ::SetFileAttributes( x_pFile, x_uAttrib ) ? oexTRUE : oexFALSE;
}

void CBaseFile_InitFindData( CBaseFile::SFindData *x_pFd )
{_STT();
	x_pFd->llSize = 0;
	x_pFd->sName.Destroy();
	x_pFd->uFileAttributes = 0;
	x_pFd->ftCreated = 0;
	x_pFd->ftLastAccess = 0;
	x_pFd->ftLastModified = 0;
}

CBaseFile::t_HFIND CBaseFile::FindFirst( oexCSTR x_pPath, oexCSTR x_pMask, CBaseFile::SFindData *x_pFd )
{_STT();
	// Sanity checks
	if ( !oexVERIFY_PTR( x_pPath ) || !oexVERIFY_PTR( x_pMask ) || !oexVERIFY_PTR( x_pFd ) )
		return oexFALSE;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Find first file
	HANDLE hFind = ::FindFirstFile( CStr::BuildPath( x_pPath, x_pMask ).Ptr(), &wfd );
	if ( INVALID_HANDLE_VALUE == hFind )
		return CBaseFile::c_InvalidFindHandle;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (oexINT64)wfd.ftCreationTime.dwLowDateTime | ( (oexINT64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (oexINT64)wfd.ftLastAccessTime.dwLowDateTime | ( (oexINT64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (oexINT64)wfd.ftLastWriteTime.dwLowDateTime | ( (oexINT64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (oexINT64)wfd.nFileSizeLow | ( (oexINT64)wfd.nFileSizeHigh << 32 );
	x_pFd->sName = wfd.cFileName;

	// Return the file handle
	return (CBaseFile::t_HFIND)hFind;
}

oexBOOL CBaseFile::FindNext( t_HFIND x_hFind, CBaseFile::SFindData *x_pFd )
{_STT();
	// Sanity checks
	if ( !oexVERIFY( vInvalidFindHandle() != x_hFind ) || !oexVERIFY_PTR( x_pFd ) )
		return oexFALSE;

	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof( wfd ) );

	// Attempt to find the next file
	if ( !::FindNextFile( x_hFind, &wfd ) )
		return oexFALSE;

	// Copy over data
	x_pFd->uFileAttributes = wfd.dwFileAttributes;
	x_pFd->ftCreated = (oexINT64)wfd.ftCreationTime.dwLowDateTime | ( (oexINT64)wfd.ftCreationTime.dwHighDateTime << 32 );
	x_pFd->ftLastAccess = (oexINT64)wfd.ftLastAccessTime.dwLowDateTime | ( (oexINT64)wfd.ftLastAccessTime.dwHighDateTime << 32 );
	x_pFd->ftLastModified = (oexINT64)wfd.ftLastWriteTime.dwLowDateTime | ( (oexINT64)wfd.ftLastWriteTime.dwHighDateTime << 32 );
	x_pFd->llSize = (oexINT64)wfd.nFileSizeLow | ( (oexINT64)wfd.nFileSizeHigh << 32 );
	x_pFd->sName = wfd.cFileName;

	return oexTRUE;
}

oexBOOL CBaseFile::FindClose( t_HFIND x_hFind )
{_STT();
	if ( vInvalidFindHandle() == x_hFind )
		return oexFALSE;
	return ::FindClose( x_hFind ) ? oexTRUE : oexFALSE;
}


oexBOOL CBaseFile::DoesExist( oexCSTR x_pPath )
{_STT();
	return INVALID_FILE_ATTRIBUTES != ::GetFileAttributes( x_pPath );
}

oexBOOL CBaseFile::CreateFolder( oexCSTR x_pPath )
{_STT();
	return ::CreateDirectory( x_pPath, NULL ) ? oexTRUE : oexFALSE;
}

CStr CBaseFile::GetModPath( oexCSTR x_pPath )
{_STT();
	if ( oexCHECK_PTR( x_pPath ) )
		return CBaseFile::GetModFileName().GetPath().BuildPath( x_pPath );
	return CBaseFile::GetModFileName().GetPath();
}

CStr CBaseFile::GetModFileName( oexCPVOID x_pInstance )
{_STT();

	oexTCHAR szFilename[ oexSTRSIZE ] = { 0 };

	// Get the module file name
	GetModuleFileName( (HMODULE)x_pInstance, szFilename, oexSTRSIZE );

	// Ensure it's NULL terminated
	szFilename[ oexSTRSIZE - 1 ] = 0;

	return szFilename;
}

oexBOOL CBaseFile::Rename( oexCSTR x_pOld, oexCSTR x_pNew )
{
	if ( !x_pOld || !*x_pOld || !x_pNew || !*x_pNew )
		return oexFALSE;

	return MoveFileEx( x_pOld, x_pNew, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );
}

oexBOOL CBaseFile::Copy( oexCSTR x_pOld, oexCSTR x_pNew )
{
	if ( !x_pOld || !*x_pOld || !x_pNew || !*x_pNew )
		return oexFALSE;

	return CopyFile( x_pOld, x_pNew, FALSE );
}

#if defined( OEX_USE_DYNAMIC_SHELL32 )
	
	// SH Types
	typedef IMalloc* t_LPMALLOC;
	typedef struct { USHORT cb; BYTE abID[ 1 ]; } t_SHITEMID;
	typedef struct { t_SHITEMID mkid; } t_ITEMIDLIST, *t_LPITEMIDLIST;
	
	// SH Functions
	typedef HRESULT (*pfn_SHGetMalloc)( t_LPMALLOC *ppMalloc );
	typedef BOOL (*pfn_SHGetPathFromIDList)( t_LPITEMIDLIST pidl, LPTSTR pszPath );
	typedef HRESULT (*pfn_SHGetSpecialFolderLocation)( HWND hwndOwner, int nFolder, t_LPITEMIDLIST *ppidl );

#else

#	define t_LPMALLOC LPMALLOC
#	define t_LPITEMIDLIST LPITEMIDLIST
#	define pSHGetMalloc SHGetMalloc
#	define pSHGetPathFromIDList SHGetPathFromIDList
#	define pSHGetSpecialFolderLocation SHGetSpecialFolderLocation

#endif

CStr CBaseFile::GetSysFolder( oexBOOL x_bShared, oexINT x_nFolderId, oexINT x_nMaxLength )
{
	// Ensure at least MAX_PATH bytes
	if ( MAX_PATH > x_nMaxLength )
		x_nMaxLength = MAX_PATH;

	BOOL trim = FALSE;
	CStr s, sub;
	if ( !s.OexAllocate( x_nMaxLength ) )
		return s;
		
	// NULL Terminate
	*s._Ptr() = 0;

	// Get the folder
	switch( x_nFolderId )
	{
		case eFidNone :
			return CStr();

		case eFidTemp :
			s.SetLength( ::GetTempPath( x_nMaxLength, s._Ptr() ) );
			return s;

		case eFidSystem :
			s.SetLength( ::GetSystemDirectory( s._Ptr(), x_nMaxLength ) );
			return s;

		case eFidUserOs :
			s.SetLength( ::GetWindowsDirectory( s._Ptr(), x_nMaxLength ) );
			return s;

		case eFidCurrent :
			s.SetLength( ::GetCurrentDirectory( x_nMaxLength, s._Ptr() ) );
			return s;

		case eFidDefDrive :
			s.SetLength( cmn::Min( (UINT)3, ::GetWindowsDirectory( s._Ptr(), x_nMaxLength ) ) );
			return s;

		case eFidRoot :
			x_nFolderId = CSIDL_DRIVES;
			break;

		case eFidSettings :
			x_nFolderId = x_bShared ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA;
			break;

		case eFidDesktop :
			x_nFolderId = x_bShared ? CSIDL_COMMON_DESKTOPDIRECTORY : CSIDL_DESKTOPDIRECTORY;
			break;

		case eFidDownloads :
			// +++ CSIDL_COMMON_DOCUMENTS is broken?
			trim = x_bShared ? 1 : 0;
			x_nFolderId = x_bShared ? CSIDL_COMMON_DESKTOPDIRECTORY : CSIDL_MYDOCUMENTS;
			sub = oexT( "Downloads" );
			break;

		case eFidRecycle :
			x_nFolderId = CSIDL_BITBUCKET;
			break;

		case eFidTemplates :
			x_nFolderId = x_bShared ? CSIDL_COMMON_TEMPLATES : CSIDL_TEMPLATES;
			break;

		case eFidPublic :
			// +++ CSIDL_COMMON_DOCUMENTS is broken?
			trim = 1; sub = oexT( "Public" );
			x_nFolderId = x_bShared ? CSIDL_COMMON_DESKTOPDIRECTORY : CSIDL_DESKTOPDIRECTORY;
			break;

		case eFidDocuments :
			// +++ CSIDL_COMMON_DOCUMENTS is broken?
			if ( x_bShared ) trim = 1, sub = oexT( "Documents" );
			x_nFolderId = x_bShared ? CSIDL_COMMON_DESKTOPDIRECTORY : CSIDL_MYDOCUMENTS;
			break;

		case eFidMusic :
			x_nFolderId = x_bShared ? CSIDL_COMMON_MUSIC : CSIDL_MYMUSIC;
			break;

		case eFidPictures :
			x_nFolderId = x_bShared ? CSIDL_COMMON_PICTURES : CSIDL_MYPICTURES;
			break;

		case eFidVideo :
			x_nFolderId = x_bShared ? CSIDL_COMMON_VIDEO : CSIDL_MYVIDEO;
			break;

		case eFidFavorites :
			x_nFolderId = x_bShared ? CSIDL_COMMON_FAVORITES : CSIDL_FAVORITES;
			break;

		case eFidStartMenu :
			x_nFolderId = x_bShared ? CSIDL_COMMON_STARTMENU : CSIDL_STARTMENU;
			break;

		case eFidStartup :
			x_nFolderId = x_bShared ? CSIDL_COMMON_STARTUP : CSIDL_STARTUP;
			break;

		case eFidCookies :
			x_nFolderId = CSIDL_COOKIES;
			break;

		case eFidNetwork :
			x_nFolderId = CSIDL_NETWORK;
			break;

		case eFidPrinters :
			x_nFolderId = CSIDL_PRINTERS;
			break;

		case eFidRecent :
			x_nFolderId = CSIDL_RECENT;
			break;

		case eFidHistory :
			x_nFolderId = CSIDL_HISTORY;
			break;

		case eFidFonts :
			x_nFolderId = CSIDL_FONTS;
			break;

		default :
			break;

	} // end switch

#if defined( OEX_USE_DYNAMIC_SHELL32 )

	// Functions
	pfn_SHGetMalloc pSHGetMalloc = NULL;
	pfn_SHGetPathFromIDList pSHGetPathFromIDList = NULL;
	pfn_SHGetSpecialFolderLocation pSHGetSpecialFolderLocation = NULL;

	// Load shell32.dll
	HMODULE hShell32 = LoadLibrary( oexT( "shell32.dll" ) );
	if ( !hShell32 )
		return CStr();

	// Load functions
	pSHGetMalloc = (pfn_SHGetMalloc)GetProcAddress( hShell32, oexT( "SHGetMalloc" ) );
	pSHGetPathFromIDList = (pfn_SHGetPathFromIDList)GetProcAddress( hShell32, oexT( "SHGetPathFromIDList" ) );
	pSHGetSpecialFolderLocation = (pfn_SHGetSpecialFolderLocation)GetProcAddress( hShell32, oexT( "SHGetSpecialFolderLocation" ) );

	// Did we get the functions?
	if ( !pSHGetMalloc || !pSHGetPathFromIDList || !pSHGetSpecialFolderLocation )
	{	FreeLibrary( hShell32 );
		return CStr();
	} // end if

#endif

	// +++ Add support for SHGetKnownFolderPath()

	t_LPITEMIDLIST pidl = NULL;
	if ( pSHGetSpecialFolderLocation( NULL, x_nFolderId, &pidl ) == NOERROR && pidl )
	{
		// Get the path name
		if ( !pSHGetPathFromIDList( pidl, s._Ptr() ) )
			s.Destroy();

		// Free the memory
		t_LPMALLOC pMalloc;
		if ( pSHGetMalloc( &pMalloc ) == NOERROR )
			pMalloc->Free( pidl );

	} // end if


#if defined( OEX_USE_DYNAMIC_SHELL32 )

	// Unload shell lib
	FreeLibrary( hShell32 );

#endif

	// Ensure length is valid
	s.Length();

	while ( trim )
		s = s.GetPath(), trim--;

	// Is there a sub directory?
	if ( sub.Length() )
		return oexBuildPath( s, sub, oexT( '\\' ) );

	return s;
}

