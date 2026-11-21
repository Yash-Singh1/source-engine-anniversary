//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Native 64-bit macOS AVFoundation video subsystem (replaces 32-bit QuickTime)
//
//=============================================================================//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>
#import <AudioToolbox/AudioToolbox.h>

#include <atomic>
#include <mutex>
#include <vector>
#include <cstring>

#define DONT_DEFINE_BOOL 1
#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier1/strtools.h"
#include "tier1/KeyValues.h"
#include "tier1/utllinkedlist.h"
#include "tier2/tier2.h"
#include "filesystem.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialsystem.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/itexture.h"
#include "vtf/vtf.h"
#include "pixelwriter.h"
#include "video/ivideoservices.h"
#include "../video_macros.h"
#include "../videosubsystem.h"

static inline int NextPowerOfTwo( int v )
{
	int p = 1;
	while ( p < v && p < 8192 )
		p <<= 1;
	return p;
}

// ===========================================================================
// Procedural BGRA texture regenerator
// ===========================================================================
class CAVFTextureRegenerator : public ITextureRegenerator
{
public:
	CAVFTextureRegenerator() : m_pPixels( NULL ), m_nWidth( 0 ), m_nHeight( 0 ), m_nRowBytes( 0 ) {}

	void SetSource( const uint8 *pPixels, int w, int h, int rowBytes )
	{
		m_pPixels = pPixels;
		m_nWidth = w;
		m_nHeight = h;
		m_nRowBytes = rowBytes;
	}

	virtual void RegenerateTextureBits( ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect ) OVERRIDE
	{
		if ( !pVTFTexture )
			return;

		int texW = pVTFTexture->Width();
		int texH = pVTFTexture->Height();
		uint8 *dst = pVTFTexture->ImageData();
		if ( !dst )
			return;

		if ( !m_pPixels || m_nWidth <= 0 || m_nHeight <= 0 )
		{
			memset( dst, 0, texW * texH * 4 );
			return;
		}

		int copyW = MIN( m_nWidth, texW );
		int copyH = MIN( m_nHeight, texH );
		int dstStride = texW * 4;
		for ( int y = 0; y < copyH; ++y )
		{
			memcpy( dst + y * dstStride, m_pPixels + y * m_nRowBytes, copyW * 4 );
		}
	}

	virtual void Release() OVERRIDE {}

private:
	const uint8 *m_pPixels;
	int m_nWidth;
	int m_nHeight;
	int m_nRowBytes;
};

// ===========================================================================
// AVFoundation + AudioQueue decoder
// ===========================================================================
class CAVFDecoder
{
public:
	CAVFDecoder()
		: m_pAsset( nil ),
		  m_pVideoReader( nil ),
		  m_pVideoOutput( nil ),
		  m_pAudioReader( nil ),
		  m_pAudioOutput( nil ),
		  m_AudioQueue( NULL ),
		  m_nWidth( 0 ),
		  m_nHeight( 0 ),
		  m_flDuration( 0.0f ),
		  m_flFPS( 30.0f ),
		  m_bHasAudio( false ),
		  m_bEnableAudio( true ),
		  m_bAudioEOF( false ),
		  m_bVideoEOF( false ),
		  m_bAudioRunning( false ),
		  m_flVolume( 1.0f ),
		  m_bMuted( false ),
		  m_flNextFramePTS( 0.0 ),
		  m_flCurrentPTS( 0.0 ),
		  m_pPendingSample( NULL )
	{
		memset( m_AudioBuffers, 0, sizeof( m_AudioBuffers ) );
	}

	~CAVFDecoder()
	{
		Close();
	}

	bool Open( const char *pszFilename, bool bEnableAudio )
	{
		Close();
		m_bEnableAudio = bEnableAudio;

		@autoreleasepool
		{
			NSString *path = [NSString stringWithUTF8String:pszFilename];
			if ( !path )
				return false;
			NSURL *url = [NSURL fileURLWithPath:path];
			if ( !url )
				return false;

			m_pAsset = [[AVURLAsset alloc] initWithURL:url options:nil];
			if ( !m_pAsset )
				return false;

			NSArray<AVAssetTrack *> *videoTracks = [m_pAsset tracksWithMediaType:AVMediaTypeVideo];
			if ( [videoTracks count] == 0 )
			{
				Close();
				return false;
			}

			AVAssetTrack *videoTrack = [videoTracks objectAtIndex:0];
			CGSize naturalSize = [videoTrack naturalSize];
			CGAffineTransform prefTransform = [videoTrack preferredTransform];
			CGSize transformedSize = CGSizeApplyAffineTransform( naturalSize, prefTransform );
			m_nWidth = (int)fabs( transformedSize.width );
			m_nHeight = (int)fabs( transformedSize.height );
			if ( m_nWidth <= 0 || m_nHeight <= 0 )
			{
				m_nWidth = (int)naturalSize.width;
				m_nHeight = (int)naturalSize.height;
			}
			if ( m_nWidth <= 0 || m_nHeight <= 0 )
			{
				Close();
				return false;
			}

			float fps = [videoTrack nominalFrameRate];
			m_flFPS = ( fps > 1.0f && fps < 240.0f ) ? fps : 24.0f;

			CMTime dur = [m_pAsset duration];
			m_flDuration = CMTIME_IS_NUMERIC( dur ) ? (float)CMTimeGetSeconds( dur ) : 0.0f;

			NSArray<AVAssetTrack *> *audioTracks = [m_pAsset tracksWithMediaType:AVMediaTypeAudio];
			m_bHasAudio = ( [audioTracks count] > 0 );

			m_FrameBuf.resize( (size_t)m_nWidth * (size_t)m_nHeight * 4, 0 );

			if ( !StartReadersAtTime( 0.0 ) )
			{
				Close();
				return false;
			}
		}

		return true;
	}

