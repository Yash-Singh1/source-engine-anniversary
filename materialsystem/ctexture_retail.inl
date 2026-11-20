//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The 2013 (pre-streaming) CTexture of the retail Half-Life 2 build.
// Included by ctexture.cpp when BYTEMATCH_RETAIL is defined.
//
//=====================================================================================//

#ifdef PROTECTED_THINGS_ENABLE
	#undef PROTECTED_THINGS_ENABLE
#endif

#include "platform.h"

// HACK: Need ShellExecute for PSD updates
#ifdef IS_WINDOWS_PC
#include <windows.h>
#include <shellapi.h>
#pragma comment ( lib, "shell32"  )
#endif

#include "materialsystem_global.h"
#include "shaderapi/ishaderapi.h"
#include "itextureinternal.h"
#include "utlsymbol.h"
#include "time.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "bitmap/imageformat.h"
#include "bitmap/tgaloader.h"
#include "bitmap/tgawriter.h"
#ifdef _WIN32
#include "direct.h"
#endif
#include "colorspace.h"
#include "string.h"
#include <stdlib.h>
#include "utlmemory.h"
#include "IHardwareConfigInternal.h"
#include "filesystem.h"
#include "tier1/strtools.h"
#include "vtf/vtf.h"
#include "materialsystem/materialsystem_config.h"
#include "mempool.h"
#include "texturemanager.h"
#include "utlbuffer.h"
#include "pixelwriter.h"
#include "tier1/callqueue.h"
#include "tier1/UtlStringMap.h"
#include "filesystem/IQueuedLoader.h"
#include "tier2/fileutils.h"
#include "filesystem.h"
#include "tier2/p4helpers.h"
#include "tier2/tier2.h"
#include "p4lib/ip4.h"
#include "ctype.h"
#include "ifilelist.h"
#include "tier0/icommandline.h"
#include "tier0/vprof.h"
#include "tier0/vprof_telemetry.h"

// NOTE: This must be the last file included!!!
#include "tier0/memdbgon.h"

// this allows the command line to force the "all mips" flag to on for all textures
bool g_bForceTextureAllMips = false;

#if defined(IS_WINDOWS_PC)
static void ConVarChanged_mat_managedtextures( IConVar *var, const char *pOldValue, float flOldValue );
static ConVar mat_managedtextures( "mat_managedtextures", "1", FCVAR_ARCHIVE, "If set, allows Direct3D to manage texture uploading at the cost of extra system memory", &ConVarChanged_mat_managedtextures );
static void ConVarChanged_mat_managedtextures( IConVar *var, const char *pOldValue, float flOldValue )
{
	if ( mat_managedtextures.GetBool() != (flOldValue != 0) )
	{
		materials->ReleaseResources();
		materials->ReacquireResources();
	}
}
#endif

static ConVar mat_spew_on_texture_size( "mat_spew_on_texture_size", "0", 0, "Print warnings about vtf content that isn't of the expected size" );

#define TEXTURE_FNAME_EXTENSION			".vtf"
#define TEXTURE_FNAME_EXTENSION_LEN		4
#define TEXTURE_FNAME_EXTENSION_NORMAL	"_normal.vtf"

//-----------------------------------------------------------------------------
// Internal texture flags
//-----------------------------------------------------------------------------
enum InternalTextureFlags
{
	TEXTUREFLAGSINTERNAL_ERROR				= 0x00000001,
	TEXTUREFLAGSINTERNAL_ALLOCATED			= 0x00000002,
	TEXTUREFLAGSINTERNAL_PRELOADED			= 0x00000004, // 360: textures that went through the preload process
	TEXTUREFLAGSINTERNAL_QUEUEDLOAD			= 0x00000008, // 360: load using the queued loader
	TEXTUREFLAGSINTERNAL_EXCLUDED			= 0x00000020, // actual exclusion state
	TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE		= 0x00000040, // desired exclusion state
	TEXTUREFLAGSINTERNAL_TEMPRENDERTARGET	= 0x00000080, // 360: should only allocate texture bits upon first resolve, destroy at level end
};

//-----------------------------------------------------------------------------
// Use Warning to show texture flags.
//-----------------------------------------------------------------------------
static void PrintFlags( unsigned int flags )
{
	if ( flags & TEXTUREFLAGS_NOMIP )
	{
		Warning( "TEXTUREFLAGS_NOMIP|" );
	}
	if ( flags & TEXTUREFLAGS_NOLOD )
	{
		Warning( "TEXTUREFLAGS_NOLOD|" );
	}
	if ( flags & TEXTUREFLAGS_SRGB )
	{
		Warning( "TEXTUREFLAGS_SRGB|" );
	}
	if ( flags & TEXTUREFLAGS_POINTSAMPLE )
	{
		Warning( "TEXTUREFLAGS_POINTSAMPLE|" );
	}
	if ( flags & TEXTUREFLAGS_TRILINEAR )
	{
		Warning( "TEXTUREFLAGS_TRILINEAR|" );
	}
	if ( flags & TEXTUREFLAGS_CLAMPS )
	{
		Warning( "TEXTUREFLAGS_CLAMPS|" );
	}
	if ( flags & TEXTUREFLAGS_CLAMPT )
	{
		Warning( "TEXTUREFLAGS_CLAMPT|" );
	}
	if ( flags & TEXTUREFLAGS_HINT_DXT5 )
	{
		Warning( "TEXTUREFLAGS_HINT_DXT5|" );
	}
	if ( flags & TEXTUREFLAGS_ANISOTROPIC )
	{
		Warning( "TEXTUREFLAGS_ANISOTROPIC|" );
	}
	if ( flags & TEXTUREFLAGS_PROCEDURAL )
	{
		Warning( "TEXTUREFLAGS_PROCEDURAL|" );
	}
	if ( flags & TEXTUREFLAGS_ALL_MIPS )
	{
		Warning( "TEXTUREFLAGS_ALL_MIPS|" );
	}
	if ( flags & TEXTUREFLAGS_SINGLECOPY )
	{
		Warning( "TEXTUREFLAGS_SINGLECOPY|" );
	}
}


namespace TextureLodOverride
{
	struct OverrideInfo
	{
		OverrideInfo() : x( 0 ), y( 0 ) {}
		OverrideInfo( int8 x_, int8 y_ ) : x( x_ ), y( y_ ) {}
		int8 x, y;
	};

	// Override map
	typedef CUtlStringMap< OverrideInfo > OverrideMap_t;
	OverrideMap_t s_OverrideMap;

	// Retrieves the override info adjustments
	OverrideInfo Get( char const *szName )
	{
		UtlSymId_t idx = s_OverrideMap.Find( szName );
		if ( idx != s_OverrideMap.InvalidIndex() )
			return s_OverrideMap[ idx ];
		else
			return OverrideInfo();
	}

	// Combines the existing override info adjustments with the given one
	void Add( char const *szName, OverrideInfo oi )
	{
		OverrideInfo oiex = Get( szName );
		oiex.x += oi.x;
		oiex.y += oi.y;
		s_OverrideMap[ szName ] = oiex;
	}

}; // end namespace TextureLodOverride


//-----------------------------------------------------------------------------
// Base texture class
//-----------------------------------------------------------------------------

class CTexture : public ITextureInternal
{
public:
	CTexture();
	virtual ~CTexture();

	virtual const char *GetName( void ) const;
	const char *GetTextureGroupName( void ) const;

	// Stats about the texture itself
	virtual ImageFormat GetImageFormat() const;
	NormalDecodeMode_t GetNormalDecodeMode() const { return NORMAL_DECODE_NONE; }
	virtual int GetMappingWidth() const;
	virtual int GetMappingHeight() const;
	virtual int GetActualWidth() const;
	virtual int GetActualHeight() const;
	virtual int GetNumAnimationFrames() const;
	virtual bool IsTranslucent() const;
	virtual void GetReflectivity( Vector& reflectivity );

	// Reference counting
	virtual void IncrementReferenceCount( );
	virtual void DecrementReferenceCount( );
	virtual int GetReferenceCount( );

	// Used to modify the texture bits (procedural textures only)
	virtual void SetTextureRegenerator( ITextureRegenerator *pTextureRegen );

	// Little helper polling methods
	virtual bool IsNormalMap( ) const;
	virtual bool IsCubeMap( void ) const;
	virtual bool IsRenderTarget( ) const;
	virtual bool IsTempRenderTarget( void ) const;
	virtual bool IsProcedural() const;
	virtual bool IsMipmapped() const;
	virtual bool IsError() const;

	// For volume textures
	virtual bool IsVolumeTexture() const;
	virtual int GetMappingDepth() const;
	virtual int GetActualDepth() const;

	// Various ways of initializing the texture
	void InitFileTexture( const char *pTextureFile, const char *pTextureGroupName );
	void InitProceduralTexture( const char *pTextureName, const char *pTextureGroupName, int w, int h, int d, ImageFormat fmt, int nFlags );

	// Releases the texture's hw memory
	void Release();

	virtual void OnRestore();

	// Sets the filtering modes on the texture we're modifying
	void SetFilteringAndClampingMode();
	void Download( Rect_t *pRect = NULL, int nAdditionalCreationFlags = 0 );

	// Loads up information about the texture 
	virtual void Precache();

	// FIXME: Bogus methods... can we please delete these?
	virtual void GetLowResColorSample( float s, float t, float *color ) const;

	// Gets texture resource data of the specified type.
	// Params:
	//		eDataType		type of resource to retrieve.
	//		pnumBytes		on return is the number of bytes available in the read-only data buffer or is undefined
	// Returns:
	//		pointer to the resource data, or NULL. Note that the data from this pointer can disappear when
	// the texture goes away - you want to copy this data!
	virtual void *GetResourceData( uint32 eDataType, size_t *pNumBytes ) const;

	virtual int GetApproximateVidMemBytes( void ) const;

	// Stretch blit the framebuffer into this texture.
	virtual void CopyFrameBufferToMe( int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL );
	virtual void CopyMeToFrameBuffer( int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL );

	virtual ITexture *GetEmbeddedTexture( int nIndex );

	// Get the shaderapi texture handle associated w/ a particular frame
	virtual ShaderAPITextureHandle_t GetTextureHandle( int nFrame, int nChannel = 0 );

	// Sets the texture as the render target
	virtual void Bind( Sampler_t sampler );
	virtual void Bind( Sampler_t sampler1, int nFrame, Sampler_t sampler2 = (Sampler_t) -1 );
	virtual void BindVertexTexture( VertexTextureSampler_t stage, int nFrame );

	// Set this texture as a render target
	bool SetRenderTarget( int nRenderTargetID );

	// Set this texture as a render target (optionally set depth texture as depth buffer as well)
	bool SetRenderTarget( int nRenderTargetID, ITexture *pDepthTexture);

	virtual void MarkAsPreloaded( bool bSet );
	virtual bool IsPreloaded() const;

	virtual void MarkAsExcluded( bool bSet, int nDimensionsLimit );
	virtual bool UpdateExcludedState( void );

	// Retrieve the vtf flags mask
	virtual unsigned int GetFlags( void ) const;

	virtual void ForceLODOverride( int iNumLodsOverrideUpOrDown );

	void GetFilename( char *pOut, int maxLen ) const;
	virtual void ReloadFilesInList( IFileList *pFilesToReload );

	// Save texture to a file.
	virtual bool SaveToFile( const char *fileName );

protected:
	bool IsDepthTextureFormat( ImageFormat fmt );
	void ReconstructTexture();
	void ReconstructPartialTexture( const Rect_t *pRect );
	bool HasBeenAllocated() const;
	void WriteDataToShaderAPITexture( int nFrameCount, int nFaceCount, int nFirstFace, int nMipCount, IVTFTexture *pVTFTexture, ImageFormat fmt );

	// Initializes/shuts down the texture
	void Init( int w, int h, int d, ImageFormat fmt, int iFlags, int iFrameCount, bool bUnused = false );
	void Shutdown();

	// Sets the texture name
	void SetName( const char* pName );

	// Assigns/releases texture IDs for our animation frames
	void AllocateTextureHandles( );
	void ReleaseTextureHandles( );

	// Calculates info about whether we can make the texture smaller and by how much
	// Returns the number of skipped mip levels
	int ComputeActualSize( bool bIgnorePicmip = false, IVTFTexture *pVTFTexture = NULL );

	// Computes the actual format of the texture given a desired src format
	ImageFormat ComputeActualFormat( ImageFormat srcFormat );

	// Compute the actual mip count based on the actual size
	int ComputeActualMipCount( ) const;

	// Creates/releases the shader api texture
	bool AllocateShaderAPITextures();
	void FreeShaderAPITextures();
	void NotifyUnloadedFile();

	// Download bits
	void DownloadTexture( Rect_t *pRect );
	void ReconstructTextureBits(Rect_t *pRect);

	// Gets us modifying a particular frame of our texture
	void Modify( int iFrame );

	// Sets the texture clamping state on the currently modified frame
	void SetWrapState( );

	// Sets the texture filtering state on the currently modified frame
	void SetFilterState();

	// Loads the texture bits from a file. Optionally provides absolute path
	IVTFTexture *LoadTextureBitsFromFile( char *pCacheFileName, char **pResolvedFilename, bool bPaired = false );
	IVTFTexture *HandleFileLoadFailedTexture( IVTFTexture *pVTFTexture );

	// Generates the procedural bits
	IVTFTexture *ReconstructProceduralBits( );
	IVTFTexture *ReconstructPartialProceduralBits( const Rect_t *pRect, Rect_t *pActualRect );

	// Sets up debugging texture bits, if appropriate
	bool SetupDebuggingTextures( IVTFTexture *pTexture );

	// Generate a texture that shows the various mip levels
	void GenerateShowMipLevelsTextures( IVTFTexture *pTexture );

	void Cleanup( void );

	// Converts a source image read from disk into its actual format
	bool ConvertToActualFormat( IVTFTexture *pTexture, bool bUnused = false );

	// Builds the low-res image from the texture 
	void LoadLowResTexture( IVTFTexture *pTexture );
	void CopyLowResImageToTexture( IVTFTexture *pTexture );

	void GetDownloadFaceCount( int &nFirstFace, int &nFaceCount );
	void ComputeMipLevelSubRect( const Rect_t* pSrcRect, int nMipLevel, Rect_t *pSubRect );

	IVTFTexture *GetScratchVTFTexture( );
	IVTFTexture *GetScratchVTFTexturePaired( );

	int GetOptimalReadBuffer( FileHandle_t hFile, int nFileSize, CUtlBuffer &optimalBuffer );
	void FreeOptimalReadBuffer( int nMaxSize );

	void ApplyRenderTargetSizeMode( int &width, int &height, ImageFormat fmt );

protected:
#ifdef _DEBUG
	char *m_pDebugName;
#endif

	// Reflectivity vector
	Vector m_vecReflectivity;

	CUtlSymbol m_Name;

	// What texture group this texture is in (winds up setting counters based on the group name,
	// then the budget panel views the counters).
	CUtlSymbol m_TextureGroupName;

	unsigned int m_nFlags;
	unsigned int m_nInternalFlags;

	CInterlockedInt m_nRefCount;

	// This is the *desired* image format, which may or may not represent reality
	ImageFormat m_ImageFormat;

	// mapping dimensions and actual dimensions can/will vary due to user settings, hardware support, etc.
	unsigned short m_nMappingWidth;
	unsigned short m_nMappingHeight;
	unsigned short m_nMappingDepth;
	unsigned short m_nActualWidth;		// needed for procedural
	unsigned short m_nActualHeight;		// needed for procedural
	unsigned short m_nActualDepth;
	unsigned short m_nActualMipCount;	// Mip count when it's actually used

	unsigned short m_nFrameCount;

