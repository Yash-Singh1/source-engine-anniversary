//========= Copyright Valve Corporation, All rights reserved. ============//
//
// cglmquery.cpp
//
//===============================================================================

#include "togl/rendermechanism.h"

#ifndef _WIN32
	#include <unistd.h>
#endif

// memdbgon -must- be the last include file in a .cpp file.
#include "tier0/memdbgon.h"

//===============================================================================

// http://www.opengl.org/registry/specs/ARB/occlusion_query.txt

extern ConVar gl_errorcheckall;
extern ConVar gl_errorcheckqueries;
extern ConVar gl_errorchecknone;

// how many microseconds to wait after a failed query-available test
// presently on MTGL this doesn't happen, but it could change, keep this handy
ConVar  gl_nullqueries( "gl_nullqueries", "0" );

#define	ERRCHECK()	( (gl_errorcheckall.GetInt() != 0) || (gl_errorcheckqueries.GetInt() != 0) ) && (gl_errorchecknone.GetInt() == 0)


//===============================================================================

CGLMQuery::CGLMQuery( GLMContext *ctx, GLMQueryParams *params )
{
	// get the type of query requested
	// generate name(s) needed
	// set initial state appropriately
	
	m_ctx = ctx;
	m_params = *params;

	m_name				=	0;
	m_syncobj			=	0;

	m_started = m_stopped = m_done = false;
	
	m_nullQuery = false;
		// assume value of convar at start time
		// does not change during individual query lifetime
		// started null = stays null
		// started live = stays live
	
	GLenum	errorcode = GL_NO_ERROR;
	
	switch(m_params.m_type)
	{
		case EOcclusion:
		{
			//make an occlusion query (and a fence to go with it)
			gGL->glGenQueriesARB( 1, &m_name );
			if (ERRCHECK())
			{
				errorcode = gGL->glGetError();
				if (errorcode)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::CGLMQuery (OQ) saw %s error (%d) from glGenQueriesARB", decodedStr, errorcode  );
					m_name = 0;
				}
			}
		}
		break;

		case EFence:
			//make a fence - no aux fence needed

			m_syncobj = 0;

			if (gGL->m_bHave_GL_NV_fence)
				gGL->glGenFencesNV(1, &m_name );
			else if (gGL->m_bHave_GL_APPLE_fence)
				gGL->glGenFencesAPPLE(1, &m_name );

			if (ERRCHECK())
			{
				errorcode = gGL->glGetError();
				if (errorcode)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::CGLMQuery (fence) saw %s error (%d) from glGenFences*", decodedStr, errorcode  );
					m_name = 0;
				}
			}
		break;
	}
	
}

CGLMQuery::~CGLMQuery()
{
	// make sure query has completed (might not be necessary)
	// delete the name(s)

	GLenum errorcode = GL_NO_ERROR;
	
	switch(m_params.m_type)
	{
		case EOcclusion:
		{
			// do a finish occlusion query ?
			gGL->glDeleteQueriesARB(1, &m_name );
			if (ERRCHECK())
			{
				errorcode = gGL->glGetError();
				if (errorcode)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::~CGLMQuery (OQ) saw %s error (%d) from glDeleteQueries", decodedStr, errorcode  );
				}
			}
		}
		break;

		case EFence:
		{
			// do a finish fence ?
			if (gGL->m_bHave_GL_NV_fence)
				gGL->glDeleteFencesNV(1, &m_name );
			else if (gGL->m_bHave_GL_APPLE_fence)
				gGL->glDeleteFencesAPPLE(1, &m_name );

			if (ERRCHECK())
			{
				errorcode = gGL->glGetError();
				if (errorcode)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::~CGLMQuery (fence) saw %s error (%d) from glDeleteFences*", decodedStr, errorcode  );
				}
			}
		}
		break;
	}
	
	m_name = 0;
	m_syncobj = 0;
}




