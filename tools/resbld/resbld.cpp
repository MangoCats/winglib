// resbld.cpp

#include "stdafx.h"

using namespace oex;

void chtoa( oexUCHAR *b, oexUCHAR ch )
{
	oexUCHAR c = ch & 0x0f;

	if ( 9 >= c )
		b[ 1 ] = '0' + c;
	else
		b[ 1 ] = 'a' + ( c - 10 );

	c = ( ch >> 4 ) & 0x0f;
	if ( 9 >= c )
		b[ 0 ] = '0' + c;
	else
		b[ 0 ] = 'a' + ( c - 10 );
}

int create_res( oex::CStr sIn, oex::CStr sOut, oex::CStr sVar, oex::CStr sPre, oex::CStr sSuf, oex::CStr sInc )
{
//	oexCHAR8 *_p = ;
//	oexINT _l = ;
//	oexEcho( oexMbToStr( oex::zip::CUncompress::Uncompress( oex::CStr8( _p, _l ) ) ).Ptr() );

	if ( !oexExists( sIn.Ptr() ) )
	{	oexEcho( oexMks( oexT( "No such file : " ), sIn ).Ptr() );
		return -20;
	} // end if

	// Do we need to create a name?
	if ( !sOut.Length() )
		sOut << sIn.RemoveFileExtension() << oexT( ".cpp" );

	oexEcho( ( oex::CStr() << sIn.GetFileName() << " -> " << sOut.GetFileName() ).Ptr() );

	// Read in and compress the file
    oex::CStr8 sData = oex::zip::CCompress::Compress( CFile().OpenExisting( sIn.Ptr() ).Read() );

	// Create a variable name based on the md5 hash
	if ( !sVar.Length() )
		sVar << oexT( "g_oexres_" ) << oexMd5( sIn );

	if ( !sPre.Length() )
		sPre << oexT( "char " ) << sVar << oexT( "[] = " oexNL "{" oexNL "\t" );

	if ( !sSuf.Length() )
		sSuf = oexT( oexNL "\t0" oexNL "};" oexNL );

	if ( sInc.Length() )
	{
		oex::CFile i;
		if ( !i.OpenAlways( sInc.Ptr() ).IsOpen() )
		{	oexEcho( oexMks( oexT( "Unable to open include file for writing : " ), sInc ).Ptr() );
			return -21;
		} // end if

		i.SetPtrPosEnd( 0 );

		// Add to include
		i.Write( CStr8() << oexNL "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
				 	     << oexNL "extern const char " << oexStrToMb( sVar ) << "[];" oexNL );

	} // end if

	oex::CFile f;
	if ( !f.CreateAlways( sOut.Ptr() ).IsOpen() )
	{	oexEcho( oexMks( oexT( "Unable to create output file : " ), sOut ).Ptr() );
		return -22;
	} // end if

	oexCONST oexUCHAR *p = (oexCONST oexUCHAR*)sData.Ptr();
	oexULONG u = sData.Length();

	// Write out the data length
	f.Write( ( CStr8() << oexNL "unsigned long " << oexStrToMb( sVar ) << "_len = " << u << ";" oexNL ) );

	// Write out the data
	f.Write( sPre );

	CStr8 buf;
	oexUCHAR *t = (oexUCHAR*)"0x.., ";
	oexUCHAR *r = (oexUCHAR*)"0x..," oexNL "\t ";
	oexUCHAR *b = (oexUCHAR*)buf.OexAllocate( 1024 ), *s;
	oexINT bl = 0;
	for ( oexULONG i = 0; i < u; i++ )
	{
		s = &b[ bl ];

		if ( !( ( i + 1 ) & 0xf ) )
			oexMemCpy( s, r, 8 ), bl += 8;
		else
			oexMemCpy( s, t, 6 ), bl += 6;

		chtoa( &s[ 2 ], (oexUCHAR)p[ i ] );

		if ( 1000 < bl )
			f.Write( b, bl ), bl = 0;

	} // end for

	if ( bl )
		f.Write( b, bl );

	f.Write( sSuf );

	return 0;
}

