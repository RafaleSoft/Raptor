/***************************************************************************/
/*                                                                         */
/*  Particle.cpp                                                           */
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

#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
	#include "Particle.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_OBJECTFACTORY_H__7F891C52_9E32_489C_B09C_5E5803522D91__INCLUDED_)
	#include "ObjectFactory.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_VERTEXSHADER_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
	#include "GLHierarchy/VertexShader.h"
#endif
#if !defined(AFX_GEOMETRYSHADER_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
    #include "GLHierarchy/GeometryShader.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__CC35D088_ADDF_4414_8CB6_C9D321F9D184__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif

RAPTOR_NAMESPACE_BEGIN

//	A cache used for rendering rotating textured particles
#define	CACHE_SIZE	1000
static bool cacheReady = false;


struct CACHEELT_t
{
	GL_COORD_VERTEX coord;
	GL_COORD_VERTEX	colors;
	float			angle;
	float			size;
} CACHEELT;
static CACHEELT_t cache[CACHE_SIZE];
static size_t CACHEPOINTER_SIZE = sizeof(CACHEELT)*CACHE_SIZE/sizeof(float);
static float *cachePointer = NULL;


RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static CParticle::CParticleClassID particleId;
static CPersistentObjectType<CParticle> particleFactory(particleId);

CParticle::CParticle(	unsigned int number, 
						PARTICLE_RENDERING type,
						PARTICLE_BLENDING blend,
						const std::string& name):
    CObject3D(particleId,name),
	m_uiQuantity(number),
	m_type(type),
	m_bPointSprite(false),
	m_attributes(),
	m_fPointSize(1.0f),
	m_pShader(NULL),
	m_pBinder(NULL)
{
    CObjectProperties &props = getProperties();
    props.setCastShadow(false);
    props.setReceiveShadow(false);
    props.setTransparent(true);

	m_attributes.resize(number);

	// Set blending depending on particule behaviour
	switch(blend)
	{
        case CGL_ALPHA_TRANSPARENT:
		{
            m_blendSrc = GL_SRC_ALPHA;
			m_blendDst = GL_ONE_MINUS_SRC_ALPHA;
			break;
		}
        case CGL_COLOR_BLEND:
		{
            m_blendSrc = GL_ZERO;
			m_blendDst = GL_ONE_MINUS_SRC_COLOR;
			break;
		}
        case CGL_ALPHA_BLEND:
		{
            m_blendSrc = GL_SRC_ALPHA;
			m_blendDst = GL_ONE;
			break;
		}
		default:
		{
			break;
		}
	}
}