	unsigned short m_nOriginalRTWidth;	// The values they initially specified. We generated a different width
	unsigned short m_nOriginalRTHeight;	// and height based on screen size and the flags they specify.

	unsigned char m_LowResImageWidth;
	unsigned char m_LowResImageHeight;

	unsigned short m_nDesiredDimensionLimit;	// part of texture exclusion
	unsigned short m_nActualDimensionLimit;		// value not necessarily accurate, but mismatch denotes dirty state

	// The set of texture ids for each animation frame
	ShaderAPITextureHandle_t *m_pTextureHandles;

	// lowresimage info - used for getting color data from a texture
	// without having a huge system mem overhead.
	// FIXME: We should keep this in compressed form. .is currently decompressed at load time.
	unsigned char *m_pLowResImage;

	ITextureRegenerator *m_pTextureRegenerator;

	// Used to help decide whether or not to recreate the render target if AA changes.
	RenderTargetType_t m_nOriginalRenderTargetType;
	RenderTargetSizeMode_t m_RenderTargetSizeMode;

	// Fixed-size allocator
//	DECLARE_FIXEDSIZE_ALLOCATOR( CTexture );
public:
	void InitRenderTarget( const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, 
		ImageFormat fmt, RenderTargetType_t type, unsigned int textureFlags,
		unsigned int renderTargetFlags );

	virtual void DeleteIfUnreferenced();

	void FixupTexture( const void *pData, int nSize, LoaderError_t loaderError );

	void SwapContents( ITexture *pOther );

protected:
	// private data, generally from VTF resource extensions
	struct DataChunk
	{
		void Allocate( unsigned int numBytes ) 
		{
			m_pvData = new unsigned char[ numBytes ];
			m_numBytes = numBytes;
		}
		void Deallocate() const { delete [] m_pvData; }

		unsigned int m_eType;
		unsigned int m_numBytes;
		unsigned char *m_pvData;
	};
	CUtlVector< DataChunk > m_arrDataChunks;
};

//////////////////////////////////////////////////////////////////////////
//
// CReferenceToHandleTexture is a special implementation of ITexture
// to be used solely for binding the texture handle when rendering.
// It is used when a D3D texture handle is available, but should be used
// at a higher level of abstraction requiring an ITexture or ITextureInternal.
//
//////////////////////////////////////////////////////////////////////////
class CReferenceToHandleTexture : public ITextureInternal
{
public:
	CReferenceToHandleTexture();
	virtual ~CReferenceToHandleTexture();

	virtual const char *GetName( void ) const { return m_Name.String(); }
	const char *GetTextureGroupName( void ) const { return m_TextureGroupName.String(); }

	// Stats about the texture itself
	virtual ImageFormat GetImageFormat() const { return IMAGE_FORMAT_UNKNOWN; }
	virtual NormalDecodeMode_t GetNormalDecodeMode() const { return NORMAL_DECODE_NONE; }
	virtual int GetMappingWidth() const { return 1; }
	virtual int GetMappingHeight() const { return 1; }
	virtual int GetActualWidth() const { return 1; }
	virtual int GetActualHeight() const { return 1; }
	virtual int GetNumAnimationFrames() const { return 1; }
	virtual bool IsTranslucent() const { return false; }
	virtual void GetReflectivity( Vector& reflectivity ) { reflectivity.Zero(); }

	// Reference counting
	virtual void IncrementReferenceCount( ) { ++ m_nRefCount; }
	virtual void DecrementReferenceCount( ) { -- m_nRefCount; }
	virtual int GetReferenceCount( ) { return m_nRefCount; }

	// Used to modify the texture bits (procedural textures only)
	virtual void SetTextureRegenerator( ITextureRegenerator *pTextureRegen ) { NULL; }

	// Little helper polling methods
	virtual bool IsNormalMap( ) const { return false; }
	virtual bool IsCubeMap( void ) const { return false; }
	virtual bool IsRenderTarget( ) const { return false; }
	virtual bool IsTempRenderTarget( void ) const { return false; }
	virtual bool IsProcedural() const { return true; }
	virtual bool IsMipmapped() const { return false; }
	virtual bool IsError() const { return false; }

	// For volume textures
	virtual bool IsVolumeTexture() const { return false; }
	virtual int GetMappingDepth() const { return 1; }
	virtual int GetActualDepth() const { return 1; }

	// Releases the texture's hw memory
	void Release() { NULL; }

	virtual void OnRestore() { NULL; }

	// Sets the filtering modes on the texture we're modifying
	void SetFilteringAndClampingMode() { NULL; }
	void Download( Rect_t *pRect = NULL, int nAdditionalCreationFlags = 0 ) { NULL; }

	// Loads up information about the texture 
	virtual void Precache() { NULL; }

	// FIXME: Bogus methods... can we please delete these?
	virtual void GetLowResColorSample( float s, float t, float *color ) const { NULL; }

	// Gets texture resource data of the specified type.
	// Params:
	//		eDataType		type of resource to retrieve.
	//		pnumBytes		on return is the number of bytes available in the read-only data buffer or is undefined
	// Returns:
	//		pointer to the resource data, or NULL. Note that the data from this pointer can disappear when
	// the texture goes away - you want to copy this data!
	virtual void *GetResourceData( uint32 eDataType, size_t *pNumBytes ) const { return NULL; }

	virtual int GetApproximateVidMemBytes( void ) const { return 32; }

	// Stretch blit the framebuffer into this texture.
	virtual void CopyFrameBufferToMe( int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL ) { NULL; }
	virtual void CopyMeToFrameBuffer( int nRenderTargetID = 0, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL ) { NULL; }

	virtual ITexture *GetEmbeddedTexture( int nIndex ) { return ( nIndex == 0 ) ? this : NULL; }

	// Get the shaderapi texture handle associated w/ a particular frame
	virtual ShaderAPITextureHandle_t GetTextureHandle( int nFrame, int nTextureChannel = 0 ) { return m_hTexture; }

	// Bind the texture
	virtual void Bind( Sampler_t sampler );
	virtual void Bind( Sampler_t sampler1, int nFrame, Sampler_t sampler2 = (Sampler_t) -1 );
	virtual void BindVertexTexture( VertexTextureSampler_t stage, int nFrame );

	// Set this texture as a render target	
	bool SetRenderTarget( int nRenderTargetID ) { return SetRenderTarget( nRenderTargetID, NULL ); }

	// Set this texture as a render target (optionally set depth texture as depth buffer as well)
	bool SetRenderTarget( int nRenderTargetID, ITexture *pDepthTexture) { return false; }

	virtual void MarkAsPreloaded( bool bSet ) { NULL; }
	virtual bool IsPreloaded() const { return true; }

	virtual void MarkAsExcluded( bool bSet, int nDimensionsLimit ) { NULL; }
	virtual bool UpdateExcludedState( void ) { return true; }

	// Retrieve the vtf flags mask
	virtual unsigned int GetFlags( void ) const { return 0; }

	virtual void ForceLODOverride( int iNumLodsOverrideUpOrDown ) { NULL; }

	virtual void ReloadFilesInList( IFileList *pFilesToReload ) {}

	// Save texture to a file.
	virtual bool SaveToFile( const char *fileName ) { return false; }

protected:
#ifdef _DEBUG
	char *m_pDebugName;
#endif

	CUtlSymbol m_Name;

	// What texture group this texture is in (winds up setting counters based on the group name,
	// then the budget panel views the counters).
	CUtlSymbol m_TextureGroupName;

	// The set of texture ids for each animation frame
	ShaderAPITextureHandle_t m_hTexture;

	// Refcount
	int m_nRefCount;

public:
	virtual void DeleteIfUnreferenced();

	void FixupTexture( const void *pData, int nSize, LoaderError_t loaderError ) { NULL; }

	void SwapContents( ITexture *pOther ) { NULL; }

public:
	void SetName( char const *szName );
	void InitFromHandle(
		const char *pTextureName,
		const char *pTextureGroupName,
		ShaderAPITextureHandle_t hTexture );
};

CReferenceToHandleTexture::CReferenceToHandleTexture() :
	m_hTexture( INVALID_SHADERAPI_TEXTURE_HANDLE ),
#ifdef _DEBUG
	m_pDebugName( NULL ),
#endif
	m_nRefCount( 0 )
{
	NULL;
}

CReferenceToHandleTexture::~CReferenceToHandleTexture()
{
#ifdef _DEBUG
	if ( m_nRefCount != 0 )
	{
		Warning( "Reference Count(%d) != 0 in ~CReferenceToHandleTexture for texture \"%s\"\n", m_nRefCount, m_Name.String() );
	}
	if ( m_pDebugName )
	{
		delete [] m_pDebugName;
	}
#endif
}

void CReferenceToHandleTexture::SetName( char const *szName )
{
	// normalize and convert to a symbol
	char szCleanName[MAX_PATH];
	m_Name = NormalizeTextureName( szName, szCleanName, sizeof( szCleanName ) );

#ifdef _DEBUG
	if ( m_pDebugName )
	{
		delete [] m_pDebugName;
	}
	int nLen = V_strlen( szCleanName ) + 1;
	m_pDebugName = new char[nLen];
	V_memcpy( m_pDebugName, szCleanName, nLen );
#endif
}

void CReferenceToHandleTexture::InitFromHandle( const char *pTextureName, const char *pTextureGroupName, ShaderAPITextureHandle_t hTexture )
{
	SetName( pTextureName );
	m_TextureGroupName = pTextureGroupName;
	m_hTexture = hTexture;
}

void CReferenceToHandleTexture::Bind( Sampler_t sampler )
{
	if ( g_pShaderDevice->IsUsingGraphics() )
	{
		g_pShaderAPI->BindTexture( sampler, m_hTexture );
	}
}


// TODO: make paired textures work with mat_texture_list
void CReferenceToHandleTexture::Bind( Sampler_t sampler1, int nFrame, Sampler_t sampler2 /* = -1 */ )
{
	if ( g_pShaderDevice->IsUsingGraphics() )
	{
		g_pShaderAPI->BindTexture( sampler1, m_hTexture );
	}
}


void CReferenceToHandleTexture::BindVertexTexture( VertexTextureSampler_t sampler, int nFrame )
{
	if ( g_pShaderDevice->IsUsingGraphics() )
	{
		g_pShaderAPI->BindVertexTexture( sampler, m_hTexture );
	}
}

void CReferenceToHandleTexture::DeleteIfUnreferenced()
{
	if ( m_nRefCount > 0 )
		return;

	TextureManager()->RemoveTexture( this );
}


//-----------------------------------------------------------------------------
// Fixed-size allocator
//-----------------------------------------------------------------------------
//DEFINE_FIXEDSIZE_ALLOCATOR( CTexture, 1024, true );


//-----------------------------------------------------------------------------
// Static instance of VTF texture
//-----------------------------------------------------------------------------
static IVTFTexture *s_pVTFTexture = NULL;
static IVTFTexture *s_pVTFTexturePaired = NULL;

static void *s_pOptimalReadBuffer = NULL;
static int s_nOptimalReadBufferSize = 0;

//-----------------------------------------------------------------------------
// Class factory methods
//-----------------------------------------------------------------------------
ITextureInternal *ITextureInternal::CreateFileTexture( const char *pFileName, const char *pTextureGroupName )
{
	CTexture *pTex = new CTexture;
	pTex->InitFileTexture( pFileName, pTextureGroupName );
	return pTex;
}

ITextureInternal *ITextureInternal::CreateReferenceTextureFromHandle(
	const char *pTextureName,
	const char *pTextureGroupName,
	ShaderAPITextureHandle_t hTexture )
{
	CReferenceToHandleTexture *pTex = new CReferenceToHandleTexture;
	pTex->InitFromHandle( pTextureName, pTextureGroupName, hTexture );
	return pTex;
}

ITextureInternal *ITextureInternal::CreateProceduralTexture( 
	const char			*pTextureName, 
	const char			*pTextureGroupName, 
	int					w, 
	int					h, 
	int					d,
	ImageFormat			fmt, 
	int					nFlags )
{
	CTexture *pTex = new CTexture;
	pTex->InitProceduralTexture( pTextureName, pTextureGroupName, w, h, d, fmt, nFlags );
	pTex->IncrementReferenceCount();
	return pTex;
}

// GR - named RT
ITextureInternal *ITextureInternal::CreateRenderTarget( 
	const char *pRTName, 
	int w, 
	int h, 
	RenderTargetSizeMode_t sizeMode, 
	ImageFormat fmt, 
	RenderTargetType_t type, 
	unsigned int textureFlags, 
	unsigned int renderTargetFlags )
{
	CTexture *pTex = new CTexture;
	pTex->InitRenderTarget( pRTName, w, h, sizeMode, fmt, type, textureFlags, renderTargetFlags );

	return pTex;
}

//-----------------------------------------------------------------------------
// Rebuild and exisiting render target in place.
//-----------------------------------------------------------------------------
void ITextureInternal::ChangeRenderTarget( 
	ITextureInternal *pTex,
	int w,
	int	h,
	RenderTargetSizeMode_t sizeMode, 
	ImageFormat fmt, 
	RenderTargetType_t type, 
	unsigned int textureFlags, 
	unsigned int renderTargetFlags )
{
	pTex->Release();
	dynamic_cast< CTexture * >(pTex)->InitRenderTarget( pTex->GetName(), w, h, sizeMode, fmt, type, textureFlags, renderTargetFlags );
}

void ITextureInternal::Destroy( ITextureInternal *pTex )
{
	delete pTex;
}

//-----------------------------------------------------------------------------
// Constructor, destructor
//-----------------------------------------------------------------------------
CTexture::CTexture() : m_ImageFormat( IMAGE_FORMAT_UNKNOWN )
{
	m_nActualMipCount = 0;
	m_nMappingWidth = 0;
	m_nMappingHeight = 0;
	m_nMappingDepth = 1;
	m_nActualWidth = 0;
	m_nActualHeight = 0;
	m_nActualDepth = 1;
	m_nRefCount = 0;
	m_nFlags = 0;
	m_nInternalFlags = 0;
	m_pTextureHandles = NULL;
	m_nFrameCount = 0;
	VectorClear( m_vecReflectivity );
	m_pTextureRegenerator = NULL;
	m_nOriginalRenderTargetType = NO_RENDER_TARGET;
	m_RenderTargetSizeMode = RT_SIZE_NO_CHANGE;
	m_nOriginalRTWidth = m_nOriginalRTHeight = 1;

	m_LowResImageWidth = 0;
	m_LowResImageHeight = 0;
	m_pLowResImage = NULL;

	m_nDesiredDimensionLimit = 0;
	m_nActualDimensionLimit = 0;

#ifdef _DEBUG
	m_pDebugName = NULL;
#endif
}

CTexture::~CTexture()
{
#ifdef _DEBUG
	if ( m_nRefCount != 0 )
	{
		Warning( "Reference Count(%d) != 0 in ~CTexture for texture \"%s\"\n", (int)m_nRefCount, m_Name.String() );
	}
#endif

	Shutdown();

#ifdef _DEBUG
	if ( m_pDebugName )
	{
		// delete[] m_pDebugName;
	}
#endif

	// Deliberately stomp our VTable so that we can detect cases where code tries to access freed materials.
	int *p = (int *)this;
	*p = 0xdeadbeef;
}


