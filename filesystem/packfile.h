//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef PACKFILE_H
#define PACKFILE_H

#ifdef _WIN32
#pragma once
#endif

#include "basefilesystem.h"
#include "tier1/refcount.h"
#include "tier1/utlbuffer.h"
#include "tier1/lzmaDecoder.h"

class CPackFile;

// A pack file handle - essentially represents a file inside the pack file.  
// Note: This is notionally a subclass of CFileHandle, but it's not polymorphic.
class CPackFileHandle
{
public:
	CPackFileHandle( CPackFile* pOwner, int64 nBase, unsigned int nLength, unsigned int nIndex = -1, unsigned int nFilePointer = 0 );
	~CPackFileHandle();

	int				Read( void* pBuffer, int nDestSize, int nBytes );
	int				Seek( int nOffset, int nWhence );
	int				Tell() { return m_nFilePointer; };
	int				Size() { return m_nLength; };

	void			SetBufferSize( int nBytes );
	int				GetSectorSize();
	int64			AbsoluteBaseOffset();

protected:
	int64			m_nBase;			// Base offset of the file inside the pack file.
	unsigned int	m_nFilePointer;		// Current seek pointer (0 based from the beginning of the file).
	CPackFile*		m_pOwner;			// Pack file that owns this handle
	unsigned int	m_nLength;			// Length of this file.
	unsigned int	m_nIndex;			// Index into the pack's directory table
};

//-----------------------------------------------------------------------------

// An abstract pack file
class CPackFile : public CRefCounted<CRefCountServiceMT>
{
public:
	CPackFile();
	virtual ~CPackFile();

	// The means by which you open files:
	virtual CFileHandle *OpenFile( const char *pFileName, const char *pOptions = "rb" );

	// The two functions a pack file must provide
	virtual bool Prepare( int64 fileLen = -1, int64 nFileOfs = 0 ) = 0;
	virtual bool FindFile( const char *pFilename,  int &nIndex, int64 &nPosition, int &nLength ) = 0;

	// This is the core IO routine for reading anything from a pack file, everything should go through here at some point
	virtual int ReadFromPack( int nIndex, void* buffer, int nDestBytes, int nBytes, int64 nOffset );

	// Returns the filename for a given file in the pack. Returns true if a filename is found, otherwise buffer is filled with "unknown"
	virtual bool IndexToFilename( int nIndex, char* buffer, int nBufferSize ) = 0;

	inline int GetSectorSize();

	virtual void SetupPreloadData() {}
	virtual void DiscardPreloadData() {}
	virtual int64 GetPackFileBaseOffset() = 0;

	CBaseFileSystem *FileSystem() { return m_fs; }

	// Note: threading model for pack files assumes that data
	// is segmented into pack files that aggregate files
	// meant to be read in one thread. Performance characteristics
	// tuned for that case
	CThreadFastMutex	m_mutex;

	// Path management:
	void SetPath( const CUtlSymbol &path ) { m_Path = path; }
	const CUtlSymbol& GetPath() const	{ Assert( m_Path != UTL_INVAL_SYMBOL ); return m_Path; }
	CUtlSymbol			m_Path;

	// possibly embedded pack
	int64				m_nBaseOffset;

	CUtlString			m_ZipName;

	bool				m_bIsMapPath;
	long				m_lPackFileTime;

	int					m_refCount;
	int					m_nOpenFiles;

	FILE				*m_hPackFileHandleFS;
#if defined( SUPPORT_PACKED_STORE )
	CPackedStoreFileHandle m_hPackFileHandleVPK;
#endif
	bool				m_bIsExcluded;

	int					m_PackFileID;
protected:
	int64				m_FileLength;
	CBaseFileSystem		*m_fs;

	friend class		CPackFileHandle;
};

class CZipPackFile : public CPackFile
{
public:
	CZipPackFile( CBaseFileSystem* fs, void *pSection = NULL );
	~CZipPackFile();

	// Loads the pack file
	virtual bool Prepare( int64 fileLen = -1, int64 nFileOfs = 0 );
	virtual bool FindFile( const char *pFilename, int &nIndex, int64 &nOffset, int &nLength );
	virtual int  ReadFromPack( int nIndex, void* buffer, int nDestBytes, int nBytes, int64 nOffset  );

	int64 GetPackFileBaseOffset() { return m_nBaseOffset; }

	bool IndexToFilename( int nIndex, char *pBuffer, int nBufferSize );

protected:
	#pragma pack(1)

	typedef struct
	{
		char name[ 112 ];
		int64 filepos;
		int64 filelen;
	} packfile64_t;

	typedef struct
	{
		char id[ 4 ];
		int64 dirofs;
		int64 dirlen;
	} packheader64_t;

	typedef struct
	{
		char id[ 8 ];
		int64 packheaderpos;
		int64 originalfilesize;
	} packappenededheader_t;

	#pragma pack()

	// A Pack file directory entry:
	class CPackFileEntry
	{
	public:
		unsigned int		m_nPosition;
		unsigned int		m_nLength;
		unsigned int		m_HashName;
		unsigned short		m_nPreloadIdx;
		unsigned short		pad;
		FileNameHandle_t	m_hFileName;
	};

	class CPackFileLessFunc
	{
	public:
		bool Less( CPackFileEntry const& src1, CPackFileEntry const& src2, void *pCtx );
	};

	// Entries to the individual files stored inside the pack file.
	CUtlSortVector< CPackFileEntry, CPackFileLessFunc > m_PackFiles;