	bool StartReadersAtTime( double startSec )
	{
		StopAudio();
		ReleaseReaders();

		@autoreleasepool
		{
			if ( !m_pAsset )
				return false;

			NSError *err = nil;
			m_pVideoReader = [[AVAssetReader alloc] initWithAsset:m_pAsset error:&err];
			if ( !m_pVideoReader || err )
				return false;

			if ( startSec > 0.0 )
			{
				CMTime startTime = CMTimeMakeWithSeconds( startSec, 600 );
				m_pVideoReader.timeRange = CMTimeRangeMake( startTime, kCMTimePositiveInfinity );
			}

			AVAssetTrack *videoTrack = [[m_pAsset tracksWithMediaType:AVMediaTypeVideo] objectAtIndex:0];
			NSDictionary *videoSettings = @{
				(NSString *)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA)
			};
			m_pVideoOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:videoTrack outputSettings:videoSettings];
			m_pVideoOutput.alwaysCopiesSampleData = NO;
			if ( ![m_pVideoReader canAddOutput:m_pVideoOutput] )
				return false;
			[m_pVideoReader addOutput:m_pVideoOutput];
			if ( ![m_pVideoReader startReading] )
				return false;

			m_bVideoEOF = false;
			m_flCurrentPTS = startSec;
			m_flNextFramePTS = startSec;

			// Prime the first video sample so its PTS is known immediately
			FetchNextVideoSample();

			// Set up audio reader if audio track exists and audio is enabled
			if ( m_bHasAudio && m_bEnableAudio )
			{
				NSError *aerr = nil;
				m_pAudioReader = [[AVAssetReader alloc] initWithAsset:m_pAsset error:&aerr];
				if ( m_pAudioReader && !aerr )
				{
					if ( startSec > 0.0 )
					{
						CMTime startTime = CMTimeMakeWithSeconds( startSec, 48000 );
						m_pAudioReader.timeRange = CMTimeRangeMake( startTime, kCMTimePositiveInfinity );
					}
					AVAssetTrack *audioTrack = [[m_pAsset tracksWithMediaType:AVMediaTypeAudio] objectAtIndex:0];
					NSDictionary *audioSettings = @{
						AVFormatIDKey : @(kAudioFormatLinearPCM),
						AVSampleRateKey : @(48000.0),
						AVNumberOfChannelsKey : @(2),
						AVLinearPCMBitDepthKey : @(16),
						AVLinearPCMIsFloatKey : @NO,
						AVLinearPCMIsBigEndianKey : @NO,
						AVLinearPCMIsNonInterleaved : @NO
					};
					m_pAudioOutput = [[AVAssetReaderTrackOutput alloc] initWithTrack:audioTrack outputSettings:audioSettings];
					m_pAudioOutput.alwaysCopiesSampleData = NO;
					if ( [m_pAudioReader canAddOutput:m_pAudioOutput] )
					{
						[m_pAudioReader addOutput:m_pAudioOutput];
						if ( ![m_pAudioReader startReading] )
						{
							[m_pAudioOutput release]; m_pAudioOutput = nil;
							[m_pAudioReader release]; m_pAudioReader = nil;
						}
					}
					else
					{
						[m_pAudioOutput release]; m_pAudioOutput = nil;
						[m_pAudioReader release]; m_pAudioReader = nil;
					}
				}
				m_bAudioEOF = ( m_pAudioOutput == nil );
			}
			else
			{
				m_bAudioEOF = true;
			}
		}

		return true;
	}

	void StartAudio()
	{
		if ( !m_bHasAudio || !m_bEnableAudio || !m_pAudioOutput || m_bAudioRunning )
			return;

		AudioStreamBasicDescription fmt;
		memset( &fmt, 0, sizeof( fmt ) );
		fmt.mSampleRate = 48000.0;
		fmt.mFormatID = kAudioFormatLinearPCM;
		fmt.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
		fmt.mFramesPerPacket = 1;
		fmt.mChannelsPerFrame = 2;
		fmt.mBitsPerChannel = 16;
		fmt.mBytesPerFrame = 4;
		fmt.mBytesPerPacket = 4;

		OSStatus st = AudioQueueNewOutput( &fmt, AudioQueueCallback, this, NULL, NULL, 0, &m_AudioQueue );
		if ( st != noErr || !m_AudioQueue )
			return;

		ApplyAudioVolume();
		m_bAudioRunning = true;

		for ( int i = 0; i < kNumAudioBuffers; ++i )
		{
			if ( AudioQueueAllocateBuffer( m_AudioQueue, kAudioBufferBytes, &m_AudioBuffers[i] ) == noErr )
			{
				FillAndEnqueueAudioBuffer( m_AudioBuffers[i] );
			}
		}

		AudioQueueStart( m_AudioQueue, NULL );
	}

	void PauseAudio( bool bPause )
	{
		if ( !m_AudioQueue || !m_bAudioRunning )
			return;
		if ( bPause )
			AudioQueuePause( m_AudioQueue );
		else
			AudioQueueStart( m_AudioQueue, NULL );
	}

	void StopAudio()
	{
		m_bAudioRunning = false;
		if ( m_AudioQueue )
		{
			AudioQueueStop( m_AudioQueue, true );
			AudioQueueDispose( m_AudioQueue, true );
			m_AudioQueue = NULL;
			memset( m_AudioBuffers, 0, sizeof( m_AudioBuffers ) );
		}
		std::lock_guard<std::mutex> lock( m_AudioMutex );
		m_AudioCarryover.clear();
	}

	void SetVolume( float vol )
	{
		m_flVolume = clamp( vol, 0.0f, 1.0f );
		ApplyAudioVolume();
	}

	void SetMuted( bool muted )
	{
		m_bMuted = muted;
		ApplyAudioVolume();
	}

	// Advances video frame(s) up to targetTimeSec. Returns true if a new frame was copied into m_FrameBuf.
	bool AdvanceToTime( double targetTimeSec )
	{
		bool bUpdated = false;
		while ( m_pPendingSample && targetTimeSec >= m_flNextFramePTS )
		{
			CMSampleBufferRef cur = m_pPendingSample;
			m_pPendingSample = NULL;
			m_flCurrentPTS = m_flNextFramePTS;

			CVImageBufferRef img = CMSampleBufferGetImageBuffer( cur );
			if ( img )
			{
				CVPixelBufferLockBaseAddress( img, kCVPixelBufferLock_ReadOnly );
				const uint8 *src = (const uint8 *)CVPixelBufferGetBaseAddress( img );
				size_t srcBytesPerRow = CVPixelBufferGetBytesPerRow( img );
				size_t srcW = CVPixelBufferGetWidth( img );
				size_t srcH = CVPixelBufferGetHeight( img );
				if ( src && (int)srcW == m_nWidth && (int)srcH == m_nHeight )
				{
					for ( int y = 0; y < m_nHeight; ++y )
					{
						memcpy( m_FrameBuf.data() + (size_t)y * (size_t)m_nWidth * 4,
						        src + (size_t)y * srcBytesPerRow,
						        (size_t)m_nWidth * 4 );
					}
					bUpdated = true;
				}
				CVPixelBufferUnlockBaseAddress( img, kCVPixelBufferLock_ReadOnly );
			}

			CFRelease( cur );
			FetchNextVideoSample();
		}
		return bUpdated;
	}

	void Close()
	{
		StopAudio();
		ReleaseReaders();
		@autoreleasepool
		{
			if ( m_pAsset )
			{
				[m_pAsset release];
				m_pAsset = nil;
			}
		}
		m_FrameBuf.clear();
		m_nWidth = 0;
		m_nHeight = 0;
	}

	int Width() const { return m_nWidth; }
	int Height() const { return m_nHeight; }
	float Duration() const { return m_flDuration; }
	float FPS() const { return m_flFPS; }
	bool HasAudio() const { return m_bHasAudio; }
	bool IsVideoEOF() const { return m_bVideoEOF && m_pPendingSample == NULL; }
	double CurrentPTS() const { return m_flCurrentPTS; }
	const uint8 *FramePixels() const { return m_FrameBuf.empty() ? NULL : m_FrameBuf.data(); }
	float Volume() const { return m_flVolume; }
	bool IsMuted() const { return m_bMuted; }

