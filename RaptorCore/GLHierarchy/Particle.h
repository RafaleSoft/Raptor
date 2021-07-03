/***************************************************************************/
/*                                                                         */
/*  Particle.h													           */
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


#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
#define AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif
#if !defined(AFX_REFERENCE_H__D29BE5EA_DA55_4BCA_A700_73E007EFE5F9__INCLUDED_)
	#include "GLHierarchy/Reference.cxx"
#endif
#if !defined(AFX_ITEXTUREOBJECT_H__3AA8C89E_BB23_483C_A547_C8A4CC53E551__INCLUDED_)
	#include "GLHierarchy/ITextureObject.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class ITextureObject;
class CShader;


class RAPTOR_API CParticle : public CObject3D
{
public:
	//!
	//!	Rendering models available to render a particle system.
	//!
	typedef enum
	{
		CGL_PARTICLE_POINT,
		CGL_PARTICLE_LINE,
		CGL_PARTICLE_TEXTURE,
        CGL_PARTICLE_VOLUMETRIC
	} PARTICLE_RENDERING;

	//!
	//!	Blending behaviour of particle systems (depends on behavior).
	//!
	typedef enum
	{
		CGL_ALPHA_BLEND,		// ~fire
		CGL_COLOR_BLEND,		// ~smoke
		CGL_ALPHA_TRANSPARENT,	// ~static
	} PARTICLE_BLENDING;

	typedef struct PARTICLE_ATTRIBUTE_t
	{
		GL_COORD_VERTEX	position;
		GL_COORD_VERTEX color;
		GL_COORD_VERTEX speed;
		GL_COORD_VERTEX pull;
		float size;
		float fade;
		float angle;
		float reserved;
	} PARTICLE_ATTRIBUTE;

public:
    //! Construction/Destruction restricted to particle factory ( @see CParticleManager )
	CParticle(	unsigned int size = 100, 
				PARTICLE_RENDERING type = CGL_PARTICLE_POINT,
				PARTICLE_BLENDING blend = CGL_ALPHA_TRANSPARENT,
				const std::string& name = "GL_PARTICLE");

	virtual ~CParticle();

	//! Manipulators
	DECLARE_OBJECT3D_MANIPULATORS


	//! Initializations
	void usePointSprite(bool use = true,float size = 1.0f);

	//! Return the size of particle system
	size_t getSize(void) const { return m_attributes.size(); };

	//! Return amount of alive particles
	size_t getQuantity(void) const { return m_uiQuantity; };

	//! Set amount of alive particles (temporary for ParticleCompute)
	void setQuantity(size_t quantity) { m_uiQuantity = quantity; };

	//!	Acces an attribute set.
	CParticle::PARTICLE_ATTRIBUTE& getAttribute(size_t numAttribute)
	{ return m_attributes[numAttribute]; };

	//! Texture accessor
	ITextureObject * const getTexture() const { return m_pTexture; };

    //! Assigns a texture for rendering a textured particle set.
    //! Rq: The ref count is incremented.
	void setTexture(ITextureObject* texture);

	//!	Implements CPersistence
	DECLARE_CLASS_ID(CParticleClassID,"Particle",CObject3D)



private:
    //! Forbid operators
    CParticle& operator=(const CParticle&);

    //! Specific rendering method for point particles
    void RAPTOR_FASTCALL glRenderPoints(void);

    //! Specific rendering method for line particles
    void RAPTOR_FASTCALL glRenderLines(void);

    //! Specific rendering method for quad textured particles
    void RAPTOR_FASTCALL glRenderTextures(void);

    //! Specific rendering method for volume textured particles
    void RAPTOR_FASTCALL glRenderVolumes(void);

	//!	A helper to initialize rendering caches
	void glInitParticle(void);


    friend class CParticleManager;

	// Current number of active particle elements
	size_t m_uiQuantity;

	//! Particule type
	PARTICLE_RENDERING	m_type;

	// Particle rendering attributes
	bool			m_bPointSprite;
	float			m_fPointSize;
	unsigned int	m_blendSrc;
	unsigned int	m_blendDst;

	// Particle texture if any
	CReference<ITextureObject>	m_pTexture;

	//! Particle shaders
	CShader			*m_pShader;

	//!	Vertex Input State Resource binder
	void	*m_pBinder;

	// Particles attributes
	std::vector<PARTICLE_ATTRIBUTE>	m_attributes;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)

