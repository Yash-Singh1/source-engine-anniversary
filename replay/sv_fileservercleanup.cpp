//========= Copyright Valve Corporation, All rights reserved. ============//
//
//=======================================================================================//

#include "sv_fileservercleanup.h"
#include "sv_replaycontext.h"
#include "sv_recordingsession.h"
#include "spew.h"

#include "replay_dbg.h"
#include "fmtstr.h"
#include "curl/curl.h"

#undef AddJob

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//----------------------------------------------------------------------------------------

#if _DEBUG
ConVar replay_fileserver_simulate_delete( "replay_fileserver_simulate_delete", "0", FCVAR_GAMEDLL, "Don't delete any actual files during replay cleanup." );
#endif

//----------------------------------------------------------------------------------------

IFileserverCleanerJob *SV_CastJobToIFileserverCleanerJob( CBaseJob *pJob )
{
	IFileserverCleanerJob *pResult = dynamic_cast< IFileserverCleanerJob * >( pJob );
	AssertMsg( pResult != NULL, "Cast failed!  Are you sure this job is an IFileserverCleanerJob?" );
	return pResult;
}

//----------------------------------------------------------------------------------------

CFileserverCleaner::CFileserverCleaner()
:	m_bRunning( false ),
	m_bPrintResult( false ),
	m_pCleanerJob( NULL ),
	m_pSpewer( NULL ),
	m_nNumFilesDeleted( 0 )
{
}

void CFileserverCleaner::MarkFileForDelete( const char *pFilename )
{
	if ( m_bRunning )
		return;

	// Create cleaner job now if need be
	if ( !m_pCleanerJob )
	{
		m_pCleanerJob = SV_CreateDeleteFileJob();
		m_nNumFilesDeleted = 0;
	}

	IFileserverCleanerJob *pCleanerJobImp = SV_CastJobToIFileserverCleanerJob( m_pCleanerJob );
	AssertMsg( pCleanerJobImp != NULL, "This cast should always work!" );
	if ( pCleanerJobImp )
	{
		pCleanerJobImp->AddFileForDelete( pFilename );
	}
}

#line 74
void CFileserverCleaner::BlockForCompletion()
{
	if ( !m_bRunning )
		return;

	if ( !m_pCleanerJob )
		return;

	m_pCleanerJob->WaitForFinish();

	Clear();
}

void CFileserverCleaner::DoCleanAsynchronous( bool bPrintResult/*=false*/, ISpewer *pSpewer/*=g_pDefaultSpewer*/ )
{
	if ( m_bRunning )
		return;

	if ( !m_pCleanerJob )
		return;

	m_pSpewer = pSpewer;
	m_bPrintResult = bPrintResult;
	m_bRunning = true;

	SV_GetThreadPool()->AddJob( m_pCleanerJob );
}

void CFileserverCleaner::Clear()
{
	m_pCleanerJob->Release();
	m_pCleanerJob = NULL;

	m_bPrintResult = false;
	m_bRunning = false;
}

void CFileserverCleaner::Think()
{
	CBaseThinker::Think();

	if ( !m_bRunning )
		return;

	if ( !m_pCleanerJob->IsFinished() )
		return;

	IFileserverCleanerJob *pCleanerJobImp = SV_CastJobToIFileserverCleanerJob( m_pCleanerJob );
	if ( pCleanerJobImp )
	{
		m_nNumFilesDeleted += pCleanerJobImp->GetNumFilesDeleted();
	}

	PrintResult();
	Clear();
}

void CFileserverCleaner::PrintResult()
{
	if ( !m_bPrintResult || !m_pSpewer )
		return;

	m_pSpewer->PrintEmptyLine();

	const int nNumFilesRemoved = SV_GetFileserverCleaner()->GetNumFilesDeleted();
	m_pSpewer->PrintValue( "Number of files removed", Replay_va( "%i", nNumFilesRemoved ) );

	m_pSpewer->PrintBlockEnd();
}


