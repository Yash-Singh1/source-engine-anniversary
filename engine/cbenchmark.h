//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//
#ifndef CBENCHMARK_H
#define CBENCHMARK_H

#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CCommand;


#define MAX_BUFFER_SIZE 2048

//-----------------------------------------------------------------------------
// Purpose: Holds benchmark data & state
//-----------------------------------------------------------------------------
class CBenchmarkResults
{
public:
	CBenchmarkResults();

	bool IsBenchmarkRunning();
	void StartBenchmark( const CCommand &args );
	void StopBenchmark();
	void SetResultsFilename( const char *pFilename );
	void Upload();
#ifndef BYTEMATCH_RETAIL
	void Frame();
#endif
#line 38

private:
	bool m_bIsTestRunning;
	char m_szFilename[256];

#ifdef BYTEMATCH_RETAIL
#line 43
	int m_flStartTime;
	int m_iStartFrame;
#else
#line 43
	float m_flStartTime;
	int m_iStartFrame;

	float m_flNextSecondTime;
	int m_iNextSecondFrame;

	CUtlVector<int> m_FPSInfo;
#endif
#line 50
};

inline CBenchmarkResults *GetBenchResultsMgr()
{
	extern CBenchmarkResults g_BenchmarkResults;
	return &g_BenchmarkResults;
}


#endif // CBENCHMARK_H
