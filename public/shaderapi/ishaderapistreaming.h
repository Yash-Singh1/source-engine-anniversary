//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Texture streaming and staging hooks the material system needs from
//          the shader API. Not part of the retail IShaderAPI (ShaderApi030);
//          this interface only exists for our texture streaming code.
//
//===========================================================================//

#ifndef ISHADERAPISTREAMING_H
#define ISHADERAPISTREAMING_H

#ifdef _WIN32
#pragma once
#endif

#include "shaderapi/ishaderapi.h"

#define SHADERAPI_STREAMING_INTERFACE_VERSION	"ShaderApiStreaming001"

abstract_class IShaderAPIStreaming
{
public:
	// Allows copying a render target to another texture by specifying them both.
	virtual void CopyRenderTargetToScratchTexture( ShaderAPITextureHandle_t srcRt, ShaderAPITextureHandle_t dstTex, Rect_t *pSrcRect = NULL, Rect_t *pDstRect = NULL ) = 0;

	// Allows locking and unlocking of very specific surface types.
	virtual void LockRect( void** pOutBits, int* pOutPitch, ShaderAPITextureHandle_t texHandle, int mipmap, int x, int y, int w, int h, bool bWrite, bool bRead ) = 0;
	virtual void UnlockRect( ShaderAPITextureHandle_t texHandle, int mipmap ) = 0;

	// Set the finest mipmap that can be used for the texture which is currently being modified.
	virtual void TexLodClamp( int finest ) = 0;

	// Set the Lod Bias for the texture which is currently being modified.
	virtual void TexLodBias( float bias ) = 0;

	virtual void CopyTextureToTexture( ShaderAPITextureHandle_t srcTex, ShaderAPITextureHandle_t dstTex ) = 0;
};

#endif // ISHADERAPISTREAMING_H
