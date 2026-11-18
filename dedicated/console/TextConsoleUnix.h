//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// TextConsoleUnix.h: Unix interface for the TextConsole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined TEXTCONSOLE_UNIX_H
#define TEXTCONSOLE_UNIX_H

#ifndef _WIN32

#include <stdio.h>
#include "textconsole.h"

#ifdef BYTEMATCH_RETAIL
#include <termios.h>

enum ESCAPE_STATE
{
	ESCAPE_CLEAR = 0,
	ESCAPE_RECEIVED,
	ESCAPE_BRACKET_RECEIVED
};

class CTextConsoleUnix : public CTextConsole
{
public:
	virtual ~CTextConsoleUnix() {};

	// CTextConsole
	bool		Init( void );
	void		ShutDown( void );
	void		PrintRaw( const char * pszMsg, int nChars = 0 );
	void		Echo( const char * pszMsg, int nChars = 0 );
	char *		GetLine( void );
	int			GetWidth( void );

private:
	int			kbhit( void );

	bool		m_bConDebug;

	struct termios termStored;
	FILE		*tty;
};
#else
#line 20
class CTextConsoleUnix : public CTextConsole
{
public:
	virtual ~CTextConsoleUnix() { }

	// CTextConsole
	bool		Init();
	void		ShutDown();
	void		Print( char * pszMsg );

	void		SetTitle( char *pszTitle );
	void		SetStatusLine( char *pszStatus );
	void		UpdateStatus();

	char *		GetLine( int index, char *buf, int buflen );
	int			GetWidth();

private:
	bool m_bConDebug;
	FILE *m_tty;
};
#endif

#endif // _ndef WIN32

#endif // !defined TEXTCONSOLE_UNIX_H