	bool						GetOffsetAndLength( const char *FileName, int &nBaseIndex, int64 &nFileOffset, int &nLength );

	// Preload Support
	void						SetupPreloadData();	
	void						DiscardPreloadData();	
	ZIP_PreloadDirectoryEntry*	GetPreloadEntry( int nEntryIndex );

	int64						m_nPreloadSectionOffset;
	unsigned int				m_nPreloadSectionSize;
	ZIP_PreloadHeader			*m_pPreloadHeader;
	unsigned short*				m_pPreloadRemapTable;
	ZIP_PreloadDirectoryEntry	*m_pPreloadDirectory;
	void*						m_pPreloadData;
	CByteswap					m_swap;

	// a section is an update zip held in memory, it has only a preload section
	void						*m_pSection;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CPackFile::CPackFile()
{
	m_FileLength = 0;
	m_hPackFileHandleFS = NULL;
#if defined( SUPPORT_PACKED_STORE )
	m_hPackFileHandleVPK = CPackedStoreFileHandle();
#endif
	m_fs = NULL;
	m_nBaseOffset = 0;
	m_bIsMapPath = false;
	m_lPackFileTime = 0L;
	m_refCount = 0;
	m_nOpenFiles = 0;
	m_PackFileID = 0;
}

inline CPackFile::~CPackFile()
{
	if ( m_nOpenFiles )
	{
		Error( "Closing pack file with %d open files!\n", m_nOpenFiles );
	}

	if ( m_hPackFileHandleFS )
	{
		m_fs->FS_fclose( m_hPackFileHandleFS );
		m_hPackFileHandleFS = NULL;
	}

#if defined( SUPPORT_PACKED_STORE )
	if ( m_hPackFileHandleVPK )
	{
		m_hPackFileHandleVPK = CPackedStoreFileHandle();
	}
#endif

	m_fs->m_ZipFiles.FindAndRemove( this );
}

inline int CPackFile::GetSectorSize()
{
	if ( m_hPackFileHandleFS )
	{
		return m_fs->FS_GetSectorSize( m_hPackFileHandleFS );
	}
#if defined( SUPPORT_PACKED_STORE )
	else if ( m_hPackFileHandleVPK )
	{
		return 2048;
	}
#endif
	else
	{
		return -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CPackFileHandle::CPackFileHandle( CPackFile* pOwner, int64 nBase, unsigned int nLength, unsigned int nIndex, unsigned int nFilePointer )
{
	m_pOwner = pOwner;
	m_nBase = nBase;
	m_nLength = nLength;
	m_nIndex = nIndex;
	m_nFilePointer = nFilePointer;
	pOwner->AddRef();
}

inline CPackFileHandle::~CPackFileHandle()
{
	m_pOwner->m_mutex.Lock();
	--m_pOwner->m_nOpenFiles;
	if ( m_pOwner->m_nOpenFiles == 0 && m_pOwner->m_bIsMapPath )
	{
		if ( m_pOwner->m_hPackFileHandleFS )
		{
			m_pOwner->FileSystem()->Trace_FClose( m_pOwner->m_hPackFileHandleFS );
			m_pOwner->m_hPackFileHandleFS = NULL;
		}
#if defined( SUPPORT_PACKED_STORE )
		if ( m_pOwner->m_hPackFileHandleVPK )
		{
			m_pOwner->m_hPackFileHandleVPK = CPackedStoreFileHandle();
		}
#endif
	}
	m_pOwner->Release();
	m_pOwner->m_mutex.Unlock();
}

inline void CPackFileHandle::SetBufferSize( int nBytes ) 
{
	if ( m_pOwner->m_hPackFileHandleFS )
	{
		m_pOwner->FileSystem()->FS_setbufsize( m_pOwner->m_hPackFileHandleFS, nBytes );
	}
}

inline int CPackFileHandle::GetSectorSize() 
{ 
	return m_pOwner->GetSectorSize(); 
}

inline int64 CPackFileHandle::AbsoluteBaseOffset() 
{ 
	return m_pOwner->GetPackFileBaseOffset() + m_nBase;
}

// Read a bit of the file from the pack file:
inline int CPackFileHandle::Read( void* pBuffer, int nDestSize, int nBytes )
{
	// Clamp nBytes to not go past the end of the file (async is still possible due to nDestSize)
	if ( nBytes + m_nFilePointer > m_nLength )
	{
		nBytes = m_nLength - m_nFilePointer;
	}

	// Seek to the given file pointer and read
	int nBytesRead = m_pOwner->ReadFromPack( m_nIndex, pBuffer, nDestSize, nBytes, m_nBase + m_nFilePointer );

	m_nFilePointer += nBytesRead;

	return nBytesRead;
}

// Seek around inside the pack:
inline int CPackFileHandle::Seek( int nOffset, int nWhence )
{
	if ( nWhence == SEEK_SET )
	{
		m_nFilePointer = nOffset;
	}
	else if ( nWhence == SEEK_CUR )
	{
		m_nFilePointer += nOffset;
	}
	else if ( nWhence == SEEK_END )
	{
		m_nFilePointer = m_nLength + nOffset;
	}

	// Clamp the file pointer to the actual bounds of the file:
	if ( m_nFilePointer > m_nLength )
	{
		m_nFilePointer = m_nLength;
	}

	return m_nFilePointer;
}

#endif // PACKFILE_H
