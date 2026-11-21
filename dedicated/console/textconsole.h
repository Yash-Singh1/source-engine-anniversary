//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#if !defined TEXTCONSOLE_H
#define TEXTCONSOLE_H
#pragma once

#if defined( BYTEMATCH_RETAIL ) && !defined( PLATFORM_64BITS )
// The retail engine links the Orange Box era console: the base class keeps the
// input line and history, children only do raw I/O.
#define MAX_CONSOLE_TEXTLEN 256
#define MAX_BUFFER_LINES	30

class CTextConsole
{
public:
	virtual ~CTextConsole() {};

	virtual bool		Init ();
	virtual void		ShutDown( void );
	virtual void		Print( char * pszMsg );

	virtual void		SetTitle( char * pszTitle ) { };
	virtual void		SetStatusLine( char * pszStatus ) { };
	virtual void		UpdateStatus( void ) { };

	// Must be provided by children
	virtual void		PrintRaw( const char * pszMsg, int nChars = 0 ) = 0;
	virtual void		Echo( const char * pszMsg, int nChars = 0 ) = 0;
	virtual char *		GetLine( void ) = 0;
	virtual int			GetWidth( void ) = 0;

	virtual void		SetVisible( bool visible );
	virtual bool		IsVisible( void );

protected:
	char	m_szConsoleText[ MAX_CONSOLE_TEXTLEN ];						// console text buffer
	int		m_nConsoleTextLen;											// console textbuffer length
	int		m_nCursorPosition;											// position in the current input line

	// Saved input data when scrolling back through command history
	char	m_szSavedConsoleText[ MAX_CONSOLE_TEXTLEN ];				// console text buffer
	int		m_nSavedConsoleTextLen;										// console textbuffer length

	char	m_aszLineBuffer[ MAX_BUFFER_LINES ][ MAX_CONSOLE_TEXTLEN ];	// command buffer last MAX_BUFFER_LINES commands
	int		m_nInputLine;												// Current line being entered
	int		m_nBrowseLine;												// current buffer line for up/down arrow
	int		m_nTotalLines;												// # of nonempty lines in the buffer

	bool	m_ConsoleVisible;

	int		ReceiveNewline( void );
	void	ReceiveBackspace( void );
	void	ReceiveTab( void );
	void	ReceiveStandardChar( const char ch );
	void	ReceiveUpArrow( void );
	void	ReceiveDownArrow( void );
	void	ReceiveLeftArrow( void );
	void	ReceiveRightArrow( void );
};
#else
#line 13
class CTextConsole
{
public:
	CTextConsole() : m_ConsoleVisible( true ) {}
	virtual ~CTextConsole() {};

	virtual bool		Init ();
	virtual void		ShutDown() = 0;
	virtual void		Print( char * pszMsg ) = 0;

	virtual void		SetTitle( char * pszTitle ) = 0;
	virtual void		SetStatusLine( char * pszStatus ) = 0;
	virtual void		UpdateStatus() = 0;

	// Must be provided by children
	virtual char *		GetLine( int index, char *buf, int buflen ) = 0;
	virtual int			GetWidth() = 0;

	virtual void		SetVisible( bool visible ) { m_ConsoleVisible = visible; }
	virtual bool		IsVisible() { return m_ConsoleVisible; }

protected:
	bool m_ConsoleVisible;
};
#endif

#endif // !defined TEXTCONSOLE_H