//-----------------------------------------------------------------------------
// Initializes the texture
//-----------------------------------------------------------------------------
void CTexture::Init( int w, int h, int d, ImageFormat fmt, int iFlags, int iFrameCount, bool bUnused )
{
	Assert( iFrameCount > 0 );

	// This is necessary to prevent blowing away the allocated state,
	// which is necessary for the ReleaseTextureHandles call below to work.
	m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_ERROR;

	// free and release previous data
	// cannot change to new intialization parameters yet
	FreeShaderAPITextures();
	ReleaseTextureHandles();

	// update to new initialization parameters
	// these are the *desired* new values
	m_nMappingWidth = w;
	m_nMappingHeight = h;
	m_nMappingDepth = d;
	m_ImageFormat = fmt;
	m_nFrameCount = iFrameCount;
	// We don't know the actual width and height until we get it ready to render
	m_nActualWidth = m_nActualHeight = 0;
	m_nActualDepth = 1;
	m_nActualMipCount = 0;

	// Clear the m_nFlags bit. If we don't, then m_nFrameCount may end up being 1, and
	//  TEXTUREFLAGS_DEPTHRENDERTARGET could be set.
	m_nFlags &= ~TEXTUREFLAGS_DEPTHRENDERTARGET;
	m_nFlags |= iFlags;

	AllocateTextureHandles();
}


//-----------------------------------------------------------------------------
// Shuts down the texture
//-----------------------------------------------------------------------------
void CTexture::Shutdown()
{
	// Clean up the low-res texture
	delete[] m_pLowResImage;
	m_pLowResImage = 0;

	// Clean up the resources data
	for ( DataChunk const *pDataChunk = m_arrDataChunks.Base(),
		  *pDataChunkEnd = pDataChunk + m_arrDataChunks.Count();
		  pDataChunk < pDataChunkEnd; ++pDataChunk )
	{
		pDataChunk->Deallocate();
	}
	m_arrDataChunks.RemoveAll();

	// Frees the texture regen class
	if ( m_pTextureRegenerator )
	{
		m_pTextureRegenerator->Release();
		m_pTextureRegenerator = NULL;
	}

	// This deletes the textures
	FreeShaderAPITextures();
	ReleaseTextureHandles();
	NotifyUnloadedFile();
}

void CTexture::Release()
{
	FreeShaderAPITextures();
	NotifyUnloadedFile();
}

IVTFTexture *CTexture::GetScratchVTFTexture( )
{
	if ( !s_pVTFTexture )
	{
		s_pVTFTexture = CreateVTFTexture();
	}
	return s_pVTFTexture;
}

IVTFTexture *CTexture::GetScratchVTFTexturePaired( )
{
	if ( !s_pVTFTexturePaired )
	{
		s_pVTFTexturePaired = CreateVTFTexture();
	}
	return s_pVTFTexturePaired;
}

//-----------------------------------------------------------------------------
// Get an optimal read buffer, persists and avoids excessive allocations
//-----------------------------------------------------------------------------
int CTexture::GetOptimalReadBuffer( FileHandle_t hFile, int nSize, CUtlBuffer &optimalBuffer )
{
	// get an optimal read buffer, only resize if necessary
	int minSize = 2 * 1024 * 1024;	// PC uses 2MB min to avoid fragmentation
	nSize = max( nSize, minSize );
	int nBytesOptimalRead = g_pFullFileSystem->GetOptimalReadSize( hFile, nSize );
	if ( nBytesOptimalRead > s_nOptimalReadBufferSize )
	{
		FreeOptimalReadBuffer( 0 );

		s_nOptimalReadBufferSize = nBytesOptimalRead;
		s_pOptimalReadBuffer = g_pFullFileSystem->AllocOptimalReadBuffer( hFile, nSize );
	}

	// set external buffer and reset to empty
	optimalBuffer.SetExternalBuffer( s_pOptimalReadBuffer, s_nOptimalReadBufferSize, 0 );

	// return the optimal read size
	return nBytesOptimalRead;
}

//-----------------------------------------------------------------------------
// Free the optimal read buffer if it grows too large
//-----------------------------------------------------------------------------
void CTexture::FreeOptimalReadBuffer( int nMaxSize )
{
	if ( s_pOptimalReadBuffer && s_nOptimalReadBufferSize >= nMaxSize )
	{
		g_pFullFileSystem->FreeOptimalReadBuffer( s_pOptimalReadBuffer );
		s_pOptimalReadBuffer = NULL;
		s_nOptimalReadBufferSize = 0;
	}
}

//-----------------------------------------------------------------------------
//
// Various initialization methods
//
//-----------------------------------------------------------------------------
void CTexture::ApplyRenderTargetSizeMode( int &width, int &height, ImageFormat fmt )
{
	width = m_nOriginalRTWidth;
	height = m_nOriginalRTHeight;

	switch ( m_RenderTargetSizeMode )
	{
		case RT_SIZE_FULL_FRAME_BUFFER:
		{
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( width, height );
			if( !HardwareConfig()->SupportsNonPow2Textures() )
			{
				width = FloorPow2( width + 1 );
				height = FloorPow2( height + 1 );
			}
		}
		break;

		case RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP:
		{
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( width, height );
			if( !HardwareConfig()->SupportsNonPow2Textures() )
			{
				width = CeilPow2( width );
				height = CeilPow2( height );
			}
		}
		break;

		case RT_SIZE_PICMIP:
		{
			int fbWidth, fbHeight;
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( fbWidth, fbHeight );
			int picmip = g_config.skipMipLevels;
			while( picmip > 0 )
			{
				width >>= 1;
				height >>= 1;
				picmip--;
			}

			while( width > fbWidth )
			{
				width >>= 1;
			}
			while( height > fbHeight )
			{
				height >>= 1;
			}
		}
		break;

		case RT_SIZE_DEFAULT:
		{
			// Assume that the input is pow2.
			Assert( ( width & ( width - 1 ) ) == 0 );
			Assert( ( height & ( height - 1 ) ) == 0 );
			int fbWidth, fbHeight;
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( fbWidth, fbHeight );
			while( width > fbWidth )
			{
				width >>= 1;
			}
			while( height > fbHeight )
			{
				height >>= 1;
			}
		}
		break;

		case RT_SIZE_HDR:
		{
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( width, height );
			width = width / 4;
			height = height / 4;
		}
		break;

		case RT_SIZE_OFFSCREEN:
		{
			int fbWidth, fbHeight;
			MaterialSystem()->GetRenderTargetFrameBufferDimensions( fbWidth, fbHeight );

			// Shrink the buffer if it's bigger than back buffer.  Otherwise, don't mess with it.
			while( (width > fbWidth) || (height > fbHeight) )
			{
				width >>= 1;
				height >>= 1;
			}
		}
		break;

		case RT_SIZE_LITERAL:
		{
			// Literal means literally don't mess with the dimensions. Unlike what OFFSCREEN does,
			// which is totally to mess with the dimensions.
		}
		break;

		case RT_SIZE_REPLAY_SCREENSHOT:
			{
				// Compute all possible resolutions if first time we're running this function
				static bool			bReplayInit = false;
				static int			m_aScreenshotWidths[ 3 ][ 2 ];
				static ConVarRef	replay_screenshotresolution( "replay_screenshotresolution" );

				if ( !bReplayInit )
				{
					bReplayInit = true;
					for ( int iAspect = 0; iAspect < 3; ++iAspect )
					{
						for ( int iRes = 0; iRes < 2; ++iRes )
						{
							int nWidth = (int)FastPow2( 9 + iRes );
							m_aScreenshotWidths[ iAspect ][ iRes ] = nWidth;
						}
					}
				}

				// Get dimensions for unpadded image
				int nUnpaddedWidth, nUnpaddedHeight;

				// Figure out the proper screenshot size to use based on the aspect ratio
				int nScreenWidth, nScreenHeight;
				MaterialSystem()->GetRenderTargetFrameBufferDimensions( nScreenWidth, nScreenHeight );
				float flAspectRatio = (float)nScreenWidth / nScreenHeight;

				// Get the screenshot res
				int iRes = clamp( replay_screenshotresolution.GetInt(), 0, 1 );

				int iAspect;
				if ( flAspectRatio == 16.0f/9 )
				{
					iAspect = 0;
				}
				else if ( flAspectRatio == 16.0f/10 )
				{
					iAspect = 1;
				}
				else
				{
					iAspect = 2;	// 4:3
				}

				static float s_flInvAspectRatios[3] = { 9.0f/16.0f, 10.0f/16, 3.0f/4 };
				nUnpaddedWidth = min( nScreenWidth, m_aScreenshotWidths[ iAspect ][ iRes ] );
				nUnpaddedHeight = m_aScreenshotWidths[ iAspect ][ iRes ] * s_flInvAspectRatios[ iAspect ];

				// Get dimensions for padded image based on unpadded size - must be power of 2 for a material/texture
				width = SmallestPowerOfTwoGreaterOrEqual( nUnpaddedWidth );
				height = SmallestPowerOfTwoGreaterOrEqual( nUnpaddedHeight ); 
			}
			break;

		default:
		{
			if ( !CommandLine()->FindParm( "-hushasserts" ) )
			{
				Assert( m_RenderTargetSizeMode == RT_SIZE_NO_CHANGE );
				Assert( m_nOriginalRenderTargetType == RENDER_TARGET_NO_DEPTH );	// Only can use NO_CHANGE if we don't have a depth buffer.
			}
		}
		break;
	}
}


//-----------------------------------------------------------------------------
// Creates named render target texture
//-----------------------------------------------------------------------------
void CTexture::InitRenderTarget( 
	const char *pRTName, 
	int w, 
	int h, 
	RenderTargetSizeMode_t sizeMode, 
	ImageFormat fmt, 
	RenderTargetType_t type, 
	unsigned int textureFlags,
	unsigned int renderTargetFlags )
{
	if ( pRTName )
	{
		SetName( pRTName );
	}
	else
	{
		static int id = 0;
		char pName[128];
		Q_snprintf( pName, sizeof( pName ), "__render_target_%d", id );
		++id;
		SetName( pName );
	}

	if ( renderTargetFlags & CREATERENDERTARGETFLAGS_HDR )
	{
		if ( HardwareConfig()->GetHDRType() == HDR_TYPE_FLOAT )
		{
			// slam the format
			fmt = IMAGE_FORMAT_RGBA16161616F;
		}
	}

	int nFrameCount = 1;

	int nFlags = TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_RENDERTARGET;
	nFlags |= textureFlags;

	if ( type == RENDER_TARGET_NO_DEPTH )
	{
		nFlags |= TEXTUREFLAGS_NODEPTHBUFFER;
	}
	else if ( type == RENDER_TARGET_WITH_DEPTH || type == RENDER_TARGET_ONLY_DEPTH || g_pShaderAPI->DoRenderTargetsNeedSeparateDepthBuffer() )
	{
		nFlags |= TEXTUREFLAGS_DEPTHRENDERTARGET;
		++nFrameCount;
	}

	if ( renderTargetFlags & CREATERENDERTARGETFLAGS_TEMP )
	{
		m_nInternalFlags |= TEXTUREFLAGSINTERNAL_TEMPRENDERTARGET;
	}

	m_nOriginalRenderTargetType = type;
	m_RenderTargetSizeMode = sizeMode;
	m_nOriginalRTWidth = w;
	m_nOriginalRTHeight = h;

	if ( ImageLoader::ImageFormatInfo(fmt).m_NumAlphaBits > 1 )
	{
		nFlags  |= TEXTUREFLAGS_EIGHTBITALPHA;
	}
	else if ( ImageLoader::ImageFormatInfo(fmt).m_NumAlphaBits == 1 )
	{
		nFlags  |= TEXTUREFLAGS_ONEBITALPHA;
	}

	ApplyRenderTargetSizeMode( w, h, fmt );

	Init( w, h, 1, fmt, nFlags, nFrameCount );
	m_TextureGroupName = TEXTURE_GROUP_RENDER_TARGET;
}


void CTexture::OnRestore()
{ 
	// May have to change whether or not we have a depth buffer.
	// Are we a render target?
	if ( m_nFlags & TEXTUREFLAGS_RENDERTARGET )
	{
		// Did they not ask for a depth buffer?
		if ( m_nOriginalRenderTargetType == RENDER_TARGET )
		{
			// But, did we force them to have one, or should we force them to have one this time around?
			bool bShouldForce = g_pShaderAPI->DoRenderTargetsNeedSeparateDepthBuffer();
			bool bDidForce = ((m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET) != 0);
			if ( bShouldForce != bDidForce )
			{
				int nFlags = m_nFlags;
				int iFrameCount = m_nFrameCount;
				if ( bShouldForce )
				{
					Assert( !( nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET ) );
					iFrameCount = 2;
					nFlags |= TEXTUREFLAGS_DEPTHRENDERTARGET;
				}
				else
				{
					Assert( ( nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET ) );
					iFrameCount = 1;
					nFlags &= ~TEXTUREFLAGS_DEPTHRENDERTARGET;
				}

				Shutdown();
				
				int newWidth, newHeight;
				ApplyRenderTargetSizeMode( newWidth, newHeight, m_ImageFormat );
				
				Init( newWidth, newHeight, 1, m_ImageFormat, nFlags, iFrameCount );
				return;
			}
		}

		// If we didn't recreate it up above, then we may need to resize it anyway if the framebuffer
		// got smaller than we are.
		int newWidth, newHeight;
		ApplyRenderTargetSizeMode( newWidth, newHeight, m_ImageFormat );
		if ( newWidth != m_nMappingWidth || newHeight != m_nMappingHeight )
		{
			Shutdown();
			Init( newWidth, newHeight, 1, m_ImageFormat, m_nFlags, m_nFrameCount );
			return;
		}
	}
}


//-----------------------------------------------------------------------------
// Creates a procedural texture
//-----------------------------------------------------------------------------
void CTexture::InitProceduralTexture( const char *pTextureName, const char *pTextureGroupName, int w, int h, int d, ImageFormat fmt, int nFlags )
{
	// We shouldn't be asking for render targets here
	Assert( (nFlags & (TEXTUREFLAGS_RENDERTARGET | TEXTUREFLAGS_DEPTHRENDERTARGET)) == 0 );

	SetName( pTextureName );

	// Eliminate flags that are inappropriate...
	nFlags &= ~TEXTUREFLAGS_HINT_DXT5 | TEXTUREFLAGS_ONEBITALPHA | TEXTUREFLAGS_EIGHTBITALPHA | 
		TEXTUREFLAGS_RENDERTARGET | TEXTUREFLAGS_DEPTHRENDERTARGET;

	// Insert required flags
	nFlags |= TEXTUREFLAGS_PROCEDURAL;
	int nAlphaBits = ImageLoader::ImageFormatInfo(fmt).m_NumAlphaBits;
	if (nAlphaBits > 1)
	{
		nFlags |= TEXTUREFLAGS_EIGHTBITALPHA;
	}
	else if (nAlphaBits == 1)
	{
		nFlags |= TEXTUREFLAGS_ONEBITALPHA;
	}
	
	// Procedural textures are always one frame only
	Init( w, h, d, fmt, nFlags, 1 );

	m_TextureGroupName = pTextureGroupName;
}


//-----------------------------------------------------------------------------
// Creates a file texture
//-----------------------------------------------------------------------------
void CTexture::InitFileTexture( const char *pTextureFile, const char *pTextureGroupName )
{
	// For files, we only really know about the file name
	// At any time, the file contents could change, and we could have
	// a different size, number of frames, etc.
	SetName( pTextureFile );
	m_TextureGroupName = pTextureGroupName;
}

//-----------------------------------------------------------------------------
// Assigns/releases texture IDs for our animation frames
//-----------------------------------------------------------------------------
void CTexture::AllocateTextureHandles()
{
	Assert( !m_pTextureHandles );
	Assert( m_nFrameCount > 0 );
#ifdef DBGFLAG_ASSERT
	if( m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET )
	{
		Assert( m_nFrameCount >= 2 );
	}
#endif

	m_pTextureHandles = new ShaderAPITextureHandle_t[m_nFrameCount];
	for( int i = 0; i != m_nFrameCount; ++i )
		m_pTextureHandles[i] = INVALID_SHADERAPI_TEXTURE_HANDLE;
}

