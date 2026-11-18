//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef FILETRACKER_H
#define FILETRACKER_H
#ifdef _WIN32
#pragma once
#endif

class CBaseFileSystem;
class CPackedStoreFileHandle;

#if !defined( DEDICATED )

// Comments from Fletcher:
// 
// TF isn’t sending any hashes to the server. (That’s probably what CSGO is doing,
// but not TF.) Comparing hashes doesn’t work when there are optional updates. (We
// release a new client without updating the server.) Also on TF, we don’t ship
// any textures or audio to the dedicated server.
//  
// On TF, the client just confirms that the files were loaded from a trusted source.
// 
// When a client connects to a “pure” server, the client is supposed to limit
// which modified files are allowed.)

#include "ifilelist.h"
#include "tier1/utldict.h"
#include "tier1/utlmap.h"
#include "tier1/utlstring.h"
#include "tier1/checksum_md5.h"
#include "tier0/tslist.h"

// VPK chunk files are hashed in fractions of this size
const int k_cbFileFraction = 1024 * 1024;

struct TrackedFile_t
{
	TrackedFile_t()
	{
		m_nFileFraction = 0;
		m_iLoadedSearchPathStoreId = 0;
		m_bIgnoredForPureServer = false;
		m_iUnused = 0;
		m_nFilePos = 0;
		m_nLength = 0;
		m_nMD5Pos = 0;
		m_PackFileID = 0;
		m_nPackFileNumber = 0;
		m_bPackOrVPKFile = false;
		m_bFileInVPK = false;
	}

	CRC32_t m_crcIdentifier;	// CRC32 of the filename and path, compared first
	CUtlString m_filename;
	CUtlString m_path;
	int m_nFileFraction;
	int m_iLoadedSearchPathStoreId; // ID of the search path that we loaded from.  Zero if we are not currently loaded.
	bool m_bIgnoredForPureServer; // Did we ignore a file by this name as a result of pure server rules, the last time it was opened?

	MD5Context_t m_md5ctx;			// running MD5 of the bytes read so far
	FileHash_t m_filehashFinal;
	FileHash_t m_filehashInProgress;

	int m_iUnused;
	int64 m_nFilePos;			// current read position
	int64 m_nLength;
	int64 m_nMD5Pos;			// m_md5ctx covers [0,m_nMD5Pos) of the file
	int m_idxRecentFileList;	// Index into CFileTracker2::m_RecentFileList

	int m_PackFileID;
	int m_nPackFileNumber;
	bool m_bPackOrVPKFile;
	bool m_bFileInVPK;

	void RebuildFileName( const char *pFilename, const char *pPathID );
	void ProcessFileRead( void *dest, size_t nBytesRead );

	void GetFileHash( FileHash_t *pFileHash )
	{
		if ( m_filehashFinal.m_eFileHashType == FileHash_t::k_EFileHashTypeEntireFile )
		{
			pFileHash->m_md5contents = m_filehashFinal.m_md5contents;
			pFileHash->m_crcIOSequence = m_filehashFinal.m_crcIOSequence;
			pFileHash->m_eFileHashType = m_filehashFinal.m_eFileHashType;
			pFileHash->m_cbFileLen = m_filehashFinal.m_cbFileLen;
			pFileHash->m_nPackFileNumber = m_filehashFinal.m_nPackFileNumber;
			pFileHash->m_PackFileID = m_filehashFinal.m_PackFileID;
		}
		else
		{
			// hash of the partial read sequence so far
			MD5Context_t ctx;
			Q_memcpy( &ctx, &m_md5ctx, sizeof( ctx ) );
			MD5Final( pFileHash->m_md5contents.bits, &ctx );
			CRC32_t crcIOSequence = m_filehashInProgress.m_crcIOSequence;
			CRC32_Final( &crcIOSequence );
			pFileHash->m_crcIOSequence = crcIOSequence;
			pFileHash->m_eFileHashType = FileHash_t::k_EFileHashTypeIncompleteFile;
			pFileHash->m_cbFileLen = m_filehashInProgress.m_cbFileLen;
			pFileHash->m_nPackFileNumber = m_filehashInProgress.m_nPackFileNumber;
			pFileHash->m_PackFileID = m_filehashInProgress.m_PackFileID;
		}
	}

