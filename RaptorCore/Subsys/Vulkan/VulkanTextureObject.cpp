// VulkanTextureObject.cpp: implementation of the CVulkanTextureObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_VULKANTEXTUREOBJECT_H__5E3E26C2_441F_4051_986F_2207AF0B3F6D__INCLUDED_)
	#include "VulkanTextureObject.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANMEMORY_H__72256FF7_DBB9_4B9C_9BF7_C36F425CF811__INCLUDED_)
	#include "Subsys/Vulkan/VulkanMemory.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif


RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVulkanTextureObject::CVulkanTextureObject(	ITextureObject::TEXEL_TYPE type,
											VkDevice device,
											PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
	:ITextureObject(type), 
	m_device(device), 
	m_image(VK_NULL_HANDLE), m_view(VK_NULL_HANDLE), m_sampler(VK_NULL_HANDLE),
	minFilter(VK_FILTER_NEAREST), magFilter(VK_FILTER_NEAREST), mipmap(VK_SAMPLER_MIPMAP_MODE_NEAREST),
	clamp_mode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
{
	//! TODO : make this static because many texture objects !
	vkCreateImageView = (PFN_vkCreateImageView)(vkGetDeviceProcAddr(device, "vkCreateImageView"));
	vkDestroyImageView = (PFN_vkDestroyImageView)(vkGetDeviceProcAddr(device, "vkDestroyImageView"));

	vkCreateSampler = (PFN_vkCreateSampler)(vkGetDeviceProcAddr(device, "vkCreateSampler"));
	vkDestroySampler = (PFN_vkDestroySampler)(vkGetDeviceProcAddr(device, "vkDestroySampler"));
}


CVulkanTextureObject::~CVulkanTextureObject()
{
	clean();
}

void CVulkanTextureObject::glvkRender(void)
{

}

void CVulkanTextureObject::vkLoadTexture(uint32_t innerFormat,
										 uint32_t pixels_format,
										 uint32_t pixels_type,
										 unsigned char* pixels)
{
	if ((0 == getWidth()) || (0 == getHeight()) || (0 == getDepth()))
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::CVulkanClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CVulkanTextureObject wrong size update");
#endif
		return;
	}
	
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = NULL;
	imageInfo.flags = 0;

	uint64_t size = getWidth() * getHeight() * getDepth();
	//!
	//! Select Image format
	//!
	switch (getTexelType())
	{
		case ITextureObject::CGL_COLOR24_ALPHA:
			imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			size = size * 4;
			break;
		case ITextureObject::CGL_COLOR24:
			imageInfo.format = VK_FORMAT_R8G8B8_UNORM;
			size = size * 3;
			break;
		default:
			imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			size = size * 4;
			break;
	}

	imageInfo.extent.width = getWidth();
	imageInfo.extent.height = getHeight();
	imageInfo.extent.depth = getDepth();

	//!
	//! Select Image type
	//!
	if (2 > getDepth())
	{
		if (2 > getHeight())
			imageInfo.imageType = VK_IMAGE_TYPE_1D;
		else
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
	}
	else
		imageInfo.imageType = VK_IMAGE_TYPE_3D;

	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	//!	Release all previous resources first
	clean();


	CTexelAllocator *tAllocator = CTexelAllocator::GetInstance();

	unsigned char *texpointer = tAllocator->allocateTexels(size);
	m_image = CTexelAllocator::GetInstance()->vkAllocateTextureImage(imageInfo, (VkDeviceSize)texpointer);

	if ((VK_NULL_HANDLE != m_image) && (NULL != pixels))
	{
		tAllocator->glvkCopyPointer(texpointer, pixels, size);

		VkImageViewCreateInfo image_view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
														NULL,
														0,
														m_image,
														VK_IMAGE_VIEW_TYPE_2D,
														getTexelFormat(),
														{ VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY,
														VK_COMPONENT_SWIZZLE_IDENTITY },
														{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };

		VkResult res = vkCreateImageView(m_device,
										 &image_view_create_info,
										 CVulkanMemory::GetAllocator(),
										 &m_view);
		CATCH_VK_ERROR(res)

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.pNext = NULL;
		samplerInfo.flags = 0;	// VkSamplerCreateFlags for future use
		samplerInfo.minFilter = minFilter;
		samplerInfo.magFilter = magFilter;
		samplerInfo.addressModeU = clamp_mode;
		samplerInfo.addressModeV = clamp_mode;
		samplerInfo.addressModeW = clamp_mode;
		samplerInfo.mipmapMode = mipmap;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;
		if (ITextureObject::CGL_ANISOTROPIC == m_filter)
			samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		res = vkCreateSampler(m_device, &samplerInfo, CVulkanMemory::GetAllocator(), &m_sampler);
		CATCH_VK_ERROR(res)
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
		Raptor::GetErrorManager()->generateRaptorError(Global::CVulkanClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CVulkanTextureObject failed to allocate texture image");
	}
#endif
}


void CVulkanTextureObject::clean(void)
{
	if (VK_NULL_HANDLE != m_sampler)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::CVulkanClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CVulkanTextureObject has a previous texture sampler");
#endif
		vkDestroySampler(m_device, m_sampler, CVulkanMemory::GetAllocator());
		m_view = VK_NULL_HANDLE;
	}

	if (VK_NULL_HANDLE != m_view)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::CVulkanClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CVulkanTextureObject has a previous image view");
#endif
		vkDestroyImageView(m_device, m_view, CVulkanMemory::GetAllocator());
		m_view = VK_NULL_HANDLE;
	}

	if (VK_NULL_HANDLE != m_image)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		Raptor::GetErrorManager()->generateRaptorError(Global::CVulkanClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_WARNING,
													   "CVulkanTextureObject has a previous image");
