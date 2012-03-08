// stdafx.cpp

#include "stdafx.h"

oexLock _g_ffmpeg_lock;

#ifndef assert
extern "C"  void assert(int expression)
{
}
#endif

extern "C"
{

#if defined( _MSC_VER ) && _MSC_VER >= 1600
	int _imp____lc_codepage = 0; // CP_ACP 1252;
#endif

#if defined( OEX_WINDOWS )
	extern "C" int usleep( int usec ) { oexMicroSleep( usec ); return usec; }
#endif

//const uint64_t ff_pw_20 = 0x0014001400140014ULL;
//const uint64_t ff_pw_20 __attribute__ ((aligned (8))) = 0x0014001400140014ULL;
//const unsigned long long ff_pw_20 = 0x0014001400140014ULL;
//DECLARE_ALIGNED_8 (const uint64_t, ff_pw_20 ) = 0x0014001400140014ULL;

//int snd_strerror( const char *p )
//{_STT();
//	return 0;
//}

}

// Custom read write protocol
int memshare_open( URLContext *h, const char *filename, int flags )
{
	// Sanity check
	if ( !h || !filename || !*filename )
		return AVERROR( EINVAL );

	// Verify that the protocol is as we expect
	if ( oex::zstr::CompareLen( "memshare://", 11, filename, 11 ) )
		return AVERROR( EINVAL );

	// Parse the url
	oex::CPropertyBag pb = oex::os::CIpAddress::ParseUrl( filename );

	// Get the real share name
	long lSize = 1024 * 1024;
	long lBuffers = 256;
	const char *pShare = pb[ "host" ].ToString().Ptr();
	if ( !*pShare )
		return AVERROR( EINVAL );

	// Attemp to create a new fifo share object
	sqbind::CSqFifoShare *pFs = OexAllocConstruct< sqbind::CSqFifoShare >();
	if ( !pFs )
		return AVERROR( ENOMEM );

	// Parse GET parameters if provided
	if ( pb[ oexT( "extra" ) ].ToString().Length() )
	{
		oex::CPropertyBag pbGet = oex::CParser::DecodeUrlParams( pb[ oexT( "extra" ) ].ToString() );

		// Get user size
		if ( pbGet.IsKey( "size" ) )
			lSize = oex::cmn::Max( pbGet[ oexT( "size" ) ].ToLong(), 1024l );

		// Get user buffers
		if ( pbGet.IsKey( "buffers" ) )
			lBuffers = oex::cmn::Max( pbGet[ oexT( "buffers" ) ].ToLong(), 8l );

	} // end if

	// Create share buffer
	if ( !pFs->Create( pShare, "", lSize, lBuffers, "" ) )
	{	OexAllocDelete( pFs );
		return AVERROR( ENOMEM );
	} // end if

	// +++ Do this correctly sometime

	// Only write supported at the moment
	if ( URL_RDONLY == flags )
		return AVERROR( ENOSYS );

	// Allow flushing
//	h->flags |= AVFMT_ALLOW_FLUSH;

	// Can't be seeked
	h->is_streamed = 1;

	// No packetized writes
	h->max_packet_size = 0;

	// Save away pointer to fifo share
	h->priv_data = (void*)pFs;

	return 0;
}

int memshare_read( URLContext *h, unsigned char *buf, int size )
{
//	return AVERROR( ENOENT );
	return AVERROR( ENOSYS );
}

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(52, 68, 0)
int memshare_write( URLContext *h, unsigned char *buf, int size )
#else
int memshare_write( URLContext *h, const unsigned char *buf, int size )
#endif
{
oexM();
	// Just ignore null writes
	if ( 0 >= size )
		return 0;

	// Sanity check
	if ( !h || !h->priv_data || !buf )
		return AVERROR( EINVAL );

oexSHOW( size );

	// Write to the shared memory buffer
	if ( !((sqbind::CSqFifoShare*)h->priv_data)->WritePtr( buf, size, "", 0, 0 ) )
		return AVERROR( ENOMEM );

	return size;
}

int64_t memshare_seek( URLContext *h, int64_t pos, int whence )
{
	// Sanity check
	if ( !h || !h->priv_data )
		return AVERROR( EINVAL );

	return -1;
/*
	switch ( whence )
	{
		case SEEK_SET:
			break;
		case SEEK_CUR:
			break;
		case SEEK_END:
			break;
		case AVSEEK_SIZE:
			break;

	} // end switch
*/
}

int memshare_close( URLContext *h )
{
	// Sanity check
	if ( !h || !h->priv_data )
		return AVERROR( EINVAL );

	// Delete fifo object
	OexAllocDelete( (sqbind::CSqFifoShare*)h->priv_data );

	// Drop pointer
	h->priv_data = 0;

	return 0;
}

URLProtocol memshare_protocol = {
    "memshare",
    memshare_open,
    memshare_read,
    memshare_write,
    memshare_seek,
    memshare_close,
};

// Export classes
static void SQBIND_Export_ffmpeg( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

#if defined( oexDEBUG )
	av_log_set_level( AV_LOG_INFO );
//	av_log_set_level( AV_LOG_DEBUG );
#else
//	av_log_set_level( AV_LOG_WARNING );
	av_log_set_level( AV_LOG_ERROR );
//	av_log_set_level( AV_LOG_FATAL );
#endif

	// Register codecs
	av_register_all();
#if !defined( OEX_WINDOWS )
//	avdevice_register_all();
#endif

	// Register the oexshare protocol
	av_register_protocol2( &memshare_protocol, sizeof( memshare_protocol ) );

	CFfDecoder::Register( x_vm );
	CFfEncoder::Register( x_vm );
	CFfAudioDecoder::Register( x_vm );
	CFfConvert::Register( x_vm );
	CFfTranscode::Register( x_vm );
	CFfCapture::Register( x_vm );
	CFfContainer::Register( x_vm );
}

#if defined( SQBIND_STATIC )
	#include "ff_decoder.cpp"
	#include "ff_encoder.cpp"
	#include "ff_audio_decoder.cpp"
	#include "ff_convert.cpp"
	#include "ff_container.cpp"
	#include "ff_transcode.cpp"
	#include "ff_captuer.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_ffmpeg( x_vm ); }

	// +++ Leaking to avoid crash on ctrl+c, please fix one day!
//	#define SQBIND_Exit _g_ffmpeg_lock.Detach();

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