	static bool Less( const TrackedFile_t& lhs, const TrackedFile_t& rhs )
	{
		if ( lhs.m_crcIdentifier < rhs.m_crcIdentifier )
			return true;
		if ( lhs.m_crcIdentifier > rhs.m_crcIdentifier )
			return false;

		if ( lhs.m_nFileFraction < rhs.m_nFileFraction )
			return true;
		if ( lhs.m_nFileFraction > rhs.m_nFileFraction )
			return false;

		int nCmp = Q_strcmp( lhs.m_filename.Get(), rhs.m_filename.Get() );
		if ( nCmp < 0 )
			return true;
		if ( nCmp > 0 )
			return false;

		nCmp = Q_strcmp( lhs.m_path.Get(), rhs.m_path.Get() );
		return nCmp < 0;
	}
};

struct TrackedVPKFile_t
{
	TrackedVPKFile_t()
	{
		m_PackFileID = 0;
		m_nPackFileNumber = 0;
	}
	int m_PackFileID;
	int m_nPackFileNumber;
	int m_nFileFraction;
	int m_idxAllOpenedFiles; // Index into m_treeAllOpenedFiles

	static bool Less( const TrackedVPKFile_t& lhs, const TrackedVPKFile_t& rhs )
	{
		if ( lhs.m_nPackFileNumber < rhs.m_nPackFileNumber )
			return true;
		else if ( lhs.m_nPackFileNumber > rhs.m_nPackFileNumber )
			return false;

		if ( lhs.m_nFileFraction < rhs.m_nFileFraction )
			return true;
		else if ( lhs.m_nFileFraction > rhs.m_nFileFraction )
			return false;

		if ( lhs.m_PackFileID < rhs.m_PackFileID )
			return true;

		return false;
	}
};

// A file inside a VPK chunk, keyed by its location in the chunk
struct FileInVPK_t
{
	FileInVPK_t()
	{
		m_PackFileID = 0;
		m_nPackFileNumber = 0;
		m_nFileOffset = 0;
	}
	int m_PackFileID;
	int m_nPackFileNumber;
	int m_nFileOffset;
	int m_idxAllOpenedFiles; // Index into m_treeAllOpenedFiles

	static bool Less( const FileInVPK_t& lhs, const FileInVPK_t& rhs )
	{
		if ( lhs.m_nFileOffset < rhs.m_nFileOffset )
			return true;
		else if ( lhs.m_nFileOffset > rhs.m_nFileOffset )
			return false;

		if ( lhs.m_nPackFileNumber < rhs.m_nPackFileNumber )
			return true;
		else if ( lhs.m_nPackFileNumber > rhs.m_nPackFileNumber )
			return false;

		if ( lhs.m_PackFileID < rhs.m_PackFileID )
			return true;

		return false;
	}
};

class StuffToMD5_t
{
public:
	uint8 *m_pubBuffer;
	int m_cubBuffer;
	MD5Value_t m_md5Value;
	int m_PackFileID;
	int m_nPackFileNumber;
	int m_nPackFileFraction;
	int m_idxListSubmittedJobs;
};

class SubmittedMd5Job_t
{
public:
	bool m_bFinished;
	MD5Value_t m_md5Value;
};

class CFileTracker2
#ifdef SUPPORT_PACKED_STORE
	: IThreadedFileMD5Processor
#endif
{
public:
	CFileTracker2( CBaseFileSystem *pFileSystem );
	~CFileTracker2();

	void ShutdownAsync();

	void MarkAllCRCsUnverified();
	int	GetUnverifiedFileHashes( CUnverifiedFileHash *pFiles, int nMaxFiles );
	EFileCRCStatus CheckCachedFileHash( const char *pPathID, const char *pRelativeFilename, int nFileFraction, FileHash_t *pFileHash );

#ifdef SUPPORT_PACKED_STORE
	unsigned ThreadedProcessMD5Requests();
	virtual int SubmitThreadedMD5Request( uint8 *pubBuffer, int cubBuffer, int PackFileID, int nPackFileNumber, int nPackFileFraction );
	virtual bool BlockUntilMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut );
	virtual bool IsMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut );

	int NotePackFileOpened( const char *pVPKAbsPath, const char *pPathID, int64 nLength );
	void NotePackFileAccess( const char *pFilename, const char *pPathID, int iSearchPathStoreId, CPackedStoreFileHandle &VPKHandle );
	void AddFileHashForVPKFile( int nPackFileNumber, int nFileFraction, int cbFileLen, MD5Value_t &md5, CPackedStoreFileHandle &fhandle );
