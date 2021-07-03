/***************************************************************************/
/*                                                                         */
/*  Vulkan.h                                                               */
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



#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
#define AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif


RAPTOR_NAMESPACE_BEGIN


class CVulkan
{
public:
	//! Emulates CPersistence for global opengl classes IDs & global namespace
	DECLARE_CLASS_ID(CVulkanClassID, "Vulkan", CPersistence)

private:
	CVulkan();
	virtual ~CVulkan();
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)

