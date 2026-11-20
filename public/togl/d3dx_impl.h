//========= Copyright Valve Corporation, All rights reserved. ============//
//
// d3dx_impl.h
//
// The D3DX helpers shaderapidx9 uses on top of togl.
//
//==================================================================================================

#ifndef D3DX_IMPL_H
#define D3DX_IMPL_H

#pragma once

#include "tier1/utlvector.h"
#include "mathlib/vmatrix.h"
#include "mathlib/vector4d.h"

// ------------------------------------------------------------------------------------------------------------------------------ //
// D3DXMATRIX operators
// ------------------------------------------------------------------------------------------------------------------------------ //

inline D3DXMATRIX* D3DXMatrixMultiply( D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2 );

inline D3DXMATRIX D3DXMATRIX::operator*( const D3DXMATRIX &o ) const
{
	D3DXMATRIX result;
	
	D3DXMatrixMultiply( &result, this, &o );	// this = lhs    o = rhs    result = this * o

	return result;
}

inline D3DXMATRIX::operator FLOAT* ()
{
	return (float*)this;
}

inline float& D3DXMATRIX::operator()( int row, int column )
{
	return m[row][column];
}

inline const float& D3DXMATRIX::operator()( int row, int column ) const
{
	return m[row][column];
}
inline bool D3DXMATRIX::operator != ( CONST D3DXMATRIX& src ) const { return V_memcmp( (void*)this, (void*)&src, sizeof(this) ) != 0; }
// ------------------------------------------------------------------------------------------------------------------------------ //
// D3DXPLANE operators
// ------------------------------------------------------------------------------------------------------------------------------ //

inline float& D3DXPLANE::operator[]( int i )
{
	return ((float*)this)[i];
}

inline bool D3DXPLANE::operator==( const D3DXPLANE &o )
{
	return a == o.a && b == o.b && c == o.c && d == o.d;
}

inline bool D3DXPLANE::operator!=( const D3DXPLANE &o )
{
	return !( *this == o );
}

inline D3DXPLANE::operator float*()
{
	return (float*)this;
}

inline D3DXPLANE::operator const float*() const
{
	return (const float*)this;
}

// ------------------------------------------------------------------------------------------------------------------------------ //
// D3DXVECTOR operators
// ------------------------------------------------------------------------------------------------------------------------------ //

inline D3DXVECTOR2::operator FLOAT* ()
{
	return (float*)this;
}

inline D3DXVECTOR2::operator CONST FLOAT* () const
{
	return (const float*)this;
}

inline D3DXVECTOR3::D3DXVECTOR3( float a, float b, float c )
{
	x = a;
	y = b;
	z = c;
}

inline D3DXVECTOR3::operator FLOAT* ()
{
	return (float*)this;
}

inline D3DXVECTOR3::operator CONST FLOAT* () const
{
	return (const float*)this;
}

inline D3DXVECTOR4::D3DXVECTOR4( float a, float b, float c, float d )
{
	x = a;
	y = b;
	z = c;
	w = d;
}

// ------------------------------------------------------------------------------------------------------------------------------ //

inline void* ID3DXBuffer::GetBufferPointer()
{
	DXABSTRACT_BREAK_ON_ERROR();
	return NULL;
}

