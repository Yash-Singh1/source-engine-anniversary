//========= Copyright Valve Corporation, All rights reserved. ============//
//
//=======================================================================================//

#include "sv_ftp.h"
#include "sv_replaycontext.h"
#include "replay/shared_defs.h"
#include "replay_dbg.h"
#include "convar.h"
#include "fmtstr.h"
#include "strtools.h"
#include "curl/curl.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//----------------------------------------------------------------------------------------

ConVar replay_publish_simulate_delay_ftp( "replay_publish_simulate_delay_ftp", "0", FCVAR_DONTRECORD,
	"Simulate a delay (in seconds) when publishing replay data via FTP.", true, 0.0f, true, 60.0f );

extern ConVar replay_fileserver_offload_remotepath;
extern ConVar replay_fileserver_offload_login;
extern ConVar replay_fileserver_offload_password;
extern ConVar replay_fileserver_offload_port;
extern ConVar replay_fileserver_offload_useproxy;
extern ConVar replay_fileserver_offload_proxy_port;

//----------------------------------------------------------------------------------------

enum FtpError_t
{
	ERROR_CURL_INIT_FAILED = 1000,
};

//----------------------------------------------------------------------------------------

CFtp::CFtp( const char *pHostname, const char *pRemotePath, const char *pLogin, const char *pPassword,
		    int nPort, IFtp *pCallbacks, ISpewer *pSpewer/*=g_pDefaultSpewer*/ )
:	CBaseSpewer( pSpewer ),
	m_bUseProxy( false ),
	m_nProxyPort( 0 )
{
	V_strncpy( m_szHostname, pHostname, sizeof( m_szHostname ) );
	V_strncpy( m_szRemotePath, pRemotePath, sizeof( m_szRemotePath ) );
	V_strncpy( m_szLogin, pLogin, sizeof( m_szLogin ) );
	V_strncpy( m_szPassword, pPassword, sizeof( m_szPassword ) );
	m_nPort = nPort;
	m_pCallbacks = pCallbacks;
	m_szProxyHostname[0] = '\0';
}

void CFtp::UseProxy( const char *pProxyHostname, int nProxyPort )
{
	m_bUseProxy = true;
	V_strncpy( m_szProxyHostname, pProxyHostname, sizeof( m_szProxyHostname ) );
	m_nProxyPort = nProxyPort;
}

bool CFtp::Go()
{
	PrintEventStartMsg( "Initializing FTP" );
	CURL *pCurl = curl_easy_init();
	if ( !pCurl )
	{
		PrintEventResult( false );
		FTP_SetError( ERROR_CURL_INIT_FAILED, "Curl init failed" );
		return false;
	}
	PrintEventResult( true );

	// Don't double up the slash if the remote path is just the root
	CFmtStr fmtURL( "ftp://%s:%i%s%s%s", m_szHostname, m_nPort, m_szRemotePath,
		V_strlen( m_szRemotePath ) == 1 ? "" : "/", FTP_GetFilename() );
	CFmtStr fmtUserPwd( "%s:%s", m_szLogin, m_szPassword );

	curl_easy_setopt( pCurl, CURLOPT_URL, fmtURL.Access() );
	curl_easy_setopt( pCurl, CURLOPT_USERPWD, fmtUserPwd.Access() );

	if ( m_bUseProxy )
	{
		CFmtStr fmtProxy( "%s:%i", m_szProxyHostname, m_nProxyPort );
		curl_easy_setopt( pCurl, CURLOPT_PROXY, fmtProxy.Access() );
	}

	if ( !FTP_SetupAdditionalCurlOperations( pCurl ) )
		return false;

	FTP_OnPreExecuteCurlOperations();

	const CURLcode nResult = curl_easy_perform( pCurl );
	curl_easy_cleanup( pCurl );

	FTP_OnPostExecuteCurlOperations();

	if ( !FTP_IsCurlResultOK( nResult ) )
	{
		PrintEventResult( false );
		FTP_SetError( nResult, curl_easy_strerror( nResult ) );
		DBG( "Upload failed!\n" );
		return false;
	}

	IF_REPLAY_DBG( Warning( "FTP operation succeeded for: %s\n", fmtURL.Access() ) );
	PrintEventResult( true );

	return true;
}

size_t CFtp::ReadCallback( void *pPtr, size_t nSize, size_t nNumItems, void *pStream )
{
	return fread( pPtr, nSize, nNumItems, (FILE *)pStream );
}

bool CFtp::FTP_IsCurlResultOK( int nCurlResult ) const
{
	if ( m_pCallbacks )
		return m_pCallbacks->FTP_IsCurlResultOK( nCurlResult );

	return nCurlResult == CURLE_OK;
}

const char *CFtp::FTP_GetFilename() const
{
	if ( m_pCallbacks )
		return m_pCallbacks->FTP_GetFilename();

	return "";
}