int from_dir( oex::CStr dir, oex::CStr &sOutDir, oex::CPropertyBag &pb,
			  oex::CStr sPath, oex::CFile *pInc, oex::CFile *pCmp, oex::CFile *pRes, oex::CFile *pDep )
{
	// Ensure directory exists
	if ( !oexExists( dir.Ptr() ) )
		return -10;

	oex::CFindFiles ff;
	if ( ff.FindFirst( dir.Ptr(), "*" ) )
		do
		{
			// Build relative path
			oex::CStr sRel = oexBuildPath( sPath, ff.GetFileName() );
//			oexSHOW( sRel );

			// Recurse into directory
			if ( ff.IsDirectory() )
				from_dir( ff.GetFullPath(), sOutDir, pb, sRel, pInc, pCmp, pRes, pDep );

			else
			{
				// Create variable name
				oex::CStr sHash = oexMd5( sRel );
				oex::CStr sVar; sVar << oexT( "g_oexres_" ) << sHash;
				oex::CStr sOut = oexBuildPath( sOutDir, sHash << oexT( ".cpp" ) );

//				oexSHOW( ff.GetFullPath() );
//				oexSHOW( sVar );
//				oexSHOW( sOut );

				// Create file
				if ( 0 == create_res( ff.GetFullPath(), sOut, sVar,
									  oexT( "" ), oexT( "" ), oexT( "" ) ) )
				{
					if ( pInc )
						pInc->Write( CStr8() << oexNL "extern unsigned long " << oexStrToMb( sVar ) << "_len;"
				 							 << oexNL "extern const char " << oexStrToMb( sVar ) << "[];" oexNL );

					if ( pCmp )
						pCmp->Write( CStr8() << oexNL "\telse if ( !strncmp( sName, \"" << sRel << "\", max ) )" oexNL
												"\t\t*p = " << oexStrToMb( sVar ) << "," oexNL
												"\t\t*l = " << oexStrToMb( sVar ) << "_len;" oexNL );

					if ( pRes )
						pRes->Write( CStr8() << "\t" << sOut << " \\" oexNL );

					if ( pDep )
						pDep->Write( CStr8() << sOut << ": " << ff.GetFullPath() << oexNL oexNL );

				} // end if

			} // end else

		} while ( ff.FindNext() );

	return 0;
}


int process(int argc, char* argv[])
{
	if ( 2 > argc )
	{	oexEcho( oexT( "Use : resbld <file>" ) );
		return -1;
	} // end if

	// Parse the command line params
	oex::CPropertyBag pb = oex::CParser::ParseCommandLine( argc, argv );
//	oexEcho( oexMks( oexT( "Command Line = " ), pb.PrintR() ).Ptr() );

	// Was an entire directory specified?
	if ( pb.IsKey( oexT( "d" ) ) )
	{
		// Output directory
		oex::CStr sOutDir = pb[ oexT( "o" ) ].ToString();

		// Output same as input if not specified
		if ( !sOutDir.Length() )
			sOutDir = pb[ oexT( "d" ) ].ToString();

		// Ensure output directory exists
		if ( !oexExists( sOutDir.Ptr() ) )
			oexCreatePath( sOutDir.Ptr() );

		// Include file?
		oex::CFile fInc;
		if ( fInc.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.h" ) ).Ptr() ).IsOpen() )
			fInc.Write( CStr8() << oexNL
				"#define OEX_RESOURCES 1" oexNL
				"#if defined( OEX_NO_RESOURCES )" oexNL
				"#\terror 'oexres.h' MUST be included BEFORE 'oexlib.h'" oexNL
				"#endif" oexNL oexNL
				"int OexGetResourceInfo( const char *sName, const void **p, long *l );" oexNL
				"int OexGetResourceInfo( const char *sName, long max, const void **p, long *l );" oexNL
				);

		oex::CFile fCmp;
		if ( fCmp.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.cpp" ) ).Ptr() ).IsOpen() )
			fCmp.Write( CStr8() << oexNL
				"#include \"string.h\"" oexNL "#include \"oexres.h\"" oexNL oexNL
				"int OexGetResourceInfo( const char *sName, const void **p, long *l )" oexNL
				"{	return OexGetResourceInfo( sName, strlen( sName ), p, l ); }" oexNL
				"int OexGetResourceInfo( const char *sName, long max, const void **p, long *l )" oexNL
				"{" oexNL
				"\tif ( 0 );" oexNL
				);

		oex::CFile fRes;
		if ( fRes.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.mk" ) ).Ptr() ).IsOpen() )
			fRes.Write( CStr8() << oexNL
				"RES_CPP := \\" oexNL
				"\t" << oexBuildPath( sOutDir, "oexres.cpp" ) << " \\" oexNL
				);

		oex::CFile fDep;
		if ( fDep.CreateAlways( oexBuildPath( sOutDir, oexT( "oexres.dpp" ) ).Ptr() ).IsOpen() )
			fDep.Write( CStr8() << oexNL
				);

		// Create resources from directory
		int ret = from_dir( pb[ oexT( "d" ) ].ToString(), sOutDir, pb, oexT( "" ), &fInc, &fCmp, &fRes, &fDep );

		if ( fCmp.IsOpen() )
			fCmp.Write( CStr8() <<
				oexNL "\telse return -1;" oexNL
				oexNL
				"\treturn 0;" oexNL
				"}" oexNL
			);

		return ret;

	} // end if

	// Compile single file
	return create_res( pb[ oexT( "0" ) ].ToString(),
					   pb[ oexT( "1" ) ].ToString(),
					   pb[ oexT( "v" ) ].ToString(),
					   pb[ oexT( "p" ) ].ToString(),
					   pb[ oexT( "s" ) ].ToString(),
					   pb[ oexT( "i" ) ].ToString() );
}


int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	int ret = process( argc, argv );

	// Unitialize the oex library
    oexUNINIT();

	return ret;
}
