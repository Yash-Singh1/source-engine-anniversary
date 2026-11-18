//========= Copyright Valve Corporation, All rights reserved. ============//
//
// cglmbuffer.cpp
//
//===============================================================================

#include "togl/rendermechanism.h"
#include "tier0/icommandline.h"

// memdbgon -must- be the last include file in a .cpp file.
#include "tier0/memdbgon.h"

// http://www.opengl.org/registry/specs/ARB/vertex_buffer_object.txt
// http://www.opengl.org/registry/specs/ARB/pixel_buffer_object.txt

// gl_bufmode: zero means we mark all vertex/index buffers static

// non zero means buffers are initially marked static..
// ->but can shift to dynamic upon first 'discard' (orphaning)

ConVar	gl_bufmode( "gl_bufmode", "1" );
ConVar	gl_buffer_alignment_quantum( "gl_buffer_alignment_quantum", "32" );
ConVar	gl_buffer_alignment_quantum_slgu( "gl_buffer_alignment_quantum_slgu", "2" );

CGLMBuffer::CGLMBuffer( GLMContext *ctx, EGLMBufferType type, uint size, uint options )
{
	m_ctx = ctx;
	m_type = type;
	switch(m_type)
	{
		case	kGLMVertexBuffer:		m_buffGLTarget = GL_ARRAY_BUFFER_ARB; break;
		case	kGLMIndexBuffer:		m_buffGLTarget = GL_ELEMENT_ARRAY_BUFFER_ARB; break;
		case	kGLMUniformBuffer:		m_buffGLTarget = GL_UNIFORM_BUFFER_EXT; break;
		case	kGLMPixelBuffer:		m_buffGLTarget = GL_PIXEL_UNPACK_BUFFER_ARB; break;
		
		default: Assert(!"Unknown buffer type" );
	}
	m_size = size;
	m_bound = false;
	m_mapped = false;
	m_lastMappedAddress = NULL;

	m_enableAsyncMap = false;
	m_enableExplicitFlush = false;
	m_dirtyMinOffset = m_dirtyMaxOffset = 0;	// adjust/grow on lock, clear on unlock

	m_revision = rand();

	// make a decision about pseudo mode
	// this is just a signal to the CGLMBuffer that it should use a malloc'd buffer instead of a VBO
	if (m_type==kGLMIndexBuffer)
	{
		m_pseudo = CommandLine()->FindParm("-gl_enable_pseudobufs") != 0;
	}
	else
	{
		m_pseudo = false;
	}

	if (m_pseudo)
	{
		m_name = 0;
		m_pseudoBuf = (char*)malloc( size );
		
		m_ctx->BindBufferToCtx( m_type, NULL );		// exit with no buffer bound
	}
	else
	{
		gGL->glGenBuffersARB( 1, &m_name );
		GLMCheckError();
		
		m_ctx->BindBufferToCtx( m_type, this );	// causes glBindBufferARB

		// buffers start out static, but if they get orphaned and gl_bufmode is non zero,
		// then they will get flipped to dynamic.
		
		GLenum hint = GL_STATIC_DRAW_ARB;
		switch(m_type)
		{
			case	kGLMVertexBuffer:		hint = (options & GLMBufferOptionDynamic) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB; break;
			case	kGLMIndexBuffer:		hint = (options & GLMBufferOptionDynamic) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB; break;
			case	kGLMUniformBuffer:		hint = GL_DYNAMIC_DRAW_ARB; break;
			case	kGLMPixelBuffer:		hint = (options & GLMBufferOptionDynamic) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB; break;

			default: Assert(!"Unknown buffer type" );
		}

		gGL->glBufferDataARB( m_buffGLTarget, m_size, NULL, hint );	// may ultimately need more hints to set the usage correctly (esp for streaming)
		
		this->SetModes( false, true, true );
		
		m_ctx->BindBufferToCtx( m_type, NULL );	// unbind me
	}
}

CGLMBuffer::~CGLMBuffer( )
{
	if (m_pseudo)
	{
		free( m_pseudoBuf );
		m_pseudoBuf = NULL;
	}
	else
	{
		gGL->glDeleteBuffersARB( 1, &m_name );
		GLMCheckError();
	}
	
	m_ctx = NULL;
	m_name = 0;
	m_bound = 0;
	
	m_lastMappedAddress = NULL;
}