void CTexture::ReleaseTextureHandles()
{
	if ( m_pTextureHandles )
	{
		delete[] m_pTextureHandles;
		m_pTextureHandles = NULL;
	}
}


//-----------------------------------------------------------------------------
// Creates the texture
//-----------------------------------------------------------------------------
bool CTexture::AllocateShaderAPITextures()
{
	Assert( !HasBeenAllocated() );

	int nCount = m_nFrameCount;

	int nCreateFlags = 0;
	if ( ( m_nFlags & TEXTUREFLAGS_ENVMAP ) && HardwareConfig()->SupportsCubeMaps() )
	{
		nCreateFlags |= TEXTURE_CREATE_CUBEMAP;
	}

	bool bIsFloat = ( m_ImageFormat == IMAGE_FORMAT_RGBA16161616F ) || ( m_ImageFormat == IMAGE_FORMAT_R32F ) || 
					( m_ImageFormat == IMAGE_FORMAT_RGB323232F ) || ( m_ImageFormat == IMAGE_FORMAT_RGBA32323232F );

	// Don't do sRGB on floating point textures
	if ( ( m_nFlags & TEXTUREFLAGS_SRGB ) && !bIsFloat )
	{
		nCreateFlags |= TEXTURE_CREATE_SRGB;	// for Posix/GL only
	}

	if ( m_nFlags & TEXTUREFLAGS_RENDERTARGET )
	{
		nCreateFlags |= TEXTURE_CREATE_RENDERTARGET;

		// This here is simply so we can use a different call to
		// create the depth texture below	
		if ( ( m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET ) && ( nCount == 2 ) ) //nCount must be 2 on pc
		{
			--nCount;
		}
	}
	else
	{
		// If it's not a render target, use the texture manager in dx
#if defined(IS_WINDOWS_PC)
		static ConVarRef mat_dxlevel( "mat_dxlevel" );
		if ( mat_dxlevel.GetInt() < 90 || mat_managedtextures.GetBool() )
#endif
		{
			nCreateFlags |= TEXTURE_CREATE_MANAGED;
		}
	}

	if ( m_nFlags & TEXTUREFLAGS_POINTSAMPLE )
	{
		nCreateFlags |= TEXTURE_CREATE_UNFILTERABLE_OK;
	}

	if ( m_nFlags & TEXTUREFLAGS_VERTEXTEXTURE )
	{
		nCreateFlags |= TEXTURE_CREATE_VERTEXTEXTURE;
	}

	int nCopies = 1;
	if ( IsProcedural() )
	{
		// This is sort of hacky... should we store the # of copies in the VTF?
		if ( !( m_nFlags & TEXTUREFLAGS_SINGLECOPY ) )
		{
			// FIXME: That 6 there is heuristically what I came up with what I
			// need to get eyes not to stall on map alyx3. We need a better way
			// of determining how many copies of the texture we should store.
			nCopies = 6;
		}
	}

	// For depth only render target: adjust texture width/height
	// Currently we just leave it the same size, will update with further testing
	int nShaderApiCreateTextureDepth = ( ( m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET ) && ( m_nOriginalRenderTargetType == RENDER_TARGET_ONLY_DEPTH ) ) ? 1 : m_nActualDepth;

	// Create all animated texture frames in a single call
	g_pShaderAPI->CreateTextures(
		m_pTextureHandles, nCount,
		m_nActualWidth, m_nActualHeight, nShaderApiCreateTextureDepth, m_ImageFormat, m_nActualMipCount,
		nCopies, nCreateFlags, GetName(), GetTextureGroupName() );

	// Create the depth render target buffer
	if ( m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET )
	{
		MEM_ALLOC_CREDIT();
		Assert( nCount == 1 );

		char debugName[128];
		Q_snprintf( debugName, ARRAYSIZE( debugName ), "%s_ZBuffer", GetName() );
		Assert( m_nFrameCount >= 2 );
		m_pTextureHandles[1] = g_pShaderAPI->CreateDepthTexture( 
				m_ImageFormat, 
				m_nActualWidth, 
				m_nActualHeight,
				debugName,
				( m_nOriginalRenderTargetType == RENDER_TARGET_ONLY_DEPTH ) );
	}

	m_nInternalFlags |= TEXTUREFLAGSINTERNAL_ALLOCATED;

	return true;
}

//-----------------------------------------------------------------------------
// Releases the texture's hardware memory
//-----------------------------------------------------------------------------
void CTexture::FreeShaderAPITextures()
{
	if ( m_pTextureHandles && HasBeenAllocated() )
	{
		// Release the frames
		for ( int i = m_nFrameCount; --i >= 0; )
		{
			if ( g_pShaderAPI->IsTexture( m_pTextureHandles[i] ) )
			{
				g_pShaderAPI->DeleteTexture( m_pTextureHandles[i] );
				m_pTextureHandles[i] = INVALID_SHADERAPI_TEXTURE_HANDLE;
			}
		}
	}
	m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_ALLOCATED;
}


//-----------------------------------------------------------------------------
// Computes the actual format of the texture
//-----------------------------------------------------------------------------
ImageFormat CTexture::ComputeActualFormat( ImageFormat srcFormat )
{
	ImageFormat dstFormat;
	bool bIsCompressed = ImageLoader::IsCompressed( srcFormat );
	if ( g_config.bCompressedTextures && HardwareConfig()->SupportsCompressedTextures() && bIsCompressed )
	{
		// don't do anything since we are already in a compressed format.
		dstFormat = g_pShaderAPI->GetNearestSupportedFormat( srcFormat );
		Assert( dstFormat == srcFormat );
		return dstFormat;
	}

	// NOTE: Below this piece of code is only called when compressed textures are
	// turned off, or if the source texture is not compressed.

#ifdef DX_TO_GL_ABSTRACTION
	if ( ( srcFormat == IMAGE_FORMAT_UVWQ8888 ) || ( srcFormat == IMAGE_FORMAT_UV88 ) || ( srcFormat == IMAGE_FORMAT_UVLX8888 )  )
	{
		// Danger, this is going to blow up on the Mac.  You better know what you're
		// doing with these exotic formats...which were introduced in 1999
		Assert( 0 );
	}
#endif

	// We use the TEXTUREFLAGS_EIGHTBITALPHA and TEXTUREFLAGS_ONEBITALPHA flags
	// to decide how many bits of alpha we need; vtex checks the alpha channel
	// for all white, etc.
	if( ( srcFormat == IMAGE_FORMAT_UVWQ8888 ) || ( srcFormat == IMAGE_FORMAT_UV88 ) || 
		( srcFormat == IMAGE_FORMAT_UVLX8888 ) || ( srcFormat == IMAGE_FORMAT_RGBA16161616 ) ||
		( srcFormat == IMAGE_FORMAT_RGBA16161616F ) )
	{
		dstFormat = g_pShaderAPI->GetNearestSupportedFormat( srcFormat, !IsOpenGL() );  // Stupid HACK!
	} 
	else if ( m_nFlags & ( TEXTUREFLAGS_EIGHTBITALPHA | TEXTUREFLAGS_ONEBITALPHA ) )
	{
		dstFormat = g_pShaderAPI->GetNearestSupportedFormat( IMAGE_FORMAT_BGRA8888 );
	}
	else if ( srcFormat == IMAGE_FORMAT_I8 )
	{
		dstFormat = g_pShaderAPI->GetNearestSupportedFormat( IMAGE_FORMAT_I8 );
	}
	else
	{
		dstFormat = g_pShaderAPI->GetNearestSupportedFormat( IMAGE_FORMAT_BGR888 );
	}
	return dstFormat;
}

//-----------------------------------------------------------------------------
// Compute the actual mip count based on the actual size
//-----------------------------------------------------------------------------
int CTexture::ComputeActualMipCount() const
{
	if ( m_nFlags & TEXTUREFLAGS_ENVMAP )
	{
		if ( !HardwareConfig()->SupportsMipmappedCubemaps() )
		{
			return 1;
		}
	}

	if ( m_nFlags & TEXTUREFLAGS_NOMIP )
	{
		return 1;
	}

	// Unless ALLMIPS is set, we stop mips at 32x32
	const int nMaxMipSize = 32;
	// Clamp border textures on Posix to fix L4D2 flashlight cookie issue
#ifdef DX_TO_GL_ABSTRACTION
	if ( ( false && !g_bForceTextureAllMips && !( m_nFlags & TEXTUREFLAGS_ALL_MIPS ) ) || ( true && ( m_nFlags & TEXTUREFLAGS_BORDER ) ) )
#else
	if ( ( true && !g_bForceTextureAllMips && !( m_nFlags & TEXTUREFLAGS_ALL_MIPS ) ) || ( false && ( m_nFlags & TEXTUREFLAGS_BORDER ) ) )
#endif
	{
		int nNumMipLevels = 1;
		int h = m_nActualWidth;
		int w = m_nActualHeight;
		while ( MIN( w, h ) > nMaxMipSize )
		{
			++nNumMipLevels;

			w >>= 1;
			h >>= 1;
		}
		return nNumMipLevels;
	}

	return ImageLoader::GetNumMipMapLevels( m_nActualWidth, m_nActualHeight, m_nActualDepth );
}

//-----------------------------------------------------------------------------
// Calculates info about whether we can make the texture smaller and by how much
//-----------------------------------------------------------------------------
int CTexture::ComputeActualSize( bool bIgnorePicmip, IVTFTexture *pVTFTexture )
{
	// Must skip mip levels if the texture is too large for our board to handle
	m_nActualWidth = m_nMappingWidth;
	m_nActualHeight = m_nMappingHeight;
	m_nActualDepth = m_nMappingDepth;

	int nClampX = m_nActualWidth;	// no clamping (clamp to texture dimensions)
	int nClampY = m_nActualHeight;
	int nClampZ = m_nActualDepth;

	// Fetch LOD settings from the VTF if available
	TextureLODControlSettings_t lcs;
	memset( &lcs, 0, sizeof( lcs ) );
	TextureLODControlSettings_t const *pLODInfo = NULL;
	if ( pVTFTexture )
	{
		pLODInfo = reinterpret_cast<TextureLODControlSettings_t const *> (
				pVTFTexture->GetResourceData( VTF_RSRC_TEXTURE_LOD_SETTINGS, NULL ) );
		if ( pLODInfo )
			lcs = *pLODInfo;
	}

	// Prepare the default LOD settings (that essentially result in no clamping)
	TextureLODControlSettings_t default_lod_settings;
	memset( &default_lod_settings, 0, sizeof( default_lod_settings ) );
	{
		for ( int w = m_nActualWidth; w > 1; w >>= 1 )
				++ default_lod_settings.m_ResolutionClampX;
		for ( int h = m_nActualHeight; h > 1; h >>= 1 )
				++ default_lod_settings.m_ResolutionClampY;
	}

	// Check for LOD control override
	{
		TextureLodOverride::OverrideInfo oi = TextureLodOverride::Get( GetName() );
			
		if ( oi.x && oi.y && !pLODInfo )	// If overriding texture that doesn't have lod info yet, then use default
			lcs = default_lod_settings;

		lcs.m_ResolutionClampX += oi.x;
		lcs.m_ResolutionClampY += oi.y;
		if ( int8( lcs.m_ResolutionClampX ) < 0 )
			lcs.m_ResolutionClampX = 0;
		if ( int8( lcs.m_ResolutionClampY ) < 0 )
			lcs.m_ResolutionClampY = 0;
	}

	// NOLOD textures are never clamped
	if ( m_nFlags & TEXTUREFLAGS_NOLOD )
	{
		lcs.m_ResolutionClampX = 0;
		lcs.m_ResolutionClampY = 0;
	}

	// Compute the requested mip0 dimensions
	if ( lcs.m_ResolutionClampX && lcs.m_ResolutionClampY )
	{
		nClampX = (1 << lcs.m_ResolutionClampX );
		nClampY = (1 << lcs.m_ResolutionClampY );
	}

	// In case clamp values exceed texture dimensions, then fix up
	// the clamping values
	nClampX = min( nClampX, (int)m_nActualWidth );
	nClampY = min( nClampY, (int)m_nActualHeight );

	//
	// Honor dimension limit restrictions
	//
	if ( m_nDesiredDimensionLimit > 0 )
	{
		while ( nClampX > m_nDesiredDimensionLimit ||
				nClampY > m_nDesiredDimensionLimit )
		{
			nClampX >>= 1;
			nClampY >>= 1;
		}
	}

	//
	// Unless ignoring picmip, reflect the global picmip level in clamp dimensions
	//
	if ( !bIgnorePicmip )
	{
		// If picmip requests texture degradation, then honor it
		// for loddable textures only
		if ( !( m_nFlags & TEXTUREFLAGS_NOLOD ) &&
			  ( g_config.skipMipLevels > 0 ) )
		{
			for ( int iDegrade = 0; iDegrade < g_config.skipMipLevels; ++ iDegrade )
			{
				// don't go lower than 4, or dxt textures won't work properly
				if ( nClampX > 4 &&
					 nClampY > 4 )
				{
					nClampX >>= 1;
					nClampY >>= 1;
				}
			}
		}

		// If picmip requests quality upgrade, then always honor it
		if ( g_config.skipMipLevels < 0 )
		{
			for ( int iUpgrade = 0; iUpgrade < - g_config.skipMipLevels; ++ iUpgrade )
			{
				if ( nClampX < m_nActualWidth &&
					 nClampY < m_nActualHeight )
				{
					nClampX <<= 1;
					nClampY <<= 1;
				}
				else
					break;
			}
		}
	}

	//
	// Now use hardware settings to clamp our "clamping dimensions"
	//
	int iHwWidth = HardwareConfig()->MaxTextureWidth();
	int iHwHeight = HardwareConfig()->MaxTextureHeight();
	int iHwDepth = HardwareConfig()->MaxTextureDepth();

	nClampX = min( nClampX, max( iHwWidth, 4 ) );
	nClampY = min( nClampY, max( iHwHeight, 4 ) );
	nClampZ = min( nClampZ, max( iHwDepth, 1 ) );

	// In case clamp values exceed texture dimensions, then fix up
	// the clamping values.
	nClampX = min( nClampX, (int)m_nActualWidth );
	nClampY = min( nClampY, (int)m_nActualHeight );
	nClampZ = min( nClampZ, (int)m_nActualDepth );
	
	//
	// Clamp to the determined dimensions
	//
	int numMipsSkipped = 0; // will compute now when clamping how many mips we drop
	while ( ( m_nActualWidth  > nClampX ) ||
		    ( m_nActualHeight > nClampY ) ||
			( m_nActualDepth  > nClampZ ) )
	{
		m_nActualWidth  >>= 1;
		m_nActualHeight >>= 1;
		m_nActualDepth  >>= 1;
		if ( m_nActualDepth < 1 )
			m_nActualDepth = 1;

		++ numMipsSkipped;
	}

	Assert( m_nActualWidth > 0 && m_nActualHeight > 0 && m_nActualDepth > 0 );

	// Now that we've got the actual size, we can figure out the mip count
	m_nActualMipCount = ComputeActualMipCount();

	// Returns the number we skipped
	return numMipsSkipped;
}


//-----------------------------------------------------------------------------
// Used to modify the texture bits (procedural textures only)
//-----------------------------------------------------------------------------
void CTexture::SetTextureRegenerator( ITextureRegenerator *pTextureRegen )
{
	// NOTE: These can only be used by procedural textures
	Assert( IsProcedural() );

	if (m_pTextureRegenerator)
	{
		m_pTextureRegenerator->Release();
	}
	m_pTextureRegenerator = pTextureRegen; 
}


