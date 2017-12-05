// ObjectProperties.cpp: implementation of the CObjectProperties class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_OBJECTPROPERTIES_H__1D80C74F_6792_4B6E_BF78_613553722315__INCLUDED_)
    #include "GLHierarchy/ObjectProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectProperties::CObjectProperties():
    m_bVisible(true),
	m_bTransparent(false),
	m_bReceiveShadow(true),
	m_bCastShadow(true),
    m_bIsMirror(false),
    m_bIsMirrored(true),
	m_eClippingMethod(CLIP_BBOX_OCCLUSION)
{
}

CObjectProperties::CObjectProperties(const CObjectProperties& rsh)
{
    m_bVisible = rsh.m_bVisible;
    m_bTransparent = rsh.m_bTransparent;
    m_bReceiveShadow = rsh.m_bReceiveShadow;
    m_bCastShadow = rsh.m_bCastShadow;
    m_bIsMirror = rsh.m_bIsMirror;
    m_bIsMirrored = rsh.m_bIsMirrored;
}

CObjectProperties& CObjectProperties::operator=(const CObjectProperties& rsh)
{
    if (&rsh != this)
    {
        m_bVisible = rsh.m_bVisible;
        m_bTransparent = rsh.m_bTransparent;
        m_bReceiveShadow = rsh.m_bReceiveShadow;
        m_bCastShadow = rsh.m_bCastShadow;
        m_bIsMirror = rsh.m_bIsMirror;
        m_bIsMirrored = rsh.m_bIsMirrored;
    }

    return *this;
}

CObjectProperties::~CObjectProperties()
{
}

bool CObjectProperties::exportObject(CRaptorIO& o)
{
    return false;
}

bool CObjectProperties::importObject(CRaptorIO& io)
{
    string name;
    io >> name;

    string data = io.getValueName();
    while (!data.empty())
    {
        if (data == "visible")
			io >> m_bVisible; 
        else if (data == "transparent")
			io >> m_bTransparent; 
        else if (data == "receive_shadow")
			io >> m_bReceiveShadow; 
        else if (data == "cast_shadow")
			io >> m_bCastShadow; 
        else if (data == "is_mirror")
			io >> m_bIsMirror; 
        else if (data == "is_mirrored")
			io >> m_bIsMirrored; 
        else
			io >> name;

        data = io.getValueName();
    }
    io >> name;

    return true;
}

