
// sq_curl.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqCurl, CSqCurl )
	SQBIND_MEMBER_FUNCTION( CSqCurl, GetLastError )
	SQBIND_MEMBER_FUNCTION( CSqCurl, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqCurl, GetUrl )
	SQBIND_MEMBER_FUNCTION( CSqCurl, PostUrl )
	SQBIND_MEMBER_FUNCTION( CSqCurl, SetBasicAuth )
	SQBIND_MEMBER_FUNCTION( CSqCurl, getCerts )
	SQBIND_MEMBER_FUNCTION( CSqCurl, setFile )
	SQBIND_MEMBER_FUNCTION( CSqCurl, getFile )
	SQBIND_MEMBER_FUNCTION( CSqCurl, setConnectTimeout )
	SQBIND_MEMBER_FUNCTION( CSqCurl, getConnectTimeout )
	SQBIND_MEMBER_FUNCTION( CSqCurl, setTimeout )
	SQBIND_MEMBER_FUNCTION( CSqCurl, getTimeout )
	SQBIND_MEMBER_FUNCTION( CSqCurl, urlInclude )
	SQBIND_MEMBER_FUNCTION( CSqCurl, urlInline )
	SQBIND_MEMBER_FUNCTION( CSqCurl, urlSpawn )
	SQBIND_MEMBER_FUNCTION( CSqCurl, urlSqExe )
	SQBIND_MEMBER_FUNCTION( CSqCurl, setCookies )
	SQBIND_MEMBER_FUNCTION( CSqCurl, getCookies )
	SQBIND_MEMBER_FUNCTION( CSqCurl, enableCookies )
	SQBIND_MEMBER_FUNCTION( CSqCurl, setHeader )
//	SQBIND_MEMBER_FUNCTION( CSqCurl,  )
//	SQBIND_MEMBER_FUNCTION( CSqCurl,  )
//	SQBIND_MEMBER_FUNCTION( CSqCurl,  )
//	SQBIND_MEMBER_FUNCTION( CSqCurl,  )
//	SQBIND_MEMBER_FUNCTION( CSqCurl,  )
	
SQBIND_REGISTER_CLASS_END()

void CSqCurl::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqCurl );
}

CSqCurl::CSqCurl()
{_STT();
	m_curl = oexNULL;
	m_headers = oexNULL;
	m_nTimeout = 0;
	m_nConnectTimeout = 0;
	m_bEnableCookies = 0;
}

CSqCurl::~CSqCurl()
{_STT();
	Destroy();
}

void CSqCurl::Destroy()
{_STT();

	m_sErr = oexT( "" );

	// Lose certs
	m_mCerts.clear();

	// Close curl handle
	if ( m_curl )
	{	curl_easy_cleanup( m_curl );
		m_curl = oexNULL;
	} // end if
	
	if ( m_headers )
	{	curl_slist_free_all( m_headers );
		m_headers = oexNULL;
	} // end if
}

int CSqCurl::StdWriter( char *data, size_t size, size_t nmemb, sqbind::CSqBinary *buffer )
{_STT();
	int res = 0;
	if ( buffer && data && size && nmemb )
		res = size * nmemb,
		buffer->AppendBuffer( data, res );
	return res;
}

int CSqCurl::StdFileWriter( char *data, size_t size, size_t nmemb, sqbind::CSqFile *file )
{_STT();
	int res = 0;
	if ( file && data && size && nmemb )
		res = size * nmemb,
		file->Obj().Write( data, res );
	return res;
}

int CSqCurl::urlInclude( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sPath, const sqbind::stdString &sUrl )
{
	if ( !pQ )
		return 0;

	// Grab the data
	sqbind::CSqBinary dat;	
	if ( !GetUrl( sUrl, 0, &dat ) || !dat.getUsed() )
		return 0;
	
	// Run the script
	return pQ->run( oexNULL, sPath, sUrl, dat.getString() );
}

sqbind::stdString CSqCurl::urlInline( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sPath, const sqbind::stdString &sUrl, sqbind::CSqMulti *pParams )
{
	if ( !pQ )
		return sqbind::stdString();

	// Grab the data
	sqbind::CSqBinary dat;	
	if ( !GetUrl( sUrl, 0, &dat ) || !dat.getUsed() )
		return sqbind::stdString();

	// Run inlined data
	sqbind::stdString res;
	pQ->run( &res, sqbind::stdString(), sUrl, sqbind::prepare_inline( dat.getString(), oex::oexFALSE ) );

	return res;
}

int CSqCurl::urlSpawn( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sPath, const sqbind::stdString &sName, const sqbind::stdString &sScriptName, const sqbind::stdString &sUrl )
{_STT();

	if ( !pQ )
		return 0;

	// Grab the data
	sqbind::CSqBinary dat;	
	if ( !GetUrl( sUrl, 0, &dat ) || !dat.getUsed() )
		return 0;

	return pQ->spawn2( oexNULL, sPath, sName, sScriptName, dat.getString(), oex::oexFALSE );
}

