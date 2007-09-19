/*------------------------------------------------------------------
// zip.cpp
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


#include "../oexlib.h"

#if defined( OEX_ENABLE_ZIP )

// Include zip functions
#include "../oss/zip/zip.cpp"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::zip;

CZip::CZip()
{
}

CZip::~CZip()
{
}

/// Yup, more indirection
class CDslZipLibCompress : public CZipLibCompress
{
public:
    CDslZipLibCompress( CCompress *p ) { m_p = p; }
    virtual unsigned OnRead( char *buf, unsigned size )
    {   return m_p->OnRead( buf, size ); }
    virtual unsigned OnWrite( const char *buf, unsigned *size )
    {   return m_p->OnWrite( buf, size ); }
private:
    CCompress *m_p;
};

CCompress::CCompress()
{   m_pCompress = new CDslZipLibCompress( this );
}

CCompress::~CCompress()
{   if ( m_pCompress )
    {   delete (CDslZipLibCompress*)m_pCompress;
        m_pCompress = oexNULL;
    } // end if
    Destroy();
}

CStr CCompress::Compress()
{   const char *pErr = ( (CDslZipLibCompress*)m_pCompress )->Compress();
    if ( !pErr ) return "";
    return pErr;
}

unsigned CCompress::OnRead( char *buf, unsigned size )
{
    // From file?
    if ( m_fInput.IsOpen() )
    {   oexUINT uRead = 0;
        if ( !m_fInput.Read( buf, size, &uRead ) )
            return 0;
        return uRead;
    } // end if

    // From string
    oexINT nCopied = str::Copy( buf, size, m_sInput.Ptr(), m_sInput.Length() );
    if ( 0 > nCopied ) return 0;

    // Trim from the string
    m_sInput.LTrim( nCopied );
    return nCopied;
}

unsigned CCompress::OnWrite( const char *buf, unsigned *size )
{
    // To file?
    if ( m_fOutput.IsOpen() )
    {   oexUINT uWritten = 0;
        if ( !m_fOutput.Write( buf, *size, &uWritten ) )
            return 0;
        *size = 0;
        return uWritten;
    } // end if

    unsigned t = *size;
    m_sOutput.Append( buf, t );
    *size = 0;
    return t;
}



#endif // DSL_ENABLE_ZIP