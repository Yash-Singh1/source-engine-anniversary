//========= Copyright Valve Corporation, All rights reserved. ============//
//
//=======================================================================================//

#ifndef SV_FILESERVERCLEANUP_H
#define SV_FILESERVERCLEANUP_H
#ifdef _WIN32
#pragma once
#endif

//----------------------------------------------------------------------------------------

#include "basethinker.h"
#include "spew.h"
#include "sv_basejob.h"
#include "sv_ftp.h"

//----------------------------------------------------------------------------------------

bool		SV_DoFileserverCleanup( bool bForceCleanAll, ISpewer *pSpewer/*=g_pDefaultSpewer*/ );
CBaseJob *SV_CreateDeleteFileJob();

//----------------------------------------------------------------------------------------

class IFileserverCleanerJob
{
public:
	virtual ~IFileserverCleanerJob() {}

	virtual void		AddFileForDelete( const char *pFilename ) = 0;
	virtual int			GetNumFilesDeleted() const = 0;
};

IFileserverCleanerJob *SV_CastJobToIFileserverCleanerJob( CBaseJob *pJob );

//----------------------------------------------------------------------------------------

class CFileserverCleaner : public CBaseThinker
{
public:
	CFileserverCleaner();

	void				MarkFileForDelete( const char *pFilename );

	int					GetNumFilesDeleted() const		{ return m_nNumFilesDeleted; }
	bool				HasFilesQueuedForDelete() const	{ return m_pCleanerJob != NULL; }

	void				BlockForCompletion();
	void				DoCleanAsynchronous( bool bPrintResult = false, ISpewer *pSpewer = g_pDefaultSpewer );

private:
	void				Clear();
	void				PrintResult();

	virtual void		Think();
	virtual float		GetNextThinkTime() const;

	CBaseJob			*m_pCleanerJob;
	bool				m_bRunning;
	bool				m_bPrintResult;
	int					m_nNumFilesDeleted;
	ISpewer				*m_pSpewer;
};

//----------------------------------------------------------------------------------------

class CLocalFileDeleterJob : public CBaseJob,
							 public IFileserverCleanerJob
{
public:
	CLocalFileDeleterJob();

	virtual void		AddFileForDelete( const char *pFilename );
	virtual int			GetNumFilesDeleted() const	{ return m_nNumDeleted; }

	enum DeleteError_t
	{
		ERROR_FILE_DOES_NOT_EXIST,
	};

private:
	virtual JobStatus_t	DoExecute();

	CUtlStringList		m_vecFiles;
	int					m_nNumDeleted;
};

CLocalFileDeleterJob *SV_CreateLocalFileDeleterJob();

//----------------------------------------------------------------------------------------

class CFtpFileDeleterJob : public CBaseJob,
						   public IFileserverCleanerJob,
						   public IFtp
{
public:
	CFtpFileDeleterJob( const char *pHostname, const char *pRemotePath, const char *pLogin,
						const char *pPassword, int nPort, const char *pProxyHostname, int nProxyPort );

	virtual void		AddFileForDelete( const char *pFilename );
	virtual int			GetNumFilesDeleted() const	{ return m_nNumDeleted; }

private:
	virtual void		DoCleanup();
	virtual JobStatus_t	DoExecute();

	//
	// IFtp
	//
	virtual const char	*FTP_GetFilename() const;
	virtual bool		FTP_SetupAdditionalCurlOperations( void *pCurl );
	virtual void		FTP_OnPreExecuteCurlOperations();
	virtual void		FTP_OnPostExecuteCurlOperations();
	virtual bool		FTP_IsCurlResultOK( int nCurlResult ) const;
	virtual void		FTP_SetError( int nError, const char *pError );

	char				m_szUnused[2][MAX_OSPATH];	// Never read or written, but part of the retail layout
	struct curl_slist	*m_pCommands;
	int					m_nNumDeleted;
	CFtp				*m_pFtp;
};

CFtpFileDeleterJob *SV_CreateFtpFileDeleterJob( const char *pFilename );

//----------------------------------------------------------------------------------------

#endif	// SV_FILESERVERCLEANUP_H