int CSqCurl::urlSqExe( sqbind::CSqMsgQueue *pQ, const sqbind::stdString &sPath, const sqbind::stdString &sDir, const sqbind::stdString &sUrl )
{_STT();

	if ( !pQ )
		return 0;

	// Grab the data
	sqbind::CSqBinary dat;	
	if ( !GetUrl( sUrl, 0, &dat ) || !dat.getUsed() )
		return 0;

	return pQ->sqexe_script( oexNULL, sPath, sqbind::stdString(), dat.getString(), sDir );
}

void CSqCurl::setHeader( const sqbind::stdString &sHeader )
{_STT();

	// Ensure non-zero length string
	if ( !sHeader.length() )
		return;

	// Add the header to the linked list
	m_headers = curl_slist_append( m_headers, oexStrToMb( sqbind::std2oex( sHeader ) ).Ptr() );
}

int CSqCurl::Init()
{_STT();

	// Lose old thing
	Destroy();

	// Initialize Curl
	m_curl = curl_easy_init();
	if ( !m_curl )
		return 0;

	if ( 0 <= m_nConnectTimeout )
		curl_easy_setopt( m_curl, CURLOPT_CONNECTTIMEOUT_MS, m_nConnectTimeout );
		
	if ( 0 <= m_nTimeout )
		curl_easy_setopt( m_curl, CURLOPT_TIMEOUT_MS, m_nTimeout );
		
	curl_easy_setopt( m_curl, CURLOPT_HEADER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_FOLLOWLOCATION, 1 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_easy_setopt( m_curl, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_easy_setopt( m_curl, CURLOPT_CERTINFO, 1 );
	
	if ( m_bEnableCookies )
	{	curl_easy_setopt( m_curl, CURLOPT_COOKIEFILE, oexT( "" ) );
//		curl_easy_setopt( m_curl, CURLOPT_COOKIE, m_sCookies.c_str() );	
		curl_easy_setopt( m_curl, CURLOPT_COOKIELIST, m_sCookies.c_str() );	
	} // end if

	return 1;
}

int CSqCurl::GetUrl( const sqbind::stdString &sUrl, SQInteger lPort, sqbind::CSqBinary *sData )
{_STT();

	if ( !sData && !m_sFile.length() )
		return 0;

	if ( sData )
		sData->setUsed( 0 );

	if ( !sUrl.length() )
		return 0;

	// Initialize
	if ( !m_curl )
		if ( !Init() )
			return 0;

	if ( m_sUsername.length() || m_sPassword.length() )
	{	curl_easy_setopt( m_curl, CURLOPT_USERPWD, 
						  ( sqbind::stdString() + m_sUsername + oexT( ":" ) + m_sPassword ).c_str() );
		curl_easy_setopt( m_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC );
	} // end if
	else
		curl_easy_setopt( m_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY );

	char sErr[ CURL_ERROR_SIZE ] = { 0 };
	curl_easy_setopt( m_curl, CURLOPT_ERRORBUFFER, &sErr[ 0 ] );

	// The URL we want
	curl_easy_setopt( m_curl, CURLOPT_URL, sUrl.c_str() );
	if ( lPort )
		curl_easy_setopt( m_curl, CURLOPT_PORT, lPort );
	
	// Setup output
	sqbind::CSqFile fOut;
	if ( m_sFile.length() )
	{	if ( !fOut.OpenAlways( m_sFile ) ) return 0;
		curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, &fOut );
		curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdFileWriter );
	} // endif
	else if ( sData )
	{	curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, sData );
		curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdWriter );
	} // end else

	if ( m_headers )
		curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, m_headers );

	// Do the thing
	CURLcode res = curl_easy_perform( m_curl );

	if ( CURLE_OK != res )
	{	m_sErr = oexMbToStrPtr( sErr );
		return 0;
	} // end if
	
	// Get cookies
	if ( m_bEnableCookies )
	{	struct curl_slist *cookies = 0;
		res = curl_easy_getinfo( m_curl, CURLINFO_COOKIELIST, &cookies );
		if ( !res && cookies )
		{
			m_sCookies = oexT( "" ); 
			
			struct curl_slist *i = cookies;
			while ( i )
			{	m_sCookies += i->data;
				m_sCookies += oexT( "\r\n" );
				i = i->next;
			} // end while
			
			curl_slist_free_all( cookies );

		} // end if
			
	} // end if
	
	// See if there are certs
	struct curl_certinfo *ci = 0;
	res = curl_easy_getinfo( m_curl, CURLINFO_CERTINFO, &ci );
	if ( !res && ci )
		for ( int i = 0; i < ci->num_of_certs; i++ )
			for ( struct curl_slist *slist = ci->certinfo[ i ]; slist; slist = slist->next )
				if ( slist->data && *slist->data )
				{
					// We're going to add as key / value pairs
					oex::CStr s = oexMbToStr( slist->data );
					oex::oexCSTR k = s.Ptr(), v = oexNULL;

					// Split key / value
					unsigned int c = 0;
					while ( c < s.Length() && s[ c ] != oexT( ':' ) ) c++;

					// Did we find a divider
					if ( s[ c ] == oexT( ':' ) )
						*s._Ptr( c++ ) = 0, v = s.Ptr( c );

					// Save cert data
					if ( k && v )
						m_mCerts[ oexMks( i ).Ptr() ][ k ] = v;

				} // end for

	return 1;
}