//-----------------------------------------------------------------------------
// Gets us modifying a particular frame of our texture
//-----------------------------------------------------------------------------
void CTexture::Modify( int iFrame )
{
	Assert( iFrame >= 0 && iFrame < m_nFrameCount );
	Assert( HasBeenAllocated() );
	g_pShaderAPI->ModifyTexture( m_pTextureHandles[iFrame] );
}


//-----------------------------------------------------------------------------
// Sets the texture clamping state on the currently modified frame
//-----------------------------------------------------------------------------
void CTexture::SetWrapState( )
{
	// Border clamp applies to all texture coordinates
	if ( m_nFlags & TEXTUREFLAGS_BORDER )
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_S, SHADER_TEXWRAPMODE_BORDER );
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_T, SHADER_TEXWRAPMODE_BORDER );
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_U, SHADER_TEXWRAPMODE_BORDER );
		return;
	}

	// Clamp mode in S
	if ( m_nFlags & TEXTUREFLAGS_CLAMPS )
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_S, SHADER_TEXWRAPMODE_CLAMP );
	}
	else
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_S, SHADER_TEXWRAPMODE_REPEAT );
	}

	// Clamp mode in T
	if ( m_nFlags & TEXTUREFLAGS_CLAMPT )
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_T, SHADER_TEXWRAPMODE_CLAMP );
	}
	else
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_T, SHADER_TEXWRAPMODE_REPEAT );
	}

	// Clamp mode in U
	if ( m_nFlags & TEXTUREFLAGS_CLAMPU )
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_U, SHADER_TEXWRAPMODE_CLAMP );
	}
	else
	{
		g_pShaderAPI->TexWrap( SHADER_TEXCOORD_U, SHADER_TEXWRAPMODE_REPEAT );
	}
}


//-----------------------------------------------------------------------------
// Sets the texture filtering state on the currently modified frame
//-----------------------------------------------------------------------------
void CTexture::SetFilterState()
{
	// Turns off filtering when we're point sampling
	if( m_nFlags & TEXTUREFLAGS_POINTSAMPLE )
	{
		g_pShaderAPI->TexMinFilter( SHADER_TEXFILTERMODE_NEAREST );
		g_pShaderAPI->TexMagFilter( SHADER_TEXFILTERMODE_NEAREST );
		return;
	}

	// NOTE: config.bMipMapTextures and config.bFilterTextures is handled in ShaderAPIDX8
	bool bEnableMipmapping = ( m_nFlags & TEXTUREFLAGS_NOMIP ) ? false : true;
	if( !bEnableMipmapping )
	{
		g_pShaderAPI->TexMinFilter( SHADER_TEXFILTERMODE_LINEAR );
		g_pShaderAPI->TexMagFilter( SHADER_TEXFILTERMODE_LINEAR );
		return;
	}

	// Determing the filtering mode
	bool bIsAnisotropic = false, bIsTrilinear = false;
	if ( HardwareConfig()->GetDXSupportLevel() >= 80 && (g_config.m_nForceAnisotropicLevel > 1) && 
		(HardwareConfig()->MaximumAnisotropicLevel() > 1) )
	{
		bIsAnisotropic = true;
	}
	else if ( g_config.ForceTrilinear() )
	{
		bIsAnisotropic = (( m_nFlags & TEXTUREFLAGS_ANISOTROPIC ) != 0) && (HardwareConfig()->MaximumAnisotropicLevel() > 1);
		bIsTrilinear = true;
	}
	else
	{
		bIsAnisotropic = (( m_nFlags & TEXTUREFLAGS_ANISOTROPIC ) != 0) && (HardwareConfig()->MaximumAnisotropicLevel() > 1);
		bIsTrilinear = ( m_nFlags & TEXTUREFLAGS_TRILINEAR ) != 0;
	}

	if ( bIsAnisotropic )
	{		    
		g_pShaderAPI->TexMinFilter( SHADER_TEXFILTERMODE_ANISOTROPIC );
		g_pShaderAPI->TexMagFilter( SHADER_TEXFILTERMODE_ANISOTROPIC );
	}
	else
	{
		// force trilinear if we are on a dx8 card or above
		if ( bIsTrilinear )
		{
			g_pShaderAPI->TexMinFilter( SHADER_TEXFILTERMODE_LINEAR_MIPMAP_LINEAR );
			g_pShaderAPI->TexMagFilter( SHADER_TEXFILTERMODE_LINEAR );
		}
		else
		{
			g_pShaderAPI->TexMinFilter( SHADER_TEXFILTERMODE_LINEAR_MIPMAP_NEAREST );
			g_pShaderAPI->TexMagFilter( SHADER_TEXFILTERMODE_LINEAR );
		}
	}
}


//-----------------------------------------------------------------------------
// Download bits main entry point!!
//-----------------------------------------------------------------------------
void CTexture::DownloadTexture( Rect_t *pRect )
{
	// No downloading necessary if there's no graphics
	if ( !g_pShaderDevice->IsUsingGraphics() )
		return;

	// We don't know the actual size of the texture at this stage...
	if ( !pRect )
	{
		ReconstructTexture();
	}
	else
	{
		// Not implemented yet.
		ReconstructPartialTexture( pRect );
	}

	// Iterate over all the frames and set the appropriate wrapping + filtering state
	SetFilteringAndClampingMode();

	// texture bits have been updated, update the exclusion state
	if ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE )
	{
		m_nInternalFlags |= TEXTUREFLAGSINTERNAL_EXCLUDED;
	}
	else
	{
		m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_EXCLUDED;
	}

	// texture bits have been picmipped, update the picmip state
	m_nActualDimensionLimit = m_nDesiredDimensionLimit;
}

void CTexture::Download( Rect_t *pRect, int nAdditionalCreationFlags /* = 0 */ )
{
	// Only download the bits if we can...
	if ( g_pShaderAPI->CanDownloadTextures() )
	{
		MaterialLock_t hLock = MaterialSystem()->Lock();
		m_nFlags |= nAdditionalCreationFlags; // Path to let stdshaders drive settings like sRGB-ness at creation time
		DownloadTexture( pRect );
		MaterialSystem()->Unlock( hLock );
	}
}

// Save texture to a file.
bool CTexture::SaveToFile( const char *fileName )
{
	bool bRet = false;
	ITexture *pTexture = materials->FindTexture( "_rt_FullFrameFB1", TEXTURE_GROUP_RENDER_TARGET );

	if ( !pTexture )
		return bRet;

	if ( pTexture->GetImageFormat() == IMAGE_FORMAT_RGBA8888 ||
		 pTexture->GetImageFormat() == IMAGE_FORMAT_ABGR8888 ||
		 pTexture->GetImageFormat() == IMAGE_FORMAT_ARGB8888 ||
		 pTexture->GetImageFormat() == IMAGE_FORMAT_BGRA8888 ||
		 pTexture->GetImageFormat() == IMAGE_FORMAT_BGRX8888 )
	{
		const int width = Min( GetActualWidth(), pTexture->GetActualWidth() );
		const int height = Min( GetActualHeight(), pTexture->GetActualHeight() );

		Rect_t SrcRect = { 0, 0, width, height };
		Rect_t DstRect = SrcRect;

		if ( ( width > 0 ) && ( height > 0 ) )
		{
			void *pixelValue = malloc( width * height * 2 * sizeof( BGRA8888_t ) );

			if( pixelValue )
			{
				CMatRenderContextPtr pRenderContext( MaterialSystem() );

				// Set the clear color to opaque black
				pRenderContext->ClearColor4ub( 0, 0, 0, 0xFF );
				pRenderContext->ClearBuffers( true, true, true );
				pRenderContext->PushRenderTargetAndViewport( pTexture, 0, 0, width, height );
				pRenderContext->CopyTextureToRenderTargetEx( 0, this, &SrcRect, &DstRect );

				pRenderContext->ReadPixels( 0, 0, width, height, ( unsigned char * )pixelValue, pTexture->GetImageFormat() );

				// Slap the alpha channel at the bottom of the tga file so we don't have to deal with crappy tools that can't
				//  handle rgb + alpha well. This means we can just do a "mat_texture_save_fonts" concommand, and then use
				//	something like Beyond Compare to look at the fonts differences between various platforms, etc.
				CPixelWriter pixelWriterSrc;
				CPixelWriter pixelWriterDst;
				pixelWriterSrc.SetPixelMemory( pTexture->GetImageFormat(), pixelValue, width * sizeof( BGRA8888_t ) );
				pixelWriterDst.SetPixelMemory( pTexture->GetImageFormat(), pixelValue, width * sizeof( BGRA8888_t ) );

				for (int y = 0; y < height; ++y)
				{
					pixelWriterSrc.Seek( 0, y );
					pixelWriterDst.Seek( 0, y + height );

					for (int x = 0; x < width; ++x)
					{
						int r, g, b, a;

						pixelWriterSrc.ReadPixelNoAdvance( r, g, b, a );
						pixelWriterSrc.WritePixel( a, a, a, 255 );
						pixelWriterDst.WritePixel( r, g, b, 255 );
					}
				}

				if ( TGAWriter::WriteTGAFile( fileName, width, height * 2, pTexture->GetImageFormat(), ( uint8 * )pixelValue, width * sizeof( BGRA8888_t ) ) )
				{
					bRet = true;
				}

				// restore our previous state
				pRenderContext->PopRenderTargetAndViewport();

				free( pixelValue );
			}
		}
	}
	else
	{
		Msg( "SaveToFile: texture '_rt_FullFrameFB1' failed. Ptr:%p Format:%d\n", pTexture, ( pTexture ? pTexture->GetImageFormat() : 0 ) );
	}

	return bRet;
}

void CTexture::Bind( Sampler_t sampler )
{
	Bind( sampler, 0 );
}

//-----------------------------------------------------------------------------
// Binds a particular texture (possibly paired)
//-----------------------------------------------------------------------------
void CTexture::Bind( Sampler_t sampler1, int nFrame, Sampler_t sampler2 /* = -1 */ )
{
	if ( g_pShaderDevice->IsUsingGraphics() )
	{
		if ( nFrame < 0 || nFrame >= m_nFrameCount )
		{
			// FIXME: Use the well-known 'error' id instead of frame 0
			nFrame = 0;
			//			Assert(0);
		}

		// Make sure we've actually allocated the texture handle
		Assert( HasBeenAllocated() );

		g_pShaderAPI->BindTexture( sampler1, m_pTextureHandles[nFrame] );
	}
}


void CTexture::BindVertexTexture( VertexTextureSampler_t sampler, int nFrame )
{
	if ( g_pShaderDevice->IsUsingGraphics() )
	{
		if ( nFrame < 0 || nFrame >= m_nFrameCount )
		{
			// FIXME: Use the well-known 'error' id instead of frame 0
			nFrame = 0;
			//			Assert(0);
		}

		// Make sure we've actually allocated the texture
		Assert( HasBeenAllocated() );

		g_pShaderAPI->BindVertexTexture( sampler, m_pTextureHandles[nFrame] );
	}
}


//-----------------------------------------------------------------------------
// Set this texture as a render target
//-----------------------------------------------------------------------------
bool CTexture::SetRenderTarget( int nRenderTargetID )
{
	return SetRenderTarget( nRenderTargetID, NULL );
}

//-----------------------------------------------------------------------------
// Set this texture as a render target
// Optionally bind pDepthTexture as depth buffer
//-----------------------------------------------------------------------------
bool CTexture::SetRenderTarget( int nRenderTargetID, ITexture *pDepthTexture )
{
	if ( ( m_nFlags & TEXTUREFLAGS_RENDERTARGET ) == 0 )
		return false;

	// Make sure we've actually allocated the texture handles
	Assert( HasBeenAllocated() );

	ShaderAPITextureHandle_t textureHandle = m_pTextureHandles[0];

	ShaderAPITextureHandle_t depthTextureHandle = (uintp)SHADER_RENDERTARGET_DEPTHBUFFER;

	if ( m_nFlags & TEXTUREFLAGS_DEPTHRENDERTARGET )
	{
		Assert( m_nFrameCount >= 2 );
		depthTextureHandle = m_pTextureHandles[1];
	} 
	else if ( m_nFlags & TEXTUREFLAGS_NODEPTHBUFFER )
	{
		// GR - render target without depth buffer	
		depthTextureHandle = (uintp)SHADER_RENDERTARGET_NONE;
	}

	if ( pDepthTexture)
	{
		depthTextureHandle = static_cast<ITextureInternal *>(pDepthTexture)->GetTextureHandle(0);
	}

	g_pShaderAPI->SetRenderTargetEx( nRenderTargetID, textureHandle, depthTextureHandle );
	return true;
}


//-----------------------------------------------------------------------------
// Reference counting
//-----------------------------------------------------------------------------
void CTexture::IncrementReferenceCount( void )
{
	++m_nRefCount;
}

void CTexture::DecrementReferenceCount( void )
{
	--m_nRefCount;
}

int CTexture::GetReferenceCount( )
{
	return m_nRefCount;
}


//-----------------------------------------------------------------------------
// Various accessor methods
//-----------------------------------------------------------------------------
const char* CTexture::GetName( ) const
{
	return m_Name.String();
}

const char* CTexture::GetTextureGroupName( ) const
{
	return m_TextureGroupName.String();
}

void CTexture::SetName( const char* pName )
{
	// normalize and convert to a symbol
	char szCleanName[MAX_PATH];
	m_Name = NormalizeTextureName( pName, szCleanName, sizeof( szCleanName ) );

#ifdef _DEBUG
	if ( m_pDebugName )
	{
		delete [] m_pDebugName;
	}
	int nLen = V_strlen( szCleanName ) + 1;
	m_pDebugName = new char[nLen];
	V_memcpy( m_pDebugName, szCleanName, nLen );
#endif
}

ImageFormat CTexture::GetImageFormat()	const
{
	return m_ImageFormat;
}

int CTexture::GetMappingWidth()	const
{
	return m_nMappingWidth;
}

int CTexture::GetMappingHeight() const
{
	return m_nMappingHeight;
}

int CTexture::GetMappingDepth() const
{
	return m_nMappingDepth;
}

int CTexture::GetActualWidth() const
{
	return m_nActualWidth;
}

int CTexture::GetActualHeight()	const
{
	return m_nActualHeight;
}

int CTexture::GetActualDepth()	const
{
	return m_nActualDepth;
}

int CTexture::GetNumAnimationFrames() const
{
	return m_nFrameCount;
}

void CTexture::GetReflectivity( Vector& reflectivity )
{
	Precache();
	VectorCopy( m_vecReflectivity, reflectivity );
}

//-----------------------------------------------------------------------------
// Little helper polling methods
//-----------------------------------------------------------------------------
bool CTexture::IsTranslucent() const
{
	return ( m_nFlags & (TEXTUREFLAGS_ONEBITALPHA | TEXTUREFLAGS_EIGHTBITALPHA) ) != 0;
}

bool CTexture::IsNormalMap( void ) const
{
	return ( ( m_nFlags & TEXTUREFLAGS_NORMAL ) != 0 );
}

bool CTexture::IsCubeMap( void ) const
{
	return ( ( m_nFlags & TEXTUREFLAGS_ENVMAP ) != 0 );
}

bool CTexture::IsRenderTarget( void ) const
{
	return ( ( m_nFlags & TEXTUREFLAGS_RENDERTARGET ) != 0 );
}

bool CTexture::IsTempRenderTarget( void ) const
{
	return ( ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_TEMPRENDERTARGET ) != 0 );
}

bool CTexture::IsProcedural() const
{
	return ( (m_nFlags & TEXTUREFLAGS_PROCEDURAL) != 0 );
}

bool CTexture::IsMipmapped() const
{
	return ( (m_nFlags & TEXTUREFLAGS_NOMIP) == 0 );
}

