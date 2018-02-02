// VulkanTextureObject.h: interface for the CVulkanTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
#define AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif



RAPTOR_NAMESPACE_BEGIN

class CVulkanTextureObject : public ITextureObject
{
public:
	CVulkanTextureObject(ITextureObject::TEXEL_TYPE type,
						 VkDevice device,
						 PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr);

	//!	Renders the textures : it is bound to the current active Texture Unit.
	virtual void glvkRender(void);

	//! Updates texture filtering function
	//! @param F : the filter function
	virtual void glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F);

	//!	Returns the sized format of the texels stored in device memory (texture)
	VkFormat getTexelFormat(void) const;

	//!	Returns the proper implementation
	virtual CVulkanTextureObject* getVulkanTextureObject(void) { return this; };


	void vkLoadTexture(uint32_t innerFormat,
					   uint32_t pixels_format,
					   uint32_t pixels_type,
					   unsigned char* pixels);


private:
	//!
	//!	Forbidden methods
	//!
	CVulkanTextureObject(ITextureObject::TEXEL_TYPE type);
	CVulkanTextureObject(const CVulkanTextureObject& rsh);
	virtual ~CVulkanTextureObject();

	//!	Release all allocated resources.
	void clean(void);

	//!
	//!	Attributes
	//!

	//!	Creator device
	VkDevice	m_device;

	//!	Vulkan texture backed image 
	VkImage		m_image;

	//!	Vulkan image accessor (format mapper)
	VkImageView	m_view;

	//! Vulkan texture sampler
	VkSampler	m_sampler;

	//!	Vulkan texture backed image 
	PFN_vkCreateImageView vkCreateImageView;
	PFN_vkDestroyImageView vkDestroyImageView;
	PFN_vkCreateSampler	vkCreateSampler;
	PFN_vkDestroySampler vkDestroySampler;
};


RAPTOR_NAMESPACE_END

#endif // !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)


