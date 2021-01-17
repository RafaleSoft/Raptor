/***************************************************************************/
/*                                                                         */
/*  RaptorConfig.cpp                                                       */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2019 by                                                 */
/*  Fabrice FERRAND.                                                       */
/*                                                                         */
/*  This file is part of the Raptor project, and may only be used,         */
/*  modified, and distributed under the terms of the Raptor project        */
/*  license, LICENSE.  By continuing to use, modify, or distribute         */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "RaptorConfig.h"
#endif

#include <math.h>


RAPTOR_NAMESPACE

static const size_t DEFAULT_POLYGONS = 1024;
static const size_t DEFAULT_VERTICES = 16 * 1024;
static const size_t DEFAULT_TEXELS = 1024 * 1024;
static const size_t DEFAULT_UNIFORMS = 128 * 1024;
static const size_t DEFAULT_GARBAGE = 1024 * 1024;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorConfig::CRaptorConfig():
    m_bRelocation(false),
    m_uiPolygons(DEFAULT_POLYGONS),
    m_uiVertices(DEFAULT_VERTICES),
    m_fSizeFactor(1.0f),
    m_bAutoDestroy(true),
    m_uiTexels(DEFAULT_TEXELS),
	m_uiUniforms(DEFAULT_UNIFORMS),
	m_uiGarbageSize(DEFAULT_GARBAGE),
	m_logFile("Raptor.log"),
	m_bCompute(false),
	m_bVulkan(false)
{

}

CRaptorConfig::~CRaptorConfig()
{

}

void CRaptorConfig::setFilterSizeFactor(float factor)
{
#ifdef GL_ARB_texture_non_power_of_two
    m_fSizeFactor = factor;
#else
    float size = floor(0.5f + (log(factor) / log(2.0f)));
    if (size >= 1)
        m_fSizeFactor = pow(2,size);
#endif
}

bool CRaptorConfig::checkConfig(void)
{
	bool check = true;

	if (m_uiPolygons < DEFAULT_POLYGONS)
	{
		m_uiPolygons = DEFAULT_POLYGONS;
		check = false;
	}
	if (m_uiVertices < DEFAULT_VERTICES)
	{
		m_uiVertices = DEFAULT_VERTICES;
		check = false;
	}
	if (m_uiTexels < DEFAULT_TEXELS)
	{
		m_uiTexels = DEFAULT_TEXELS;
		check = false;
	}
	if (m_uiUniforms < DEFAULT_UNIFORMS)
	{
		m_uiUniforms = DEFAULT_UNIFORMS;
		check = false;
	}
	if (m_uiGarbageSize < DEFAULT_GARBAGE)
	{
		m_uiGarbageSize = DEFAULT_GARBAGE;
		check = false;
	}

	return check;
}