float CFileserverCleaner::GetNextThinkTime() const
{
	return 0.0f;
}

//----------------------------------------------------------------------------------------

CLocalFileDeleterJob::CLocalFileDeleterJob()
:	m_nNumDeleted( 0 )
{
}

void CLocalFileDeleterJob::AddFileForDelete( const char *pFilename )
{
	CFmtStr fmtFullFilename( "%s%s", g_pServerReplayContext->GetLocalFileServerPath(), pFilename );
	m_vecFiles.CopyAndAddToTail( fmtFullFilename.Access() );
}

JobStatus_t	CLocalFileDeleterJob::DoExecute()
{
	bool bResult = true;

	FOR_EACH_VEC( m_vecFiles, i )
	{
		const char *pCurFilename = m_vecFiles[ i ];

		// File exists?
		PrintEventStartMsg( "File exists?" );
		if ( !g_pFullFileSystem->FileExists( pCurFilename ) )
		{
			CFmtStr fmtError( "File '%s' does not exist", pCurFilename );
			SetError( ERROR_FILE_DOES_NOT_EXIST, fmtError.Access() );	// TODO: This will only catch the last filename
			PrintEventResult( false );
			bResult = false;
			continue;
		}
		PrintEventResult( true );

		// Delete the file
		PrintEventStartMsg( "Deleting file" );
		g_pFullFileSystem->RemoveFile( pCurFilename );

		// File gone?
		const bool bDeleted = !g_pFullFileSystem->FileExists( pCurFilename );
		PrintEventResult( bDeleted );

		// Increment # deleted if appropriate
		if ( bDeleted )
		{
			++m_nNumDeleted;
		}

		bResult = bResult && bDeleted;
	}

	return bResult ? JOB_OK : JOB_FAILED;
}

//----------------------------------------------------------------------------------------

CLocalFileDeleterJob *SV_CreateLocalFileDeleterJob()
{
	return new CLocalFileDeleterJob();
}

//----------------------------------------------------------------------------------------

CFtpFileDeleterJob::CFtpFileDeleterJob( const char *pHostname, const char *pRemotePath, const char *pLogin,
										const char *pPassword, int nPort, const char *pProxyHostname, int nProxyPort )
:	m_pCommands( NULL ),
	m_nNumDeleted( 0 ),
	m_pFtp( NULL )
{
	m_pFtp = new CFtp( pHostname, pRemotePath, pLogin, pPassword, nPort, this, g_pSimpleSpewer );

	if ( pProxyHostname )
	{
		m_pFtp->UseProxy( pProxyHostname, nProxyPort );
	}
}

void CFtpFileDeleterJob::DoCleanup()
{
	delete m_pFtp;
	m_pFtp = NULL;
}

JobStatus_t CFtpFileDeleterJob::DoExecute()
{
	return m_pFtp->Go() ? JOB_OK : JOB_FAILED;
}

const char *CFtpFileDeleterJob::FTP_GetFilename() const
{
	// The deletes are sent as pre-transfer commands, so the transfer itself needs a filename that won't exist
	return ".dummyfilename";
}

void CFtpFileDeleterJob::AddFileForDelete( const char *pFilename )
{
	++m_nNumDeleted;

	// The '*' prefix tells curl to ignore a failure for this command
	CFmtStr fmtCommand( "*DELE %s", pFilename );
	IF_REPLAY_DBG2( Warning( "Adding FTP command: %s\n", fmtCommand.Access() ) );
	m_pCommands = curl_slist_append( m_pCommands, fmtCommand.Access() );
}

bool CFtpFileDeleterJob::FTP_SetupAdditionalCurlOperations( void *pCurl )
{
	return curl_easy_setopt( pCurl, CURLOPT_PREQUOTE, m_pCommands ) == CURLE_OK;
}

void CFtpFileDeleterJob::FTP_OnPreExecuteCurlOperations()
{
	PrintEventStartMsg( "Logging in and deleting" );
}

