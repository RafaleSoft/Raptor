/***************************************************************************/
/*                                                                         */
/*  RaptorInstance.h                                                       */
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



#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
#define AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_IMAGE_H__F545D0D5_5F10_4EFA_BE3B_3F3D34D4DBF3__INCLUDED_)
	#include "System/Image.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
	#include "System/RaptorConfig.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class C3DEngine;
class CAnimator;
class CRaptorMessages;
class CRaptorErrorManager;


class CRaptorInstance
{
public:
	//!	Create a singleton instance.
	static CRaptorInstance &GetInstance(void);

	//! (Re)Initialise all instance objects.
	void initInstance();

	//	the second pipeline has exited, raptor can be closed safely
	bool					terminate;
	//!	3Dengine to use for geometric queries.
	C3DEngine				*p3DEngine;
	//	current animator
	CAnimator				*pAnimator;
	//!	Raptor Error manager.
	CRaptorErrorManager		*pErrorMgr;
	//!	Raptor Error messages database.
	CRaptorMessages			*pMessages;
	//!	Raptor global configuration.
	CRaptorConfig			config;


private:
	//! Constructor.
	CRaptorInstance();
	//! Copy Constructor.
	CRaptorInstance(const CRaptorInstance&);
	//!	Destructor.
	~CRaptorInstance();

	static CRaptorInstance *m_pInstance;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)