private:
	static const int kNumAudioBuffers = 6;
	static const UInt32 kAudioBufferBytes = 32768;

	void ApplyAudioVolume()
	{
		if ( m_AudioQueue )
		{
			AudioQueueSetParameter( m_AudioQueue, kAudioQueueParam_Volume, m_bMuted ? 0.0f : m_flVolume );
		}
	}

	void ReleaseReaders()
	{
		if ( m_pPendingSample )
		{
			CFRelease( m_pPendingSample );
			m_pPendingSample = NULL;
		}
		@autoreleasepool
		{
			if ( m_pVideoReader )
			{
				[m_pVideoReader cancelReading];
				[m_pVideoReader release];
				m_pVideoReader = nil;
			}
			if ( m_pVideoOutput )
			{
				[m_pVideoOutput release];
				m_pVideoOutput = nil;
			}
			std::lock_guard<std::mutex> lock( m_AudioMutex );
			if ( m_pAudioReader )
			{
				[m_pAudioReader cancelReading];
				[m_pAudioReader release];
				m_pAudioReader = nil;
			}
			if ( m_pAudioOutput )
			{
				[m_pAudioOutput release];
				m_pAudioOutput = nil;
			}
		}
	}

	void FetchNextVideoSample()
	{
		if ( !m_pVideoOutput || !m_pVideoReader )
		{
			m_bVideoEOF = true;
			return;
		}
		@autoreleasepool
		{
			CMSampleBufferRef sb = [m_pVideoOutput copyNextSampleBuffer];
			if ( sb )
			{
				CMTime pts = CMSampleBufferGetPresentationTimeStamp( sb );
				m_flNextFramePTS = CMTIME_IS_NUMERIC( pts ) ? CMTimeGetSeconds( pts ) : ( m_flCurrentPTS + 1.0 / m_flFPS );
				m_pPendingSample = sb;
			}
			else
			{
				m_bVideoEOF = true;
			}
		}
	}

	static void AudioQueueCallback( void *userData, AudioQueueRef queue, AudioQueueBufferRef buffer )
	{
		CAVFDecoder *self = static_cast<CAVFDecoder *>( userData );
		if ( !self || !self->m_bAudioRunning )
			return;
		self->FillAndEnqueueAudioBuffer( buffer );
	}

	void FillAndEnqueueAudioBuffer( AudioQueueBufferRef buffer )
	{
		if ( !buffer || !m_AudioQueue || !m_bAudioRunning )
			return;

		std::lock_guard<std::mutex> lock( m_AudioMutex );
		uint8 *dst = (uint8 *)buffer->mAudioData;
		UInt32 cap = buffer->mAudioDataBytesCapacity;
		UInt32 filled = 0;

		if ( !m_AudioCarryover.empty() )
		{
			size_t n = MIN( (size_t)cap, m_AudioCarryover.size() );
			memcpy( dst, m_AudioCarryover.data(), n );
			m_AudioCarryover.erase( m_AudioCarryover.begin(), m_AudioCarryover.begin() + n );
			filled += (UInt32)n;
		}

		@autoreleasepool
		{
			while ( filled < cap && !m_bAudioEOF && m_pAudioOutput )
			{
				CMSampleBufferRef sb = [m_pAudioOutput copyNextSampleBuffer];
				if ( !sb )
				{
					m_bAudioEOF = true;
					break;
				}
				CMBlockBufferRef bb = CMSampleBufferGetDataBuffer( sb );
				if ( bb )
				{
					size_t totalLen = CMBlockBufferGetDataLength( bb );
					size_t offset = 0;
					while ( offset < totalLen )
					{
						char *chunkPtr = NULL;
						size_t chunkLen = 0;
						if ( CMBlockBufferGetDataPointer( bb, offset, &chunkLen, NULL, &chunkPtr ) != kCMBlockBufferNoErr || !chunkPtr || chunkLen == 0 )
							break;
						size_t space = cap - filled;
						if ( chunkLen <= space )
						{
							memcpy( dst + filled, chunkPtr, chunkLen );
							filled += (UInt32)chunkLen;
						}
						else
						{
							if ( space > 0 )
							{
								memcpy( dst + filled, chunkPtr, space );
								filled += (UInt32)space;
							}
							m_AudioCarryover.insert( m_AudioCarryover.end(), chunkPtr + space, chunkPtr + chunkLen );
						}
						offset += chunkLen;
					}
				}
				CFRelease( sb );
			}
		}

		if ( filled > 0 && m_bAudioRunning )
		{
			buffer->mAudioDataByteSize = filled;
			AudioQueueEnqueueBuffer( m_AudioQueue, buffer, 0, NULL );
		}
	}

	AVURLAsset *m_pAsset;
	AVAssetReader *m_pVideoReader;
	AVAssetReaderTrackOutput *m_pVideoOutput;
	AVAssetReader *m_pAudioReader;
	AVAssetReaderTrackOutput *m_pAudioOutput;
	AudioQueueRef m_AudioQueue;
	AudioQueueBufferRef m_AudioBuffers[kNumAudioBuffers];
	std::mutex m_AudioMutex;
	std::vector<uint8> m_AudioCarryover;
	std::vector<uint8> m_FrameBuf;

	int m_nWidth;
	int m_nHeight;
	float m_flDuration;
	float m_flFPS;
	bool m_bHasAudio;
	bool m_bEnableAudio;
	bool m_bAudioEOF;
	bool m_bVideoEOF;
	std::atomic<bool> m_bAudioRunning;
	float m_flVolume;
	bool m_bMuted;
	double m_flNextFramePTS;
	double m_flCurrentPTS;
	CMSampleBufferRef m_pPendingSample;
};

