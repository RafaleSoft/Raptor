// GenericRenderObject.cpp: implementation of the CGenericRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericRenderObject.h"
#include "Texture.h"
#include "Surfacer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGenericRenderObject::CGenericRenderObject(const std::string& name)
:surface(FLAT),m_pSurfacer(NULL),m_name(name)
{
}


CGenericRenderObject::~CGenericRenderObject() 
{
	if (m_pSurfacer != NULL)
		delete m_pSurfacer;
}

CSurfacer* CGenericRenderObject::GetSurfacer(CGenericRenderObject::SURFACETYPE type)
{
	//	Coherence should be tested here
	surface = type;

	if (m_pSurfacer == NULL)
		m_pSurfacer = new CSurfacer();

	return m_pSurfacer; 
}

CGenericVector<float>& CGenericRenderObject::GetNormal( const CGenericVector<float> &hit )
{
	if (surface != CGenericRenderObject::FLAT)
	{
		tmpVect = GetLocalNormal(hit);
	}
	else
	{
		tmpVect = Normal( hit ) ;
	}

	if ((surface == CGenericRenderObject::BUMP_USER) && (m_pSurfacer != NULL))
	{
		m_pSurfacer->PerturbNormal(hit,tmpVect);
	}

	return tmpVect;
}

bool CGenericRenderObject::InitPlugins(const CRaytracerData& world)
{
	if (m_pSurfacer != NULL)
		return m_pSurfacer->InitPlugins(world);
	return true;
}