inline DWORD ID3DXBuffer::GetBufferSize()
{
	DXABSTRACT_BREAK_ON_ERROR();
	return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------ //
// Matrix stack
// ------------------------------------------------------------------------------------------------------------------------------ //

inline void D3DXMatrixIdentity( D3DXMATRIX *mat );

struct ID3DXMatrixStack
{
	int						m_refcount[2];
	CUtlVector<D3DMATRIX>	m_stack;
	int						m_stackTop;	// top of stack is at the highest index, this is that index.  push increases, pop decreases.

	ID3DXMatrixStack( void )
	{
		m_refcount[0] = 1;
		m_refcount[1] = 0;
	}

	void AddRef( int which=0, char *comment = NULL )
	{
		Assert( which >= 0 );
		Assert( which < 2 );
		m_refcount[which]++;
	}

	ULONG Release( int which=0, char *comment = NULL )
	{
		Assert( which >= 0 );
		Assert( which < 2 );

		m_refcount[which]--;
		if ( (!m_refcount[0]) && (!m_refcount[1]) )
		{
			delete this;
			return 0;
		}
		return m_refcount[0];
	}

	FORCEINLINE HRESULT	Create( void )
	{
		m_stack.EnsureCapacity( 16 );	// 1KB ish
		m_stack.AddToTail();
		m_stackTop = 0;				// top of stack is at index 0 currently

		LoadIdentity();

		return S_OK;
	}

	D3DXMATRIX* GetTop()
	{
		return (D3DXMATRIX*)&m_stack[ m_stackTop ];
	}

	void Push()
	{
		D3DMATRIX temp = m_stack[ m_stackTop ];
		m_stack.AddToTail( temp );
		m_stackTop ++;
	}

	void Pop()
	{
		int elem = m_stackTop--;
		m_stack.Remove( elem );
	}

	void LoadIdentity()
	{
		D3DXMATRIX *mat = GetTop();

		D3DXMatrixIdentity( mat );
	}

	void LoadMatrix( const D3DXMATRIX *pMat )
	{
		*(GetTop()) = *pMat;
	}

	// http://msdn.microsoft.com/en-us/library/bb174057(VS.85).aspx
	//	This method right-multiplies the given matrix to the current matrix
	//	(transformation is about the current world origin).
	void MultMatrix( const D3DXMATRIX *pMat )
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}

	// http://msdn.microsoft.com/en-us/library/bb174058(VS.85).aspx
	//	This method left-multiplies the given matrix to the current matrix
	//	(transformation is about the local origin of the object).
	void MultMatrixLocal( const D3DXMATRIX *pMat )
	{
		DXABSTRACT_BREAK_ON_ERROR();
	}

	// Left multiply the current matrix with the computed scale matrix
	// (scaling is about the local origin of the object)
	HRESULT ScaleLocal( FLOAT x, FLOAT y, FLOAT z )
	{
		DXABSTRACT_BREAK_ON_ERROR();
		return S_OK;
	}

	// Left multiply the current matrix with the computed rotation
	// matrix, counterclockwise about the given axis with the given angle.
	// (rotation is about the local origin of the object)
	HRESULT RotateAxisLocal( CONST D3DXVECTOR3* pV, FLOAT Angle )
	{
		DXABSTRACT_BREAK_ON_ERROR();
		return S_OK;
	}

	// Left multiply the current matrix with the computed translation
	// matrix. (transformation is about the local origin of the object)
	HRESULT TranslateLocal( FLOAT x, FLOAT y, FLOAT z )
	{
		DXABSTRACT_BREAK_ON_ERROR();
		return S_OK;
	}
};
typedef ID3DXMatrixStack* LPD3DXMATRIXSTACK;

inline HRESULT D3DXCreateMatrixStack( DWORD Flags, LPD3DXMATRIXSTACK* ppStack )
{
	*ppStack = new ID3DXMatrixStack;

	(*ppStack)->Create();

	return S_OK;
}

// ------------------------------------------------------------------------------------------------------------------------------ //
// D3DX math
// ------------------------------------------------------------------------------------------------------------------------------ //

inline const char* D3DXGetPixelShaderProfile( IDirect3DDevice9 *pDevice )
{
	DXABSTRACT_BREAK_ON_ERROR();
	return "";
}

inline D3DXMATRIX* D3DXMatrixMultiply( D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2 )
{
	D3DXMATRIX temp;
	
	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++)
		{
			temp.m[i][j]	=	(pM1->m[ i ][ 0 ] * pM2->m[ 0 ][ j ])
							+	(pM1->m[ i ][ 1 ] * pM2->m[ 1 ][ j ])
							+	(pM1->m[ i ][ 2 ] * pM2->m[ 2 ][ j ])
							+	(pM1->m[ i ][ 3 ] * pM2->m[ 3 ][ j ]);
		}
	}
	*pOut = temp;
	return pOut;
}

// Transform a 3D vector by a given matrix, projecting the result back into w = 1
// http://msdn.microsoft.com/en-us/library/ee417622(VS.85).aspx
inline D3DXVECTOR3* D3DXVec3TransformCoord( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM )
{
	D3DXVECTOR3 vOut;
	
	vOut.x = vOut.y = vOut.z = 0.0f;
	float norm = (pM->m[0][3] * pV->x) + (pM->m[1][3] * pV->y) + (pM->m[2][3] *pV->z) + pM->m[3][3];
	if ( norm )
	{
		vOut.x = (pM->m[0][0] * pV->x + pM->m[1][0] * pV->y + pM->m[2][0] * pV->z + pM->m[3][0]) / norm;
		vOut.y = (pM->m[0][1] * pV->x + pM->m[1][1] * pV->y + pM->m[2][1] * pV->z + pM->m[3][1]) / norm;
		vOut.z = (pM->m[0][2] * pV->x + pM->m[1][2] * pV->y + pM->m[2][2] * pV->z + pM->m[3][2]) / norm;
	}
	
	*pOut = vOut;

	return pOut;
}

inline void D3DXMatrixIdentity( D3DXMATRIX *mat )
{
	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++)
		{
			mat->m[i][j] = (i==j) ? 1.0f : 0.0f;	// 1's on the diagonal.
		}
	}
}

inline D3DXVECTOR3* D3DXVec3Subtract( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
	pOut->x = pV1->x - pV2->x;
	pOut->y = pV1->y - pV2->y;
	pOut->z = pV1->z - pV2->z;
	return pOut;
}