// ===========================================================================
// IVideoMaterial implementation backed by CAVFDecoder
// ===========================================================================
class CAVFVideoMaterial : public IVideoMaterial
{
public:
	CAVFVideoMaterial()
		: m_LastResult( VideoResult::SUCCESS ),
		  m_pMaterial( NULL ),
		  m_pTexture( NULL ),
		  m_nTexWidth( 0 ),
		  m_nTexHeight( 0 ),
		  m_flMaxU( 1.0f ),
		  m_flMaxV( 1.0f ),
		  m_bReady( false ),
		  m_bPlaying( false ),
		  m_bPaused( false ),
		  m_bLooping( false ),
		  m_bFinished( false ),
		  m_bNewFrameReady( false ),
		  m_flStartTime( 0.0 ),
		  m_flPauseTime( 0.0 ),
		  m_flTimeOffset( 0.0 )
	{
		m_szFileName[0] = 0;
		m_FrameRate.Clear();
	}

	virtual ~CAVFVideoMaterial()
	{
		Shutdown();
	}

	bool Init( const char *pMaterialName, const char *pVideoFileName, VideoPlaybackFlags_t flags, IVideoCommonServices *pCommon )
	{
		V_strncpy( m_szFileName, pVideoFileName ? pVideoFileName : "", sizeof( m_szFileName ) );
		bool bNoAudio = BITFLAGS_SET( flags, VideoPlaybackFlags::NO_AUDIO );
		m_bLooping = BITFLAGS_SET( flags, VideoPlaybackFlags::LOOP_VIDEO );

		if ( !m_Decoder.Open( m_szFileName, !bNoAudio ) )
		{
			m_LastResult = VideoResult::FILE_ERROR_OCCURED;
			return false;
		}

		if ( pCommon )
		{
			m_Decoder.SetVolume( pCommon->GetSystemVolume() );
		}

		m_FrameRate.SetFPS( m_Decoder.FPS() );
		m_nTexWidth = NextPowerOfTwo( m_Decoder.Width() );
		m_nTexHeight = NextPowerOfTwo( m_Decoder.Height() );
		m_flMaxU = (float)m_Decoder.Width() / (float)m_nTexWidth;
		m_flMaxV = (float)m_Decoder.Height() / (float)m_nTexHeight;

		char texName[256];
		V_snprintf( texName, sizeof( texName ), "%s_avf_tex", pMaterialName );

		m_pTexture = materials->CreateProceduralTexture(
			texName,
			TEXTURE_GROUP_OTHER,
			m_nTexWidth,
			m_nTexHeight,
			IMAGE_FORMAT_BGRA8888,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD | TEXTUREFLAGS_SINGLECOPY );

		if ( !m_pTexture )
		{
			m_LastResult = VideoResult::MATERIAL_NOT_FOUND;
			return false;
		}

		m_pTexture->SetTextureRegenerator( &m_Regen );

		KeyValues *pVMT = new KeyValues( "UnlitGeneric" );
		pVMT->SetString( "$basetexture", texName );
		pVMT->SetInt( "$nofog", 1 );
		pVMT->SetInt( "$ignorez", 1 );
		pVMT->SetInt( "$nolod", 1 );
		pVMT->SetInt( "$no_fullbright", 1 );
		pVMT->SetInt( "$nocull", 1 );

		m_pMaterial = materials->CreateMaterial( pMaterialName, pVMT );
		if ( !m_pMaterial )
		{
			m_LastResult = VideoResult::MATERIAL_NOT_FOUND;
			return false;
		}
		m_pMaterial->Refresh();

		// Upload initial frame
		m_Decoder.AdvanceToTime( 0.0 );
		UploadCurrentFrame();

		m_bReady = true;
		m_bPlaying = true;
		m_bPaused = false;
		m_bFinished = false;
		m_flStartTime = Plat_FloatTime();
		m_flTimeOffset = 0.0;
		m_Decoder.StartAudio();
		m_LastResult = VideoResult::SUCCESS;
		return true;
	}

