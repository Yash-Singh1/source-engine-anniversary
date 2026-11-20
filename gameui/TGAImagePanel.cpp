//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "TGAImagePanel.h"
#include "bitmap/tgaloader.h"
#include "vgui/ISurface.h"
#include "gameui_proportional.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
#ifdef BYTEMATCH_RETAIL
#define V_strcpy_safe( pDest, pSrc ) Q_strcpy( pDest, pSrc )
#endif
#line 14

using namespace vgui;

CTGAImagePanel::CTGAImagePanel( vgui::Panel *parent, const char *name ) : BaseClass( parent, name )
{
	m_iTextureID = -1;
	m_bHasValidTexture = false;
	m_bLoadedTexture = false;
	m_szTGAName[0] = 0;
#ifndef BYTEMATCH_RETAIL
	m_iImageWidth = 0;
	m_iImageHeight = 0;
	m_iImageRealWidth = 0;
	m_iImageRealHeight = 0;
#endif

	SetPaintBackgroundEnabled( false );
}

CTGAImagePanel::~CTGAImagePanel()
{
	// release the texture memory
	if ( vgui::surface() && m_iTextureID != -1 )
	{
		vgui::surface()->DestroyTextureID( m_iTextureID );
		m_iTextureID = -1;
	}
}

void CTGAImagePanel::SetTGA( const char *filename, const char *pathID )
{
	Q_snprintf( m_szTGAName, sizeof(m_szTGAName), "//%s/%s", pathID, filename );
}

void CTGAImagePanel::SetTGA( const char *filename )
{
	Q_snprintf( m_szTGAName, sizeof(m_szTGAName), "//MOD/%s", filename );
}

void CTGAImagePanel::SetTGANonMod( const char *filename )
{
	V_strcpy_safe( m_szTGAName, filename );
}

#ifdef BYTEMATCH_RETAIL
void CTGAImagePanel::Paint()
{
	if ( !m_bLoadedTexture )
	{
		m_bLoadedTexture = true;
		// get a texture id, if we haven't already
		if ( m_iTextureID == -1 )
		{
			m_iTextureID = vgui::surface()->CreateNewTextureID( true );
			SetSize( QuickPropScale( 180 ), QuickPropScale( 100 ) );
		}

		// load the file
		CUtlMemory<unsigned char> tga;
		if ( TGALoader::LoadRGBA8888( m_szTGAName, tga, m_iImageWidth, m_iImageHeight ) )
		{
			// set the textureID
			surface()->DrawSetTextureRGBA( m_iTextureID, tga.Base(), m_iImageWidth, m_iImageHeight, true, true );
			m_bHasValidTexture = true;
			// set our size to be the size of the tga
			SetSize( QuickPropScale( m_iImageWidth ), QuickPropScale( m_iImageHeight ) );
		}
		else
		{
			m_bHasValidTexture = false;
		}
	}

	// draw the image
	int wide = 180, tall = 100;
	if ( m_bHasValidTexture )
	{
		surface()->DrawGetTextureSize( m_iTextureID, wide, tall );
		wide = QuickPropScale( wide );
		tall = QuickPropScale( tall );
		surface()->DrawSetTexture( m_iTextureID );
		surface()->DrawSetColor( 255, 255, 255, 255 );
		surface()->DrawTexturedRect( 0, 0, wide, tall );
	}
	else
	{
		// draw a black fill instead
		wide = QuickPropScale( wide );
		tall = QuickPropScale( tall );
		surface()->DrawSetColor( 0, 0, 0, 255 );
		surface()->DrawFilledRect( 0, 0, wide, tall );
	}
}
#else
void CTGAImagePanel::Paint()
{
	if ( !m_bLoadedTexture )
	{
		m_bLoadedTexture = true;
		// get a texture id, if we haven't already
		if ( m_iTextureID == -1 )
		{
			m_iTextureID = vgui::surface()->CreateNewTextureID( true );
			SetSize( 180, 100 );
		}

		// load the file
		CUtlMemory<unsigned char> tga;
#ifndef _XBOX
		if ( TGALoader::LoadRGBA8888( m_szTGAName, tga, m_iImageWidth, m_iImageHeight ) )
		{
			// set the textureID
			surface()->DrawSetTextureRGBA( m_iTextureID, tga.Base(), m_iImageWidth, m_iImageHeight, false, true );
			m_bHasValidTexture = true;

			surface()->DrawGetTextureSize(m_iTextureID, m_iImageRealWidth, m_iImageRealHeight);
			if (IsProportional())
			{
				m_iImageRealWidth = scheme()->GetProportionalScaledValueEx(GetScheme(), m_iImageRealWidth);
				m_iImageRealHeight = scheme()->GetProportionalScaledValueEx(GetScheme(), m_iImageRealHeight);

				m_iImageWidth = scheme()->GetProportionalScaledValueEx(GetScheme(), m_iImageWidth);
				m_iImageHeight = scheme()->GetProportionalScaledValueEx(GetScheme(), m_iImageHeight);
			}

			// set our size to be the size of the tga
			SetSize(m_iImageWidth, m_iImageHeight);
		}
		else
#endif
		{
			m_bHasValidTexture = false;
		}
	}

	// draw the image
	int wide, tall;
	if ( m_bHasValidTexture )
	{
		surface()->DrawSetTexture( m_iTextureID );
		surface()->DrawSetColor( 255, 255, 255, 255 );
		surface()->DrawTexturedRect( 0, 0, m_iImageRealWidth, m_iImageRealHeight );
	}
	else
	{
		// draw a black fill instead
		wide = 180, tall = 100;
		surface()->DrawSetColor( 0, 0, 0, 255 );
		surface()->DrawFilledRect( 0, 0, wide, tall );
	}
}
#endif
