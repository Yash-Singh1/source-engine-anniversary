//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "basefilesystem.h"
#include "tier0/vprof.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

#if !defined( DEDICATED )

#ifdef SUPPORT_PACKED_STORE

uintp ThreadStubProcessMD5Requests( void *pParam )
{
	return ((CFileTracker2 *)pParam)->ThreadedProcessMD5Requests();
}

//-----------------------------------------------------------------------------
// ThreadedProcessMD5Requests
// Calculate the MD5s of all the blocks submitted to us
//-----------------------------------------------------------------------------
unsigned CFileTracker2::ThreadedProcessMD5Requests()
{
	while ( m_bThreadShouldRun )
	{
		StuffToMD5_t stuff;

		while ( m_PendingJobs.PopItem( &stuff ) )
		{
			tmZone( TELEMETRY_LEVEL0, TMZF_NONE, "%s", __FUNCTION__ ); 

			MD5Context_t ctx;
			memset( &ctx, 0, sizeof(MD5Context_t) );
			MD5Init( &ctx );
			MD5Update( &ctx, stuff.m_pubBuffer, stuff.m_cubBuffer );
			MD5Final( stuff.m_md5Value.bits, &ctx);

			{
				TrackedVPKFile_t trackedVPKFileFind;
				trackedVPKFileFind.m_nPackFileNumber = stuff.m_nPackFileNumber;
				trackedVPKFileFind.m_PackFileID = stuff.m_PackFileID;
				trackedVPKFileFind.m_nFileFraction = stuff.m_nPackFileFraction;

				// update the FileTracker MD5 database
				AUTO_LOCK( m_Mutex );

				int idxTrackedVPKFile = m_treeTrackedVPKFiles.Find( trackedVPKFileFind );
				if ( idxTrackedVPKFile != m_treeTrackedVPKFiles.InvalidIndex() )
				{
					TrackedVPKFile_t &trackedVPKFile = m_treeTrackedVPKFiles[ idxTrackedVPKFile ];
					TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ trackedVPKFile.m_idxAllOpenedFiles ];

					memcpy( trackedfile.m_filehashFinal.m_md5contents.bits, stuff.m_md5Value.bits, sizeof( trackedfile.m_filehashFinal.m_md5contents.bits ) );
					trackedfile.m_filehashFinal.m_crcIOSequence = stuff.m_cubBuffer;
					trackedfile.m_filehashFinal.m_cbFileLen = stuff.m_cubBuffer;
					trackedfile.m_filehashFinal.m_eFileHashType = FileHash_t::k_EFileHashTypeEntireFile;
					trackedfile.m_filehashFinal.m_nPackFileNumber = trackedVPKFileFind.m_nPackFileNumber;
					trackedfile.m_filehashFinal.m_PackFileID = trackedVPKFileFind.m_PackFileID;
				}
			}

			m_CompletedJobs.PushItem( stuff );
			m_threadEventWorkCompleted.Set();
		}

		{
			tmZone( TELEMETRY_LEVEL0, TMZF_IDLE, "m_threadEventWorkToDo" ); 

			m_threadEventWorkToDo.Wait( 1000 );
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// SubmitThreadedMD5Request
// add pubBuffer,cubBuffer to our queue of stuff to MD5
// caller promises that the memory will remain valid
// until BlockUntilMD5RequestComplete() is called
// returns: request handle
//-----------------------------------------------------------------------------
int CFileTracker2::SubmitThreadedMD5Request( uint8 *pubBuffer, int cubBuffer, int PackFileID, int nPackFileNumber, int nPackFileFraction )
{
	int idxList;
	StuffToMD5_t stuff;

	{
		AUTO_LOCK( m_Mutex );

		if ( !m_bComputeFileHashes )
			return 0;

		TrackedVPKFile_t trackedVPKFileFind;
		trackedVPKFileFind.m_nPackFileNumber = nPackFileNumber;
		trackedVPKFileFind.m_PackFileID = PackFileID;
		trackedVPKFileFind.m_nFileFraction = nPackFileFraction;

		int idxTrackedVPKFile = m_treeTrackedVPKFiles.Find( trackedVPKFileFind );
		if ( idxTrackedVPKFile == m_treeTrackedVPKFiles.InvalidIndex() )
		{
			// we don't know about this chunk, nothing to do
			return 0;
		}

		SubmittedMd5Job_t submittedjob;
		submittedjob.m_bFinished = false;
		idxList = m_SubmittedJobs.AddToTail( submittedjob );

		stuff.m_pubBuffer = pubBuffer;
		stuff.m_cubBuffer = cubBuffer;
		stuff.m_PackFileID = PackFileID;
		stuff.m_nPackFileNumber = nPackFileNumber;
		stuff.m_nPackFileFraction = nPackFileFraction;
		stuff.m_idxListSubmittedJobs = idxList;
	}

	// submit the work
	m_PendingJobs.PushItem( stuff );
	m_threadEventWorkToDo.Set();

	return idxList + 1;
}

//-----------------------------------------------------------------------------
// IsMD5RequestComplete
// is request identified by iRequest finished?
// ( the caller wants to free the memory, but now must wait until we finish
// calculating the MD5 )
//-----------------------------------------------------------------------------
bool CFileTracker2::IsMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut )
{
	AUTO_LOCK( m_Mutex );
	int idxListWaiting = iRequest - 1;

	// deal with all completed jobs
	StuffToMD5_t stuff;
	while ( m_CompletedJobs.PopItem( &stuff ) )
	{
		int idxList = stuff.m_idxListSubmittedJobs;
		Q_memcpy( &m_SubmittedJobs[ idxList ].m_md5Value, &stuff.m_md5Value, sizeof( MD5Value_t ) );
		m_SubmittedJobs[ idxList ].m_bFinished = true;
	}

	// did the one we wanted finish?
	if ( m_SubmittedJobs[ idxListWaiting ].m_bFinished )
	{
		Q_memcpy( pMd5ValueOut, &m_SubmittedJobs[ idxListWaiting ].m_md5Value, sizeof( MD5Value_t ) );
		// you can not ask again, we have removed it from the list
		m_SubmittedJobs.Remove(idxListWaiting);
		return true;
	}

	// not done yet
	return false;
}

//-----------------------------------------------------------------------------
// BlockUntilMD5RequestComplete
// block until request identified by iRequest is finished
// ( the caller wants to free the memory, but now must wait until we finish
// calculating the MD5 )
//-----------------------------------------------------------------------------
bool CFileTracker2::BlockUntilMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut )
{
	while ( 1 )
	{
		if ( IsMD5RequestComplete( iRequest, pMd5ValueOut ) )
			return true;
		m_cThreadBlocks++;
		m_threadEventWorkCompleted.Wait( 1 );
	}
	return false;
}