	void Shutdown()
	{
		m_Decoder.Close();
		if ( m_pTexture )
		{
			m_pTexture->SetTextureRegenerator( NULL );
			m_pTexture->DecrementReferenceCount();
			m_pTexture = NULL;
		}
		if ( m_pMaterial )
		{
			m_pMaterial->DecrementReferenceCount();
			m_pMaterial = NULL;
		}
		m_bReady = false;
		m_bPlaying = false;
	}

	virtual const char *GetVideoFileName() OVERRIDE { return m_szFileName; }
	virtual VideoResult_t GetLastResult() OVERRIDE { return m_LastResult; }
	virtual VideoFrameRate_t &GetVideoFrameRate() OVERRIDE { return m_FrameRate; }

	virtual bool HasAudio() OVERRIDE { return m_Decoder.HasAudio(); }
	virtual bool SetVolume( float fVolume ) OVERRIDE { m_Decoder.SetVolume( fVolume ); return true; }
	virtual float GetVolume() OVERRIDE { return m_Decoder.Volume(); }
	virtual void SetMuted( bool bMuteState ) OVERRIDE { m_Decoder.SetMuted( bMuteState ); }
	virtual bool IsMuted() OVERRIDE { return m_Decoder.IsMuted(); }
	virtual VideoResult_t SoundDeviceCommand( VideoSoundDeviceOperation_t, void *, void * ) OVERRIDE { return VideoResult::SUCCESS; }

	virtual bool IsVideoReadyToPlay() OVERRIDE { return m_bReady; }
	virtual bool IsVideoPlaying() OVERRIDE { return m_bPlaying && !m_bFinished; }
	virtual bool IsNewFrameReady() OVERRIDE { return m_bNewFrameReady; }
	virtual bool IsFinishedPlaying() OVERRIDE { return m_bFinished; }

	virtual bool StartVideo() OVERRIDE
	{
		if ( !m_bReady )
			return false;
		if ( m_bFinished )
		{
			m_Decoder.StartReadersAtTime( 0.0 );
			m_bFinished = false;
		}
		m_bPlaying = true;
		m_bPaused = false;
		m_flStartTime = Plat_FloatTime();
		m_flTimeOffset = m_Decoder.CurrentPTS();
		m_Decoder.StartAudio();
		return true;
	}

	virtual bool StopVideo() OVERRIDE
	{
		m_bPlaying = false;
		m_bPaused = false;
		m_Decoder.StopAudio();
		return true;
	}

	virtual void SetLooping( bool bLoopVideo ) OVERRIDE { m_bLooping = bLoopVideo; }
	virtual bool IsLooping() OVERRIDE { return m_bLooping; }

	virtual void SetPaused( bool bPauseState ) OVERRIDE
	{
		if ( m_bPaused == bPauseState )
			return;
		m_bPaused = bPauseState;
		m_Decoder.PauseAudio( bPauseState );
		if ( bPauseState )
		{
			m_flPauseTime = Plat_FloatTime();
		}
		else
		{
			m_flStartTime += ( Plat_FloatTime() - m_flPauseTime );
		}
	}

	virtual bool IsPaused() OVERRIDE { return m_bPaused; }
	virtual float GetVideoDuration() OVERRIDE { return m_Decoder.Duration(); }
	virtual int GetFrameCount() OVERRIDE { return (int)( m_Decoder.Duration() * m_Decoder.FPS() ); }

	virtual bool SetFrame( int FrameNum ) OVERRIDE
	{
		float fps = m_Decoder.FPS();
		return SetTime( fps > 0.0f ? (float)FrameNum / fps : 0.0f );
	}

	virtual int GetCurrentFrame() OVERRIDE
	{
		return (int)( m_Decoder.CurrentPTS() * m_Decoder.FPS() );
	}

