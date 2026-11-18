//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Mixer for ADPCM encoded audio
//
//=============================================================================//

#ifndef SND_WAVE_MIXER_MP3_H
#define SND_WAVE_MIXER_MP3_H
#pragma once

#include "vaudio/ivaudio.h"

static const int MP3_BUFFER_SIZE = 16384;

class CAudioMixerWaveMP3 : public CAudioMixerWave, public IAudioStreamEvent
{
public:
	CAudioMixerWaveMP3( IWaveData *data );
	~CAudioMixerWaveMP3( void );

	virtual void Mix( IAudioDevice *pDevice, channel_t *pChannel, void *pData, int outputOffset, int inputOffset, fixedint fracRate, int outCount, int timecompress );
	virtual int	 GetOutputData( void **pData, int sampleCount, char copyBuf[AUDIOSOURCE_COPYBUF_SIZE] );

	// need to override this to fixup blocks
	// UNDONE: This doesn't quite work with MP3 - we need a MP3 position, not a sample position
	void SetSampleStart( int newPosition );

#ifdef BYTEMATCH_RETAIL
	int GetPositionForSave() { return m_pStream->GetPosition(); }
	void SetPositionFromSaved(int position) { m_pStream->SetPosition(position); }
#else
	int GetPositionForSave() { return GetStream() ? GetStream()->GetPosition() : 0; }
	void SetPositionFromSaved(int position) { if ( GetStream() ) GetStream()->SetPosition(position); }
#endif
#line 30

	// IAudioStreamEvent
	virtual int StreamRequestData( void *pBuffer, int bytesRequested, int offset );

	virtual void SetStartupDelaySamples( int delaySamples );
	virtual int GetMixSampleSize() { return CalcSampleSize( 16, m_channelCount ); }

#ifdef BYTEMATCH_RETAIL
	virtual int GetStreamOutputRate() { return m_pStream->GetOutputRate(); }

	bool IsValid() { return m_pStream != NULL; }

private:
	bool					DecodeBlock( void );
	void					GetID3HeaderOffset();

	IAudioStream			*m_pStream;
#else
	virtual int GetStreamOutputRate() { return GetStream() ? GetStream()->GetOutputRate() : 0; }

private:
	IAudioStream			*GetStream();
	bool					DecodeBlock( void );
	void					GetID3HeaderOffset();

	// Lazily initialized, use GetStream
	IAudioStream			*m_pStream;
	bool					m_bStreamInit;
#endif
#line 46

	char					m_samples[MP3_BUFFER_SIZE];
	int						m_sampleCount;
	int						m_samplePosition;
	int						m_channelCount;
	int						m_offset;
	int						m_delaySamples;
	int						m_headerOffset;
};

#ifdef BYTEMATCH_RETAIL
CAudioMixerWaveMP3 *CreateMP3Mixer( IWaveData *data, int *pSampleRate );
#else
CAudioMixerWaveMP3 *CreateMP3Mixer( IWaveData *data );
#endif
#line 57

#endif // SND_WAVE_MIXER_MP3_H
