// Particle.h: interface for the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
#define AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Object3D.h"
#endif
#if !defined(AFX_OBJECTREFERENCE_H__0D47C721_2B2D_4163_AB88_BE1B4E08A84D__INCLUDED_)
	#include "ObjectReference.h"
#endif


RAPTOR_NAMESPACE_BEGIN

class CTextureObject;
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
	CTextureObject * const getTexture() const { return m_pTexture; };

    //! Assigns a texture for rendering a textured particle set.
    //! Rq: The ref count is incremented.
	void setTexture(CTextureObject* texture);

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
	CReference<CTextureObject>	m_pTexture;

	//! Particle shaders
	CShader			*m_pShader;

	// Particles attributes
	std::vector<PARTICLE_ATTRIBUTE>	m_attributes;
};

RAPTOR_NAMESPACE_END

#endif // !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)