	virtual bool SetTime( float flTime ) OVERRIDE
	{
		if ( !m_bReady )
			return false;
		double t = clamp( (double)flTime, 0.0, (double)m_Decoder.Duration() );
		if ( !m_Decoder.StartReadersAtTime( t ) )
			return false;
		m_flStartTime = Plat_FloatTime();
		m_flTimeOffset = t;
		m_bFinished = false;
		if ( m_bPlaying && !m_bPaused )
		{
			m_Decoder.StartAudio();
		}
		if ( m_Decoder.AdvanceToTime( t ) )
		{
			UploadCurrentFrame();
		}
		return true;
	}

	virtual float GetCurrentVideoTime() OVERRIDE
	{
		if ( !m_bPlaying )
			return (float)m_Decoder.CurrentPTS();
		if ( m_bPaused )
			return (float)( m_flTimeOffset + ( m_flPauseTime - m_flStartTime ) );
		return (float)( m_flTimeOffset + ( Plat_FloatTime() - m_flStartTime ) );
	}

	virtual bool Update() OVERRIDE
	{
		m_bNewFrameReady = false;
		if ( !m_bReady || !m_bPlaying || m_bPaused )
			return false;

		double elapsed = m_flTimeOffset + ( Plat_FloatTime() - m_flStartTime );
		if ( m_Decoder.AdvanceToTime( elapsed ) )
		{
			UploadCurrentFrame();
			m_bNewFrameReady = true;
		}

		if ( m_Decoder.IsVideoEOF() && elapsed >= (double)m_Decoder.Duration() - 0.02 )
		{
			if ( m_bLooping )
			{
				m_Decoder.StartReadersAtTime( 0.0 );
				m_flStartTime = Plat_FloatTime();
				m_flTimeOffset = 0.0;
				m_Decoder.StartAudio();
				if ( m_Decoder.AdvanceToTime( 0.0 ) )
				{
					UploadCurrentFrame();
					m_bNewFrameReady = true;
				}
			}
			else
			{
				m_bFinished = true;
				m_bPlaying = false;
				m_Decoder.StopAudio();
			}
		}

		return m_bNewFrameReady;
	}

	virtual IMaterial *GetMaterial() OVERRIDE { return m_pMaterial; }
	virtual void GetVideoTexCoordRange( float *pMaxU, float *pMaxV ) OVERRIDE
	{
		if ( pMaxU ) *pMaxU = m_flMaxU;
		if ( pMaxV ) *pMaxV = m_flMaxV;
	}
	virtual void GetVideoImageSize( int *pWidth, int *pHeight ) OVERRIDE
	{
		if ( pWidth ) *pWidth = m_Decoder.Width();
		if ( pHeight ) *pHeight = m_Decoder.Height();
	}

private:
	void UploadCurrentFrame()
	{
		if ( !m_pTexture || !m_Decoder.FramePixels() )
			return;
		m_Regen.SetSource( m_Decoder.FramePixels(), m_Decoder.Width(), m_Decoder.Height(), m_Decoder.Width() * 4 );
		m_pTexture->Download();
	}

	char m_szFileName[MAX_PATH];
	VideoResult_t m_LastResult;
	VideoFrameRate_t m_FrameRate;
	CAVFDecoder m_Decoder;
	CAVFTextureRegenerator m_Regen;
	IMaterial *m_pMaterial;
	ITexture *m_pTexture;
	int m_nTexWidth;
	int m_nTexHeight;
	float m_flMaxU;
	float m_flMaxV;
	bool m_bReady;
	bool m_bPlaying;
	bool m_bPaused;
	bool m_bLooping;
	bool m_bFinished;
	bool m_bNewFrameReady;
	double m_flStartTime;
	double m_flPauseTime;
	double m_flTimeOffset;
};

// ===========================================================================
// IVideoSubSystem exposed as video_quicktime on 64-bit macOS
// ===========================================================================
static VideoFileExtensionInfo_t s_AVFExtensions[] =
{
	{ ".mov", VideoSystem::QUICKTIME, VideoSystemFeature::FULL_PLAYBACK },
	{ ".mp4", VideoSystem::QUICKTIME, VideoSystemFeature::FULL_PLAYBACK },
	{ ".m4v", VideoSystem::QUICKTIME, VideoSystemFeature::FULL_PLAYBACK },
};

class CAVFVideoSubSystem : public CTier2AppSystem< IVideoSubSystem >
{
	typedef CTier2AppSystem< IVideoSubSystem > BaseClass;

public:
	CAVFVideoSubSystem()
		: m_bInitialized( false ),
		  m_LastResult( VideoResult::SUCCESS ),
		  m_Status( VideoSystemStatus::NOT_INITIALIZED ),
		  m_pCommonServices( NULL ),
		  m_nMatCounter( 0 )
	{}

	virtual ~CAVFVideoSubSystem()
	{
		ShutdownVideoSystem();
	}

	virtual bool Connect( CreateInterfaceFn factory ) OVERRIDE
	{
		if ( !BaseClass::Connect( factory ) )
			return false;
		return ( g_pFullFileSystem != NULL && materials != NULL );
	}

	virtual void Disconnect() OVERRIDE
	{
		BaseClass::Disconnect();
	}

	virtual void *QueryInterface( const char *pInterfaceName ) OVERRIDE
	{
		if ( pInterfaceName && V_strcmp( pInterfaceName, VIDEO_SUBSYSTEM_INTERFACE_VERSION ) == 0 )
			return (IVideoSubSystem *)this;
		return NULL;
	}

