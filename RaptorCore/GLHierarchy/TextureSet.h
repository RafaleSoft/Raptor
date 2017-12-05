// TextureSet.h: interface for the CTextureSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
#define AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __CGLTYPES_HPP__
    #include "System/CGLTypes.h"
#endif
#if !defined(AFX_PERSISTENCE_H__5561BA28_831B_11D3_9142_EEB51CEBBDB0__INCLUDED_)
	#include "GLHierarchy/Persistence.h"
#endif

RAPTOR_NAMESPACE_BEGIN

class CTextureObject;


class RAPTOR_API CTextureSet : public CPersistence
{
public:
	CTextureSet(const std::string& name = "GL_TEXTURE_SET");
	virtual ~CTextureSet(void);

	//! Returns the numbre of holded texture objects.
    unsigned int getNbTexture(void) const { return m_pTextures.size(); };

	//! Adds a texture object to this vector. 
	//!	TextureObject ref count is incremented.
    //! Returns false if object is already in this vector
    bool addTexture(CTextureObject *t);

	//! Remove a texture object from this vector. 
	//! TextureObject ref count is decremented, 
	//! and can be destroyed if it is the last reference.
    //! Returns false if object is not found in this vector
    bool removeTexture(CTextureObject *t);

    //! Removes all texture objects from this vector. Reference count of
    //! texture objects are decremented.
	//! Same remark as removeTexture(CTextureObject *t);
    void removeAllTextures(void);


    //! Returns the first texture of this factory whose
    //! name is matched by the name parameter.
    //! NULL is returned if no texture is found
    CTextureObject*     const getTexture(const std::string& name) const;

    //! Returns the first texture of this factory whose
    //! zero-based index in this vector matches the index parameter.
    //! NULL is returned if no texture is found
    CTextureObject*     const getTexture(unsigned int index) const;

	//! Returns a generic pointer on the objet.
	//! This is handle can be used for binding to a display.
    virtual operator RAPTOR_HANDLE() const;

	//! Implements CPersistence
	DECLARE_IO
	DECLARE_CLASS_ID(CTextureSetClassID,"TextureSet",CPersistence)


private:
    //! A helper to import texture objects
    bool importTextureObject(CRaptorIO& io);

	vector<CTextureObject*>	m_pTextures;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)