#endif // SUPPORT_PACKED_STORE

CFileTracker2::CFileTracker2( CBaseFileSystem *pFileSystem ):
	m_treeAllOpenedFiles( TrackedFile_t::Less ),
	m_mapAllOpenFiles( DefLessFunc( FILE * ) ),
	m_treeFileInVPK( FileInVPK_t::Less ),
	m_treeTrackedVPKFiles( TrackedVPKFile_t::Less )
{
#if defined( DEDICATED )
    Assert( 0 );
#endif

	m_pFileSystem = pFileSystem;
	m_cMissedReads = 0;
	m_bComputeFileHashes = true;
	m_cThreadBlocks = 0;

#ifdef SUPPORT_PACKED_STORE
	m_bThreadShouldRun = true;
	m_hWorkThread = CreateSimpleThread( ThreadStubProcessMD5Requests, this );
#endif
}

CFileTracker2::~CFileTracker2()
{
}

void CFileTracker2::ShutdownAsync()
{
#ifdef SUPPORT_PACKED_STORE
	m_bThreadShouldRun = false;
	m_threadEventWorkToDo.Set();
	// wait for it to die
	ThreadJoin( m_hWorkThread );
	ReleaseThreadHandle( m_hWorkThread );
	m_hWorkThread = NULL;
#endif
}

