//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// TextConsole.cpp: implementation of the TextConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "textconsole.h"

#ifdef BYTEMATCH_RETAIL
#include <string.h>
#include "tier0/dbg.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

bool CTextConsole::Init()
{
	memset( m_szConsoleText, 0, sizeof( m_szConsoleText ) );
	m_nConsoleTextLen = 0;
	m_nCursorPosition = 0;

	memset( m_szSavedConsoleText, 0, sizeof( m_szSavedConsoleText ) );
	m_nSavedConsoleTextLen = 0;

	memset( m_aszLineBuffer, 0, sizeof( m_aszLineBuffer ) );
	m_nTotalLines = 0;
	m_nInputLine = 0;
	m_nBrowseLine = 0;

	// these are log messages, not related to console
	Msg( "\n" );
	Msg( "Console initialized.\n" );

	m_ConsoleVisible = true;

	return true;
}

void CTextConsole::SetVisible( bool visible )
{
	m_ConsoleVisible = visible;
}

bool CTextConsole::IsVisible( void )
{
	return m_ConsoleVisible;
}

void CTextConsole::ShutDown( void )
{
	;
}

void CTextConsole::Print( char * pszMsg )
{
	if ( m_nConsoleTextLen )
	{
		int nLen;

		nLen = m_nConsoleTextLen;

		while ( nLen-- )
		{
			PrintRaw( "\b \b" );
		}
	}

	PrintRaw( pszMsg );

	if ( m_nConsoleTextLen )
	{
		PrintRaw( m_szConsoleText, m_nConsoleTextLen );
	}

	UpdateStatus();
}

int CTextConsole::ReceiveNewline( void )
{
	int nLen = 0;

	Echo( "\n" );

	if ( m_nConsoleTextLen )
	{
		nLen = m_nConsoleTextLen;

		m_szConsoleText[ m_nConsoleTextLen ] = 0;
		m_nConsoleTextLen = 0;
		m_nCursorPosition = 0;

		// cache line in buffer, but only if it's not a duplicate of the previous line
		if ( ( m_nInputLine == 0 ) || ( strcmp( m_aszLineBuffer[ m_nInputLine - 1 ], m_szConsoleText ) ) )
		{
			strncpy( m_aszLineBuffer[ m_nInputLine ], m_szConsoleText, MAX_CONSOLE_TEXTLEN );

			m_nInputLine++;

			if ( m_nInputLine > m_nTotalLines )
				m_nTotalLines = m_nInputLine;

			if ( m_nInputLine >= MAX_BUFFER_LINES )
				m_nInputLine = 0;

		}

		m_nBrowseLine = m_nInputLine;
	}

	return nLen;
}

void CTextConsole::ReceiveBackspace( void )
{
	int nCount;

	if ( m_nCursorPosition == 0 )
	{
		return;
	}

	m_nConsoleTextLen--;
	m_nCursorPosition--;

	Echo( "\b" );

	for ( nCount = m_nCursorPosition; nCount < m_nConsoleTextLen; nCount++ )
	{
		m_szConsoleText[ nCount ] = m_szConsoleText[ nCount + 1 ];
		Echo( m_szConsoleText + nCount, 1 );
	}

	Echo( " " );

	nCount = m_nConsoleTextLen;
	while ( nCount >= m_nCursorPosition )
	{
		Echo( "\b" );
		nCount--;
	}

	m_nBrowseLine = m_nInputLine;
}

void CTextConsole::ReceiveTab( void )
{
	// No command completion without a command system to ask
	m_szConsoleText[ m_nConsoleTextLen ] = 0;
}