void CParticle::glInitParticle(void)
{
	bool res = false;

	if (NULL == m_pShader)
	{
		if (m_type == CGL_PARTICLE_TEXTURE)
		{
			m_pShader = new CShader(getName() + "_SHADER");
			COpenGLShaderStage *stage = m_pShader->glGetOpenGLShader();

			CVertexShader *vp = stage->glGetVertexShader("PARTICLE_VTX_PROGRAM");
			CProgramParameters params;
			params.addParameter("fPointSize", GL_COORD_VERTEX(m_fPointSize, 0.0f, 0.0f, 0.0f));

			CGeometryShader *gp = stage->glGetGeometryShader("PARTICLE2D_GEO_PROGRAM");
			gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);

			CFragmentShader *fs = stage->glGetFragmentShader("TEXTURE_QUAD_TEX_PROGRAM");
			params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

			stage->setProgramParameters(params);
			res = stage->glCompileShader();
		}
		else if ((m_type == CGL_PARTICLE_VOLUMETRIC) && (NULL == m_pShader))
		{
			m_pShader = new CShader(getName() + "_VOLUME_SHADER");
			COpenGLShaderStage *stage = m_pShader->glGetOpenGLShader();

			CVertexShader *vp = stage->glGetVertexShader("PARTICLE_VTX_PROGRAM");
			CProgramParameters params;
			params.addParameter("fPointSize", GL_COORD_VERTEX(m_fPointSize, 0.0f, 0.0f, 0.0f));

			CGeometryShader *gp = stage->glGetGeometryShader("PARTICLE3D_GEO_PROGRAM");
			gp->setGeometry(GL_POINTS, GL_TRIANGLE_STRIP, 4);

			CFragmentShader *fs = stage->glGetFragmentShader("PARTICLE3D_TEX_PROGRAM");
			params.addParameter("diffuseMap", CTextureUnitSetup::IMAGE_UNIT_0);

			stage->setProgramParameters(params);
			res = stage->glCompileShader();
		}
	}

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	//	Precompute data
	if (!cacheReady)
	{
        cacheReady = true;
        unsigned int i=0;

		if (pAllocator->isMemoryRelocated())
		{
			cachePointer = pAllocator->allocateVertices(CACHEPOINTER_SIZE);
			pAllocator->glvkSetPointerData(cachePointer,(float*)&cache,CACHEPOINTER_SIZE);
		}
		else
			cachePointer = &cache[0].coord.x;
	}

	if (NULL != m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
		delete binder;
	}
	
	CACHEELT_t* pCache = (CACHEELT_t*)cachePointer;
	CResourceAllocator::CResourceBinder *binder = new CResourceAllocator::CResourceBinder();
	if (!pAllocator->isMemoryRelocated() && (CGL_PARTICLE_POINT == m_type))
	{
		binder->setArray(CProgramParameters::POSITION, &m_attributes[0].position, 4, sizeof(PARTICLE_ATTRIBUTE));
		binder->setArray(CProgramParameters::PRIMARY_COLOR, &m_attributes[0].color, 4, sizeof(PARTICLE_ATTRIBUTE));
		binder->useVertexArrayObjects();
	}
	else
	{
		binder->setArray(CProgramParameters::POSITION, &pCache[0].coord, 4, sizeof(CACHEELT));
		binder->setArray(CProgramParameters::PRIMARY_COLOR, &pCache[0].colors, 4, sizeof(CACHEELT));
		binder->useVertexArrayObjects();
	}
	if ((CGL_PARTICLE_TEXTURE == m_type) ||
		(CGL_PARTICLE_VOLUMETRIC == m_type))
	{
		binder->setArray(CProgramParameters::WEIGHTS, &pCache[0].size, 1, sizeof(CACHEELT));
		binder->setArray(CProgramParameters::FOG_COORDINATE, &pCache[0].angle, 1, sizeof(CACHEELT));
		binder->useVertexArrayObjects();
	}

	m_pBinder = binder;
}

CParticle::~CParticle()
{
	m_pTexture = NULL;

	if (NULL != m_pBinder)
	{
		CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
		delete binder;
	}
}

void CParticle::setTexture(CTextureObject* texture) 
{
    m_pTexture = texture;
}


//////////////////////////////////////////////////////////////////////

void CParticle::usePointSprite(bool use,float size) 
{
	m_fPointSize = size;

	if (NULL != m_pShader)
	{
		CProgramParameters params;
		params.addParameter("fPointSize", GL_COORD_VERTEX(m_fPointSize, 0.0f, 0.0f, 0.0f));
		
		COpenGLShaderStage *stage = m_pShader->glGetOpenGLShader();
		stage->updateProgramParameters(params);
	}

#if defined(GL_NV_point_sprite)
	if ((Raptor::glIsExtensionSupported(GL_NV_POINT_SPRITE_EXTENSION_NAME)) ||
		(Raptor::glIsExtensionSupported(GL_ARB_POINT_SPRITE_EXTENSION_NAME)))
	{
		m_bPointSprite = use;
	}
	else
#endif
		m_bPointSprite = false;
}