void CFileTracker2::MarkAllCRCsUnverified()
{
	AUTO_LOCK( m_Mutex );

	m_RecentFileList.Purge();

	// Files we have read from go first, VPK chunks ahead of loose files
	for ( int i = m_treeAllOpenedFiles.FirstInorder(); i != m_treeAllOpenedFiles.InvalidIndex(); i = m_treeAllOpenedFiles.NextInorder( i ) )
	{
		TrackedFile_t &file = m_treeAllOpenedFiles[ i ];
		if ( file.m_filehashInProgress.m_cbFileLen || file.m_filehashFinal.m_cbFileLen )
		{
			if ( file.m_bPackOrVPKFile )
			{
				file.m_idxRecentFileList = m_RecentFileList.AddToHead( i );
				continue;
			}
			if ( !file.m_bFileInVPK && file.m_path.Length() )
			{
				file.m_idxRecentFileList = m_RecentFileList.AddToTail( i );
				continue;
			}
		}
		file.m_idxRecentFileList = m_RecentFileList.InvalidIndex();
	}

	// Then everything else
	for ( int i = m_treeAllOpenedFiles.FirstInorder(); i != m_treeAllOpenedFiles.InvalidIndex(); i = m_treeAllOpenedFiles.NextInorder( i ) )
	{
		TrackedFile_t &file = m_treeAllOpenedFiles[ i ];
		if ( file.m_idxRecentFileList != m_RecentFileList.InvalidIndex() )
			continue;

		if ( file.m_bPackOrVPKFile )
			file.m_idxRecentFileList = m_RecentFileList.AddToTail( i );
		else if ( !file.m_bFileInVPK && file.m_path.Length() )
			file.m_idxRecentFileList = m_RecentFileList.AddToTail( i );
		else
			file.m_idxRecentFileList = m_RecentFileList.InvalidIndex();
	}
}

int CFileTracker2::GetUnverifiedFileHashes( CUnverifiedFileHash *pFiles, int nMaxFiles )
{
	AUTO_LOCK( m_Mutex );

	int iOutFile = 0;
	while ( m_RecentFileList.Head() != m_RecentFileList.InvalidIndex() )
	{
		int idxList = m_RecentFileList.Head();
		int idxFile = m_RecentFileList[ idxList ];
		TrackedFile_t &file = m_treeAllOpenedFiles[ idxFile ];

		if ( !file.m_filehashInProgress.m_cbFileLen && !file.m_filehashFinal.m_cbFileLen )
		{
			// Not read yet, try again later
			m_RecentFileList.Unlink( idxList );
			file.m_idxRecentFileList = m_RecentFileList.AddToTail( idxFile );
			break;
		}

		m_RecentFileList.Unlink( idxList );
		file.m_idxRecentFileList = m_RecentFileList.InvalidIndex();

		if ( !file.m_path.Length() )
			continue;

		CUnverifiedFileHash &outFile = pFiles[ iOutFile ];
		V_strncpy( outFile.m_Filename, file.m_filename.String(), sizeof( outFile.m_Filename ) );
		V_strncpy( outFile.m_PathID, file.m_path.String(), sizeof( outFile.m_PathID ) );
		outFile.m_nFileFraction = file.m_nFileFraction;
		file.GetFileHash( &outFile.m_FileHash );

		iOutFile++;
		if ( iOutFile >= nMaxFiles )
			break;
	}

	return iOutFile;
}