	virtual InitReturnVal_t Init() OVERRIDE
	{
		return BaseClass::Init();
	}

	virtual void Shutdown() OVERRIDE
	{
		ShutdownVideoSystem();
		BaseClass::Shutdown();
	}

	virtual VideoSystem_t GetSystemID() OVERRIDE { return VideoSystem::QUICKTIME; }
	virtual VideoSystemStatus_t GetSystemStatus() OVERRIDE { return m_Status; }
	virtual VideoSystemFeature_t GetSupportedFeatures() OVERRIDE { return VideoSystemFeature::FULL_PLAYBACK; }
	virtual const char *GetVideoSystemName() OVERRIDE { return "AVFoundation (macOS)"; }

	virtual bool InitializeVideoSystem( IVideoCommonServices *pCommonServices ) OVERRIDE
	{
		m_pCommonServices = pCommonServices;
		m_bInitialized = true;
		m_Status = VideoSystemStatus::OK;
		m_LastResult = VideoResult::SUCCESS;
		return true;
	}

	virtual bool ShutdownVideoSystem() OVERRIDE
	{
		m_bInitialized = false;
		m_Status = VideoSystemStatus::NOT_INITIALIZED;
		return true;
	}

	virtual VideoResult_t VideoSoundDeviceCMD( VideoSoundDeviceOperation_t, void *, void * ) OVERRIDE
	{
		return SetResult( VideoResult::SUCCESS );
	}

	virtual int GetSupportedFileExtensionCount() OVERRIDE { return ARRAYSIZE( s_AVFExtensions ); }
	virtual const char *GetSupportedFileExtension( int num ) OVERRIDE
	{
		return ( num >= 0 && num < ARRAYSIZE( s_AVFExtensions ) ) ? s_AVFExtensions[num].m_FileExtension : NULL;
	}
	virtual VideoSystemFeature_t GetSupportedFileExtensionFeatures( int num ) OVERRIDE
	{
		return ( num >= 0 && num < ARRAYSIZE( s_AVFExtensions ) ) ? s_AVFExtensions[num].m_VideoFeatures : VideoSystemFeature::NO_FEATURES;
	}

	virtual VideoResult_t PlayVideoFileFullScreen( const char *filename, void *mainWindow, int windowWidth, int windowHeight, int desktopWidth, int desktopHeight, bool windowed, float forcedMinTime, VideoPlaybackFlags_t playbackFlags ) OVERRIDE
	{
		if ( !filename || !filename[0] || !materials || !m_pCommonServices )
			return SetResult( VideoResult::BAD_INPUT_PARAMETERS );

		bool bNoAudio = BITFLAGS_SET( playbackFlags, VideoPlaybackFlags::NO_AUDIO );
		CAVFDecoder decoder;
		if ( !decoder.Open( filename, !bNoAudio ) )
			return SetResult( VideoResult::FILE_ERROR_OCCURED );

		decoder.SetVolume( m_pCommonServices->GetSystemVolume() );

		int backW = 0, backH = 0;
		materials->GetBackBufferDimensions( backW, backH );
		if ( backW <= 0 || backH <= 0 )
		{
			backW = ( windowWidth > 0 ) ? windowWidth : desktopWidth;
			backH = ( windowHeight > 0 ) ? windowHeight : desktopHeight;
		}

		int drawW = decoder.Width();
		int drawH = decoder.Height();
		int drawX = 0, drawY = 0;
		if ( !m_pCommonServices->CalculateVideoDimensions( decoder.Width(), decoder.Height(), backW, backH, playbackFlags, &drawW, &drawH, &drawX, &drawY ) )
			return SetResult( VideoResult::VIDEO_ERROR_OCCURED );

		int texW = NextPowerOfTwo( decoder.Width() );
		int texH = NextPowerOfTwo( decoder.Height() );

		char texName[128], matName[128];
		int id = ++m_nMatCounter;
		V_snprintf( texName, sizeof( texName ), "__avf_fullscreen_tex_%d", id );
		V_snprintf( matName, sizeof( matName ), "__avf_fullscreen_mat_%d", id );

		CAVFTextureRegenerator regen;
		ITexture *pTex = materials->CreateProceduralTexture(
			texName,
			TEXTURE_GROUP_OTHER,
			texW,
			texH,
			IMAGE_FORMAT_BGRA8888,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_NOMIP | TEXTUREFLAGS_NOLOD | TEXTUREFLAGS_SINGLECOPY );
		if ( !pTex )
			return SetResult( VideoResult::SYSTEM_ERROR_OCCURED );

		pTex->SetTextureRegenerator( &regen );

		KeyValues *pVMT = new KeyValues( "UnlitGeneric" );
		pVMT->SetString( "$basetexture", texName );
		pVMT->SetInt( "$nofog", 1 );
		pVMT->SetInt( "$ignorez", 1 );
		pVMT->SetInt( "$nolod", 1 );
		pVMT->SetInt( "$no_fullbright", 1 );
		pVMT->SetInt( "$nocull", 1 );

		IMaterial *pMat = materials->CreateMaterial( matName, pVMT );
		if ( !pMat )
		{
			pTex->SetTextureRegenerator( NULL );
			pTex->DecrementReferenceCount();
			return SetResult( VideoResult::SYSTEM_ERROR_OCCURED );
		}
		pMat->Refresh();

		if ( m_pCommonServices->InitFullScreenPlaybackInputHandler( playbackFlags, forcedMinTime, windowed ) != VideoResult::SUCCESS )
		{
			pTex->SetTextureRegenerator( NULL );
			pTex->DecrementReferenceCount();
			pMat->DecrementReferenceCount();
			return SetResult( VideoResult::SYSTEM_ERROR_OCCURED );
		}

		// Prime first frame and start audio
		decoder.AdvanceToTime( 0.0 );
		if ( decoder.FramePixels() )
		{
			regen.SetSource( decoder.FramePixels(), decoder.Width(), decoder.Height(), decoder.Width() * 4 );
			pTex->Download();
		}

		double startTime = Plat_FloatTime();
		double pauseTime = 0.0;
		bool bPaused = false;
		double duration = (double)decoder.Duration();
		decoder.StartAudio();

		while ( true )
		{
			bool bAbort = false, bPauseEvent = false, bQuit = false;
			if ( m_pCommonServices->ProcessFullScreenInput( bAbort, bPauseEvent, bQuit ) )
			{
				if ( bAbort || bQuit )
					break;
				if ( bPauseEvent )
				{
					bPaused = !bPaused;
					decoder.PauseAudio( bPaused );
					if ( bPaused )
						pauseTime = Plat_FloatTime();
					else
						startTime += ( Plat_FloatTime() - pauseTime );
				}
			}

			if ( !bPaused )
			{
				double elapsed = Plat_FloatTime() - startTime;
				if ( decoder.AdvanceToTime( elapsed ) && decoder.FramePixels() )
				{
					regen.SetSource( decoder.FramePixels(), decoder.Width(), decoder.Height(), decoder.Width() * 4 );
					pTex->Download();
				}

				{
					CMatRenderContextPtr pRenderContext( materials );
					pRenderContext->Viewport( 0, 0, backW, backH );
					pRenderContext->DepthRange( 0, 1 );
					pRenderContext->ClearColor3ub( 0, 0, 0 );
					pRenderContext->ClearBuffers( true, true, true );
					pRenderContext->SetToneMappingScaleLinear( Vector( 1.0f, 1.0f, 1.0f ) );
					pRenderContext->DrawScreenSpaceRectangle(
						pMat,
						drawX, drawY, drawW, drawH,
						0.0f, 0.0f, (float)decoder.Width() - 1.0f, (float)decoder.Height() - 1.0f,
						texW, texH );
				}
				materials->SwapBuffers();

				if ( decoder.IsVideoEOF() && elapsed >= duration - 0.02 )
					break;
			}

			ThreadSleep( 4 );
		}

		decoder.StopAudio();
		m_pCommonServices->TerminateFullScreenPlaybackInputHandler();

		// Clear screen back to black before handing back to engine
		{
			CMatRenderContextPtr pRenderContext( materials );
			pRenderContext->Viewport( 0, 0, backW, backH );
			pRenderContext->ClearColor3ub( 0, 0, 0 );
			pRenderContext->ClearBuffers( true, false, false );
		}
		materials->SwapBuffers();

		pTex->SetTextureRegenerator( NULL );
		pTex->DecrementReferenceCount();
		pMat->DecrementReferenceCount();
		decoder.Close();

		return SetResult( VideoResult::SUCCESS );
	}

