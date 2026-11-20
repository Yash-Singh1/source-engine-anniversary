//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef VGUIFONT_H
#define VGUIFONT_H
#ifdef _WIN32
#pragma once
#endif

// Structure passed to CWin32Font::GetCharsRGBA
struct newChar_t
{
	wchar_t	wch;		// A new character to generate texture data for
	int		fontWide;	// Texel width of the character
	int		fontTall;	// Texel height of the character
	int     offset;		// Offset into the buffer given to GetCharsRGBA
};

#ifdef GetCharABCWidths
#undef GetCharABCWidths
#endif

#if ( defined( WIN32 ) && !defined( _X360 ) ) || ( defined( OSX ) && defined( BYTEMATCH_RETAIL ) ) || defined( LINUX )
//-----------------------------------------------------------------------------
// Purpose: interface the font manager keeps its fonts behind.  On Windows both
//			CWin32Font and CLinuxFont implement it; on OSX COSXFont does not,
//			so only bitmap fonts get created there.
//-----------------------------------------------------------------------------
class IVGuiFontImpl
{
public:
	virtual ~IVGuiFontImpl() {}
	virtual bool Create( const char *windowsFontName, void *data, int size, int tall, int weight, int blur, int scanlines, int flags ) = 0;
	virtual void GetCharRGBA( wchar_t ch, int rgbaWide, int rgbaTall, unsigned char *rgba ) = 0;
	virtual bool IsEqualTo( const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags ) = 0;
	virtual bool IsValid() = 0;
	virtual void GetCharABCWidths( int ch, int &a, int &b, int &c, int *pColorGlyph = 0 ) = 0;
	virtual void *SetAsActiveFont( void *pContext ) = 0;
	virtual int GetHeight() = 0;
	virtual int GetHeightRequested() = 0;
	virtual int GetAscent() = 0;
	virtual int GetMaxCharWidth() = 0;
	virtual int GetFlags() = 0;
	virtual bool GetUnderlined() = 0;
	virtual const char *GetName() = 0;
	virtual const char *GetFamilyName() = 0;
	virtual int GetWeight() = 0;
	virtual void GetKernedCharWidth( wchar_t ch, wchar_t chBefore, wchar_t chAfter, float &wide, float &abcA, float &abcC ) = 0;
	virtual bool SupportsFullCharacterRange() { return false; }
};

#define VGUI_FONT_INTERFACE
#endif

#if defined( WIN32 ) && !defined( _X360 )
#include "Win32Font.h"
#include "linuxfont.h"
typedef IVGuiFontImpl font_t;
typedef CWin32Font font_base_t;
#define VGUI_FONT_FREETYPE
#elif defined( WIN32 )
#include "Win32Font.h"
typedef CWin32Font font_t;
typedef CWin32Font font_base_t;
#elif defined(OSX) && defined(BYTEMATCH_RETAIL)
#include "osxfont.h"
typedef IVGuiFontImpl font_t;
typedef COSXFont font_base_t;
#elif defined( LINUX )
#include "linuxfont.h"
typedef IVGuiFontImpl font_t;
typedef CLinuxFont font_base_t;
#define VGUI_FONT_FREETYPE
#elif defined(POSIX)
// The fork renders fonts with FreeType on macOS too.
#include "linuxfont.h"
typedef CLinuxFont font_t;
typedef CLinuxFont font_base_t;
#define VGUI_FONT_FREETYPE
#else
#error
#endif


#endif //VGUIFONT_H 