void	CGLMBuffer::SetModes		( bool asyncMap, bool explicitFlush, bool force )
{
	// assumes buffer is bound. called by constructor and by Lock.

	if (m_pseudo)
	{
		// ignore it...
	}
	else
	{
		if (force || (m_enableAsyncMap != asyncMap) )
		{
			// note the sense of the parameter, it's TRUE if you *want* serialization, so for async you turn it to false.
			if ( (gGL->m_bHave_GL_APPLE_flush_buffer_range) && (!gGL->m_bHave_GL_ARB_map_buffer_range) )
			{
				gGL->glBufferParameteriAPPLE(this->m_buffGLTarget, GL_BUFFER_SERIALIZED_MODIFY_APPLE, asyncMap==false );
			}
			m_enableAsyncMap = asyncMap;
		}

		if (force || (m_enableExplicitFlush != explicitFlush) )
		{
			// note the sense of the parameter, it's TRUE if you *want* auto-flush-on-unmap, so for explicit-flush, you turn it to false.
			if ( (gGL->m_bHave_GL_APPLE_flush_buffer_range) && (!gGL->m_bHave_GL_ARB_map_buffer_range) )
			{
				gGL->glBufferParameteriAPPLE(this->m_buffGLTarget, GL_BUFFER_FLUSHING_UNMAP_APPLE, explicitFlush==false );
			}
			m_enableExplicitFlush = explicitFlush;
		}
	}
}

void	CGLMBuffer::FlushRange	( uint offset, uint size )
{
	if (m_pseudo)
	{
		// nothing to do
	}
	else
	{
		// assumes buffer is bound.
		if ( (gGL->m_bHave_GL_APPLE_flush_buffer_range) && (!gGL->m_bHave_GL_ARB_map_buffer_range) )
		{
			gGL->glFlushMappedBufferRangeAPPLE(this->m_buffGLTarget, (GLintptr)offset, (GLsizeiptr)size);
		}
		else if (gGL->m_bHave_GL_ARB_map_buffer_range)
		{
			gGL->glFlushMappedBufferRange(this->m_buffGLTarget, (GLintptr)( offset - m_dirtyMinOffset ), (GLsizeiptr)size);
		}
	}
}

void	CGLMBuffer::Lock( GLMBuffLockParams *params, char **addressOut )
{
	char	*resultPtr = NULL;
	
	GLMCheckError();

	Assert( !m_mapped);
	
	if (params->m_offset >= m_size)
		Debugger();

	if (params->m_offset + params->m_size > m_size)
		Debugger();
	
	// bind (yes, even for pseudo - this binds name 0)
	m_ctx->BindBufferToCtx( this->m_type, this );

	if (m_pseudo)
	{
		// discard is a no-op

		// async map modes are a no-op
		
		// latch last mapped address (silly..)
		m_lastMappedAddress = (float*)m_pseudoBuf;

		// calc lock address
		resultPtr = m_pseudoBuf + params->m_offset;
	}
	else
	{
		// perform discard if requested
		if (params->m_discard)
		{
			// observe gl_bufmode on any orphan event.
			// if orphaned and bufmode is nonzero, flip it to dynamic.
			if (!gGL->m_bHave_GL_ARB_map_buffer_range)
			{
				GLenum hint = gl_bufmode.GetInt() ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB;
				gGL->glBufferDataARB( m_buffGLTarget, m_size, NULL, hint );
			}

			m_lastMappedAddress = NULL;

			m_revision++;	// revision grows on orphan event
		}

		// adjust async map option appropriately, leave explicit flush unchanged
		SetModes( params->m_nonblocking, m_enableExplicitFlush );

		// map
		char	*mapPtr;
		if (gGL->m_bHave_GL_ARB_map_buffer_range)
		{
			// translate options into glMapBufferRange access bits
			mapPtr = (char*)gGL->glMapBufferRange( m_buffGLTarget, params->m_offset, params->m_size,
				(m_enableAsyncMap ? GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT : GL_MAP_WRITE_BIT) | (params->m_discard ? GL_MAP_INVALIDATE_BUFFER_BIT : 0) | (m_enableExplicitFlush ? GL_MAP_FLUSH_EXPLICIT_BIT : 0) );
		}
		else
		{
			mapPtr = (char*)gGL->glMapBufferARB( this->m_buffGLTarget, GL_WRITE_ONLY_ARB );
		}

		if (!mapPtr)
		{
			Debugger();
		}

		m_lastMappedAddress = (float*)mapPtr;

		// calculate offset location
		resultPtr = mapPtr;
		if (!gGL->m_bHave_GL_ARB_map_buffer_range)
		{
			resultPtr += params->m_offset;
		}

		// set range
		m_dirtyMinOffset = params->m_offset;
		m_dirtyMaxOffset = params->m_offset + params->m_size;
	}
	
	m_mapped = true;

	*addressOut = resultPtr;
}

void	CGLMBuffer::Unlock( void )
{
	if (!m_pseudo)
	{
		// bind it
		m_ctx->BindBufferToCtx( this->m_type, this );

		// time to do explicit flush
		if (m_enableExplicitFlush)
		{
			this->FlushRange( m_dirtyMinOffset, m_dirtyMaxOffset - m_dirtyMinOffset );
		}
		
		// clear dirty range no matter what
		m_dirtyMinOffset = m_dirtyMaxOffset = 0;	// adjust/grow on lock, clear on unlock
		
		gGL->glUnmapBuffer( this->m_buffGLTarget );
	}

	m_mapped = false;
}