bool CFtp::FTP_SetupAdditionalCurlOperations( void *pCurl )
{
	if ( m_pCallbacks )
		return m_pCallbacks->FTP_SetupAdditionalCurlOperations( pCurl );

	return true;
}

void CFtp::FTP_OnPreExecuteCurlOperations()
{
	if ( m_pCallbacks )
	{
		m_pCallbacks->FTP_OnPreExecuteCurlOperations();
	}
}

void CFtp::FTP_OnPostExecuteCurlOperations()
{
	if ( m_pCallbacks )
	{
		m_pCallbacks->FTP_OnPostExecuteCurlOperations();
	}
}

void CFtp::FTP_SetError( int nError, const char *pError )
{
	if ( m_pCallbacks )
	{
		m_pCallbacks->FTP_SetError( nError, pError );
	}
}

//----------------------------------------------------------------------------------------

CFtpUploadJob::CFtpUploadJob( const char *pLocalFilename, const char *pHostname, const char *pRemotePath,
							  const char *pLogin, const char *pPassword, int nPort, const char *pProxyHostname,
							  int nProxyPort )
:	m_pFile( NULL )
{
	SetFlags( GetFlags() | JF_IO );

	V_strcpy( m_szLocalFilename, pLocalFilename );

	m_pFtp = new CFtp( pHostname, pRemotePath, pLogin, pPassword, nPort, this );

	if ( pProxyHostname )
	{
		m_pFtp->UseProxy( pProxyHostname, nProxyPort );
	}
}

void CFtpUploadJob::DoCleanup()
{
	delete m_pFtp;
	m_pFtp = NULL;
}

JobStatus_t CFtpUploadJob::DoExecute()
{
	SimulateDelay( replay_publish_simulate_delay_ftp.GetInt(), "FTP" );

	return m_pFtp->Go() ? JOB_OK : JOB_FAILED;
}

bool CFtpUploadJob::FTP_SetupAdditionalCurlOperations( void *pCurl )
{
	PrintEventStartMsg( "Getting file size" );
	const unsigned int nFileSize = g_pFullFileSystem->Size( m_szLocalFilename );
	if ( !nFileSize )
	{
		PrintEventResult( false );
		CFmtStr fmtError( "Failed to get file size for file '%s'\n", m_szLocalFilename );
		SetError( ERROR_FAILED_TO_OPEN_FILE, fmtError.Access() );
		return false;
	}
	PrintEventResult( true );

	PrintEventStartMsg( "Opening local file for read" );
	m_pFile = fopen( m_szLocalFilename, "rb" );
	if ( !m_pFile )
	{
		PrintEventResult( false );
		CFmtStr fmtError( "Failed to open input file '%s': %s\n", m_szLocalFilename, strerror( errno ) );
		SetError( ERROR_FAILED_TO_OPEN_FILE, fmtError.Access() );
		return false;
	}
	PrintEventResult( true );

	curl_easy_setopt( pCurl, CURLOPT_READFUNCTION, CFtp::ReadCallback );
	curl_easy_setopt( pCurl, CURLOPT_UPLOAD, 1L );
	curl_easy_setopt( pCurl, CURLOPT_READDATA, m_pFile );
	curl_easy_setopt( pCurl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)nFileSize );

	return true;
}

void CFtpUploadJob::FTP_OnPreExecuteCurlOperations()
{
	PrintEventStartMsg( "Logging in and uploading" );
}

const char *CFtpUploadJob::FTP_GetFilename() const
{
	return V_UnqualifiedFileName( m_szLocalFilename );
}

bool CFtpUploadJob::FTP_IsCurlResultOK( int nCurlResult ) const
{
	return nCurlResult == CURLE_OK;
}

void CFtpUploadJob::FTP_OnPostExecuteCurlOperations()
{
	if ( fclose( m_pFile ) )
	{
		CFmtStr fmtError( "Failed to close file: %s", strerror( errno ) );
		PrintEventError( fmtError.Access() );
	}
}

void CFtpUploadJob::FTP_SetError( int nError, const char *pError )
{
	SetError( nError, pError );
}

//----------------------------------------------------------------------------------------

CFtpUploadJob *SV_CreateFtpUploadJob( const char *pLocalFilename )
{
	return new CFtpUploadJob(
		pLocalFilename,
		SV_GetFileserverIP(),
		replay_fileserver_offload_remotepath.GetString(),
		replay_fileserver_offload_login.GetString(),
		replay_fileserver_offload_password.GetString(),
		replay_fileserver_offload_port.GetInt(),
		replay_fileserver_offload_useproxy.GetBool() ? SV_GetFileserverProxyIP() : NULL,
		replay_fileserver_offload_proxy_port.GetInt()
	);
}

//----------------------------------------------------------------------------------------