	virtual IVideoMaterial *CreateVideoMaterial( const char *pMaterialName, const char *pVideoFileName, VideoPlaybackFlags_t flags ) OVERRIDE
	{
		if ( !pMaterialName || !pVideoFileName )
		{
			SetResult( VideoResult::BAD_INPUT_PARAMETERS );
			return NULL;
		}
		CAVFVideoMaterial *pMat = new CAVFVideoMaterial();
		if ( !pMat->Init( pMaterialName, pVideoFileName, flags, m_pCommonServices ) )
		{
			SetResult( pMat->GetLastResult() );
			delete pMat;
			return NULL;
		}
		SetResult( VideoResult::SUCCESS );
		return pMat;
	}

	virtual VideoResult_t DestroyVideoMaterial( IVideoMaterial *pVideoMaterial ) OVERRIDE
	{
		if ( !pVideoMaterial )
			return SetResult( VideoResult::BAD_INPUT_PARAMETERS );
		delete static_cast<CAVFVideoMaterial *>( pVideoMaterial );
		return SetResult( VideoResult::SUCCESS );
	}

	virtual IVideoRecorder *CreateVideoRecorder() OVERRIDE
	{
		SetResult( VideoResult::FEATURE_NOT_AVAILABLE );
		return NULL;
	}

	virtual VideoResult_t DestroyVideoRecorder( IVideoRecorder * ) OVERRIDE
	{
		return SetResult( VideoResult::FEATURE_NOT_AVAILABLE );
	}

	virtual VideoResult_t CheckCodecAvailability( VideoEncodeCodec_t ) OVERRIDE
	{
		return SetResult( VideoResult::FEATURE_NOT_AVAILABLE );
	}

	virtual VideoResult_t GetLastResult() OVERRIDE { return m_LastResult; }

private:
	VideoResult_t SetResult( VideoResult_t r ) { m_LastResult = r; return r; }

	bool m_bInitialized;
	VideoResult_t m_LastResult;
	VideoSystemStatus_t m_Status;
	IVideoCommonServices *m_pCommonServices;
	int m_nMatCounter;
};

static CAVFVideoSubSystem g_AVFVideoSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CAVFVideoSubSystem, IVideoSubSystem, VIDEO_SUBSYSTEM_INTERFACE_VERSION, g_AVFVideoSystem );