void RAPTOR_FASTCALL CParticle::glRenderPoints(void)
{
#ifdef GL_ARB_multitexture
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif

	float oldSize = 1.0f;
	glGetFloatv(GL_POINT_SIZE,&oldSize);
	glPointSize(m_fPointSize);

	if (m_bPointSprite)
	{
		glEnable(GL_TEXTURE_2D);
		m_pTexture->glvkRender();
		#if defined(GL_ARB_point_sprite)
			glEnable(GL_POINT_SPRITE_ARB);
			glTexEnvi(GL_POINT_SPRITE_ARB,GL_COORD_REPLACE_ARB,GL_TRUE);
		#elif defined(GL_POINT_SPRITE_NV)
			glEnable(GL_POINT_SPRITE_NV);
			glTexEnvi(GL_POINT_SPRITE_NV,GL_COORD_REPLACE_NV,GL_TRUE);
		#endif
	}
	else
		glDisable(GL_TEXTURE_2D);

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	glEnable(GL_POINT_SMOOTH);

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	if (pAllocator->isMemoryRelocated())
	{
		unsigned int nbElt = 0;
		while ((nbElt+CACHE_SIZE) < m_uiQuantity)
		{
			for (size_t i=0;i<CACHE_SIZE;i++)
			{
				memcpy(&cache[i].coord,&m_attributes[nbElt+i].position,sizeof(GL_COORD_VERTEX));
				memcpy(&cache[i].colors,&m_attributes[nbElt+i].color,sizeof(GL_COORD_VERTEX));
			}

			pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);
			glDrawArrays(GL_POINTS, 0, CACHE_SIZE);

			nbElt += CACHE_SIZE;
		}
		if (nbElt < m_uiQuantity)
		{
			unsigned int sz = m_uiQuantity - nbElt;

			for (size_t i=0;i<sz;i++)
			{
				memcpy(&cache[i].coord,&m_attributes[nbElt+i].position,sizeof(GL_COORD_VERTEX));
				memcpy(&cache[i].colors,&m_attributes[nbElt+i].color,sizeof(GL_COORD_VERTEX));
			}

			pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*sz/sizeof(float));
			glDrawArrays(GL_POINTS, 0, sz);
		}
	}
	else
	{
		glDrawArrays(GL_POINTS, 0, m_uiQuantity);
	}

	binder->glvkUnbindArrays();

	glPointSize(oldSize);

	if (m_bPointSprite)
	{
#if defined(GL_ARB_point_sprite)
		glDisable(GL_POINT_SPRITE_ARB);
#elif defined(GL_POINT_SPRITE_NV)
		glDisable(GL_POINT_SPRITE_NV);
#endif
	}
}

//
//	Is this method usefull ?
//	How to configure & use it ?
//
void RAPTOR_FASTCALL CParticle::glRenderLines(void)
{
#ifdef GL_ARB_multitexture
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif

	glDisable(GL_TEXTURE_2D);

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	unsigned int nbElt = 0;
	CGenericVector<float> position;

	for(unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];

		float *c = attrs.color;

		memcpy(&cache[nbElt].colors,c,4*sizeof(float));
        memcpy(&cache[nbElt+1].colors,c,4*sizeof(float));

		float size = attrs.size;
		position = attrs.position;

		cache[nbElt].coord.x = position.X() - size;
		cache[nbElt].coord.y = position.Y() - size;
		cache[nbElt].coord.z = position.Z() - size;
		cache[nbElt].coord.h = position.H();
		cache[nbElt+1].coord.x = position.X() + size;
		cache[nbElt+1].coord.y = position.Y() + size;
		cache[nbElt+1].coord.z = position.Z() + size;
		cache[nbElt+1].coord.h = position.H();

		nbElt += 2;

		if (nbElt == CACHE_SIZE)
		{
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
			if (pAllocator->isMemoryRelocated())
				pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);

			glDrawArrays(GL_LINES, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}

	if (nbElt > 0)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*nbElt/sizeof(float));

		glDrawArrays(GL_LINES, 0, nbElt);
		nbElt = 0;
	}

	binder->glvkUnbindArrays();
}


