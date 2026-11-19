//========= Copyright Valve Corporation, All rights reserved. ============//
//
//=======================================================================================//

#ifndef SV_FTP_H
#define SV_FTP_H
#ifdef _WIN32
#pragma once
#endif

//----------------------------------------------------------------------------------------

#include "spew.h"
#include "sv_filepublish.h"
#include <stdio.h>

//----------------------------------------------------------------------------------------

//
// Callbacks a CFtp uses to customize a transfer.
//
class IFtp
{
public:
	virtual bool		FTP_IsCurlResultOK( int nCurlResult ) const = 0;
	virtual const char	*FTP_GetFilename() const = 0;
	virtual bool		FTP_SetupAdditionalCurlOperations( void *pCurl ) = 0;
	virtual void		FTP_OnPreExecuteCurlOperations() = 0;
	virtual void		FTP_OnPostExecuteCurlOperations() = 0;
	virtual void		FTP_SetError( int nError, const char *pError ) = 0;
};

//----------------------------------------------------------------------------------------

class CFtp : public IFtp,
			 public CBaseSpewer
{
public:
	CFtp( const char *pHostname, const char *pRemotePath, const char *pLogin, const char *pPassword,
		  int nPort, IFtp *pCallbacks, ISpewer *pSpewer = g_pDefaultSpewer );

	void				UseProxy( const char *pProxyHostname, int nProxyPort );

	bool				Go();

	static size_t		ReadCallback( void *pPtr, size_t nSize, size_t nNumItems, void *pStream );

	//
	// IFtp - forwards to the callbacks passed in, if any
	//
	virtual bool		FTP_IsCurlResultOK( int nCurlResult ) const;
	virtual const char	*FTP_GetFilename() const;
	virtual bool		FTP_SetupAdditionalCurlOperations( void *pCurl );
	virtual void		FTP_OnPreExecuteCurlOperations();
	virtual void		FTP_OnPostExecuteCurlOperations();
	virtual void		FTP_SetError( int nError, const char *pError );

private:
	char				m_szHostname[MAX_OSPATH];
	char				m_szRemotePath[MAX_OSPATH];
	char				m_szLogin[MAX_OSPATH];
	char				m_szPassword[MAX_OSPATH];
	int					m_nPort;
	bool				m_bUseProxy;
	int					m_nProxyPort;
	char				m_szProxyHostname[MAX_OSPATH];
	IFtp				*m_pCallbacks;
};

//----------------------------------------------------------------------------------------

class CFtpUploadJob : public CBasePublishJob,
					  public IFtp
{
public:
	CFtpUploadJob( const char *pLocalFilename, const char *pHostname, const char *pRemotePath,
				   const char *pLogin, const char *pPassword, int nPort, const char *pProxyHostname,
				   int nProxyPort );

	enum FtpUploadError_t
	{
		ERROR_FAILED_TO_OPEN_FILE = 1001,
	};

private:
	virtual void		DoCleanup();
	virtual JobStatus_t	DoExecute();

	//
	// IFtp
	//
	virtual const char	*FTP_GetFilename() const;
	virtual bool		FTP_IsCurlResultOK( int nCurlResult ) const;
	virtual bool		FTP_SetupAdditionalCurlOperations( void *pCurl );
	virtual void		FTP_OnPreExecuteCurlOperations();
	virtual void		FTP_OnPostExecuteCurlOperations();
	virtual void		FTP_SetError( int nError, const char *pError );

	FILE				*m_pFile;
	char				m_szLocalFilename[MAX_OSPATH];
	CFtp				*m_pFtp;
};

CFtpUploadJob *SV_CreateFtpUploadJob( const char *pLocalFilename );

//----------------------------------------------------------------------------------------

#endif	// SV_FTP_H