void CTextConsole::ReceiveStandardChar( const char ch )
{
	int nCount;

	// If the line buffer is maxed out, ignore this char
	if ( m_nConsoleTextLen >= ( sizeof( m_szConsoleText ) - 2 ) )
	{
		return;
	}

	nCount = m_nConsoleTextLen;
	while ( nCount > m_nCursorPosition )
	{
		m_szConsoleText[ nCount ] = m_szConsoleText[ nCount - 1 ];
		nCount--;
	}

	m_szConsoleText[ m_nCursorPosition ] = ch;

	Echo( m_szConsoleText + m_nCursorPosition, m_nConsoleTextLen - m_nCursorPosition + 1 );

	m_nConsoleTextLen++;
	m_nCursorPosition++;

	nCount = m_nConsoleTextLen;
	while ( nCount > m_nCursorPosition )
	{
		Echo( "\b" );
		nCount--;
	}

	m_nBrowseLine = m_nInputLine;
}

void CTextConsole::ReceiveUpArrow( void )
{
	int nLastCommandInHistory;

	nLastCommandInHistory = m_nInputLine + 1;
	if ( nLastCommandInHistory > m_nTotalLines )
	{
		nLastCommandInHistory = 0;
	}

	if ( m_nBrowseLine == nLastCommandInHistory )
	{
		return;
	}

	if ( m_nBrowseLine == m_nInputLine )
	{
		if ( m_nConsoleTextLen > 0 )
		{
			// Save off current text
			strncpy( m_szSavedConsoleText, m_szConsoleText, m_nConsoleTextLen );
			// No terminator, it's a raw buffer we always know the length of
		}

		m_nSavedConsoleTextLen = m_nConsoleTextLen;
	}

	m_nBrowseLine--;
	if ( m_nBrowseLine < 0 )
	{
		m_nBrowseLine = m_nTotalLines - 1;
	}

	while ( m_nConsoleTextLen-- )	// delete old line
	{
		Echo( "\b \b" );
	}

	// copy buffered line
	Echo( m_aszLineBuffer[ m_nBrowseLine ] );

	strncpy( m_szConsoleText, m_aszLineBuffer[ m_nBrowseLine ], MAX_CONSOLE_TEXTLEN );
	m_nConsoleTextLen = strlen( m_aszLineBuffer[ m_nBrowseLine ] );

	m_nCursorPosition = m_nConsoleTextLen;
}

void CTextConsole::ReceiveDownArrow( void )
{
	if ( m_nBrowseLine == m_nInputLine )
	{
		return;
	}

	m_nBrowseLine++;
	if ( m_nBrowseLine > m_nTotalLines )
	{
		m_nBrowseLine = 0;
	}

	while ( m_nConsoleTextLen-- )	// delete old line
	{
		Echo( "\b \b" );
	}

	if ( m_nBrowseLine == m_nInputLine )
	{
		if ( m_nSavedConsoleTextLen > 0 )
		{
			// Restore current text
			strncpy( m_szConsoleText, m_szSavedConsoleText, m_nSavedConsoleTextLen );
			// No terminator, it's a raw buffer we always know the length of

			Echo( m_szConsoleText, m_nSavedConsoleTextLen );
		}

		m_nConsoleTextLen = m_nSavedConsoleTextLen;
	}
	else
	{
		// copy buffered line
		Echo( m_aszLineBuffer[ m_nBrowseLine ] );

		strncpy( m_szConsoleText, m_aszLineBuffer[ m_nBrowseLine ], MAX_CONSOLE_TEXTLEN );

		m_nConsoleTextLen = strlen( m_aszLineBuffer[ m_nBrowseLine ] );
	}

	m_nCursorPosition = m_nConsoleTextLen;
}

void CTextConsole::ReceiveLeftArrow( void )
{
	if ( m_nCursorPosition == 0 )
	{
		return;
	}

	Echo( "\b" );
	m_nCursorPosition--;
}

void CTextConsole::ReceiveRightArrow( void )
{
	if ( m_nCursorPosition == m_nConsoleTextLen )
	{
		return;
	}

	Echo( m_szConsoleText + m_nCursorPosition, 1 );
	m_nCursorPosition++;
}
#else
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CTextConsole::Init()
{
	m_ConsoleVisible = true;
	return true;
}
#endif
