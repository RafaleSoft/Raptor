/***************************************************************************/
/*                                                                         */
/*  RenderingProperties.cpp                                                */
/*                                                                         */
/*    Raptor OpenGL & Vulkan realtime 3D Engine SDK.                       */
/*                                                                         */
/*  Copyright 1998-2021 by                                                 */
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

#ifndef __RAPTOR_GLEXT_H__
	#include "System/Glext.h"
#endif

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif

#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "IRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IRenderingProperties::IRenderingProperties()
	:m_bTexturing(IGNORE_PROPERTY/*DISABLE*/),m_bLighting(IGNORE_PROPERTY/*DISABLE*/),
	m_bWireframe(IGNORE_PROPERTY/*DISABLE*/),m_bCullFace(ENABLE),
	m_bDepthTest(ENABLE),m_bBlend(IGNORE_PROPERTY),m_bStencilTest(IGNORE_PROPERTY),
	m_bMultisample(IGNORE_PROPERTY/*DISABLE*/),m_bClampFloats(IGNORE_PROPERTY),
	m_clear(0),m_pPrevious(NULL)
{

}

IRenderingProperties::IRenderingProperties(const IRenderingProperties &properties)
{
    if (&properties != this)
    {
        m_clear = properties.m_clear;
        m_bWireframe = properties.m_bWireframe;
        m_bTexturing = properties.m_bTexturing;
        m_bLighting = properties.m_bLighting;
        m_bDepthTest = properties.m_bDepthTest;
        m_bCullFace = properties.m_bCullFace;
        m_bBlend = properties.m_bBlend;
        m_bStencilTest = properties.m_bStencilTest;
		m_bMultisample = properties.m_bMultisample;
		m_bClampFloats = properties.m_bClampFloats;
        m_pPrevious = NULL;
    }
}

IRenderingProperties& IRenderingProperties::operator=(const IRenderingProperties &properties)
{
    if (&properties != this)
    {
        m_clear = properties.m_clear;
        m_bWireframe = properties.m_bWireframe;
        m_bTexturing = properties.m_bTexturing;
        m_bLighting = properties.m_bLighting;
        m_bDepthTest = properties.m_bDepthTest;
        m_bCullFace = properties.m_bCullFace;
        m_bBlend = properties.m_bBlend;
        m_bStencilTest = properties.m_bStencilTest;
		m_bMultisample = properties.m_bMultisample;
		m_bClampFloats = properties.m_bClampFloats;
    }

    return *this;
}

IRenderingProperties::~IRenderingProperties()
{
}

bool IRenderingProperties::exportObject(CRaptorIO& o)
{
    return false;
}

bool IRenderingProperties::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

    string data = io.getValueName();
    while (io.hasMoreValues())
    {
        //  TODO : converting an enum to int& is dangerous.
        //  Keep this only while testing IO
        if (data == "texturing")
            io >> (int&)m_bTexturing;
        else if (data == "lighting")
            io >> (int&)m_bLighting;
        else if (data == "wireframe")
            io >> (int&)m_bWireframe;
        else if (data == "cullface")
            io >> (int&)m_bCullFace;
        else if (data == "depthtest")
            io >> (int&)m_bCullFace;
        else if (data == "blending")
            io >> (int&)m_bBlend;
        else if (data == "stenciltest")
            io >> (int&)m_bStencilTest;
		else if (data == "multisampling")
            io >> (int&)m_bMultisample;
		else if (data == "clamping")
            io >> (int&)m_bClampFloats;

        data = io.getValueName();
    }
    io >> name;

    return true;
}