void	CGLMQuery::Start( void )		// "start counting"
{
	m_started = true;
	m_stopped = false;
	m_done = false;

	m_nullQuery = (gl_nullqueries.GetInt() != 0);	// latch value for remainder of query life

	GLenum errorcode = GL_NO_ERROR;

	switch(m_params.m_type)
	{
		case EOcclusion:
		{
			if (m_nullQuery)
			{
				// do nothing..
			}
			else
			{
				gGL->glBeginQueryARB( GL_SAMPLES_PASSED_ARB, m_name );
				if (ERRCHECK())
				{
					errorcode = gGL->glGetError();
					if (errorcode)
					{
						const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
						printf( "\nCGLMQuery::Start(OQ) saw %s error (%d) from glBeginQueryARB (GL_SAMPLES_PASSED_ARB) name=%d", decodedStr, errorcode, m_name  );
					}
				}
			}
		}
		break;

		case EFence:
			if (gGL->m_bHave_GL_NV_fence)
				gGL->glSetFenceNV( m_name, GL_ALL_COMPLETED_NV );
			else if (gGL->m_bHave_GL_APPLE_fence)
				gGL->glSetFenceAPPLE( m_name );
			
			if (ERRCHECK())
			{
				errorcode = gGL->glGetError();
				if (errorcode)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::Start(fence) saw %s error (%d) from glSetFence* name=%u", decodedStr, errorcode, m_name  );
				}
			}

			m_stopped = true;	// caller should not call Stop on a fence, it self-stops
		break;
	}
}

void	CGLMQuery::Stop( void )			// "stop counting"
{
	Assert(m_started);
	
	if ( m_stopped )
		return;

	GLenum errorcode = GL_NO_ERROR;

	switch(m_params.m_type)
	{
		case EOcclusion:
		{
			if (m_nullQuery)
			{
				// do nothing..
			}
			else
			{
				gGL->glEndQueryARB( GL_SAMPLES_PASSED_ARB );	// we are only putting the request-to-stop-counting into the cmd stream.
				if (ERRCHECK())
				{
					errorcode = gGL->glGetError();
					if (errorcode)
					{
						const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
						printf( "\nCGLMQuery::Stop(OQ) saw %s error (%d) from glEndQueryARB( GL_SAMPLES_PASSED_ARB ) name=%d", decodedStr, errorcode, m_name  );
					}
				}
			}
		}
		break;

		case EFence:
			// nop - you don't "end" a fence, you just test it and/or finish it out in Complete
		break;
	}
	
	m_stopped = true;
}

bool	CGLMQuery::IsDone( void )
{
	Assert(m_started);
	Assert(m_stopped);

	GLenum errorcode = GL_NO_ERROR;
	
	if(!m_done)		// you can ask more than once, but we only check until it comes back as done.
	{
		// on occlusion: glGetQueryObjectivARB - large cost on pre SLGU, cheap after
		// on fence: glTestFence* on the fence
		switch(m_params.m_type)
		{
			case EOcclusion:	// just test the fence that was set after the query begin
			{
				if (m_nullQuery)
				{
					// do almost nothing.. but claim work is complete
					m_done = true;
				}
				else
				{
					// prepare to pay a big price on drivers prior to 10.6.4+SLGU
					
					GLint available = 0;
					gGL->glGetQueryObjectivARB(m_name, GL_QUERY_RESULT_AVAILABLE_ARB, &available );
					if (ERRCHECK())
					{
						errorcode = gGL->glGetError();
						if (errorcode)
						{
							const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
							printf( "\nCGLMQuery::IsDone saw %s error (%d) from glGetQueryObjectivARB(a2) name=%d", decodedStr, errorcode, m_name  );
						}
					}
					
					m_done = (available != 0);					
				}
			}
			break;

			case EFence:
			{
				if ( m_name == 0 )
					m_done = true;
				else if (gGL->m_bHave_GL_NV_fence)
					m_done = gGL->glTestFenceNV( m_name ) != 0;
				else if (gGL->m_bHave_GL_APPLE_fence)
					m_done = gGL->glTestFenceAPPLE( m_name ) != 0;

				if (m_done)
				{
					if (gGL->m_bHave_GL_NV_fence)
						gGL->glFinishFenceNV( m_name );	// no set fence goes un-finished
					else if (gGL->m_bHave_GL_APPLE_fence)
						gGL->glFinishFenceAPPLE( m_name );	// no set fence goes un-finished

					if (ERRCHECK())
					{
						errorcode = gGL->glGetError();
						if (errorcode)
						{
							const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
							printf( "\nCGLMQuery::IsDone saw %s error (%d) from glFinishFence*(b) name=%u", decodedStr, errorcode, m_name  );
						}
					}
				}
			}
			break;
		}
	}
	
	return m_done;
}

