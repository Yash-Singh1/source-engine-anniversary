//========= Copyright Valve Corporation, All rights reserved. ============//
//
// dx_iunknown.h
//
//==================================================================================================

#ifndef DX_IUNKNOWN_H
#define DX_IUNKNOWN_H

#pragma once

// turn this on to get refcount logging from IUnknown
#define	IUNKNOWN_ALLOC_SPEW 0
#define	IUNKNOWN_ALLOC_SPEW_MARK_ALL 0

struct IUnknown
{
	int	m_refcount[2];
		
	IUnknown( void )
	{
		m_refcount[0] = 1;
		m_refcount[1] = 0;

		#if IUNKNOWN_ALLOC_SPEW
			if (IUNKNOWN_ALLOC_SPEW_MARK_ALL)
			{
				GLMPRINTF(("-A- IUnew (%08x) refc -> (%d,%d) ",this,m_refcount[0],m_refcount[1]));
			}
		#endif
	};
				
	virtual	~IUnknown( void )
	{
		#if IUNKNOWN_ALLOC_SPEW
			if (IUNKNOWN_ALLOC_SPEW_MARK_ALL)
			{
				GLMPRINTF(("-A- IUdel (%08x) ",this ));
			}
		#endif
	};
		
	void	AddRef( int which=0, char *comment = NULL )
	{
		Assert( which >= 0 );
		Assert( which < 2 );
		m_refcount[which]++;
			
		#if IUNKNOWN_ALLOC_SPEW
			if (IUNKNOWN_ALLOC_SPEW_MARK_ALL)
			{
				GLMPRINTF(("-A- IUAddRef  (%08x,%d) refc -> (%d,%d) [%s]",this,which,m_refcount[0],m_refcount[1],comment?comment:"..."))	;
				if (!comment)
				{
					GLMPRINTF((""))	;	// place to hang a breakpoint
				}
			}
		#endif
	};
		
	ULONG __stdcall	Release( int which=0, char *comment = NULL )
	{
		Assert( which >= 0 );
		Assert( which < 2 );
			
		//int oldrefcs[2] = { m_refcount[0], m_refcount[1] };
		bool deleting = false;
			
		m_refcount[which]--;
		if ( (!m_refcount[0]) && (!m_refcount[1]) )
		{
			deleting = true;
		}
			
		#if IUNKNOWN_ALLOC_SPEW
			if (IUNKNOWN_ALLOC_SPEW_MARK_ALL)
			{
				GLMPRINTF(("-A- IURelease (%08x,%d) refc -> (%d,%d) [%s] %s",this,which,m_refcount[0],m_refcount[1],comment?comment:"...",deleting?"->DELETING":""));
				if (!comment)
				{
					GLMPRINTF((""))	;	// place to hang a breakpoint
				}
			}
		#endif

		if (deleting)
		{
			delete this;
			return 0;
		}
		else
		{
			return m_refcount[0];
		}
	};
		
	void	SetMark( bool markValue, char *comment=NULL )
	{
		#if IUNKNOWN_ALLOC_SPEW
			if (markValue)	// leading edge detect
			{
				// print the same thing that the constructor would have printed if it had been marked from the beginning
				// i.e. it's anticipated that callers asking for marking will do so right at create time
				GLMPRINTF(("-A- IUSetMark (%08x) refc -> (%d,%d) (%s) ",this,m_refcount[0],m_refcount[1],comment?comment:"..."));
			}
		#endif

	}
};

#endif // DX_IUNKNOWN_H
