// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WorkUnit.h"
#include "Environment.h"
#include "Raytracer.h"
#include "Texture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CEnvironment	*CEnvironment::m_pEnvironment = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnvironment::CEnvironment():
	m_pEnvironMap(NULL)
{
	tmpClr.X(0);
	tmpClr.Y(0);
	tmpClr.Z(0);
	tmpClr.H(65535);
}

CEnvironment::~CEnvironment()
{

}


CEnvironment* CEnvironment::GetInstance(void)
{
	if (m_pEnvironment == NULL)
	{
		m_pEnvironment = new CEnvironment();
	}

	return m_pEnvironment;
}

CWVector& CEnvironment::GetLocalColor(const CGenericVector<float>	&direction)
{
	if (m_pEnvironMap != NULL)
	{
		float	theta;

		theta = (float)sqrt((direction.X() * direction.X()) + (direction.Z() * direction.Z()));
		theta = (float)(atan2(direction.Y() , theta));

		tmpVect.Y() = (float)(UN_SUR_2_PI * (M_PI + theta));
		
		theta = (float)atan2(direction.Z(),direction.X());
		tmpVect.X() = (float)(UN_SUR_2_PI * (M_PI + theta));

		tmpVect.X() *= 2;
		tmpVect.Y() *= 2;

		tmpClr = m_pEnvironMap->GetMapColor(tmpVect);
	}

	return tmpClr;
}