unsigned int CTexture::GetFlags() const
{
	return m_nFlags;
}

void CTexture::ForceLODOverride( int iNumLodsOverrideUpOrDown )
{
	TextureLodOverride::OverrideInfo oi( iNumLodsOverrideUpOrDown, iNumLodsOverrideUpOrDown );
	TextureLodOverride::Add( GetName(), oi );
	Download( NULL );
}


bool CTexture::IsError() const
{
	return ( (m_nInternalFlags & TEXTUREFLAGSINTERNAL_ERROR) != 0 );
}

bool CTexture::HasBeenAllocated() const
{
	return ( (m_nInternalFlags & TEXTUREFLAGSINTERNAL_ALLOCATED) != 0 );
}

bool CTexture::IsVolumeTexture() const
{
	return (m_nMappingDepth > 1);
}

//-----------------------------------------------------------------------------
// Sets the filtering + clamping modes on the texture
//-----------------------------------------------------------------------------
void CTexture::SetFilteringAndClampingMode()
{
	if( !HasBeenAllocated() )
		return;

	int nCount = m_nFrameCount;
	if ( IsX360() && IsRenderTarget() )
	{
		// 360 render targets have a reserved surface
		nCount--;
	}

	for ( int iFrame = 0; iFrame < nCount; ++iFrame )
	{
		Modify( iFrame );			// Indicate we're changing state with respect to a particular frame
		SetWrapState();				// Send the appropriate wrap/clamping modes to the shaderapi.
		SetFilterState();			// Set the filtering mode for the texture after downloading it.
									// NOTE: Apparently, the filter state cannot be set until after download
	}
}

//-----------------------------------------------------------------------------
// Loads up the non-fallback information about the texture 
//-----------------------------------------------------------------------------
void CTexture::Precache()
{
	// We only have to do something in the case of a file texture
	if ( IsRenderTarget() || IsProcedural() )
		return;

	if ( HasBeenAllocated() )
		return;

	// Blow off env_cubemap too...
	if ( !Q_strnicmp( m_Name.String(), "env_cubemap", 12 ))
		return;

	IVTFTexture *pVTFTexture = GetScratchVTFTexture();

	// The texture name doubles as the relative file name
	// It's assumed to have already been set by this point	
	// Compute the cache name
	char pCacheFileName[MATERIAL_MAX_PATH];
	Q_snprintf( pCacheFileName, sizeof( pCacheFileName ), "materials/%s" TEXTURE_FNAME_EXTENSION, m_Name.String() );

	int nHeaderSize = VTFFileHeaderSize( VTF_MAJOR_VERSION );
	unsigned char *pMem = (unsigned char *)stackalloc( nHeaderSize );
	CUtlBuffer buf( pMem, nHeaderSize );
	if ( !g_pFullFileSystem->ReadFile( pCacheFileName, NULL, buf, nHeaderSize ) )	
	{
		goto precacheFailed;
	}

	if ( !pVTFTexture->Unserialize( buf, true ) )
	{
		Warning( "Error reading material \"%s\"\n", pCacheFileName );
		goto precacheFailed;
	}

	// NOTE: Don't set the image format in case graphics are active
	VectorCopy( pVTFTexture->Reflectivity(), m_vecReflectivity );
	m_nMappingWidth = pVTFTexture->Width();
	m_nMappingHeight = pVTFTexture->Height();
	m_nMappingDepth = pVTFTexture->Depth();
	m_nFlags = pVTFTexture->Flags();
	m_nFrameCount = pVTFTexture->FrameCount();
	if ( !m_pTextureHandles )
	{
		// NOTE: m_nFrameCount and m_pTextureHandles are strongly associated
		// whenever one is modified the other must also be modified
		AllocateTextureHandles();
	}

	return;

precacheFailed:
	m_vecReflectivity.Init( 0, 0, 0 );
	m_nMappingWidth = 32;
	m_nMappingHeight = 32;
	m_nMappingDepth = 1;
	m_nFlags = TEXTUREFLAGS_NOMIP;
	m_nInternalFlags |= TEXTUREFLAGSINTERNAL_ERROR;
	m_nFrameCount = 1;
	if ( !m_pTextureHandles )
	{
		// NOTE: m_nFrameCount and m_pTextureHandles are strongly associated
		// whenever one is modified the other must also be modified
		AllocateTextureHandles();
	}
}


//-----------------------------------------------------------------------------
// Loads the low-res image from the texture 
//-----------------------------------------------------------------------------
void CTexture::LoadLowResTexture( IVTFTexture *pTexture )
{
	delete [] m_pLowResImage;
	m_pLowResImage = NULL;

	if ( pTexture->LowResWidth() == 0 || pTexture->LowResHeight() == 0 )
	{
		m_LowResImageWidth = m_LowResImageHeight = 0;
		return;
	}

	m_LowResImageWidth = pTexture->LowResWidth();
	m_LowResImageHeight = pTexture->LowResHeight();

	m_pLowResImage = new unsigned char[m_LowResImageWidth * m_LowResImageHeight * 3];
#ifdef DBGFLAG_ASSERT
	bool retVal = 
#endif
		ImageLoader::ConvertImageFormat( pTexture->LowResImageData(), pTexture->LowResFormat(), 
			m_pLowResImage, IMAGE_FORMAT_RGB888, m_LowResImageWidth, m_LowResImageHeight );
	Assert( retVal );
}

void *CTexture::GetResourceData( uint32 eDataType, size_t *pnumBytes ) const
{
	for ( DataChunk const *pDataChunk = m_arrDataChunks.Base(),
		  *pDataChunkEnd = pDataChunk + m_arrDataChunks.Count();
		  pDataChunk < pDataChunkEnd; ++pDataChunk )
	{
		if ( ( pDataChunk->m_eType & ~RSRCF_MASK ) == eDataType )
		{
			if ( ( pDataChunk->m_eType & RSRCF_HAS_NO_DATA_CHUNK ) == 0 )
			{
				if ( pnumBytes)
					*pnumBytes = pDataChunk->m_numBytes;
				return pDataChunk->m_pvData;
			}
			else
			{
				if ( pnumBytes )
					*pnumBytes = sizeof( pDataChunk->m_numBytes );

				return ( void *)( &pDataChunk->m_numBytes );
			}
		}
	}
	if ( pnumBytes )
		pnumBytes = 0;
	return NULL;
}

#pragma pack(1)

struct DXTColBlock
{
	unsigned short col0;
	unsigned short col1;

	// no bit fields - use bytes
	unsigned char row[4];
};

struct DXTAlphaBlock3BitLinear
{
	unsigned char alpha0;
	unsigned char alpha1;

	unsigned char stuff[6];
};

#pragma pack()

static void FillCompressedTextureWithSingleColor( int red, int green, int blue, int alpha, unsigned char *pImageData, 
												 int width, int height, int depth, ImageFormat imageFormat )
{
	Assert( ( width < 4 ) || !( width % 4 ) );
	Assert( ( height < 4 ) || !( height % 4 ) );
	Assert( ( depth < 4 ) || !( depth % 4 ) );

	if ( width < 4 && width > 0 )
	{
		width = 4;
	}
	if ( height < 4 && height > 0 )
	{
		height = 4;
	}
	if ( depth < 4 && depth > 1 )
	{
		depth = 4;
	}
	int numBlocks = ( width * height ) >> 4;
	numBlocks *= depth;
	
	DXTColBlock colorBlock;
	memset( &colorBlock, 0, sizeof( colorBlock ) );
	( ( BGR565_t * )&( colorBlock.col0 ) )->Set( red, green, blue );
	( ( BGR565_t * )&( colorBlock.col1 ) )->Set( red, green, blue );

	switch( imageFormat )
	{
	case IMAGE_FORMAT_DXT1:
	case IMAGE_FORMAT_ATI1N:	// Invalid block data, but correct memory footprint
		{
			int i;
			for( i = 0; i < numBlocks; i++ )
			{
				memcpy( pImageData + i * 8, &colorBlock, sizeof( colorBlock ) );
			}
		}
		break;
	case IMAGE_FORMAT_DXT5:
	case IMAGE_FORMAT_ATI2N:
		{
			int i;
			for( i = 0; i < numBlocks; i++ )
			{
//				memset( pImageData + i * 16, 0, 16 );
				memcpy( pImageData + i * 16 + 8, &colorBlock, sizeof( colorBlock ) );
//				memset( pImageData + i * 16 + 8, 0xffff, 8 ); // alpha block
			}
		}
		break;
	default:
		Assert( 0 );
		break;
	}
}

// This table starts out like the programmatic logic that used to be here,
// but then has some other colors, so that we don't see repeats.
// Also, there is no black, which seems to be an error condition on OpenGL.
// There also aren't any zeros in this table, since these colors may get
// multiplied with, say, vertex colors which are tinted, resulting in black pixels.
int sg_nMipLevelColors[14][3] = { {  64, 255,  64 },  // Green
								  { 255,  64,  64 },  // Red
								  { 255, 255,  64 },  // Yellow
								  {  64,  64, 255 },  // Blue
								  {  64, 255, 255 },  // Cyan
								  { 255,  64, 255 },  // Magenta
								  { 255, 255, 255 },  // White
								  { 255, 150, 150 },  // Light Red
								  { 255, 255, 150 },  // Light Yellow
								  { 150, 150, 255 },  // Light Blue
								  { 150, 255, 255 },  // Light Cyan
								  { 255, 150, 255 },  // Light Magenta
								  { 150, 150, 128 },  // Light Gray
								  { 138, 131,  64 } };// Brown

