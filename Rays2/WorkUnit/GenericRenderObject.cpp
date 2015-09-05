// GenericRenderObject.cpp: implementation of the CGenericRenderObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericRenderObject.h"
#include "Texture.h"
#include "Surfacer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//	Acces to plugins.
//
//	Yeah, yeah, I know, there should be a class
//	or a namespace for global data. But right know
//	I have something else to do.
extern RAYTRACERDATA	raytracer_data;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGenericRenderObject::CGenericRenderObject(const CString& name)
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

bool CGenericRenderObject::InitPlugins(void)
{
	if (m_pSurfacer != NULL)
		return m_pSurfacer->InitPlugins();
	return true;
}