#endif

	void NoteFileIgnoredForPureServer( const char *pFilename, const char *pPathID, int iSearchPathStoreId );
	void NoteFileLoadedFromDisk( const char *pFilename, const char *pPathID, int iSearchPathStoreId, FILE *fp, int64 nLength );
	void NoteFileUnloaded( const char *pFilename, const char *pPathID );

	void RecordFileClose( FILE *fp );
	void RecordFileSeek( FILE *fp, int64 pos, int seekType );
	void RecordFileRead( void *dest, size_t nBytesRead, size_t nBytesRequested, FILE *fp );

	IFileList *GetFilesToUnloadForWhitelistChange( IPureServerWhitelist *pNewWhiteList );

private:
	int IdxFileFromName( const char *pFilename, const char *pPathID, int nFileFraction, int64 nLength, bool bPackOrVPKFile, bool bRecordInRecentList );

	CUtlRBTree< TrackedFile_t, int > m_treeAllOpenedFiles;
	CUtlMap< FILE *, int, int >		m_mapAllOpenFiles;			// open FILE * -> index into m_treeAllOpenedFiles
	CUtlRBTree< FileInVPK_t, int >	m_treeFileInVPK;
	CUtlRBTree< TrackedVPKFile_t, int > m_treeTrackedVPKFiles;
	CUtlLinkedList< int >			m_RecentFileList;			// files whose hashes have not been sent yet, oldest first
	int								m_cMissedReads;				// reads from files we were not tracking

	CBaseFileSystem					*m_pFileSystem;
	CThreadMutex					m_Mutex;	// Threads call into here, so we need to be safe.

	bool							m_bComputeFileHashes;		// cleared once the VPK chunk hashes are known

#ifdef SUPPORT_PACKED_STORE
	CThreadEvent m_threadEventWorkToDo;
	CThreadEvent m_threadEventWorkCompleted;
	volatile bool m_bThreadShouldRun;
	ThreadHandle_t m_hWorkThread;

	CTSQueue< StuffToMD5_t >				m_PendingJobs;
	CTSQueue< StuffToMD5_t >				m_CompletedJobs;
	CUtlLinkedList< SubmittedMd5Job_t >		m_SubmittedJobs;
#endif // SUPPORT_PACKED_STORE

	// Stats
	int m_cThreadBlocks;
};

#else

//
// Dedicated server NULL filetracker. Pretty much does nothing.
//
class CFileTracker2
#ifdef SUPPORT_PACKED_STORE
	: IThreadedFileMD5Processor
#endif
{
public:
	CFileTracker2( CBaseFileSystem *pFileSystem ) {}
	~CFileTracker2() {}

	void ShutdownAsync() {}

	void MarkAllCRCsUnverified() {}
	int	GetUnverifiedFileHashes( CUnverifiedFileHash *pFiles, int nMaxFiles ) { return 0; }
	EFileCRCStatus CheckCachedFileHash( const char *pPathID, const char *pRelativeFilename, int nFileFraction, FileHash_t *pFileHash )
		{ return k_eFileCRCStatus_CantOpenFile; }

#ifdef SUPPORT_PACKED_STORE
	virtual int SubmitThreadedMD5Request( uint8 *pubBuffer, int cubBuffer, int PackFileID, int nPackFileNumber, int nPackFileFraction )
		{ return 0; }
	virtual bool BlockUntilMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut ) { Assert(0); return true; }
	virtual bool IsMD5RequestComplete( int iRequest, MD5Value_t *pMd5ValueOut ) { Assert(0); return true; }

	int NotePackFileOpened( const char *pVPKAbsPath, const char *pPathID, int64 nLength ) { return 0; }
	void NotePackFileAccess( const char *pFilename, const char *pPathID, int iSearchPathStoreId, CPackedStoreFileHandle &VPKHandle ) {}
	void AddFileHashForVPKFile( int nPackFileNumber, int nFileFraction, int cbFileLen, MD5Value_t &md5, CPackedStoreFileHandle &fhandle ) {}
#endif

	void NoteFileIgnoredForPureServer( const char *pFilename, const char *pPathID, int iSearchPathStoreId ) {}
	void NoteFileLoadedFromDisk( const char *pFilename, const char *pPathID, int iSearchPathStoreId, FILE *fp, int64 nLength ) {}
	void NoteFileUnloaded( const char *pFilename, const char *pPathID ) {}

	void RecordFileClose( FILE *fp ) {}
	void RecordFileSeek( FILE *fp, int64 pos, int seekType ) {}
	void RecordFileRead( void *dest, size_t nBytesRead, size_t nBytesRequested, FILE *fp ) {}

	IFileList *GetFilesToUnloadForWhitelistChange( IPureServerWhitelist *pNewWhiteList ) { return NULL; }
};

#endif // DEDICATED

#endif // FILETRACKER_H