EFileCRCStatus CFileTracker2::CheckCachedFileHash( const char *pPathID, const char *pRelativeFilename, int nFileFraction, FileHash_t *pFileHash )
{
	AUTO_LOCK( m_Mutex );

	CRC32_t crcFilename;
	CRC32_Init( &crcFilename );
	CRC32_ProcessBuffer( &crcFilename, pRelativeFilename, Q_strlen( pRelativeFilename ) );
	CRC32_ProcessBuffer( &crcFilename, pPathID, Q_strlen( pPathID ) );
	CRC32_Final( &crcFilename );

	TrackedFile_t trackedfileFind;
	trackedfileFind.m_filename = pRelativeFilename;
	trackedfileFind.m_path = pPathID;
	trackedfileFind.m_crcIdentifier = crcFilename;
	trackedfileFind.m_nFileFraction = nFileFraction;

	int idx = m_treeAllOpenedFiles.Find( trackedfileFind );
	if ( idx != m_treeAllOpenedFiles.InvalidIndex() )
	{
		TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idx ];

		if ( trackedfile.m_bFileInVPK )
		{
			// the FileHash is not meaningful, because the file is in a VPK, we have hashed the entire VPK
			// if the user is sending us a hash for this file, it means he has extracted it from the VPK and tricked the client into loading it
			// instead of the version in the VPK.
			return k_eFileCRCStatus_FileInVPK;
		}

		trackedfile.GetFileHash( pFileHash );
		return k_eFileCRCStatus_GotCRC;
	}

	return k_eFileCRCStatus_CantOpenFile;
}

void TrackedFile_t::RebuildFileName( const char *pFilename, const char *pPathID )
{
	char szFixedName[ MAX_PATH ];

	V_strncpy( szFixedName, pFilename, sizeof( szFixedName ) );
	V_RemoveDotSlashes( szFixedName );
	V_FixSlashes( szFixedName );
	V_strlower( szFixedName ); // !KLUDGE!
	m_filename = szFixedName;

	m_path = pPathID;
	V_strupr( (char *)m_path.String() ); // !KLUDGE!

	CRC32_t crcFilename;
	CRC32_Init( &crcFilename );
	CRC32_ProcessBuffer( &crcFilename, szFixedName, Q_strlen( szFixedName ) );
	CRC32_ProcessBuffer( &crcFilename, m_path.String(), m_path.Length() );
	CRC32_Final( &crcFilename );

	m_crcIdentifier = crcFilename;
}

//-----------------------------------------------------------------------------
// ProcessFileRead
// Hash the data as it is read. A single in-order pass over the whole file
// gives the hash of the entire file; anything else gives a hash of the
// read sequence (the seek positions are folded into the CRC).
//-----------------------------------------------------------------------------
void TrackedFile_t::ProcessFileRead( void *dest, size_t nBytesRead )
{
	if ( m_filehashFinal.m_eFileHashType == FileHash_t::k_EFileHashTypeEntireFile )
		return;

	m_filehashInProgress.m_cbFileLen += nBytesRead;

	if ( m_nFilePos == 0 && m_nLength == nBytesRead )
	{
		// reading the whole file in one go, start over
		memset( &m_md5ctx, 0, sizeof( m_md5ctx ) );
		MD5Init( &m_md5ctx );
	}

	MD5Update( &m_md5ctx, (unsigned char *)dest, nBytesRead );

	if ( m_nFilePos == m_nMD5Pos )
		m_nMD5Pos += nBytesRead;

	if ( ( m_nFilePos == 0 && m_nLength == nBytesRead ) ||
		( m_nLength == m_nMD5Pos && m_nMD5Pos == m_filehashInProgress.m_cbFileLen ) )
	{
		MD5Final( m_filehashInProgress.m_md5contents.bits, &m_md5ctx );
		m_filehashInProgress.m_crcIOSequence = m_nLength;
		m_filehashInProgress.m_eFileHashType = FileHash_t::k_EFileHashTypeEntireFile;
		m_filehashFinal = m_filehashInProgress;
	}
	else
	{
		CRC32_ProcessBuffer( &m_filehashInProgress.m_crcIOSequence, &m_nFilePos, sizeof( m_nFilePos ) );
	}

	m_nFilePos += nBytesRead;
}

#ifdef SUPPORT_PACKED_STORE

