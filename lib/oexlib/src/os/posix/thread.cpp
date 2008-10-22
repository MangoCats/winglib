/*------------------------------------------------------------------
// thread.cpp
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

/*
						   ,-.        _.---._
						  |  `\.__.-''       `.
						   \  _        _  ,.   \
	 ,+++=._________________)_||______|_|_||    |
	(_.ooo.===================||======|=|=||    |
	   ~~'                 |  ~'      `~' o o  /
							\   /~`\     o o  /
							 `~'    `-.____.-'
*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Ensure our type is large enough to hold the thread value
//oexSTATIC_ASSERT( sizeof( CThread::t_HTHREAD ) == sizeof( HANDLE ) );

/// Invalid thread handle value.
oexCONST CThread::t_HTHREAD CThread::c_InvalidThread = oexNULL;

CThread::CThread()
{
    m_pData = 0;
    m_uSleep = 0;
    m_hThread = oexNULL;
    m_uThreadId = 0;
    oexZeroMemory( &m_td, sizeof( m_td ) );
}

CThread::~CThread()
{   Stop();
}

//==============================================================
// CThread::CThreadProcImpl
//==============================================================
/// This is just a function stub that calls CThread::ThreadProc()
/**
	\param [in] x_pData		-	CThread class pointer

	I'm just going to leave this adapter in here in case the
	thread signature ever includes custom variables.

	\return Thread return value
*/
class CThread::CThreadProcImpl
{
public:

	static oexPVOID ThreadProc( oexPVOID x_pData )
	{	return CThread::ThreadProc( x_pData );
	}

};

oexPVOID CThread::ThreadProc( oexPVOID x_pData )
{
    // Get pointer
    CThread *pThread = (CThread*)x_pData;
    if ( !oexVERIFY_PTR( x_pData ) )
        return (oexPVOID)-1;

    // Count one thread
    CThread::IncThreadCount();
    CThread::IncRunningThreadCount();

    // Get user params
    oexPVOID pData = pThread->GetUserData();
    oexUINT uSleep = pThread->GetSleepTime();
    oexUINT uThreadId = (oexUINT)pThread->GetThreadHandle();

    // Verify thread handle
    // If you get stopped here,
    // you're thread was shutdown before it could start
    oexINT nRet = -1111;
//    if ( oexVERIFY( uThreadId ) )
    {
        // Initialize thread
        if ( pThread->InitThread( pData ) )
        {
            // Ensure they haven't already killed us
            if ( oexVERIFY( !pThread->m_evQuit.Wait( 0 ) ) )
            {
                // Signal that we're initialized
                pThread->m_evInit.Set();

                // *** Now it's ok to get shutdown ***

                // Do the work
                while ( pThread->DoThread( pData ) &&
                        !pThread->m_evQuit.Wait( uSleep ) );

                // Kill the thread
                nRet = pThread->EndThread( pData );

            } // end if

            // Early shutdown
            else
            	nRet = -3333;

        } // end if

    } // end if

    // Early shutdown
//    else nRet = -2222;

    /// Decrement the running thread count
    CThread::DecRunningThreadCount();

    return (oexPVOID)nRet;

}

oexBOOL CThread::Start( oexUINT x_uSleep, oexPVOID x_pData )
{
    // Are we already running?
    if ( IsRunning() )
        return oexTRUE;

    // Save users data
    m_pData = x_pData;
    m_uSleep = x_uSleep;

    // Give the thread a fighting chance
    m_evQuit.Reset();
    m_evInit.Reset();

	// Allocate thread data
	m_td.pContext = (oexPVOID)OexAllocNew< pthread_t >( 1 );
	if ( !m_td.pContext )
		return oexFALSE;

	// Create the thread
	m_hThread = (oexPVOID)pthread_create( (pthread_t*)m_td.pContext, NULL,
									      CThreadProcImpl::ThreadProc, this );

/*
	// Create a thread
	m_hThread = CreateThread(	(LPSECURITY_ATTRIBUTES)NULL,
								0,
                                CThreadProcImpl::ThreadProc,
								(LPVOID)this,
								0,
								(LPDWORD)&m_uThreadId );


	a = b / ( b - c )

	b = ac/(a-1)

*/
    // Developer will probably want to hear about this
    oexASSERT( c_InvalidThread != m_hThread);

    // Did we get our thread?
    if ( c_InvalidThread == m_hThread )
    {   Stop(); m_uThreadId = 0; return oexFALSE; }

    return oexTRUE;
}

oexBOOL CThread::Stop( oexBOOL x_bKill, oexUINT x_uWait )
{
    // Save the thread handle
    t_HTHREAD hThread = m_hThread;

    // Valid thread?
    if ( c_InvalidThread == hThread || !m_td.pContext )
        return oexTRUE;

    // Wait for thread to completely initialize
    oexVERIFY( !x_uWait || m_evInit.Wait( x_uWait ) );

    // Clear thread data
    m_pData = 0;
    m_uSleep = 0;
    m_hThread = oexNULL;
    m_uThreadId = 0;

    // Tell the thread to stop
    m_evQuit.Set();

    // Punt if we don't want to wait for the thread do shutdown
    if ( !x_uWait )
        return oexTRUE;

    // Wait for the thead to exit
	if ( pthread_join( *(pthread_t*)m_td.pContext, oexNULL ) )
    {
        // iii  This should not happen, don't ignore the problem,
        //      figure out how to shut this thread down properly!
		oexTRACE( oexT( "!! TerminateThread() being called !!\n" ) );

        // Kill the thread
        pthread_cancel( *(pthread_t*)m_td.pContext );

    } // end if

	if ( m_td.pContext )
		OexAllocDelete( (pthread_t*)m_td.pContext );

    oexZeroMemory( &m_td, sizeof( m_td ) );

    return oexTRUE;
}

// The number of threads running
oexLONG CThread::m_lThreadCount = 0;
oexLONG CThread::m_lRunningThreadCount = 0;

oexUINT CThread::GetThreadCount()
{   return m_lThreadCount; }

oexUINT CThread::GetRunningThreadCount()
{   return m_lRunningThreadCount; }

void CThread::IncThreadCount()
{   CSys::InterlockedIncrement( &m_lThreadCount ); }

void CThread::IncRunningThreadCount()
{   CSys::InterlockedIncrement( &m_lRunningThreadCount ); }

void CThread::DecRunningThreadCount()
{   if ( oexVERIFY( m_lRunningThreadCount ) )
        CSys::InterlockedDecrement( &m_lRunningThreadCount );
}


