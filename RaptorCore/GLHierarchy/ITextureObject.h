// ITextureObject.h: interface for the ITextureObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
#define AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __CGLTYPES_HPP__
	#include "System/CGLTypes.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class RAPTOR_API ITextureObject
{
public:
	ITextureObject();
	virtual ~ITextureObject();

	//!	Renders the textures : it is bound to the current active Texture Unit.
	virtual void glvkRender(void) = 0;

	//! Defines the size of the texel array ( texel wrapper ) for texture loading/updates/reading.
	//! Note that the server texture object will not be modified, only the factory is allowed
	//! to proceed a GL resize. If texels have been allocated and the new size do not match,
	//! texels are lost and the wrapper is reallocated.
	//! This method must be used with allocateTexels, any other usage is not supported.
	void setSize(uint32_t width, uint32_t height, uint32_t depth = 1);


	//! Return texture width
	unsigned int	getWidth(void) const { return m_width; };

	//! Return texture height
	unsigned int	getHeight(void) const { return m_height; };

	//! Return texture depth : assume it is a volumetric texture.
	unsigned int	getDepth(void) const { return m_depth; };

protected:
	ITextureObject::ITextureObject(const ITextureObject& rsh);

private:
	//! Texture dimensions
	uint32_t	m_width;
	uint32_t	m_height;
	uint32_t	m_depth;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)