void CFileTracker2::NotePackFileAccess( const char *pFilename, const char *pPathID, int iSearchPathStoreId, CPackedStoreFileHandle &VPKHandle )
{
#if !defined( _GAMECONSOLE ) && !defined( DEDICATED )
	AUTO_LOCK( m_Mutex );

	// The VPK must have been opened through NotePackFileOpened
	if ( !m_treeAllOpenedFiles.IsValidIndex( VPKHandle.m_pOwner->m_PackFileID - 1 ) )
		return;

	int idxFile = IdxFileFromName( pFilename, pPathID, 0, VPKHandle.m_nFileSize, false, false );
	TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];

	// we could use the CRC data from the VPK header - and verify it
	// VPKHandle.GetFileCRCFromHeaderData();
	// for now all we are going to do is track that this file came from a VPK
	trackedfile.m_nLength = VPKHandle.m_nFileSize;
	trackedfile.m_PackFileID = VPKHandle.m_pOwner->m_PackFileID;
	trackedfile.m_nPackFileNumber = VPKHandle.m_nFileNumber; // this might be useful to send up
	trackedfile.m_iLoadedSearchPathStoreId = iSearchPathStoreId;
	trackedfile.m_bFileInVPK = true;

	FileInVPK_t fileInVPK;
	fileInVPK.m_PackFileID = VPKHandle.m_pOwner->m_PackFileID;
	fileInVPK.m_nPackFileNumber = VPKHandle.m_nFileNumber;
	fileInVPK.m_nFileOffset = VPKHandle.m_nFileOffset;
	fileInVPK.m_idxAllOpenedFiles = idxFile;
	if ( m_treeFileInVPK.Find( fileInVPK ) == m_treeFileInVPK.InvalidIndex() )
		m_treeFileInVPK.Insert( fileInVPK );

	// Track every chunk fraction this file touches
	TrackedVPKFile_t trackedVPKFile;
	trackedVPKFile.m_nPackFileNumber = VPKHandle.m_nFileNumber;
	trackedVPKFile.m_PackFileID = VPKHandle.m_pOwner->m_PackFileID;
	trackedVPKFile.m_nFileFraction = VPKHandle.m_nFileOffset & ~( k_cbFileFraction - 1 );
	int nFileFractionLast = ( VPKHandle.m_nFileOffset + VPKHandle.m_nFileSize ) & ~( k_cbFileFraction - 1 );
	for ( ; trackedVPKFile.m_nFileFraction <= nFileFractionLast; trackedVPKFile.m_nFileFraction += k_cbFileFraction )
	{
		if ( m_treeTrackedVPKFiles.Find( trackedVPKFile ) != m_treeTrackedVPKFiles.InvalidIndex() )
			continue;

		char szDataFileName[MAX_PATH];
		VPKHandle.m_pOwner->GetPackFileName( VPKHandle, szDataFileName, sizeof(szDataFileName) );
		const char *pszFileName = V_GetFileName( szDataFileName );

		trackedVPKFile.m_idxAllOpenedFiles = IdxFileFromName( pszFileName, "GAME", trackedVPKFile.m_nFileFraction, 0, true, true );
		m_treeTrackedVPKFiles.Insert( trackedVPKFile );
	}
#endif // !defined( _GAMECONSOLE ) && !defined( DEDICATED )
}

#endif // SUPPORT_PACKED_STORE

int CFileTracker2::IdxFileFromName( const char *pFilename, const char *pPathID, int nFileFraction, int64 nLength, bool bPackOrVPKFile, bool bRecordInRecentList )
{
	TrackedFile_t trackedfile;

	trackedfile.RebuildFileName( pFilename, pPathID );
	trackedfile.m_nLength = nLength;
	trackedfile.m_bPackOrVPKFile = bPackOrVPKFile;
	trackedfile.m_nFileFraction = nFileFraction;
	MD5Init( &trackedfile.m_md5ctx );
	CRC32_Init( &trackedfile.m_filehashInProgress.m_crcIOSequence );
	trackedfile.m_idxRecentFileList = m_RecentFileList.InvalidIndex();

	int idxFile = m_treeAllOpenedFiles.Find( trackedfile );
	if ( idxFile == m_treeAllOpenedFiles.InvalidIndex() )
	{
		idxFile = m_treeAllOpenedFiles.Insert( trackedfile );
		if ( bRecordInRecentList )
			m_treeAllOpenedFiles[ idxFile ].m_idxRecentFileList = m_RecentFileList.AddToTail( idxFile );
	}
	else if ( bRecordInRecentList && m_treeAllOpenedFiles[ idxFile ].m_idxRecentFileList == m_RecentFileList.InvalidIndex() )
	{
		m_treeAllOpenedFiles[ idxFile ].m_idxRecentFileList = m_RecentFileList.AddToTail( idxFile );
	}

	return idxFile;
}

