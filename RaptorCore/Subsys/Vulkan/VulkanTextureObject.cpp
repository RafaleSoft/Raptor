// VulkanTextureObject.cpp: implementation of the CVulkanTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
	#include "VulkanTextureObject.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(__RAPTOR_VKEXT_H__)
	#include "System/vkext.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVulkanTextureObject::CVulkanTextureObject(	ITextureObject::TEXEL_TYPE type,
											VkDevice device,
											PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
	:ITextureObject(type), m_device(device)
{
	vkCreateImageView = (PFN_vkCreateImageView)(vkGetDeviceProcAddr(device, "vkCreateImageView"));
}


CVulkanTextureObject::~CVulkanTextureObject()
{
}

void CVulkanTextureObject::glvkRender(void)
{

}

void CVulkanTextureObject::vkLoadTexture(uint32_t innerFormat,
										 uint32_t width,
										 uint32_t height,
										 uint32_t pixels_format,
										 uint32_t pixels_type,
										 unsigned char* pixels)
{
	m_image = CTexelAllocator::GetInstance()->vkAllocateTextureImage(	getWidth(),
																		getHeight(),
																		getDepth(),
																		getTexelType(),
																		(uint8_t*)pixels);

	VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
													NULL,
													0,
													m_image,
													VK_IMAGE_VIEW_TYPE_2D,
													getTexelFormat(),
													{	VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY },
													{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };

	VkResult res = vkCreateImageView(	m_device,
										&image_view_create_info,
										CVulkanMemory::GetAllocator(),
										&m_view);
	CATCH_VK_ERROR(res)
}


VkFormat CVulkanTextureObject::getTexelFormat(void) const
{
	//!	A default value, though it may be wrong.
	VkFormat res = VK_FORMAT_R8G8B8A8_UNORM;

	switch (getTexelType())
	{
		case CGL_COLOR24:
			res = VK_FORMAT_R8G8B8_UNORM;
			break;
		case CGL_COLOR24_ALPHA:
			res = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case CGL_LIGHTMAP:
			res = VK_FORMAT_R8_UNORM;
			break;
		case CGL_LIGHTMAP_ALPHA:
			res = VK_FORMAT_R8G8_UNORM;
			break;
		case CGL_LIGHTMAP16:
			res = VK_FORMAT_R16_UNORM;
			break;
		case CGL_LIGHTMAP16_ALPHA:
			res = VK_FORMAT_R16G16_UNORM;
			break;
#if defined(GL_ARB_texture_float)
		case CGL_COLOR_FLOAT16:
			res = VK_FORMAT_R16G16B16_SFLOAT;
			break;
		case CGL_COLOR_FLOAT16_ALPHA:
			res = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case CGL_COLOR_FLOAT32:
			res = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case CGL_COLOR_FLOAT32_ALPHA:
			res = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
#endif
#ifdef GL_ARB_depth_texture
		//case CGL_DEPTH8:
		//	res = GL_DEPTH_COMPONENT;
		//	break;
		case CGL_DEPTH16:
			res = VK_FORMAT_D16_UNORM;
			break;
		case CGL_DEPTH24:
			res = VK_FORMAT_X8_D24_UNORM_PACK32;
			break;
		case CGL_DEPTH32:
			res = VK_FORMAT_D32_SFLOAT;
			break;
#endif
#if defined(GL_EXT_packed_depth_stencil)
		case CGL_DEPTH24_STENCIL8:
			res = VK_FORMAT_D24_UNORM_S8_UINT;
			break;
#endif
	}

	return res;
}
