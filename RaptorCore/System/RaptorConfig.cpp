// RaptorConfig.cpp: implementation of the CRaptorConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "RaptorConfig.h"
#endif

#include <math.h>


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorConfig::CRaptorConfig():
    m_bRelocation(false),
    m_uiPolygons(1000),
    m_uiVertices(4000),
    m_fSizeFactor(1.0f),
    m_bAutoDestroy(true),
    m_uiTexels(65536),
	m_uiUniforms(1024),
	m_uiGarbageSize(1024*1024),
	m_logFile("Raptor.log"),
	m_bCompute(false),
	m_bVulkan(false)
{

}

CRaptorConfig::~CRaptorConfig()
{

}

bool CRaptorConfig::setFilterSizeFactor(float factor)
{
#ifdef GL_ARB_texture_non_power_of_two
    m_fSizeFactor = factor;
#else
    float size = floor(0.5f + (log(factor) / log(2.0f)));
    if (size >= 1)
        m_fSizeFactor = pow(2,size);
#endif

    return true;
}