struct FileListToUnloadForWhitelistChange : public IFileList
{
	virtual bool IsFileInList( const char *pFilename )
	{
		char szFixedName[ MAX_PATH ];
		GetFixedName( pFilename, szFixedName );
		return m_dictFiles.Find( szFixedName ) >= 0;
	}

	virtual void Release()
	{
		delete this;
	}

	void AddFile( const char *pszFilename )
	{
		char szFixedName[ MAX_PATH ];
		GetFixedName( pszFilename, szFixedName );
		if ( m_dictFiles.Find( szFixedName ) < 0 )
			m_dictFiles.Insert( szFixedName );
	}

	void GetFixedName( const char *pszFilename, char *pszFixedName )
	{
		V_strncpy( pszFixedName, pszFilename, MAX_PATH );
		V_strlower( pszFixedName );
		V_FixSlashes( pszFixedName );
	}

	CUtlDict<int> m_dictFiles;
};

IFileList *CFileTracker2::GetFilesToUnloadForWhitelistChange( IPureServerWhitelist *pNewWhiteList )
{
	FileListToUnloadForWhitelistChange *pResult = new FileListToUnloadForWhitelistChange;

	for ( int i = m_treeAllOpenedFiles.FirstInorder() ; i >= 0 ; i = m_treeAllOpenedFiles.NextInorder( i ) )
	{
		TrackedFile_t &f = m_treeAllOpenedFiles[i];

		// !KLUDGE! If we ignored it at all, just reload it.
		// This is more conservative than we need to be, but the set of files we are ignoring is probably
		// pretty small so it should be fine.
		if ( f.m_bIgnoredForPureServer )
		{
			f.m_bIgnoredForPureServer = false;
#ifdef PURE_SERVER_DEBUG_SPEW
			Msg( "%s was ignored for pure server purposes.  Queuing for reload\n", f.m_filename.String() );
#endif
			pResult->AddFile( f.m_filename );
			continue;
		}

		if ( f.m_iLoadedSearchPathStoreId != 0 && pNewWhiteList && pNewWhiteList->GetFileClass( f.m_filename ) == ePureServerFileClass_AnyTrusted )
		{
			// Check if we loaded it from a path that no longer exists or is no longer trusted
			const CBaseFileSystem::CSearchPath *pSearchPath = m_pFileSystem->FindSearchPathByStoreId( f.m_iLoadedSearchPathStoreId );
			if ( pSearchPath == NULL )
			{
#ifdef PURE_SERVER_DEBUG_SPEW
				Msg( "%s was loaded from search path that's no longer mounted.  Queuing for reload\n", f.m_filename.String() );
#endif
				pResult->AddFile( f.m_filename );
			}
			else if ( !pSearchPath->m_bIsTrustedForPureServer )
			{
#ifdef PURE_SERVER_DEBUG_SPEW
				Msg( "%s was loaded from search path that's not currently trusted.  Queuing for reload\n", f.m_filename.String() );
#endif
				pResult->AddFile( f.m_filename );
			}
			else
			{
#if defined( _DEBUG ) && defined( PURE_SERVER_DEBUG_SPEW )
				Msg( "%s is OK.  Keeping\n", f.m_filename.String() );
#endif
			}
		}
	}

	// Do we need to reload anything?
	if ( pResult->m_dictFiles.Count() > 0 )
		return pResult;

	// Nothing to reload, return an empty list as an optimization
	pResult->Release();
	return NULL;
}