//-----------------------------------------------------------------------------
// Generate a texture that shows the various mip levels
//-----------------------------------------------------------------------------
void CTexture::GenerateShowMipLevelsTextures( IVTFTexture *pTexture )
{
	if( pTexture->FaceCount() > 1 )
		return;

	switch( pTexture->Format() )
	{
	// These are formats that we don't bother with for generating mip level textures.
	case IMAGE_FORMAT_RGBA16161616F:
	case IMAGE_FORMAT_R32F:
	case IMAGE_FORMAT_RGB323232F:
	case IMAGE_FORMAT_RGBA32323232F:
	case IMAGE_FORMAT_UV88:
		break;
	default:
		for (int iFrame = 0; iFrame < pTexture->FrameCount(); ++iFrame )
		{
			for (int iFace = 0; iFace < pTexture->FaceCount(); ++iFace )
			{
				for (int iMip = 0; iMip < pTexture->MipCount(); ++iMip )
				{
					int red	  = sg_nMipLevelColors[iMip][0];//( ( iMip + 1 ) & 2 ) ? 255 : 0;
					int green = sg_nMipLevelColors[iMip][1];//( ( iMip + 1 ) & 1 ) ? 255 : 0;
					int blue  = sg_nMipLevelColors[iMip][2];//( ( iMip + 1 ) & 4 ) ? 255 : 0;

					int nWidth, nHeight, nDepth;
					pTexture->ComputeMipLevelDimensions( iMip, &nWidth, &nHeight, &nDepth );
					if( pTexture->Format() == IMAGE_FORMAT_DXT1  || pTexture->Format() == IMAGE_FORMAT_DXT5 ||
					    pTexture->Format() == IMAGE_FORMAT_ATI1N || pTexture->Format() == IMAGE_FORMAT_ATI2N )
					{
						unsigned char *pImageData = pTexture->ImageData( iFrame, iFace, iMip, 0, 0, 0 );
						int alpha = 255;
						FillCompressedTextureWithSingleColor( red, green, blue, alpha, pImageData, nWidth, nHeight, nDepth, pTexture->Format() );
					}
					else
					{
						for ( int z = 0; z < nDepth; ++z )
						{
							CPixelWriter pixelWriter;
							pixelWriter.SetPixelMemory( pTexture->Format(), 
								pTexture->ImageData( iFrame, iFace, iMip, 0, 0, z ), pTexture->RowSizeInBytes( iMip ) );

							for (int y = 0; y < nHeight; ++y)
							{
								pixelWriter.Seek( 0, y );
								for (int x = 0; x < nWidth; ++x)
								{
									pixelWriter.WritePixel( red, green, blue, 255 );
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}


//-----------------------------------------------------------------------------
// Generate a texture that shows the various mip levels
//-----------------------------------------------------------------------------
void CTexture::CopyLowResImageToTexture( IVTFTexture *pTexture )
{
	int nFlags = pTexture->Flags();
	nFlags |= TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_POINTSAMPLE;
	nFlags &= ~(TEXTUREFLAGS_TRILINEAR | TEXTUREFLAGS_ANISOTROPIC | TEXTUREFLAGS_HINT_DXT5);
	nFlags &= ~(TEXTUREFLAGS_NORMAL | TEXTUREFLAGS_ENVMAP);
	nFlags &= ~(TEXTUREFLAGS_ONEBITALPHA | TEXTUREFLAGS_EIGHTBITALPHA);

	Assert( pTexture->FrameCount() == 1 );

	Init( pTexture->Width(), pTexture->Height(), 1, IMAGE_FORMAT_BGR888, nFlags, 1 );
	pTexture->Init( m_LowResImageWidth, m_LowResImageHeight, 1, IMAGE_FORMAT_BGR888, nFlags, 1 );

	// Don't bother computing the actual size; it's actually equal to the low-res size
	// With only one mip level
	m_nActualWidth = m_LowResImageWidth;
	m_nActualHeight = m_LowResImageHeight;
	m_nActualDepth = 1;
	m_nActualMipCount = 1;

	// Copy the row-res image into the VTF Texture
	CPixelWriter pixelWriter;
	pixelWriter.SetPixelMemory( pTexture->Format(), 
		pTexture->ImageData( 0, 0, 0 ), pTexture->RowSizeInBytes( 0 ) );

	unsigned char *pLowResImage = m_pLowResImage;
	for ( int y = 0; y < m_LowResImageHeight; ++y )
	{
		pixelWriter.Seek( 0, y );
		for ( int x = 0; x < m_LowResImageWidth; ++x )
		{
			int red = pLowResImage[0];
			int green = pLowResImage[1];
			int blue = pLowResImage[2];
			pLowResImage += 3;

			pixelWriter.WritePixel( red, green, blue, 255 );
		}
	}
}

//-----------------------------------------------------------------------------
// Sets up debugging texture bits, if appropriate
//-----------------------------------------------------------------------------
bool CTexture::SetupDebuggingTextures( IVTFTexture *pVTFTexture )
{
	if ( pVTFTexture->Flags() & TEXTUREFLAGS_NODEBUGOVERRIDE )
		return false;

	// The all mips flag is typically used on detail textures, which can
	// really mess up visualization if we apply the debug-colorized
	// versions of them to debug-colorized base textures, so skip 'em
	if ( g_config.nShowMipLevels && !(pVTFTexture->Flags() & TEXTUREFLAGS_ALL_MIPS) )
	{
		// mat_showmiplevels 1 means don't do normal maps
		if ( ( g_config.nShowMipLevels == 1 ) && ( pVTFTexture->Flags() & ( TEXTUREFLAGS_NORMAL | TEXTUREFLAGS_SSBUMP ) ) )
			return false;

		// mat_showmiplevels 2 means don't do base textures
		if ( ( g_config.nShowMipLevels == 2 ) && !( pVTFTexture->Flags() & ( TEXTUREFLAGS_NORMAL | TEXTUREFLAGS_SSBUMP ) ) )
			return false;

		// This mode shows the mip levels as different colors
		GenerateShowMipLevelsTextures( pVTFTexture );
		return true;
	}
	else if ( g_config.bShowLowResImage && pVTFTexture->FrameCount() == 1 && 
		pVTFTexture->FaceCount() == 1 && ((pVTFTexture->Flags() & TEXTUREFLAGS_NORMAL) == 0) &&
		m_LowResImageWidth != 0 && m_LowResImageHeight != 0 )
	{
		// This mode just uses the low res texture
		CopyLowResImageToTexture( pVTFTexture );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Converts the texture to the actual format
// Returns true if conversion applied, false otherwise
//-----------------------------------------------------------------------------
bool CTexture::ConvertToActualFormat( IVTFTexture *pVTFTexture, bool bUnused )
{
	if ( !g_pShaderDevice->IsUsingGraphics() )
		return false;

	bool bConverted = false;

	ImageFormat fmt = m_ImageFormat;

	ImageFormat dstFormat = ComputeActualFormat( pVTFTexture->Format() );
	if ( fmt != dstFormat )
	{
		pVTFTexture->ConvertImageFormat( dstFormat, false );

		m_ImageFormat = dstFormat;
		bConverted = true;
	}
	else if ( HardwareConfig()->GetHDRType() == HDR_TYPE_INTEGER &&
		     fmt == dstFormat && dstFormat == IMAGE_FORMAT_RGBA16161616F )
	{
		// This is to force at most the precision of int16 for fp16 texture when running the integer path.
		pVTFTexture->ConvertImageFormat( IMAGE_FORMAT_RGBA16161616, false );
		pVTFTexture->ConvertImageFormat( IMAGE_FORMAT_RGBA16161616F, false );
		bConverted = true;
	}

	return bConverted;
}

void CTexture::GetFilename( char *pOut, int maxLen ) const
{
	const char *pName = m_Name.String();
	bool bIsUNCName = ( pName[0] == '/' && pName[1] == '/' && pName[2] != '/' );

	if ( !bIsUNCName )
	{
		Q_snprintf( pOut, maxLen, 
			"materials/%s" TEXTURE_FNAME_EXTENSION, pName );
	}
	else
	{
		Q_snprintf( pOut, maxLen, "%s" TEXTURE_FNAME_EXTENSION, pName );
	}
}


void CTexture::ReloadFilesInList( IFileList *pFilesToReload )
{
	if ( IsProcedural() || IsRenderTarget() )
		return;

	char filename[MAX_PATH];
	GetFilename( filename, sizeof( filename ) );
	if ( pFilesToReload->IsFileInList( filename ) )
	{
		Download();
	}
}

//-----------------------------------------------------------------------------
// Loads the texture bits from a file.
//-----------------------------------------------------------------------------
IVTFTexture *CTexture::LoadTextureBitsFromFile( char *pCacheFileName, char **ppResolvedFilename, bool bPaired )
{
#line 2989
	tmZoneFiltered( TELEMETRY_LEVEL0, 50, TMZF_NONE, "%s %s", __FUNCTION__, tmDynamicString( TELEMETRY_LEVEL0, pCacheFileName ) );
#line 2710
	IVTFTexture *pVTFTexture = bPaired ? GetScratchVTFTexturePaired() : GetScratchVTFTexture();

	CUtlBuffer buf;

	FileHandle_t fileHandle = FILESYSTEM_INVALID_HANDLE;
	while ( fileHandle == FILESYSTEM_INVALID_HANDLE )			// run until found a file or out of rules
	{
		fileHandle = g_pFullFileSystem->OpenEx( pCacheFileName, "rb", 0, MaterialSystem()->GetForcedTextureLoadPathID(), ppResolvedFilename );
		if ( fileHandle == FILESYSTEM_INVALID_HANDLE )
		{
			// try any fallbacks.
			char *pHdrExt = Q_stristr( pCacheFileName, ".hdr" TEXTURE_FNAME_EXTENSION );
			if ( pHdrExt )
			{
				DevWarning( "A custom HDR cubemap \"%s\": cannot be found on disk.\n"
					"This really should have a HDR version, trying a fall back to a non-HDR version.\n", pCacheFileName );
				strcpy( pHdrExt, TEXTURE_FNAME_EXTENSION );
			}
			else
			{
				// no more fallbacks
				break;
			}
		}
	}

	if ( fileHandle == FILESYSTEM_INVALID_HANDLE )
	{
		if ( Q_strnicmp( m_Name.String(), "env_cubemap", 12 ) )
		{
			if ( IsPosix() )
			{
				Msg( "\n ##### CTexture::LoadTextureBitsFromFile couldn't find %s\n", pCacheFileName );
			}
			DevWarning( "\"%s\": can't be found on disk\n", pCacheFileName );
		}
		return HandleFileLoadFailedTexture( pVTFTexture );
	}

	int nHeaderSize = VTFFileHeaderSize( VTF_MAJOR_VERSION );

	// restrict read to the header only!
	// header provides info to avoid reading the entire file
	int nBytesOptimalRead = GetOptimalReadBuffer( fileHandle, nHeaderSize, buf );
	int nBytesRead = g_pFullFileSystem->ReadEx( buf.Base(), nBytesOptimalRead, MIN( nHeaderSize, (int)g_pFullFileSystem->Size( fileHandle ) ), fileHandle ); // only read as much as the file has
	buf.SeekPut( CUtlBuffer::SEEK_HEAD, nBytesRead );
	nBytesRead = nHeaderSize = ((VTFFileBaseHeader_t *)buf.Base())->headerSize;
	g_pFullFileSystem->Seek( fileHandle, nHeaderSize, FILESYSTEM_SEEK_HEAD );

	// Unserialize the header only
	// need the header first to determine remainder of data
	if ( !pVTFTexture->Unserialize( buf, true ) )
	{
		Warning( "Error reading texture header \"%s\"\n", pCacheFileName );
		g_pFullFileSystem->Close( fileHandle );
		return HandleFileLoadFailedTexture( pVTFTexture );
	}

	// OSX hackery
	int nPreserveFlags = m_nFlags & TEXTUREFLAGS_SRGB;

	// Seek the reading back to the front of the buffer
	buf.SeekGet( CUtlBuffer::SEEK_HEAD, 0 );

	// Initialize the texture class with vtf header data before operations
	Init( 	pVTFTexture->Width(), 
		pVTFTexture->Height(), 
		pVTFTexture->Depth(), 
		pVTFTexture->Format(), 
		pVTFTexture->Flags() | nPreserveFlags, 
		pVTFTexture->FrameCount() );

	VectorCopy( pVTFTexture->Reflectivity(), m_vecReflectivity );

	// Compute the actual texture dimensions
	int nMipSkipCount = ComputeActualSize( false, pVTFTexture );

	// Determine how much of the file to read in
	int nFileSize = pVTFTexture->FileSize( nMipSkipCount );
	int nActualFileSize = (int)g_pFullFileSystem->Size( fileHandle );
	if ( nActualFileSize < nFileSize )
	{
		if ( mat_spew_on_texture_size.GetInt() )
			DevMsg( "Bad VTF data for %s, expected file size:%d actual file size:%d \n", pCacheFileName, nFileSize, nActualFileSize );
		nFileSize = nActualFileSize;
	}

	// Read only the portion of the file that we care about
	g_pFullFileSystem->Seek( fileHandle, 0, FILESYSTEM_SEEK_HEAD );
	nBytesOptimalRead = GetOptimalReadBuffer( fileHandle, nFileSize, buf );
	nBytesRead = g_pFullFileSystem->ReadEx( buf.Base(), nBytesOptimalRead, nFileSize, fileHandle );
	g_pFullFileSystem->Close( fileHandle );
	buf.SeekPut( CUtlBuffer::SEEK_HEAD, nBytesRead );

	// NOTE: Skipping mip levels here will cause the size to be changed...
	if ( !pVTFTexture->Unserialize( buf, false, nMipSkipCount ) )
	{
		Warning( "Error reading material data \"%s\"\n", pCacheFileName );
		return HandleFileLoadFailedTexture( pVTFTexture );
	}

	// Build the low-res texture
	LoadLowResTexture( pVTFTexture );

	// Load the resources
	if ( unsigned int uiRsrcCount = pVTFTexture->GetResourceTypes( NULL, 0 ) )
	{
		uint32 *arrRsrcTypes = ( uint32 * )_alloca( uiRsrcCount * sizeof( unsigned int ) );
		pVTFTexture->GetResourceTypes( arrRsrcTypes, uiRsrcCount );

		m_arrDataChunks.EnsureCapacity( uiRsrcCount );
		for ( uint32 *arrRsrcTypesEnd = arrRsrcTypes + uiRsrcCount;
			  arrRsrcTypes < arrRsrcTypesEnd; ++arrRsrcTypes )
		{
			switch ( *arrRsrcTypes )
			{
			case VTF_LEGACY_RSRC_LOW_RES_IMAGE:
			case VTF_LEGACY_RSRC_IMAGE:
				// These stock types use specific load routines
				continue;
					
			default:
				{
					DataChunk dc;
					dc.m_eType = *arrRsrcTypes;
					dc.m_eType &= ~RSRCF_MASK;

					size_t numBytes;
					if ( void *pvData = pVTFTexture->GetResourceData( dc.m_eType, &numBytes ) )
					{
						Assert( numBytes >= sizeof( uint32 ) );
						if ( numBytes == sizeof( dc.m_numBytes ) )
						{
							dc.m_eType |= RSRCF_HAS_NO_DATA_CHUNK;
							dc.m_pvData = NULL;
							memcpy( &dc.m_numBytes, pvData, numBytes );
						}
						else
						{
							dc.Allocate( numBytes );
							memcpy( dc.m_pvData, pvData, numBytes );
						}

						m_arrDataChunks.AddToTail( dc );
					}
				}
			}
		}
	}

	// Try to set up debugging textures, if we're in a debugging mode
	if ( !IsProcedural() )
		SetupDebuggingTextures( pVTFTexture );

	ConvertToActualFormat( pVTFTexture );

	return pVTFTexture;
}


IVTFTexture *CTexture::HandleFileLoadFailedTexture( IVTFTexture *pVTFTexture )
{
	// create the error texture

	// This will make a checkerboard texture to indicate failure
	pVTFTexture->Init( 32, 32, 1, IMAGE_FORMAT_BGRA8888, m_nFlags, 1 );
	Init( pVTFTexture->Width(), pVTFTexture->Height(), pVTFTexture->Depth(), pVTFTexture->Format(), 
		  pVTFTexture->Flags(), pVTFTexture->FrameCount() );
	m_vecReflectivity.Init( 0.5f, 0.5f, 0.5f );

	// NOTE: For mat_picmip to work, we must use the same size (32x32)
	// Which should work since every card can handle textures of that size
	m_nActualWidth = pVTFTexture->Width();
	m_nActualHeight = pVTFTexture->Height();
	m_nActualMipCount = 1;

	// generate the checkerboard
	TextureManager()->GenerateErrorTexture( this, pVTFTexture );
	ConvertToActualFormat( pVTFTexture );

	// Deactivate procedural texture...
	m_nFlags &= ~TEXTUREFLAGS_PROCEDURAL;
	m_nInternalFlags |= TEXTUREFLAGSINTERNAL_ERROR;

	return pVTFTexture;
}

//-----------------------------------------------------------------------------
// Computes subrect for a particular miplevel
//-----------------------------------------------------------------------------
void CTexture::ComputeMipLevelSubRect( const Rect_t* pSrcRect, int nMipLevel, Rect_t *pSubRect )
{
	if (nMipLevel == 0)
	{
		*pSubRect = *pSrcRect;
		return;
	}

	float flInvShrink = 1.0f / (float)(1 << nMipLevel);
	pSubRect->x = pSrcRect->x * flInvShrink;
	pSubRect->y = pSrcRect->y * flInvShrink;
	pSubRect->width = (int)ceil( (pSrcRect->x + pSrcRect->width) * flInvShrink ) - pSubRect->x;
	pSubRect->height = (int)ceil( (pSrcRect->y + pSrcRect->height) * flInvShrink ) - pSubRect->y;
}


//-----------------------------------------------------------------------------
// Computes the face count + first face
//-----------------------------------------------------------------------------
void CTexture::GetDownloadFaceCount( int &nFirstFace, int &nFaceCount )
{
	nFaceCount = 1;
	nFirstFace = 0;
	if ( IsCubeMap() )
	{
		if ( HardwareConfig()->SupportsCubeMaps() )
		{
			nFaceCount = CUBEMAP_FACE_COUNT-1;
		}
		else
		{
			// This will cause us to use the spheremap instead of the cube faces
			// in the case where we don't support cubemaps
			nFirstFace = CUBEMAP_FACE_SPHEREMAP;
		}
	}
}

//-----------------------------------------------------------------------------
// Fixup a queue loaded texture with the delayed hi-res data
//-----------------------------------------------------------------------------
void CTexture::FixupTexture( const void *pData, int nSize, LoaderError_t loaderError )
{
	if ( loaderError != LOADERERROR_NONE )
	{
		// mark as invalid
		nSize = 0;
	}

	m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_QUEUEDLOAD;

	// Make sure we've actually allocated the texture handles
	Assert( HasBeenAllocated() );
} 

static void QueuedLoaderCallback( void *pContext, void *pContext2, const void *pData, int nSize, LoaderError_t loaderError )
{
	reinterpret_cast< CTexture * >( pContext )->FixupTexture( pData, nSize, loaderError );
}

//-----------------------------------------------------------------------------
// Generates the procedural bits
//-----------------------------------------------------------------------------
IVTFTexture *CTexture::ReconstructPartialProceduralBits( const Rect_t *pRect, Rect_t *pActualRect )
{
	// Figure out the actual size for this texture based on the current mode
	ComputeActualSize();

	// Figure out how many mip levels we're skipping...
	int nSizeFactor = GetMappingWidth() / GetActualWidth();
	int nMipSkipCount = 0;
	while (nSizeFactor > 1)
	{
		nSizeFactor >>= 1;
		++nMipSkipCount;
	}

	// Determine a rectangle appropriate for the actual size...
	// It must bound all partially-covered pixels..
	ComputeMipLevelSubRect( pRect, nMipSkipCount, pActualRect );

	// Create the texture
	IVTFTexture *pVTFTexture = GetScratchVTFTexture();

	// Initialize the texture
	pVTFTexture->Init( m_nActualWidth, m_nActualHeight, m_nActualDepth,
		ComputeActualFormat( m_ImageFormat ), m_nFlags, m_nFrameCount );

	// Generate the bits from the installed procedural regenerator
	if ( m_pTextureRegenerator )
	{
		m_pTextureRegenerator->RegenerateTextureBits( this, pVTFTexture, pActualRect );
	}
	else
	{
		// In this case, we don't have one, so just use a checkerboard...
		TextureManager()->GenerateErrorTexture( this, pVTFTexture );
	}

	return pVTFTexture;
}


//-----------------------------------------------------------------------------
// Regenerates the bits of a texture within a particular rectangle
//-----------------------------------------------------------------------------
void CTexture::ReconstructPartialTexture( const Rect_t *pRect )
{
	// FIXME: for now, only procedural textures can handle sub-rect specification.
	Assert( IsProcedural() );

	// Also, we need procedural textures that have only a single copy!!
	// Otherwise this partial upload will not occur on all copies
	Assert( m_nFlags & TEXTUREFLAGS_SINGLECOPY );

	Rect_t vtfRect;
	IVTFTexture *pVTFTexture = ReconstructPartialProceduralBits( pRect, &vtfRect );

	// FIXME: for now, depth textures do not work with this.
	Assert( pVTFTexture->Depth() == 1 );

	// Make sure we've allocated the API textures
	if ( !HasBeenAllocated() )
	{
		AllocateShaderAPITextures();
	}

	int nFaceCount, nFirstFace;
	GetDownloadFaceCount( nFirstFace, nFaceCount );
	
	// Blit down portions of the various VTF frames into the board memory
	int nStride;
	Rect_t mipRect;
	for ( int iFrame = 0; iFrame < m_nFrameCount; ++iFrame )
	{
		Modify( iFrame );

		for ( int iFace = 0; iFace < nFaceCount; ++iFace )
		{
			for ( int iMip = 0; iMip < m_nActualMipCount;  ++iMip )
			{
				pVTFTexture->ComputeMipLevelSubRect( &vtfRect, iMip, &mipRect );
				nStride = pVTFTexture->RowSizeInBytes( iMip );
				unsigned char *pBits = pVTFTexture->ImageData( iFrame, iFace + nFirstFace, iMip, mipRect.x, mipRect.y, 0 );
				g_pShaderAPI->TexSubImage2D( 
					iMip, 
					iFace, 
					mipRect.x, 
					mipRect.y,
					0,
					mipRect.width, 
					mipRect.height, 
					pVTFTexture->Format(), 
					nStride, 
					false,
					pBits );
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Generates the procedural bits
//-----------------------------------------------------------------------------
IVTFTexture *CTexture::ReconstructProceduralBits()
{
	// Figure out the actual size for this texture based on the current mode
	ComputeActualSize();

	// Create the texture
	IVTFTexture *pVTFTexture = GetScratchVTFTexture();

	// Initialize the texture
	pVTFTexture->Init( m_nActualWidth, m_nActualHeight, m_nActualDepth,
		ComputeActualFormat( m_ImageFormat ), m_nFlags, m_nFrameCount );

	// Generate the bits from the installed procedural regenerator
	if ( m_pTextureRegenerator )
	{
		Rect_t rect;
		rect.x = 0; rect.y = 0;
		rect.width = m_nActualWidth; 
		rect.height = m_nActualHeight; 
		m_pTextureRegenerator->RegenerateTextureBits( this, pVTFTexture, &rect );
	}
	else
	{
		// In this case, we don't have one, so just use a checkerboard...
		TextureManager()->GenerateErrorTexture( this, pVTFTexture );
	}

	return pVTFTexture;
}

void CTexture::WriteDataToShaderAPITexture( int nFrameCount, int nFaceCount, int nFirstFace, int nMipCount, IVTFTexture *pVTFTexture, ImageFormat fmt )
{
	for ( int iFrame = 0; iFrame < m_nFrameCount; ++iFrame )
	{
		Modify( iFrame );
		g_pShaderAPI->TexImageFromVTF( pVTFTexture, iFrame );
	}
}

bool CTexture::IsDepthTextureFormat( ImageFormat fmt )
{
	return ( ( m_ImageFormat == IMAGE_FORMAT_NV_DST16  ) ||
			 ( m_ImageFormat == IMAGE_FORMAT_NV_DST24  ) ||
			 ( m_ImageFormat == IMAGE_FORMAT_NV_INTZ   ) ||
			 ( m_ImageFormat == IMAGE_FORMAT_NV_RAWZ   ) ||
			 ( m_ImageFormat == IMAGE_FORMAT_ATI_DST16 ) ||
			 ( m_ImageFormat == IMAGE_FORMAT_ATI_DST24 ) );
}

//-----------------------------------------------------------------------------
void CTexture::NotifyUnloadedFile()
{
	// Make sure we have a regular texture that was loaded from a file
	if ( IsProcedural() || IsRenderTarget() || !m_Name.IsValid() )
		return;
	const char *pName = m_Name.String();
	if ( *pName == '\0' )
		return;
	bool bIsUNCName = ( pName[0] == '/' && pName[1] == '/' && pName[2] != '/' );
	if ( bIsUNCName )
		return;

	// Generate the filename
	char pCacheFileName[MATERIAL_MAX_PATH];
	Q_snprintf( pCacheFileName, sizeof( pCacheFileName ), "materials/%s" TEXTURE_FNAME_EXTENSION, pName );

	// Let filesystem know that the file is uncached, so it knows
	// what to do with tracking info
	g_pFullFileSystem->NotifyFileUnloaded( pCacheFileName, "GAME" );
}

//-----------------------------------------------------------------------------
// Sets or updates the texture bits
//-----------------------------------------------------------------------------
void CTexture::ReconstructTexture()
{
	int oldWidth = m_nActualWidth;
	int oldHeight = m_nActualHeight;
	int oldDepth = m_nActualDepth;
	int oldMipCount = m_nActualMipCount;
	int oldFrameCount = m_nFrameCount;

	// FIXME: Should RenderTargets be a special case of Procedural?
	char *pResolvedFilename = NULL;
	IVTFTexture *pVTFTexture = NULL;
	char pCacheFileName[ MATERIAL_MAX_PATH ] = { 0 };
	if ( IsProcedural() )
	{
		// This will call the installed texture bit regeneration interface
		pVTFTexture = ReconstructProceduralBits();
	}
	else if ( IsRenderTarget() )
	{
		// Compute the actual size + format based on the current mode
		ComputeActualSize( true );
	}
	else
	{
		NotifyUnloadedFile();

		const char *pName;
		if ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE )
		{
			pName = "dev/dev_exclude_error";
		}
		else
		{
			pName = m_Name.String();
		}

		bool bIsUNCName = ( pName[0] == '/' && pName[1] == '/' && pName[2] != '/' );
		if ( !bIsUNCName )
		{
			Q_snprintf( pCacheFileName, sizeof( pCacheFileName ), "materials/%s" TEXTURE_FNAME_EXTENSION, pName );
		}
		else
		{
			Q_snprintf( pCacheFileName, sizeof( pCacheFileName ), "%s" TEXTURE_FNAME_EXTENSION, pName );
		}

		// Get the data from disk...
		// NOTE: Reloading the texture bits can cause the texture size, frames, format, pretty much *anything* can change.
		pVTFTexture = LoadTextureBitsFromFile( pCacheFileName, &pResolvedFilename );
	}

	if ( !HasBeenAllocated() ||
		m_nActualWidth != oldWidth ||
		m_nActualHeight != oldHeight ||
		m_nActualDepth != oldDepth ||
		m_nActualMipCount != oldMipCount ||
		m_nFrameCount != oldFrameCount )
	{
		if ( HasBeenAllocated() )
		{
			// This is necessary for the reload case, we may discover there
			// are more frames of a texture animation, for example, which means
			// we can't rely on having the same number of texture frames.
			FreeShaderAPITextures();
		}

		// Create the shader api textures
		AllocateShaderAPITextures();
	}

	// Render Targets just need to be cleared, they have no upload
	if ( IsRenderTarget() )
	{
		// Clear the render target to opaque black

		// Only clear if we're not a depth-stencil texture
		if ( !IsDepthTextureFormat( m_ImageFormat ) )
		{
			CMatRenderContextPtr pRenderContext( MaterialSystem() );
			ITexture *pThisTexture = GetEmbeddedTexture( 0 );
			pRenderContext->PushRenderTargetAndViewport( pThisTexture );						// Push this texture on the stack
			g_pShaderAPI->ClearColor4ub( 0, 0, 0, 0xFF );										// Set the clear color to opaque black
			g_pShaderAPI->ClearBuffers( true, false, false, m_nActualWidth, m_nActualHeight );	// Clear the target
			pRenderContext->PopRenderTargetAndViewport();										// Pop back to previous target
		}
		// no upload
		return;
	}

	// Blit down the texture faces, frames, and mips into the board memory
	int nFirstFace, nFaceCount;
	GetDownloadFaceCount( nFirstFace, nFaceCount );
	
	WriteDataToShaderAPITexture( m_nFrameCount, nFaceCount, nFirstFace, m_nActualMipCount, pVTFTexture, m_ImageFormat );

	// allocated by strdup
	free( pResolvedFilename );

	// the pc can afford to persist a large buffer
	FreeOptimalReadBuffer( 4*1024*1024 );
}


// Get the shaderapi texture handle associated w/ a particular frame
ShaderAPITextureHandle_t CTexture::GetTextureHandle( int nFrame, int nTextureChannel )
{
	if ( nFrame < 0 )
	{
		Warning( "CTexture::GetTextureHandle(): nFrame is < 0!\n" );
		nFrame = 0;
	}
	Assert( nFrame < m_nFrameCount );
	Assert( nTextureChannel < 2 );

	// Make sure we've actually allocated the texture handles
	Assert( m_pTextureHandles );
	Assert( HasBeenAllocated() );

	// Don't get paired handle here...callers of this function don't know about paired textures
	return m_pTextureHandles[nFrame];
}

void CTexture::GetLowResColorSample( float s, float t, float *color ) const
{
	if ( m_LowResImageWidth <= 0 || m_LowResImageHeight <= 0 )
	{
//		Warning( "Programming error: GetLowResColorSample \"%s\": %dx%d\n", m_pName, ( int )m_LowResImageWidth, ( int )m_LowResImageHeight );
		return;
	}

	// force s and t into [0,1)
	if ( s < 0.0f )
	{
		s = ( 1.0f - ( float )( int )s ) + s;
	}
	if ( t < 0.0f )
	{
		t = ( 1.0f - ( float )( int )t ) + t;
	}
	s = s - ( float )( int )s;
	t = t - ( float )( int )t;
	
	s *= m_LowResImageWidth;
	t *= m_LowResImageHeight;
	
	int wholeS, wholeT;
	wholeS = ( int )s;
	wholeT = ( int )t;
	float fracS, fracT;
	fracS = s - ( float )( int )s;
	fracT = t - ( float )( int )t;
	
	// filter twice in the s dimension.
	float sColor[2][3];
	int wholeSPlusOne = ( wholeS + 1 ) % m_LowResImageWidth;
	int wholeTPlusOne = ( wholeT + 1 ) % m_LowResImageHeight;
	sColor[0][0] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeT * m_LowResImageWidth ) * 3 + 0] * ( 1.0f / 255.0f ) );
	sColor[0][1] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeT * m_LowResImageWidth ) * 3 + 1] * ( 1.0f / 255.0f ) );
	sColor[0][2] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeT * m_LowResImageWidth ) * 3 + 2] * ( 1.0f / 255.0f ) );
	sColor[0][0] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeT * m_LowResImageWidth ) * 3 + 0] * ( 1.0f / 255.0f ) );
	sColor[0][1] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeT * m_LowResImageWidth ) * 3 + 1] * ( 1.0f / 255.0f ) );
	sColor[0][2] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeT * m_LowResImageWidth ) * 3 + 2] * ( 1.0f / 255.0f ) );

	sColor[1][0] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeTPlusOne * m_LowResImageWidth ) * 3 + 0] * ( 1.0f / 255.0f ) );
	sColor[1][1] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeTPlusOne * m_LowResImageWidth ) * 3 + 1] * ( 1.0f / 255.0f ) );
	sColor[1][2] = ( 1.0f - fracS ) * ( m_pLowResImage[( wholeS + wholeTPlusOne * m_LowResImageWidth ) * 3 + 2] * ( 1.0f / 255.0f ) );
	sColor[1][0] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeTPlusOne * m_LowResImageWidth ) * 3 + 0] * ( 1.0f / 255.0f ) );
	sColor[1][1] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeTPlusOne * m_LowResImageWidth ) * 3 + 1] * ( 1.0f / 255.0f ) );
	sColor[1][2] += fracS * ( m_pLowResImage[( wholeSPlusOne + wholeTPlusOne * m_LowResImageWidth ) * 3 + 2] * ( 1.0f / 255.0f ) );

	color[0] = sColor[0][0] * ( 1.0f - fracT ) + sColor[1][0] * fracT;
	color[1] = sColor[0][1] * ( 1.0f - fracT ) + sColor[1][1] * fracT;
	color[2] = sColor[0][2] * ( 1.0f - fracT ) + sColor[1][2] * fracT;
}

