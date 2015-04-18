// RenderEntryPoint.cpp: implementation of the CRenderEntryPoint class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "RenderEntryPoint.h"
#endif

#ifndef __GLOBAL_H__
	#include "Global.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderEntryPoint::CRenderEntryPoint()
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();

	status.renderEntryPoints.push_back(this);
}

CRenderEntryPoint::~CRenderEntryPoint()
{
	Global::RAPTOR_CURRENT_STATUS& status = Global::GetInstance().getCurrentStatus();

	vector<CRenderEntryPoint*>::iterator itr = status.renderEntryPoints.begin();
	vector<CRenderEntryPoint*>::iterator end = status.renderEntryPoints.end();

	while (itr != end)
	{
		CRenderEntryPoint* rep = (*itr);
		if (rep == this)
		{
			status.renderEntryPoints.erase(itr);
			itr = end;
		}
		else
			itr++;
	}
}