#endif
		CVulkanMemory::CVulkanMemoryWrapper* memory = CVulkanDevice::getCurrentDevice().getMemory();
		memory->releaseImage(m_image);

		m_image = VK_NULL_HANDLE;
	}
}

void CVulkanTextureObject::glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F)
{
	if (F == ITextureObject::CGL_BILINEAR)
	{
		minFilter = VK_FILTER_LINEAR;
		magFilter = VK_FILTER_LINEAR;
		mipmap = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}
	else if (F == ITextureObject::CGL_BILINEAR_MIPMAPPED)
	{
		minFilter = VK_FILTER_LINEAR;
		magFilter = VK_FILTER_LINEAR;
		mipmap = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	}
	else if (F == ITextureObject::CGL_TRILINEAR)
	{
		minFilter = VK_FILTER_LINEAR;
		magFilter = VK_FILTER_LINEAR;
		mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
	else if (F == ITextureObject::CGL_ANISOTROPIC)
	{
		minFilter = VK_FILTER_LINEAR;
		magFilter = VK_FILTER_LINEAR;
		mipmap = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	}
	// else VK_FILTER_CUBIC_IMG

	m_filter = F;
}

void CVulkanTextureObject::glvkUpdateClamping(ITextureObject::CLAMP_MODE C)
{
	switch (C)
	{
		case ITextureObject::CGL_REPEAT:
			clamp_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			break;
		case ITextureObject::CGL_CLAMP:
			clamp_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			break;
		case ITextureObject::CGL_EDGECLAMP:
			clamp_mode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			break;
		case ITextureObject::CGL_MIRROR_REPEAT:
			clamp_mode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			break;
		case ITextureObject::CGL_MIRROR_EDGECLAMP:
		{
#if defined(VK_KHR_sampler_mirror_clamp_to_edge)
			if (Raptor::vkIsExtensionSupported("VK_KHR_sampler_mirror_clamp_to_edge"))
				clamp_mode = VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
#endif
			break;
		}
	}
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