int CSqCurl::PostUrl( const sqbind::stdString &sUrl, SQInteger lPort, const sqbind::stdString &sPost, sqbind::CSqBinary *sData )
{_STT();

	if ( !sData && !m_sFile.length() )
		return 0;

	if ( sData )
		sData->setUsed( 0 );

	if ( !sUrl.length() )
		return 0;

	// Initialize
	if ( !m_curl )
		if ( !Init() )
			return 0;

	curl_easy_setopt( m_curl, CURLOPT_POST, 1 );
	curl_easy_setopt( m_curl, CURLOPT_POSTFIELDS, sPost.c_str() );
	curl_easy_setopt( m_curl, CURLOPT_POSTFIELDSIZE, sPost.length() );

	if ( m_sUsername.length() || m_sPassword.length() )
	{	curl_easy_setopt( m_curl, CURLOPT_USERPWD, 
						  ( sqbind::stdString() + m_sUsername + oexT( ":" ) + m_sPassword ).c_str() );
		curl_easy_setopt( m_curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC );
	} // end if

	char sErr[ CURL_ERROR_SIZE ] = { 0 };
	curl_easy_setopt( m_curl, CURLOPT_ERRORBUFFER, &sErr[ 0 ] );

	// The URL we want
	curl_easy_setopt( m_curl, CURLOPT_URL, sUrl.c_str() );
	if ( lPort )
		curl_easy_setopt( m_curl, CURLOPT_PORT, lPort );

	// Setup output
	sqbind::CSqFile fOut;
	if ( m_sFile.length() )
	{	oexSHOW( m_sFile.c_str() );
		if ( !fOut.OpenAlways( m_sFile ) ) return 0;
		curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, &fOut );
		curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdFileWriter );
	} // endif
	else if ( sData )
	{	curl_easy_setopt( m_curl, CURLOPT_WRITEDATA, sData );
		curl_easy_setopt( m_curl, CURLOPT_WRITEFUNCTION, StdWriter );
	} // end else

	if ( m_headers )
		curl_easy_setopt( m_curl, CURLOPT_HTTPHEADER, m_headers );

	// Do the thing
	CURLcode res = curl_easy_perform( m_curl );

	if ( CURLE_OK != res )
	{	m_sErr = oexMbToStrPtr( sErr );
		return 0;
	} // end if

	// Get cookies
	if ( m_bEnableCookies )
	{	struct curl_slist *cookies = 0;
		res = curl_easy_getinfo( m_curl, CURLINFO_COOKIELIST, &cookies );
		if ( !res && cookies )
		{
			m_sCookies = oexT( "" ); 
			
			struct curl_slist *i = cookies;
			while ( i )
			{	m_sCookies += i->data;
				m_sCookies += oexT( "\r\n" );
				i = i->next;
			} // end while

			curl_slist_free_all( cookies );

		} // end if

	} // end if

	// See if there are certs
	struct curl_certinfo *ci = NULL;
	res = curl_easy_getinfo( m_curl, CURLINFO_CERTINFO, &ci );
	if ( !res && ci )
		for ( int i = 0; i < ci->num_of_certs; i++ )
			for ( struct curl_slist *slist = ci->certinfo[ i ]; slist; slist = slist->next )
				if ( slist->data && *slist->data )
				{
					// We're going to add as key / value pairs
					oex::CStr s = oexMbToStr( slist->data );
					oex::oexCSTR k = s.Ptr(), v = oexNULL;

					// Split key / value
					oex::CStr::t_size c = 0;
					while ( c < s.Length() && s[ c ] != oexT( ':' ) ) c++;

					// Did we find a divider
					if ( s[ c ] == oexT( ':' ) )
						*s._Ptr( c++ ) = 0, v = s.Ptr( c );

					// Save cert data
					if ( k && v )
						m_mCerts[ oexMks( i ).Ptr() ][ k ] = v;

				} // end for

	return 1;
}

int CSqCurl::SetBasicAuth( const sqbind::stdString &sUsername, const sqbind::stdString &sPassword )
{
	m_sUsername = sUsername;
	m_sPassword = sPassword;
	return ( m_sUsername.length() || m_sPassword.length() );
}