#ifdef SUPPORT_PACKED_STORE

void CFileTracker2::AddFileHashForVPKFile( int nPackFileNumber, int nFileFraction, int cbFileLen, MD5Value_t &md5, CPackedStoreFileHandle &VPKHandle )
{
#if !defined( DEDICATED )
	AUTO_LOCK( m_Mutex );

	// We have the chunk hashes now, no need to compute them ourselves
	m_bComputeFileHashes = false;

	char szDataFileName[MAX_PATH];
	VPKHandle.m_nFileNumber = nPackFileNumber;
	VPKHandle.m_pOwner->GetPackFileName( VPKHandle, szDataFileName, sizeof(szDataFileName) );
	const char *pszFileName = V_GetFileName( szDataFileName );

	TrackedVPKFile_t trackedVPKFile;
	trackedVPKFile.m_nPackFileNumber = VPKHandle.m_nFileNumber;
	trackedVPKFile.m_PackFileID = VPKHandle.m_pOwner->m_PackFileID;
	trackedVPKFile.m_nFileFraction = nFileFraction;
	int idxFile = IdxFileFromName( pszFileName, "GAME", nFileFraction, 0, true, true );
	trackedVPKFile.m_idxAllOpenedFiles = idxFile;

	if ( idxFile != m_treeAllOpenedFiles.InvalidIndex() )
		m_treeTrackedVPKFiles.Insert( trackedVPKFile );

	TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];
    // These set in IdxFileFromName:
    //   trackedfile.m_crcIdentifier
    //   trackedfile.m_filename
    //   trackedfile.m_path
    //   trackedfile.m_bPackOrVPKFile
    //   trackedfile.m_nFileFraction
    // Not set:
    //   trackedfile.m_iLoadedSearchPathStoreId
    //   trackedfile.m_bIgnoredForPureServer
	trackedfile.m_bFileInVPK = false;
	trackedfile.m_bPackOrVPKFile = true;
	trackedfile.m_nLength = k_cbFileFraction;
	trackedfile.m_filehashFinal.m_cbFileLen = cbFileLen;
	trackedfile.m_filehashFinal.m_eFileHashType = FileHash_t::k_EFileHashTypeEntireFile;
	trackedfile.m_filehashFinal.m_nPackFileNumber = nPackFileNumber;
	trackedfile.m_filehashFinal.m_PackFileID = VPKHandle.m_pOwner->m_PackFileID;
	trackedfile.m_filehashFinal.m_crcIOSequence = cbFileLen;
	Q_memcpy( trackedfile.m_filehashFinal.m_md5contents.bits, md5.bits, sizeof( md5.bits) );
#endif // !DEDICATED
}

int CFileTracker2::NotePackFileOpened( const char *pVPKAbsPath, const char *pPathID, int64 nLength )
{
#if !defined( _GAMECONSOLE )
	AUTO_LOCK( m_Mutex );

	int idxFile = IdxFileFromName( pVPKAbsPath, pPathID, 0, nLength, true, false );

	TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];
	// we have the real name we want to use. correct the name
	trackedfile.m_bPackOrVPKFile = true;
	trackedfile.m_PackFileID = idxFile + 1;
	trackedfile.m_filehashFinal.m_PackFileID = trackedfile.m_PackFileID;
	trackedfile.m_filehashFinal.m_nPackFileNumber = -1;
	trackedfile.m_filehashInProgress.m_PackFileID = trackedfile.m_PackFileID;
	trackedfile.m_filehashInProgress.m_nPackFileNumber = -1;
	m_treeAllOpenedFiles.Reinsert( idxFile );
	return idxFile + 1;
#else
	return 0;
#endif
}

#endif // SUPPORT_PACKED_STORE

void CFileTracker2::NoteFileIgnoredForPureServer( const char *pFilename, const char *pPathID, int iSearchPathStoreId )
{
#if !defined( _GAMECONSOLE )
	AUTO_LOCK( m_Mutex );

	int idxFile = IdxFileFromName( pFilename, pPathID, 0, 0, false, false );
	m_treeAllOpenedFiles[ idxFile ].m_bIgnoredForPureServer = true;
#endif
}