int CTexture::GetApproximateVidMemBytes( void ) const
{
	ImageFormat format = GetImageFormat();
	int width = GetActualWidth();
	int height = GetActualHeight();
	int depth = GetActualDepth();
	int numFrames = GetNumAnimationFrames();
	bool isMipmapped = IsMipmapped();

	return numFrames * ImageLoader::GetMemRequired( width, height, depth, format, isMipmapped );
}

void CTexture::CopyFrameBufferToMe( int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect )
{
	Assert( m_pTextureHandles && m_nFrameCount >= 1 );

	if ( m_pTextureHandles && m_nFrameCount >= 1 )
	{
		g_pShaderAPI->CopyRenderTargetToTextureEx( m_pTextureHandles[0], nRenderTargetID, pSrcRect, pDstRect );
	}
}

void CTexture::CopyMeToFrameBuffer( int nRenderTargetID, Rect_t *pSrcRect, Rect_t *pDstRect )
{
	Assert( m_pTextureHandles && m_nFrameCount >= 1 );

	if ( m_pTextureHandles && m_nFrameCount >= 1 )
	{
		g_pShaderAPI->CopyTextureToRenderTargetEx( nRenderTargetID, m_pTextureHandles[0], pSrcRect, pDstRect );
	}
}

ITexture *CTexture::GetEmbeddedTexture( int nIndex )
{
	return ( nIndex == 0 ) ? this : NULL;
}

void CTexture::DeleteIfUnreferenced()
{
	if ( m_nRefCount > 0 )
		return;

	TextureManager()->RemoveTexture( this );
}

//Swap everything about a texture except the name. Created to support Portal mod's need for swapping out water render targets in recursive stencil views
void CTexture::SwapContents( ITexture *pOther )
{
	if( (pOther == NULL) || (pOther == this) )
		return;

	AssertMsg( dynamic_cast<CTexture *>(pOther) != NULL, "Texture swapping broken" );

	CTexture *pOtherAsCTexture = (CTexture *)pOther;

	CTexture *pTemp = (CTexture *)stackalloc( sizeof( CTexture ) );
	
	//swap everything. Note that this copies the entire object including the
	// vtable pointer, thus ruining polymorphism. Use with care.
	// The unnecessary casts to (void*) hint to clang that we know what we
	// are doing.
	memcpy( (void*)pTemp, (const void*)this, sizeof( CTexture ) );
	memcpy( (void*)this, (const void*)pOtherAsCTexture, sizeof( CTexture ) );
	memcpy( (void*)pOtherAsCTexture, (const void*)pTemp, sizeof( CTexture ) );

	//we have the other's name, give it back
	memcpy( &pOtherAsCTexture->m_Name, &m_Name, sizeof( m_Name ) );

	//pTemp still has our name
	memcpy( &m_Name, &pTemp->m_Name, sizeof( m_Name ) );
}

void CTexture::MarkAsPreloaded( bool bSet )
{
	if ( bSet )
	{
		m_nInternalFlags |= TEXTUREFLAGSINTERNAL_PRELOADED;
	}
	else
	{
		m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_PRELOADED;
	}
}

bool CTexture::IsPreloaded() const
{
	return ( ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_PRELOADED ) != 0 );
}

void CTexture::MarkAsExcluded( bool bSet, int nDimensionsLimit )
{
	if ( bSet )
	{
		// exclusion trumps picmipping
		m_nInternalFlags |= TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE;
		m_nDesiredDimensionLimit = 0;
	}
	else
	{
		// not excluding, but can optionally picmip
		m_nInternalFlags &= ~TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE;
		m_nDesiredDimensionLimit = nDimensionsLimit;
	}
}

bool CTexture::UpdateExcludedState( void )
{
	bool bDesired = ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_SHOULDEXCLUDE ) != 0;
	bool bActual = ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_EXCLUDED ) != 0;
	if ( ( bDesired == bActual ) && ( m_nDesiredDimensionLimit == m_nActualDimensionLimit ) )
	{
 		return false;
	}

	if ( m_nInternalFlags & TEXTUREFLAGSINTERNAL_QUEUEDLOAD )
	{
		// already scheduled
		return true;
	}

	// force the texture to re-download, causes the texture bits to match its desired exclusion state
	Download();

	return true;
}
