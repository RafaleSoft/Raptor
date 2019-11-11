/***************************************************************************/
/*                                                                         */
/*  RenderEntryPoint.cpp                                                   */
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

#if !defined(AFX_RENDERENTRYPOINT_H__0C53B135_BE95_4B42_AE37_0CBE00093DEB__INCLUDED_)
	#include "RenderEntryPoint.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif

RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderEntryPoint::CRenderEntryPoint()
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.renderEntryPoints.push_back(this);
}

CRenderEntryPoint::~CRenderEntryPoint()
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();

	vector<CRenderEntryPoint*>::iterator itr = instance.renderEntryPoints.begin();
	vector<CRenderEntryPoint*>::iterator end = instance.renderEntryPoints.end();

	while (itr != end)
	{
		CRenderEntryPoint* rep = (*itr);
		if (rep == this)
		{
			instance.renderEntryPoints.erase(itr);
			itr = end;
		}
		else
			itr++;
	}
}

