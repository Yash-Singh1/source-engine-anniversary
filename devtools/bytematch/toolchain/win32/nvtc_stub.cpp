// Stand-ins for NVIDIA's nvtc.lib (S3TC encoder), which bitmap and vguimatsurface
// link and which isn't available. Built by scripts/waifulib/retail_win32.py
// against public/nvtc.h; the encoder entry points write nothing.
#include <windows.h>
#include "nvtc.h"

void S3TCsetAlphaReference( int nRef )
{
}

unsigned int S3TCgetEncodeSize( DDSURFACEDESC *lpDesc, unsigned int dwEncodeType )
{
	return 0;
}

void S3TCencode( DDSURFACEDESC *lpSrc, PALETTEENTRY *lpPal, DDSURFACEDESC *lpDest, void *lpDestBuf, unsigned int dwEncodeType, float *weight )
{
}

int S3TCencodeEx( DDSURFACEDESC *lpSrc, PALETTEENTRY *lpPal, DDSURFACEDESC *lpDest, void *lpDestBuf, unsigned int dwEncodeType,
	float *weight, LP_S3TC_PROGRESS_CALLBACK lpS3TCProgressProc, LPVOID lpArg1, LPVOID lpArg2 )
{
	return ERROR_ABORTED;
}

unsigned int S3TCgetDecodeSize( DDSURFACEDESC *lpDesc )
{
	return 0;
}

void S3TCdecode( DDSURFACEDESC *lpSrc, DDSURFACEDESC *lpDest, void *lpDestBuf )
{
}