void CFileTracker2::NoteFileLoadedFromDisk( const char *pFilename, const char *pPathID, int iSearchPathStoreId, FILE *fp, int64 nLength )
{
#if !defined( _GAMECONSOLE ) && !defined( DEDICATED )
	AUTO_LOCK( m_Mutex );

	Assert( iSearchPathStoreId != 0 );

	int idxFile = IdxFileFromName( pFilename, pPathID, 0, nLength, false, true );
	TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];
	trackedfile.m_iLoadedSearchPathStoreId = iSearchPathStoreId;

	// so the reads through this handle get hashed
	m_mapAllOpenFiles.Insert( fp, idxFile );
#endif
}

void CFileTracker2::NoteFileUnloaded( const char *pFilename, const char *pPathID )
{
#if !defined( _GAMECONSOLE )
	AUTO_LOCK( m_Mutex );

	// Locate bookeeping entry, if any
	TrackedFile_t trackedfile;
	trackedfile.RebuildFileName( pFilename, pPathID );

	int idxFile = m_treeAllOpenedFiles.Find( trackedfile );
	if ( idxFile >= 0 )
	{
		// Clear state
		TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];
		trackedfile.m_iLoadedSearchPathStoreId = 0;
		trackedfile.m_bIgnoredForPureServer = false;
	}
#endif
}

void CFileTracker2::RecordFileClose( FILE *fp )
{
	AUTO_LOCK( m_Mutex );

	int idx = m_mapAllOpenFiles.Find( fp );
	if ( idx != m_mapAllOpenFiles.InvalidIndex() )
	{
		TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ m_mapAllOpenFiles[ idx ] ];
		trackedfile.m_nFilePos = 0;
		trackedfile.m_nMD5Pos = 0;
		m_mapAllOpenFiles.RemoveAt( idx );
	}
}

void CFileTracker2::RecordFileSeek( FILE *fp, int64 pos, int seekType )
{
	AUTO_LOCK( m_Mutex );

	int idx = m_mapAllOpenFiles.Find( fp );
	if ( idx == m_mapAllOpenFiles.InvalidIndex() )
		return;

	int idxFile = m_mapAllOpenFiles[ idx ];
	if ( idxFile == m_treeAllOpenedFiles.InvalidIndex() )
		return;

	TrackedFile_t &trackedfile = m_treeAllOpenedFiles[ idxFile ];
	if ( trackedfile.m_filehashFinal.m_eFileHashType == FileHash_t::k_EFileHashTypeEntireFile )
		return;

	if ( seekType == FILESYSTEM_SEEK_HEAD )
	{
		trackedfile.m_nFilePos = pos;
	}
	else if ( seekType == FILESYSTEM_SEEK_TAIL )
	{
		trackedfile.m_nFilePos = trackedfile.m_nLength - pos;
	}
	else if ( seekType == FILESYSTEM_SEEK_CURRENT )
	{
		trackedfile.m_nFilePos += pos;
	}

	CRC32_ProcessBuffer( &trackedfile.m_filehashInProgress.m_crcIOSequence, &pos, sizeof( pos ) );
	CRC32_ProcessBuffer( &trackedfile.m_filehashInProgress.m_crcIOSequence, &seekType, sizeof( seekType ) );
}

void CFileTracker2::RecordFileRead( void *dest, size_t nBytesRead, size_t nBytesRequested, FILE *fp )
{
	AUTO_LOCK( m_Mutex );

	int idx = m_mapAllOpenFiles.Find( fp );
	if ( idx == m_mapAllOpenFiles.InvalidIndex() )
	{
		m_cMissedReads++;
		return;
	}

	int idxFile = m_mapAllOpenFiles[ idx ];
	if ( idxFile != m_treeAllOpenedFiles.InvalidIndex() )
		m_treeAllOpenedFiles[ idxFile ].ProcessFileRead( dest, nBytesRead );
}

#endif // !DEDICATED