void	CGLMQuery::Complete( uint *result )
{
	uint resultval = 0;
	GLint available = 0;
	
	GLenum errorcode = GL_NO_ERROR;
	
	// blocking call if not done
	Assert(m_started);
	Assert(m_stopped);

	switch(m_params.m_type)
	{
		case EOcclusion:
		{
			if (m_nullQuery)
			{
				m_done = true;
				resultval = 0;		// we did say "null queries..."
			}
			else
			{
				// accept that the query is going to drain pipe in 10.6.4 and prior.
				// check the error on the spot.
				gGL->glGetQueryObjectivARB(m_name, GL_QUERY_RESULT_AVAILABLE_ARB, &available );
				if (ERRCHECK() && (errorcode = gGL->glGetError()) != GL_NO_ERROR)
				{
					const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
					printf( "\nCGLMQuery::Complete saw %s error (%d) from glGetQueryObjectivARB GL_QUERY_RESULT_AVAILABLE_ARB name=%d", decodedStr, errorcode, m_name  );

					resultval = 0;
				}
				else
				{
					if (!available)
					{
						// this does happen with some very modest frequency.
						if (!m_ctx->Caps().m_hasPerfPackage1)
						{
							gGL->glFlush();		// ISTR some deadlock cases on pre-SLGU drivers if you didn't do this to kick the queue along..
						}
					}
					
					gGL->glGetQueryObjectuivARB( m_name, GL_QUERY_RESULT_ARB, &resultval);
					if (ERRCHECK())
					{
						errorcode = gGL->glGetError();
						if (errorcode)
						{
							const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
							printf( "\nCGLMQuery::Complete saw %s error (%d) from glGetQueryObjectivARB GL_QUERY_RESULT_ARB name=%d", decodedStr, errorcode, m_name  );

							resultval = 0;
						}
					}
				}
				m_done = true;
			}
		}
		break;

		case EFence:
		{
			if(!m_done)
			{
				if (gGL->m_bHave_GL_NV_fence)
					gGL->glFinishFenceNV( m_name );
				else if (gGL->m_bHave_GL_APPLE_fence)
					gGL->glFinishFenceAPPLE( m_name );
				
				if (ERRCHECK())
				{
					errorcode = gGL->glGetError();
					if (errorcode)
					{
						const char	*decodedStr = GLMDecode( eGL_ERROR, errorcode );
						printf( "\nCGLMQuery::Complete saw %s error (%d) from glFinishFence* (EFence) name=%u", decodedStr, errorcode, m_name  );
					}
				}
				
				m_done = true;					// for clarity or if they try to Complete twice
			}
		}
		break;
	}

	Assert( m_done );
	
	// reset state for re-use - i.e. you have to call Complete if you want to re-use the object
	m_started = m_stopped = m_done = false;
	
	if (result)	// caller may pass NULL if not interested in result, for example to clear a fence
	{
		*result = resultval;
	}
}



	// accessors for the started/stopped state
bool	CGLMQuery::IsStarted	( void )
{
	return m_started;
}

bool	CGLMQuery::IsStopped	( void )
{
	return m_stopped;
}
