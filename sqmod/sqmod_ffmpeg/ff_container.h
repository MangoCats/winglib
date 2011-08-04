// ff_container.h

class CFfContainer
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfContainer )
	_SQBIND_CLASS_CTOR_END( CFfContainer )

	/// Default constructor
	CFfContainer();

	/// Destructor
	virtual ~CFfContainer() { Destroy(); }

	/** \addtogroup CFfContainer
		@{
	*/

	/// Frees all resources
	void Destroy();

	/// Closes the stream without releasing codecs
	int CloseStream();

	/// Open link
	int Open( const sqbind::stdString &sUrl, sqbind::CSqMulti *m );

	/// Reads frame data
	int ReadFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Reads and decodes a frame
	int DecodeFrame( int stream, int fmt, sqbind::CSqBinary *dat, sqbind::CSqMulti *m, int flip );

	/// Decodes a frame from the specified buffer
	/**
		!!! DEPRECATED !!!
		
		use getAudioDec().Decode() instead
	*/
	int DecodeFrameBin( sqbind::CSqBinary *in, int fmt, sqbind::CSqBinary *out, sqbind::CSqMulti *m, int flip );

	/// Decodes an audio frame from the specified buffer
	int DecodeAudioFrameBin( sqbind::CSqBinary *in, sqbind::CSqBinary *out, sqbind::CSqMulti *m );

	/// Creates a container
	int Create( const sqbind::stdString &sUrl, const sqbind::stdString &sType, sqbind::CSqMulti *m );

	/// Initializes file writing
	int InitWrite();

	/// Adds a video stream to the file
	int AddVideoStream( int fmt, int width, int height, int fps );

	/// Writes the specified image to the file
	int WriteFrame( sqbind::CSqBinary *dat, sqbind::CSqMulti *m );

	/// Writes the specified image to the file
	int WriteVideoFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m );
	
	/// Adds an audio stream to the avi
	int AddAudioStream( int codec_id, int channels, int sample_rate, int bps );
	
	/// Writes the specified audio to the file
	int WriteAudioFrame( sqbind::CSqBinary *dat, SQInteger nPts, SQInteger nDts, sqbind::CSqMulti *m );
	
	/// Returns video width
	double getFps()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		long den = m_pFormatContext->streams[ m_nVideoStream ]->codec->time_base.den;
		long num = m_pFormatContext->streams[ m_nVideoStream ]->codec->time_base.num;
		if ( !num ) num = 1;

		return (double)den / (double)num;
	}

	/// Returns video width
	int getWidth()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->width;
	}

	/// Returns video width
	int getHeight()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->height;
	}

	/// Returns video format
	int getVideoCodecId()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->codec_id;
	}

	/// Returns video format
	int getVideoFormat()
	{	if ( !m_pFormatContext || 0 > m_nVideoStream
			 || !m_pFormatContext->streams[ m_nVideoStream ]->codec )
			return 0;
		return m_pFormatContext->streams[ m_nVideoStream ]->codec->pix_fmt;
	}

	/// Returns the audio sample rate
//	int getAudioSampleRate()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->sample_rate;
//	}

	/// Returns the audio bit rate
//	int getAudioBitRate()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->bit_rate;
//	}

	/// Returns the audio bit rate
//	int getAudioBitsPerSample()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->bits_per_coded_sample;
//	}

	/// Returns the audio channels
//	int getAudioChannels()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->channels;
//	}

	/// Returns the audio frame size
//	int getAudioFrameSize()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->frame_size;
//	}

	/// Returns the audio codec id
//	int getAudioCodecID()
//	{	if ( !m_pAudioCodecContext ) return 0;
//		return m_pAudioCodecContext->codec_id;
//	}

	/// Returns the codec type
//	sqbind::stdString getAudioType()
//	{	if ( !m_pAudioCodecContext || !m_pAudioCodecContext->codec )
//			return oexT( "" );
//		return oexMbToStrPtr( m_pAudioCodecContext->codec->name );
//	}

	/// Returns the codec name
//	sqbind::stdString getAudioName()
//	{	if ( !m_pAudioCodecContext || !m_pAudioCodecContext->codec )
//			return oexT( "" );
//		return oexMbToStrPtr( m_pAudioCodecContext->codec->long_name );
//	}

	/// Returns a pointer to the audio decoder
	CFfAudioDecoder* getAudioDec() { return &m_audio_dec; }

	/// Returns video stream index
	int getVideoStream()
	{	return m_nVideoStream;
	}

	/// Returns video stream index
	int getAudioStream()
	{	return m_nAudioStream;
	}

	/// Returns non-zero if a container is open
	int isOpen()
	{	return m_pFormatContext ? 1 : 0;
	}

	/// Return the frame count
	int getFrameCount()
	{	return m_nFrames;
	}

	/// Returns non zero if there is a valid audio codec
	int isAudioCodec() 
	{	return m_audio_dec.isValid(); 
	}

	/// Returns non zero if there is a valid video codec
	int isVideoCodec()
	{	return m_pCodecContext ? 1 : 0; }

	/// Seeks to the specified time
	/**
		\param [in] nStreamId		-	Stream Id to use as a reference
		\param [in] nOffset			-	Seek offset
		\param [in] nFlags			-	1 = Seek backward
										2 = Seek in bytes
										4 = Seek to any frame (not just key frames)
										8 = Seek based on frame number
	*/
	int Seek( int nStreamId, int nOffset, int nFlags );

	/// Returns extra audio codec data
	sqbind::CSqBinary getAudioExtraData() { return m_audio_extra; }

	/// Sets extra audio codec data
	void setAudioExtraData( sqbind::CSqBinary *p ) { if ( p ) m_audio_extra = *p; }

	/** @} */

private:

	/// Write ability?
	int 					m_nWrite;

	/// Read ability?
	int 					m_nRead;

	/// Output container context
	AVFormatContext			*m_pFormatContext;

	/// Codec context
	AVCodecContext			*m_pCodecContext;

	/// AV Packet
	AVPacket				m_pkt;

	/// Decode frame
	AVFrame					*m_pFrame;

	/// Non-zero if key frame has been received
	int						m_bKeyRxd;

	/// Video stream index
	int						m_nVideoStream;

	/// Audio stream index
	int						m_nAudioStream;

	/// Left over packet data
	sqbind::CSqBinary		m_buf;

	/// Number of frames that have been processed
	int						m_nFrames;
	
	/// Audio decoder
	CFfAudioDecoder			m_audio_dec;

	/// Extra codec data
	sqbind::CSqBinary		m_audio_extra;
	
/*
	/// Number of frames that have been processed
	int						m_nAudioFrames;

	/// Codec context
	AVCodecContext			*m_pAudioCodecContext;

	/// Left over packet data
	sqbind::CSqBinary		m_audio_buf;
	
	/// AV Packet
	AVPacket				m_audio_pkt;
*/

};
