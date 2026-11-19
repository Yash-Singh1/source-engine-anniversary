//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <math.h>
#include <tier0/dbg.h>
#include <vgui/ISurface.h>
#include <tier0/mem.h>
#include <utlbuffer.h>

#include "vgui_surfacelib/osxfont.h"
#include "FontEffects.h"

struct MetricsTweaks_t
{
	const char *m_windowsFontName;
	int m_sizeAdjust;
	float m_ascentMultiplier;
	float m_descentMultiplier;
	float m_leadingMultiplier;
};

static const MetricsTweaks_t g_defaultMetricTweaks = { NULL, 0, 1.0, 1.0, 1.0 };

static MetricsTweaks_t g_FontMetricTweaks[] =
{
	{ "Helvetica", 0, 1.0, 1.0, 1.05 },
	{ "Helvetica Bold", 0, 1.0, 1.0, 1.0 },
	{ "HL2cross", 0, 0.8, 1.0, 1.1},
	{ "Counter-Strike Logo", 0, 1.0, 1.0, 1.1 },
	{ "TF2", -2, 1.0, 1.0, 1.0 },
	{ "TF2 Professor", -2, 1.0, 2.0, 1.1 },
	{ "TF2 Build", -2, 1.0, 1.0, 1.0 },
	{ "Stubble bold", -6, 1.3, 1.0, 1.0 },
};

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
COSXFont::COSXFont() : m_ExtendedABCWidthsCache( 256, 0, &ExtendedABCWidthsCacheLessFunc ),
						m_ExtendedKernedABCWidthsCache( 256, 0, &ExtendedKernedABCWidthsCacheLessFunc )
{
	m_iTall = 0;
	m_iWeight = 0;
	m_iFlags = 0;
	m_bAntiAliased = false;
	m_bRotary = false;
	m_bAdditive = false;
	m_iDropShadowOffset;
	m_iMaxCharWidth = 0;
	m_bUnderlined = false;

	m_iBlur = 0;
	m_pGaussianDistribution = NULL;
	m_iScanLines = 0;

	m_pContextMemory = NULL;
	m_ContextRef = 0;
	m_ATSUStyle = NULL;
	m_ATSUStyleNoUnderline = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
COSXFont::~COSXFont()
{
	if ( m_ContextRef )
		CGContextRelease( m_ContextRef );

	if ( m_pContextMemory )
		delete [] m_pContextMemory;

	if ( m_ATSUStyle )
		ATSUDisposeStyle( m_ATSUStyle );

	if ( m_ATSUStyleNoUnderline )
		ATSUDisposeStyle( m_ATSUStyleNoUnderline );
}

//-----------------------------------------------------------------------------
// Purpose: creates the font from windows.  returns false if font does not exist in the OS.
//-----------------------------------------------------------------------------
bool COSXFont::Create( const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	// setup font properties
	m_szName = windowsFontName;
	m_iTall = tall;
	m_iWeight = weight;
	m_iFlags = flags;
	m_bAntiAliased = flags & vgui::ISurface::FONTFLAG_ANTIALIAS;
	if ( tall < 10 )
		m_bAntiAliased = false;
	m_bUnderlined = flags & vgui::ISurface::FONTFLAG_UNDERLINE;
	m_iDropShadowOffset = ( flags & vgui::ISurface::FONTFLAG_DROPSHADOW ) ? 1 : 0;
	m_iOutlineSize = ( flags & vgui::ISurface::FONTFLAG_OUTLINE ) ? 1 : 0;
	m_iBlur = blur;
	m_iScanLines = scanlines;
	m_bRotary = flags & vgui::ISurface::FONTFLAG_ROTARY;
	m_bAdditive = flags & vgui::ISurface::FONTFLAG_ADDITIVE;

	m_ATSUFont = kATSFontRefUnspecified;
	CFStringRef fontName = CFStringCreateWithCString( kCFAllocatorDefault, windowsFontName, kCFStringEncodingUTF8 );
	if ( fontName )
	{
		m_ATSUFont = ATSFontFindFromPostScriptName( fontName, kATSOptionFlagsDefault );
		CFRelease( fontName );
	}

	if ( m_ATSUFont == kATSFontRefUnspecified )
	{
		CFStringRef fontName = CFStringCreateWithCString( kCFAllocatorDefault, windowsFontName, kCFStringEncodingUTF8 );
		if ( fontName )
		{
			m_ATSUFont = ATSFontFindFromName( fontName, kATSOptionFlagsDefault );
			CFRelease( fontName );
		}
	}

	if ( m_ATSUFont == kATSFontRefUnspecified )
	{
		m_szName = "";
		return false;
	}

	OSStatus err = ATSUCreateStyle( &m_ATSUStyle );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	ATSFontMetrics aMetrics;
	err = ATSFontGetHorizontalMetrics( m_ATSUFont, kATSOptionFlagsDefault, &aMetrics );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	MetricsTweaks_t metricTweaks = g_defaultMetricTweaks;
	for ( int i = 0; i < Q_ARRAYSIZE( g_FontMetricTweaks ); i++ )
	{
		if ( !Q_stricmp( windowsFontName, g_FontMetricTweaks[ i ].m_windowsFontName ) )
		{
			metricTweaks = g_FontMetricTweaks[ i ];
			break;
		}
	}

	float flFontSize = ( (float)( m_iTall + metricTweaks.m_sizeAdjust ) / ( aMetrics.ascent - aMetrics.descent + aMetrics.leading ) );
	Fixed fontsize = FloatToFixed( flFontSize );  // font size is based on 72dpi and not pixels, so convert to pixels
	Boolean isItalic = m_iFlags & vgui::ISurface::FONTFLAG_ITALIC;
	Boolean isBold = Q_stristr( windowsFontName, "bold" ) != NULL;
	Boolean isUnderlined = m_bUnderlined;
	ATSStyleRenderingOptions renderOpt = m_bAntiAliased ? kATSStyleApplyAntiAliasing : kATSStyleNoAntiAliasing;

	ATSUAttributeTag styleTags[ 6 ];
	ATSUAttributeValuePtr styleValues[ 6 ];
	ByteCount styleSizes[ 6 ];

	styleTags[ 0 ] = kATSUFontTag;
	styleTags[ 1 ] = kATSUSizeTag;
	styleTags[ 2 ] = kATSUQDItalicTag;
	styleTags[ 3 ] = kATSUQDBoldfaceTag;
	styleTags[ 4 ] = kATSUStyleRenderingOptionsTag;
	styleTags[ 5 ] = kATSUQDUnderlineTag;

	styleValues[ 0 ] = &m_ATSUFont;
	styleValues[ 1 ] = &fontsize;
	styleValues[ 2 ] = &isItalic;
	styleValues[ 3 ] = &isBold;
	styleValues[ 4 ] = &renderOpt;
	styleValues[ 5 ] = &isUnderlined;

	styleSizes[ 0 ] = sizeof( ATSUFontID );
	styleSizes[ 1 ] = sizeof( Fixed );
	styleSizes[ 2 ] = sizeof( Boolean );
	styleSizes[ 3 ] = sizeof( Boolean );
	styleSizes[ 4 ] = sizeof( renderOpt );
	styleSizes[ 5 ] = sizeof( Boolean );

	err = ATSUSetAttributes( m_ATSUStyle, Q_ARRAYSIZE( styleTags ), styleTags, styleSizes, styleValues );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	// metrics are measured without the underline
	err = ATSUCreateStyle( &m_ATSUStyleNoUnderline );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	isUnderlined = false;
	err = ATSUSetAttributes( m_ATSUStyleNoUnderline, Q_ARRAYSIZE( styleTags ), styleTags, styleSizes, styleValues );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	m_iAscent = ceil( ( aMetrics.ascent / ( aMetrics.ascent - aMetrics.descent + aMetrics.leading ) ) *
	                  ( m_iTall + metricTweaks.m_sizeAdjust ) * ( metricTweaks.m_ascentMultiplier ) );
	m_iHeight = ceil( ((float)( m_iTall + metricTweaks.m_sizeAdjust ) *
	                   ( aMetrics.ascent * metricTweaks.m_ascentMultiplier -
	                     aMetrics.descent * metricTweaks.m_descentMultiplier +
	                     aMetrics.leading * metricTweaks.m_leadingMultiplier ) +
	                   m_iDropShadowOffset + 2 * m_iOutlineSize ) );

	m_iMaxCharWidth = ( metricTweaks.m_leadingMultiplier * aMetrics.maxAdvanceWidth * m_iTall ) + 0.5;

	unsigned int bytesPerRow = m_iMaxCharWidth * 4;

	m_pContextMemory = new char[ (int)bytesPerRow * m_iHeight ];
	Q_memset( m_pContextMemory, 0x0, (int)( bytesPerRow * m_iHeight ) );

	const size_t bitsPerComponent = 8;
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	m_ContextRef = CGBitmapContextCreate(
						  m_pContextMemory,
						  m_iMaxCharWidth,
						  m_iHeight,
						  bitsPerComponent,
						  bytesPerRow,
						  colorSpace,
						  kCGImageAlphaPremultipliedLast );

	CGColorSpaceRelease( colorSpace );

	CGContextSetAllowsAntialiasing( m_ContextRef, m_bAntiAliased );
	CGContextSetShouldAntialias( m_ContextRef, m_bAntiAliased );
	CGContextSetTextDrawingMode( m_ContextRef, kCGTextFill );
	CGContextSetRGBStrokeColor( m_ContextRef, 1.0f, 1.0f, 1.0f, 1.0f );
	CGContextSetLineWidth( m_ContextRef, 1 );

	float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ATSUAttributeTag colorTag = kATSURGBAlphaColorTag;
	ByteCount colorSize = sizeof( color );
	ATSUAttributeValuePtr colorValue = &color;
	err = ATSUSetAttributes( m_ATSUStyle, 1, &colorTag, &colorSize, &colorValue );
	if ( err != noErr )
	{
		m_szName = "";
		return false;
	}

	// Calculate our gaussian distribution for if we're blurred.
	if ( m_iBlur > 1 )
	{
		m_pGaussianDistribution = new float[ m_iBlur * 2 + 1 ];
		double sigma = 0.683 * m_iBlur;

		for (int x = 0; x <= (m_iBlur * 2); x++)
		{
			int val = x - m_iBlur;
			m_pGaussianDistribution[x] = (float)(1.0f / sqrt(2 * 3.14 * sigma * sigma)) * pow(2.7, -1 * (val * val) / (2 * sigma * sigma));
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: writes the char into the specified 32bpp texture
//-----------------------------------------------------------------------------
void COSXFont::GetCharRGBA( wchar_t ch, int rgbaWide, int rgbaTall, unsigned char *rgba )
{
	if ( !m_ContextRef )
	{
		Assert( !"Context ref not setup to allow GetCharRGBA" );
		return;
	}

	UniChar buffer[ 3 ] = { 0 };
	UniCharCount runLength;
	if ( ch <= 0xFFFF )
	{
		buffer[ 0 ] = (UniChar)ch;
		runLength = 1;
	}
	else
	{
		ch -= 0x010000;
		buffer[ 0 ] = (UniChar)( ch >> 10 ) | 0xD800;
		buffer[ 1 ] = (UniChar)( ch & 0x3FF ) | 0xDC00;
		runLength = 2;
	}

	ATSUTextLayout textLayout;
	OSStatus err = ATSUCreateTextLayoutWithTextPtr( buffer, 0, 1, 1, 1, &runLength, &m_ATSUStyle, &textLayout );
	if ( err != noErr )
		return;

	ATSUAttributeTag theTags[ 8 ];
	ByteCount theSizes[ 8 ];
	ATSUAttributeValuePtr theValues[ 8 ];

	theTags[ 0 ] = kATSUCGContextTag;
	theSizes[ 0 ] = sizeof( CGContextRef );
	theValues[ 0 ] = &m_ContextRef;

	ATSUTextMeasurement lineWidth = IntToFixed( 32000 );
	theTags[ 1 ] = kATSULineWidthTag;
	theSizes[ 1 ] = sizeof( ATSUTextMeasurement );
	theValues[ 1 ] = &lineWidth;

	Fract alignment = kATSUStartAlignment;
	theTags[ 2 ] = kATSULineFlushFactorTag;
	theSizes[ 2 ] = sizeof( Fract );
	theValues[ 2 ] = &alignment;

	err = ATSUSetLayoutControls( textLayout, 3, theTags, theSizes, theValues );
	if ( err != noErr )
		return;

	err = ATSUSetTransientFontMatching( textLayout, true );
	if ( err != noErr )
		return;

	CGRect rect = { { 0, 0 }, { m_iMaxCharWidth, m_iHeight } };
	CGContextClearRect( m_ContextRef, rect );

	CGContextFlush( m_ContextRef );

	int a, b, c;
	GetCharABCWidths( ch, a, b, c );

	// You are not seeing a bug.  We need the background to have a full
	// alpha channel since it's going to bitblt in the overlay.  But osx when it renders
	// to a full alpha channel likes to 'antialias' the alpha as well as the color blending.
	// Turning off antialiasing doesn't just make the characters jagged, but when a font
	// is 1 pixel wide it ends up alpha blending into the background.  By rendering it
	// more than once we dominate this alpha.  Looking at worst case, 3 was the magic number.
	// Messing with the anti-aliasing settings on the bitmap, or the alpha config (including
	// alpha-skip-last) caused no change, or complete disabling of antialiasing.  Glad these
	// are cached.
	if ( m_iHeight < 20 )
	{
		ATSUDrawText( textLayout, 0, 1, Long2Fix( 1 - a ), Long2Fix( m_iHeight - m_iAscent ) );
	}

	if ( ( m_iHeight < 16 ) || ( m_iFlags & vgui::ISurface::FONTFLAG_CUSTOM ) )
	{
		ATSUDrawText( textLayout, 0, 1, Long2Fix( 1 - a ), Long2Fix( m_iHeight - m_iAscent ) );
	}

	err = ATSUDrawText( textLayout, 0, 1, Long2Fix( 1 - a ), Long2Fix( m_iHeight - m_iAscent ) );
	if ( err != noErr )
		return;

	char *pContextData = (char *)CGBitmapContextGetData( m_ContextRef );

	unsigned char *pchPixelData = rgba;
	for ( int y = 0; y < rgbaTall; y++ )
	{
		char *row = pContextData + y * m_iMaxCharWidth * 4;
		memcpy( pchPixelData, row, rgbaWide * 4 );
		pchPixelData+= ( rgbaWide * 4 );
	}

	// apply requested effects in specified order
	ApplyDropShadowToTexture( rgbaWide, rgbaTall, rgba, m_iDropShadowOffset );
	ApplyOutlineToTexture( rgbaWide, rgbaTall, rgba, m_iOutlineSize );
	ApplyGaussianBlurToTexture( rgbaWide, rgbaTall, rgba, m_iBlur, false );
	ApplyScanlineEffectToTexture( rgbaWide, rgbaTall, rgba, m_iScanLines );
	ApplyRotaryEffectToTexture( rgbaWide, rgbaTall, rgba, m_bRotary );

	ATSUDisposeTextLayout( textLayout );
}

//-----------------------------------------------------------------------------
// Purpose: gets the abc widths for a character
//-----------------------------------------------------------------------------
void COSXFont::GetCharABCWidths( int ch, int &a, int &b, int &c, int *pColorGlyph )
{
	Assert( IsValid() );

	// Look for it in the cache.
	abc_cache_t finder = { (wchar_t)ch };

	unsigned short i = m_ExtendedABCWidthsCache.Find( finder );
	if ( m_ExtendedABCWidthsCache.IsValidIndex( i ) )
	{
		a = m_ExtendedABCWidthsCache[i].abc.a;
		b = m_ExtendedABCWidthsCache[i].abc.b;
		c = m_ExtendedABCWidthsCache[i].abc.c;
		return;
	}

	wchar_t wch[ 2 ] = { (wchar_t)ch, 0 };
	UniChar uniChars[ 2 ];
	if ( !V_UnicodeToUCS2( wch, sizeof( wch ), (char *)uniChars, sizeof( uniChars ) ) )
		return;

	UniCharCount runLength = kATSUToTextEnd;
	ATSUTextLayout textLayout;
	OSStatus err = ATSUCreateTextLayoutWithTextPtr( uniChars, 0, 2, 2, 1, &runLength, &m_ATSUStyleNoUnderline, &textLayout );
	if ( err != noErr )
		return;

	ATSUAttributeTag theTags[ 3 ];
	ByteCount theSizes[ 3 ];
	ATSUAttributeValuePtr theValues[ 3 ];

	ATSUTextMeasurement lineWidth = IntToFixed( 2000 );
	theTags[ 0 ] = kATSULineWidthTag;
	theSizes[ 0 ] = sizeof( ATSUTextMeasurement );
	theValues[ 0 ] = &lineWidth;

	Fract alignment = kATSUStartAlignment;
	theTags[ 1 ] = kATSULineFlushFactorTag;
	theSizes[ 1 ] = sizeof( Fract );
	theValues[ 1 ] = &alignment;

	err = ATSUSetLayoutControls( textLayout, 2, theTags, theSizes, theValues );
	if ( err != noErr )
		return;

	ATSLayoutRecord *layoutRecords;
	ItemCount glyphCount;
	err = ATSUDirectGetLayoutDataArrayPtrFromTextLayout( textLayout, 0, kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, ( void ** )&layoutRecords, &glyphCount );
	if ( err != noErr )
		return;

	ATSGlyphScreenMetrics gm[ 2 ];
	err = ATSUGlyphGetScreenMetrics( m_ATSUStyleNoUnderline, 2, &layoutRecords[ 0 ].glyphID, sizeof( ATSLayoutRecord ), false, false, gm );
	if ( err != noErr )
		return;

	ATSUDirectReleaseLayoutDataArrayPtr( NULL, kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, ( void ** )&layoutRecords );

	finder.abc.a = roundf( gm[ 0 ].sideBearing.x - (float)m_iBlur - m_iOutlineSize );
	finder.abc.b = ceilf( gm[ 0 ].deviceAdvance.x + ( ( (float)m_iBlur + m_iOutlineSize ) * 2 ) + m_iDropShadowOffset );
	finder.abc.c = ceilf( gm[ 0 ].otherSideBearing.x - (float)m_iBlur - m_iDropShadowOffset - m_iOutlineSize );

	if ( m_iFlags & vgui::ISurface::FONTFLAG_ITALIC )
	{
		finder.abc.b += 4.0f;
	}

	if ( finder.abc.a + finder.abc.b + finder.abc.c == 0 )
	{
		if ( finder.abc.b + finder.abc.c == 0 )
			finder.abc.c = 0;
		else if ( finder.abc.a + finder.abc.b == 0 )
			finder.abc.a = 0;
		else
			finder.abc.a = finder.abc.c = 0;
	}

	// spaces come out too wide in most fonts
	if ( ch == ' ' && !strstr( m_szName.String(), "Helvetica" ) && !strstr( m_szName.String(), "Verdana" ) && m_iFlags == 0 )
	{
		if ( m_iHeight > 20 )
			finder.abc.b -= 3;
		if ( m_iHeight > 10 )
			finder.abc.b -= 1;
	}

	a = finder.abc.a;
	b = finder.abc.b;
	c = finder.abc.c;

	m_ExtendedABCWidthsCache.Insert( finder );

	ATSUDirectReleaseLayoutDataArrayPtr( NULL, kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, ( void ** )&layoutRecords );
	ATSUDisposeTextLayout( textLayout );
}


//-----------------------------------------------------------------------------
// Purpose: returns true if the font is equivalent to that specified
//-----------------------------------------------------------------------------
bool COSXFont::IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	if ( !Q_stricmp( windowsFontName, m_szName.String() ) &&
		( m_iTall == tall ) &&
		( m_iWeight == weight ) &&
		( m_iBlur == blur ) &&
		( m_iFlags == flags ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: returns true only if this font is valid for use
//-----------------------------------------------------------------------------
bool COSXFont::IsValid()
{
	if ( !m_szName.IsEmpty() && m_szName.String()[ 0 ] )
		return true;
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: returns the height of the font, in pixels
//-----------------------------------------------------------------------------
int COSXFont::GetHeight()
{
	Assert( IsValid() );
	return m_iHeight;
}

//-----------------------------------------------------------------------------
// Purpose: returns the requested height of the font
//-----------------------------------------------------------------------------
int COSXFont::GetHeightRequested()
{
	Assert( IsValid() );
	return m_iTall;
}

//-----------------------------------------------------------------------------
// Purpose: returns the ascent of the font, in pixels (ascent=units above the base line)
//-----------------------------------------------------------------------------
int COSXFont::GetAscent()
{
	Assert( IsValid() );
	return m_iAscent;
}

//-----------------------------------------------------------------------------
// Purpose: returns the maximum width of a character, in pixels
//-----------------------------------------------------------------------------
int COSXFont::GetMaxCharWidth()
{
	Assert( IsValid() );
	return m_iMaxCharWidth;
}

//-----------------------------------------------------------------------------
// Purpose: returns the flags used to make this font, used by the dynamic resizing code
//-----------------------------------------------------------------------------
int COSXFont::GetFlags()
{
	return m_iFlags;
}

//-----------------------------------------------------------------------------
// Purpose: Comparison function for abc widths storage
//-----------------------------------------------------------------------------
bool COSXFont::ExtendedABCWidthsCacheLessFunc(const abc_cache_t &lhs, const abc_cache_t &rhs)
{
	return lhs.wch < rhs.wch;
}

//-----------------------------------------------------------------------------
// Purpose: Comparison function for abc widths storage
//-----------------------------------------------------------------------------
bool COSXFont::ExtendedKernedABCWidthsCacheLessFunc(const kerned_abc_cache_t &lhs, const kerned_abc_cache_t &rhs)
{
	return ( lhs.wch < rhs.wch ) ||
			( lhs.wch == rhs.wch && lhs.wchBefore < rhs.wchBefore ) ||
			( lhs.wch == rhs.wch && lhs.wchBefore == rhs.wchBefore && lhs.wchAfter < rhs.wchAfter );
}


ATSUStyle *COSXFont::SetAsActiveFont( CGContextRef cgContext )
{
	CGContextSelectFont ( cgContext, m_szName.String(), m_iHeight, kCGEncodingMacRoman );
	return &m_ATSUStyle;
}

#ifdef DBGFLAG_VALIDATE

//-----------------------------------------------------------------------------
// Purpose: Ensure that all of our internal structures are consistent, and
//			account for all memory that we've allocated.
// Input:	validator -		Our global validator object
//			pchName -		Our name (typically a member var in our container)
//-----------------------------------------------------------------------------
void COSXFont::Validate( CValidator &validator, char *pchName )
{
	validator.Push( "COSXFont", this, pchName );

	m_ExtendedABCWidthsCache.Validate( validator, "m_ExtendedABCWidthsCache" );
	m_ExtendedKernedABCWidthsCache.Validate( validator, "m_ExtendedKernedABCWidthsCache" );
	validator.ClaimMemory( m_pGaussianDistribution );

	validator.Pop();
}

#endif // DBGFLAG_VALIDATE
