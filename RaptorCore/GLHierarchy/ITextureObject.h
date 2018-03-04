// ITextureObject.h: interface for the ITextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
#define AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "ObjectReference.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;
class CVulkanTextureObject;


class RAPTOR_API ITextureObject : public CObjectReference
{
public:
	//!	Server-side texel type
	typedef enum
	{
		CGL_COLOR24,
		CGL_COLOR24_ALPHA,
		CGL_LIGHTMAP,
		CGL_LIGHTMAP_ALPHA,
		CGL_LIGHTMAP16,
		CGL_LIGHTMAP16_ALPHA,
		CGL_COLOR_FLOAT16,
		CGL_COLOR_FLOAT16_ALPHA,
		CGL_COLOR_FLOAT32,
		CGL_COLOR_FLOAT32_ALPHA,
		CGL_DEPTH8,
		CGL_DEPTH16,
		CGL_DEPTH24,
		CGL_DEPTH32,
		CGL_DEPTH24_STENCIL8
	} TEXEL_TYPE;

	//! Texel sampler filter
	typedef enum
	{
		CGL_UNFILTERED,
		CGL_BILINEAR,
		CGL_BILINEAR_MIPMAPPED,
		CGL_TRILINEAR,
		CGL_ANISOTROPIC
	} TEXTURE_FILTER;

	//! Texture sampler clampping model
	typedef enum
	{
		CGL_REPEAT,
		CGL_MIRROR_REPEAT,
		CGL_CLAMP,
		CGL_EDGECLAMP,
		CGL_MIRROR_EDGECLAMP
	} CLAMP_MODE;


public:
	ITextureObject(TEXEL_TYPE m_type);
	virtual ~ITextureObject();

	//!	Renders the textures : it is bound to the current active Texture Unit.
	virtual void glvkRender(void) = 0;

	//! Updates texture filtering function
	//! @param F : the filter function
	virtual void glvkUpdateFilter(ITextureObject::TEXTURE_FILTER F) = 0;

	//! Updates texture sampler clamping mode. This method
	//!	is a simple helper, clamping mode is applied equally 
	//!	to each dimension.
	//! @param C : the clamp mode
	virtual void glvkUpdateClamping(ITextureObject::CLAMP_MODE C) = 0;

	//! Returns texture object implementation or NULL
	virtual CTextureObject* getGLTextureObject(void) { return NULL; };

	//! Returns texture object implementation or NULL
	virtual CVulkanTextureObject* getVulkanTextureObject(void) { return NULL; };


	//! @return texture name ( default is the source filename )
	const std::string & getName(void) const { return m_name; };

	//! @param name: the new texture name
	void setName(const std::string & name) { m_name = name; };


	//! Defines the size of the texel array ( texel wrapper ) for texture loading/updates/reading.
	//! Note that the server texture object will not be modified, only the factory is allowed
	//! to proceed a texture resize. If texels have been allocated and the new size do not match,
	//! texels are lost and the wrapper is reallocated.
	void setSize(uint32_t width, uint32_t height, uint32_t depth = 1);

	//! Returns the texture texel type for internal storage
	TEXEL_TYPE getTexelType(void) const { return m_type; };

	//! Return texture width
	uint32_t	getWidth(void) const { return m_width; };

	//! Return texture height
	uint32_t	getHeight(void) const { return m_height; };

	//! Return texture depth : assume it is a volumetric texture.
	uint32_t	getDepth(void) const { return m_depth; };

	//! Return texture global transparency.
	//! @ return : the value set using the method glSetTransparency
	uint32_t	getTransparency(void) const { return m_alpha; };

	//! Returns the selected filtering method
	TEXTURE_FILTER getFilter(void) const { return m_filter; };



protected:
	ITextureObject::ITextureObject(const ITextureObject& rsh);

	//! The texture texel type is stored only to avoid server calls to query internal format
	TEXEL_TYPE	m_type;
		
	//! Texture dimensions
	uint32_t	m_width;
	uint32_t	m_height;
	uint32_t	m_depth;

	//! Aplha value. It is stored to be applyed before or after texture loading.
	uint32_t	m_alpha;

	//! Texel sampler filter
	TEXTURE_FILTER	m_filter;


private:
	ITextureObject::ITextureObject();

	//!	Object name ( default is filename )
	std::string	m_name;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)