void CFtpFileDeleterJob::FTP_OnPostExecuteCurlOperations()
{
	curl_slist_free_all( m_pCommands );
}

bool CFtpFileDeleterJob::FTP_IsCurlResultOK( int nCurlResult ) const
{
	// Downloading the dummy file is expected to fail
	const bool bResult = nCurlResult == CURLE_OK || nCurlResult == CURLE_REMOTE_FILE_NOT_FOUND;
	PrintEventResult( bResult );
	return bResult;
}

void CFtpFileDeleterJob::FTP_SetError( int nError, const char *pError )
{
	SetError( nError, pError );
}

//----------------------------------------------------------------------------------------

CFtpFileDeleterJob *SV_CreateFtpFileDeleterJob( const char *pFilename )
{
	extern ConVar replay_fileserver_offload_remotepath;
	extern ConVar replay_fileserver_offload_login;
	extern ConVar replay_fileserver_offload_password;
	extern ConVar replay_fileserver_offload_port;
	extern ConVar replay_fileserver_offload_useproxy;
	extern ConVar replay_fileserver_offload_proxy_port;

	CFtpFileDeleterJob *pJob = new CFtpFileDeleterJob(
		SV_GetFileserverIP(),
		replay_fileserver_offload_remotepath.GetString(),
		replay_fileserver_offload_login.GetString(),
		replay_fileserver_offload_password.GetString(),
		replay_fileserver_offload_port.GetInt(),
		replay_fileserver_offload_useproxy.GetBool() ? SV_GetFileserverProxyIP() : NULL,
		replay_fileserver_offload_proxy_port.GetInt()
	);

	if ( pFilename )
	{
		pJob->AddFileForDelete( pFilename );
	}

	return pJob;
}

CBaseJob *SV_CreateDeleteFileJob()
{
	extern ConVar replay_fileserver_offload_enable;
	if ( replay_fileserver_offload_enable.GetBool() )
		return SV_CreateFtpFileDeleterJob( NULL );

	return SV_CreateLocalFileDeleterJob();
}
#line 309


//----------------------------------------------------------------------------------------

bool SV_DoFileserverCleanup( bool bForceCleanAll, ISpewer *pSpewer )
{
	CServerRecordingSessionManager *pSessionManager = SV_GetRecordingSessionManager();
	CBaseRecordingSession *pRecordingSession = SV_GetRecordingSessionInProgress();

	for ( int i = 0; i < pSessionManager->Count(); )
	{
		CServerRecordingSession *pCurSession = SV_CastSession( SV_GetRecordingSessionManager()->m_vecObjs[ i ] );

		// Skip session in progress
		bool bRemoved = false;
		if ( pCurSession != NULL && pCurSession != pRecordingSession )
		{
			// Session expired?
			if ( bForceCleanAll || pCurSession->SessionExpired() )
			{
				// The session's OnDelete() will add the session file to the cleanup system,
				// and also delete all associated blocks, whose OnDelete() will also add their
				// associated .block files to the cleanup system.
				pSessionManager->RemoveFromIndex( i );

				bRemoved = true;
			}
		}

		if ( !bRemoved )
		{
			++i;
		}
	}

	pSpewer->PrintBlockStart();

	pSpewer->PrintMsg( "Attempting to clean up stale replay data..." );
	pSpewer->PrintEmptyLine();

	// NOTE: There may be files queued up in addition to those marked above
	if ( !SV_GetFileserverCleaner()->HasFilesQueuedForDelete() )
	{
		pSpewer->PrintMsg( "No replay data to clean up." );
		pSpewer->PrintBlockEnd();
	}
	else
	{
		// Asynchronously delete all collected files
		SV_GetFileserverCleaner()->DoCleanAsynchronous( true, g_pBlockSpewer );
	}

	return true;
}

//----------------------------------------------------------------------------------------