void RAPTOR_FASTCALL CParticle::glRenderTextures(void)
{
	m_pShader->glRender();

#ifdef GL_ARB_multitexture
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
	glEnable(GL_TEXTURE_2D);
	m_pTexture->glvkRender();

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	unsigned int nbElt = 0;
	CGenericVector<float> position;
	for (unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];
		memcpy(&cache[nbElt].coord, attrs.position, 4 * sizeof(float));
		memcpy(&cache[nbElt].colors, attrs.color, 4 * sizeof(float));
		cache[nbElt].size = attrs.size;
		cache[nbElt].angle = TO_RADIAN(attrs.angle);

		nbElt += 1;
		if (nbElt == CACHE_SIZE)
		{
			if (pAllocator->isMemoryRelocated())
				pAllocator->glvkSetPointerData(cachePointer, &cache[0].coord.x, CACHEPOINTER_SIZE);
			glDrawArrays(GL_POINTS, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}
	if (nbElt > 0)
	{
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkSetPointerData(cachePointer, &cache[0].coord.x, sizeof(CACHEELT)*nbElt / sizeof(float));
		glDrawArrays(GL_POINTS, 0, nbElt);
		nbElt = 0;
	}

	m_pShader->glStop();
	glDisable(GL_TEXTURE_2D);

	binder->glvkUnbindArrays();
}


void RAPTOR_FASTCALL CParticle::glRenderVolumes(void)
{
#ifdef GL_EXT_texture3D
	m_pShader->glRender();

#ifdef GL_ARB_multitexture
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
	glEnable(GL_TEXTURE_3D_EXT);
	m_pTexture->glvkRender();

	CResourceAllocator::CResourceBinder *binder = (CResourceAllocator::CResourceBinder*)m_pBinder;
	binder->glvkBindArrays();

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	unsigned int nbElt = 0;
	CGenericVector<float> position;
	for(unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];
		memcpy(&cache[nbElt].coord, attrs.position, 4 * sizeof(float));
		memcpy(&cache[nbElt].colors, attrs.color, 4 * sizeof(float));
		cache[nbElt].size = attrs.size;
		cache[nbElt].angle = TO_RADIAN(attrs.angle);

		nbElt += 1;
		if (nbElt == CACHE_SIZE)
		{
			if (pAllocator->isMemoryRelocated())
				pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);

			glDrawArrays(GL_POINTS, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}

	if (nbElt > 0)
	{
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkSetPointerData(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*nbElt/sizeof(float));

		glDrawArrays(GL_POINTS, 0, nbElt);
		nbElt = 0;
	}

	m_pShader->glStop();
	glDisable(GL_TEXTURE_3D_EXT);

	binder->glvkUnbindArrays();
#endif
}



//////////////////////////////////////////////////////////////////////
void CParticle::glRender()
{
	if (!properties.isVisible())
		return;

	glPushAttrib(GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);

	if (!CGeometryAllocator::GetInstance()->isMemoryRelocated())
		CGeometryAllocator::GetInstance()->glvkLockMemory(false);

	// Render depending on particule material
	switch(m_type)
	{
	    case CGL_PARTICLE_POINT:
		{
            glRenderPoints();
			break;
		}
	    case CGL_PARTICLE_LINE:
		{
            glRenderLines();
	        break;		
		}
	    case CGL_PARTICLE_TEXTURE:
		{
			if (m_pTexture != NULL)
			{
				// Transparency
				GLint blendSrc;
				GLint blendDst;
				glGetIntegerv(GL_BLEND_SRC,&blendSrc);
				glGetIntegerv(GL_BLEND_DST,&blendDst);

				glEnable(GL_BLEND);
				glBlendFunc(m_blendSrc,m_blendDst);
				glDepthMask(GL_FALSE);

#if defined(GL_NV_point_sprite) || defined(GL_ARB_point_sprite)
				if (m_bPointSprite)
					glRenderPoints();
				else
#endif
					glRenderTextures();
				glBlendFunc(blendSrc,blendDst);
			}
			break;
		}
        case CGL_PARTICLE_VOLUMETRIC:
        {
            if (m_pTexture != NULL)
			{
				// Transparency
				GLint blendSrc;
				GLint blendDst;
				glGetIntegerv(GL_BLEND_SRC,&blendSrc);
				glGetIntegerv(GL_BLEND_DST,&blendDst);

				glEnable(GL_BLEND);
				glBlendFunc(m_blendSrc,m_blendDst);
				glDepthMask(GL_FALSE);

                glRenderVolumes();

				glBlendFunc(blendSrc,blendDst);
			}
            break;
        }
	    default:
		{
			break;
		}
	}

	if (!CGeometryAllocator::GetInstance()->isMemoryRelocated())
        CGeometryAllocator::GetInstance()->glvkLockMemory(true);

	glPopAttrib();

	CATCH_GL_ERROR
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationX(float rx)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationY(float ry)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationZ(float rz)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationX(float rx,float cy,float cz)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationY(float ry,float cx,float cz)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotationZ(float rz,float cx,float cy)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::rotation(double angle,float ax,float ay,float az)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::scale(float alpha,float beta,float gamma)
{
}

//////////////////////////////////////////////////////////////////////

void CParticle::translateAbsolute(float alpha,float beta,float gamma)
{
    GL_COORD_VERTEX center;
	getCenter(center);

	translateCenter(alpha - center.x,beta - center.y,gamma - center.z);
}

//////////////////////////////////////////////////////////////////////

void CParticle::translate(float alpha,float beta,float gamma)
{
	translateCenter(alpha,beta,gamma);
}

//////////////////////////////////////////////////////////////////////

void CParticle::transform(GL_MATRIX &m)
{
}
