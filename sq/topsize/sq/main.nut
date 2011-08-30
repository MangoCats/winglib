
//------------------------------------------------------------------
// Globals
//------------------------------------------------------------------

// Path to scan
local path = _self.get( "/", "cmdline.0" );

// Max results to show
local max_results = _self.tolong( _self.get( "/", "cmdline.1" ) );
if ( 0 >= max_results ) max_results = 10;

// Quite mode?
local bQuiet = _self.isset( "/", "cmdline.q" );

// Files only
local bFileOnly = _self.isset( "/", "cmdline.f" );

// No size formatting
local bNoSizeFormat = _self.isset( "/", "cmdline.n" );

// Maximum number of bars to show
local max_bars = 25;

//------------------------------------------------------------------
// Functions
//------------------------------------------------------------------

// Display formatted row
function row( a, a_sz, b, c, bscale )
{	local str = format( " %-" + a_sz + "s %12s %6d%% ", a.tostring(), b.tostring(), c.tointeger() );
	local l = ( c * bscale ).tointeger();
	while ( l ) str += "|", l--;
	_self.echo( str );
}

// Show total processed status
function show_total( t ) : ( bQuiet )
{
	if ( !bQuiet ) 
		_self.print( "\rTotal : " + _self.create_size_string( t.tofloat(), 1024., 3, "  B, KB, MB, GB, TB" ) + "      " );
}

// Calculate folder contents size
function get_folder_size( p, total )
{
	local t = 0;

	// Add file sizes
	local d = CSqFile().get_dirlist( p, "*", 1, 0 );
	foreach( k,v in d )
		t += _self.get_file_size( _self.build_path( p, k ) );

	// Add sub folder sizes
	local d = CSqFile().get_dirlist( p, "*", 0, 1 );
	foreach( k,v in d )
		t += get_folder_size( _self.build_path( p, k ), total + t );

	// Let the user know we're still alive
	show_total( total + t );

	return t;
}


//------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------

	if ( !bQuiet ) 
		_self.echo( "\nScanning : " + path );

	local total = 0, smap = {}, sizes = [];
	local dirs = CSqFile().get_dirlist( path, "*", 1, 1 )

	// Map the file sizes
	foreach( k,v in dirs )
	{
		local sz = ( "f" == v.str() )
				   ? _self.get_file_size( _self.build_path( path, k ) )
				   : get_folder_size( _self.build_path( path, k ), total );

		total += sz;
		smap[ sz ] <- k;
		sizes.append( sz );
	}

	// Show final total
	show_total( total );

	// Sort from highest to lowest
	function compare_desc( a, b )
	{	return ( a > b ) ? -1 : ( ( a < b ) ? 1 : 0 ); }
	sizes.sort( compare_desc );

	if ( !bQuiet ) 
		_self.echo( "\n\n------------------- LARGEST FILES AND FOLDERS -------------------\n" );

	// Figure out the longest string for formatting purposes
	local max_strlen = 0, max_percent = ( 0 < sizes.len() ) ? ( sizes[ 0 ].tofloat() * 100 / total ) : 0;
	for( local i = 0; i < max_results && i < sizes.len(); i++ )
		if ( max_strlen < smap[ sizes[ i ] ].len() )
			max_strlen = smap[ sizes[ i ] ].len();

	// For max_results rows
	for( local i = 0; i < max_results && i < sizes.len(); i++ )

		// Full blown report
		if ( !bQuiet )
			row( smap[ sizes[ i ] ], max_strlen, 
				 bNoSizeFormat ? sizes[ i ] : _self.create_size_string( sizes[ i ].tofloat(), 1024., 2, "  B, KB, MB, GB, TB" ),
				 sizes[ i ].tofloat() * 100 / total, max_bars / max_percent );

		// File name + size
		else if ( !bFileOnly )
			_self.echo( format( " %-" + max_strlen + "s %12s", 
						smap[ sizes[ i ] ], 
						( bNoSizeFormat ? sizes[ i ].tostring() : _self.create_size_string( sizes[ i ].tofloat(), 1024., 2, "  B, KB, MB, GB, TB" ) ) ) );

		// File name only
		else
			_self.echo( smap[ sizes[ i ] ] );

	if ( !bQuiet ) 
		_self.echo( "\n-----------------------------------------------------------------\n" );
