/***************************************************************************/
/*                                                                         */
/*  Vulkan.cpp                                                             */
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

#if !defined(AFX_VULKAN_H__625F6BC5_F386_44C2_85C1_EDBA23B16921__INCLUDED_)
	#include "Subsys/Vulkan/RaptorVulkan.h"
#endif


RAPTOR_NAMESPACE


static CVulkan::CVulkanClassID vulkanID;
const CPersistence::CPersistenceClassID& CVulkan::CVulkanClassID::GetClassId(void)
{
	return vulkanID;
}