inline D3DXVECTOR3* D3DXVec3Cross( D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
	D3DXVECTOR3 v;

	v.x = pV1->y * pV2->z - pV1->z * pV2->y;
	v.y = pV1->z * pV2->x - pV1->x * pV2->z;
	v.z = pV1->x * pV2->y - pV1->y * pV2->x;

	*pOut = v;
	return pOut;
}

inline FLOAT D3DXVec3Dot( CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2 )
{
	return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

inline D3DXMATRIX* D3DXMatrixTranslation( D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z )
{
	D3DXMatrixIdentity( pOut );
	pOut->m[3][0] = x;
	pOut->m[3][1] = y;
	pOut->m[3][2] = z;
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixInverse( D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM )
{
	Assert( sizeof( D3DXMATRIX ) == (16 * sizeof(float) ) );
	Assert( sizeof( VMatrix ) == (16 * sizeof(float) ) );
	Assert( pDeterminant == NULL );	// homey don't play that
	
	VMatrix *origM = (VMatrix*)pM;
	VMatrix *destM = (VMatrix*)pOut;
	
	bool success = MatrixInverseGeneral( *origM, *destM );
	Assert( success );
	(void)success;
	return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranspose( D3DXMATRIX *pOut, CONST D3DXMATRIX *pM )
{
	if (pOut != pM)
	{
		for( int i=0; i<4; i++)
		{
			for( int j=0; j<4; j++)
			{
				pOut->m[i][j] = pM->m[j][i];
			}
		}
	}
	else
	{
		D3DXMATRIX temp = *pM;
		D3DXMatrixTranspose( pOut, &temp );
	}

	return NULL;
}

inline D3DXPLANE* D3DXPlaneNormalize( D3DXPLANE *pOut, CONST D3DXPLANE *pP )
{
	// not very different from normalizing a vector.
	// figure out the square root of the sum-of-squares of the x,y,z components
	// make sure that's non zero
	// then divide all four components by that value
	// or return some dummy plane like 0,0,1,0 if it fails
	
	float	len = sqrt( (pP->a * pP->a) + (pP->b * pP->b) + (pP->c * pP->c) );
	if (len > 1e-10)	//FIXME need a real epsilon here ?
	{
		pOut->a = pP->a / len;		pOut->b = pP->b / len;		pOut->c = pP->c / len;		pOut->d = pP->d / len;
	}
	else
	{
		pOut->a = 0.0f;				pOut->b = 0.0f;				pOut->c = 1.0f;				pOut->d = 0.0f;
	}
	return pOut;
}

inline D3DXVECTOR4* D3DXVec4Transform( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, CONST D3DXMATRIX *pM )
{
	VMatrix *mat = (VMatrix*)pM;
	Vector4D *vIn = (Vector4D*)pV;
	Vector4D *vOut = (Vector4D*)pOut;

	Vector4DMultiplyTranspose( *mat, *vIn, *vOut );

	return pOut;
}

inline D3DXVECTOR4* D3DXVec4Normalize( D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV )
{
	Vector4D *vIn = (Vector4D*) pV;
	Vector4D *vOut = (Vector4D*) pOut;

	*vOut = *vIn;
	Vector4DNormalize( *vOut );
	
	return pOut;
}

// Build an ortho projection matrix. (right-handed)
inline D3DXMATRIX* D3DXMatrixOrthoOffCenterRH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf )
{
	DXABSTRACT_BREAK_ON_ERROR();
	return NULL;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveRH( D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf )
{
	DXABSTRACT_BREAK_ON_ERROR();
	return NULL;
}

inline D3DXMATRIX* D3DXMatrixPerspectiveOffCenterRH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf )
{
	DXABSTRACT_BREAK_ON_ERROR();
	return NULL;
}

// Transform a plane by a matrix.  The vector (a,b,c) must be normal.
// M should be the inverse transpose of the transformation desired.
inline D3DXPLANE* D3DXPlaneTransform( D3DXPLANE *pOut, CONST D3DXPLANE *pP, CONST D3DXMATRIX *pM )
{
	float *out = &pOut->a;

	// dot dot dot
	for( int x=0; x<4; x++ )
	{
		out[x] =	(pM->m[0][x] * pP->a)
				+	(pM->m[1][x] * pP->b)
				+	(pM->m[2][x] * pP->c)
				+	(pM->m[3][x] * pP->d);
	}
	
	return pOut;
}

inline HRESULT D3DXCompileShader(
		LPCSTR                          pSrcData,
		UINT                            SrcDataLen,
		CONST D3DXMACRO*                pDefines,
		LPD3DXINCLUDE                   pInclude,
		LPCSTR                          pFunctionName,
		LPCSTR                          pProfile,
		DWORD                           Flags,
		LPD3DXBUFFER*                   ppShader,
		LPD3DXBUFFER*                   ppErrorMsgs,
		LPD3DXCONSTANTTABLE*            ppConstantTable )
{
	DXABSTRACT_BREAK_ON_ERROR();	// is anyone calling this ?
#ifdef _WIN32
	return E_FAIL;
#endif
}

#endif // D3DX_IMPL_H
