//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#ifdef TOGLES
#include "togles/rendermechanism.h"
#else

#ifndef RENDERMECHANISM_H
#define RENDERMECHANISM_H

#if defined(DX_TO_GL_ABSTRACTION)

#undef PROTECTED_THINGS_ENABLE

#if defined(TOGL_OSX)

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include "tier0/basetypes.h"
#include "tier0/platform.h"

#include "togl/osx/glmdebug.h"
#include "togl/osx/glentrypoints.h"
#include "togl/osx/glmdisplay.h"
#include "togl/osx/glmdisplaydb.h"
#include "togl/osx/glmgrbasics.h"
#include "togl/osx/glmgrext.h"
#include "togl/osx/cglmbuffer.h"
#include "togl/osx/cglmtex.h"
#include "togl/osx/cglmfbo.h"
#include "togl/osx/cglmprogram.h"
#include "togl/osx/cglmquery.h"
#include "togl/osx/glmgr.h"
#include "togl/osx/dxabstract_types.h"
#include "togl/osx/dxabstract.h"

#else

#include "SDL_opengl.h"
#include "tier0/basetypes.h"
#include "tier0/platform.h"

#include "togl/linuxwin/glmdebug.h"
#include "togl/linuxwin/glbase.h"
#include "togl/linuxwin/glentrypoints.h"
#include "togl/linuxwin/glmdisplay.h"
#include "togl/linuxwin/glmdisplaydb.h"
#include "togl/linuxwin/glmgrbasics.h"
#include "togl/linuxwin/glmgrext.h"
#include "togl/linuxwin/cglmbuffer.h"
#include "togl/linuxwin/cglmtex.h"
#include "togl/linuxwin/cglmfbo.h"
#include "togl/linuxwin/cglmprogram.h"
#include "togl/linuxwin/cglmquery.h"
#include "togl/linuxwin/glmgr.h"
#include "togl/linuxwin/dxabstract_types.h"
#include "togl/linuxwin/dxabstract.h"

#endif // TOGL_OSX

#else
	//USE_ACTUAL_DX
	#ifdef WIN32
		#ifdef _X360
			#include "d3d9.h"
			#include "d3dx9.h"
		#elif defined( BYTEMATCH_RETAIL )
			#include <windows.h>
			#include "../../dx9sdk/include/d3d9.h"
			#include "tier0/dbg.h"
			#ifndef DXABSTRACT_BREAK_ON_ERROR
			#define DXABSTRACT_BREAK_ON_ERROR() Assert( 0 )
			#endif
			#define D3DXSHADER_DEBUG                    (1 << 0)
			#define D3DXSHADER_AVOID_FLOW_CONTROL       (1 << 9)
			class D3DXMATRIX : public D3DMATRIX
			{
			public:
				D3DXMATRIX operator*( const D3DXMATRIX &o ) const;
				operator FLOAT* ();
				float& operator()( int row, int column );
				const float& operator()( int row, int column ) const;
				bool operator != ( CONST D3DXMATRIX& src ) const;
			};
			typedef struct D3DXPLANE
			{
				float& operator[]( int i );
				bool operator==( const D3DXPLANE &o );
				bool operator!=( const D3DXPLANE &o );
				operator float*();
				operator const float*() const;
				float a, b, c, d;
			} D3DXPLANE;
			class D3DXVECTOR2
			{
			public:
				operator FLOAT* ();
				operator CONST FLOAT* () const;
				float x, y;
			};
			class D3DXVECTOR3 : public D3DVECTOR
			{
			public:
				D3DXVECTOR3() {}
				D3DXVECTOR3( float a, float b, float c );
				operator FLOAT* ();
				operator CONST FLOAT* () const;
			};
			typedef enum _D3DXINCLUDE_TYPE
			{
				D3DXINC_LOCAL,
				D3DXINC_SYSTEM,
				D3DXINC_FORCE_DWORD = 0x7fffffff
			} D3DXINCLUDE_TYPE;
			class D3DXVECTOR4
			{
			public:
				D3DXVECTOR4() {}
				D3DXVECTOR4( float a, float b, float c, float d );
				float x, y, z, w;
			};
			typedef struct _D3DXMACRO
			{
				LPCSTR Name;
				LPCSTR Definition;
			} D3DXMACRO, *LPD3DXMACRO;
			struct ID3DXInclude
			{
				virtual HRESULT STDMETHODCALLTYPE Open( D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes ) = 0;
				virtual HRESULT STDMETHODCALLTYPE Close( LPCVOID pData ) = 0;
			};
			typedef ID3DXInclude* LPD3DXINCLUDE;
			struct ID3DXBuffer : public IUnknown
			{
				void* GetBufferPointer();
				DWORD GetBufferSize();
			};
			typedef ID3DXBuffer* LPD3DXBUFFER;
			class ID3DXConstantTable : public IUnknown
			{
			};
			typedef ID3DXConstantTable* LPD3DXCONSTANTTABLE;
			inline DWORD D3DXGetShaderVersion( CONST DWORD *pFunction ) { return pFunction ? *pFunction : 0; }
			#include "togl/d3dx_impl.h"
		#else
			#include <windows.h>
			#include "../../dx9sdk/include/d3d9.h"
			#include "../../dx9sdk/include/d3dx9.h"
		#endif
		typedef HWND VD3DHWND;
	#endif

	#define	GLMPRINTF(args)	
	#define	GLMPRINTSTR(args)
	#define	GLMPRINTTEXT(args)
#endif // defined(DX_TO_GL_ABSTRACTION)

#endif // RENDERMECHANISM_H

#endif
