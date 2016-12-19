// Particle.cpp: implementation of the CParticle class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_PARTICLE_H__12CF5A59_2AA5_464D_9D42_81048A2B4E68__INCLUDED_)
	#include "Particle.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "TextureFactory.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
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
#if !defined(AFX_VERTEXPROGRAM_H__204F7213_B40B_4B6A_9BCA_828409871B68__INCLUDED_)
    #include "GLHierarchy/VertexProgram.h"
#endif
#if !defined(AFX_GEOMETRYPROGRAM_H__1981EA98_8F3C_4881_9429_A9ACA5B285D3__INCLUDED_)
    #include "GLHierarchy/GeometryProgram.h"
#endif

RAPTOR_NAMESPACE_BEGIN

//	A cache used for rendering rotating textured particles
#define	CACHE_SIZE	1000
static bool cacheReady = false;


struct CACHEELT_t
{
	GL_COORD_VERTEX coord;
	CColor::RGBA	colors;
	float			texDepth;
} CACHEELT;
static CACHEELT_t cache[CACHE_SIZE];
static size_t CACHEPOINTER_SIZE = sizeof(CACHEELT)*CACHE_SIZE/sizeof(float);
static float *cachePointer = NULL;


RAPTOR_NAMESPACE_END

static const std::string particle_vp_src =
"#version 120 \n\
\n\
vec4 tcoords[4] = vec4[4](vec4(1.0,1.0,0.0,1.0), \n\
					      vec4(0.0,1.0,0.0,1.0), \n\
						  vec4(0.0,0.0,0.0,1.0), \n\
						  vec4(1.0,0.0,0.0,1.0));\n\
void main (void) \n\
{\n\
	int t_index = int(gl_Vertex.w); \n\
	gl_TexCoord[0] = tcoords[t_index]; \n\
	/*gl_TexCoord[0] = vec4(0.5,0.5,0.5,1.0); */\n\
	gl_FrontColor = gl_Color; \n\
	vec4 pos = vec4(vec3(gl_Vertex.xyz),1.0); \n\
	/*gl_Position = ftransform(); */ \n\
	 gl_Position =  gl_ModelViewProjectionMatrix * pos; \n\
}\n\
";

static const std::string particle2_vp_src =
"#version 120 \n\
\n\
attribute float	texDepth; \n\
\n\
vec4 tcoords[4] = vec4[4](vec4(1.0,1.0,0.0,1.0), \n\
						  vec4(0.0,1.0,0.0,1.0), \n\
						  vec4(0.0,0.0,0.0,1.0), \n\
						  vec4(1.0,0.0,0.0,1.0));\n\
void main (void) \n\
{\n\
	int t_index = int(gl_Vertex.w); \n\
	gl_TexCoord[0] = tcoords[t_index]; \n\
	gl_TexCoord[0].z = texDepth; \n\
	gl_FrontColor = gl_Color; \n\
	vec4 pos = vec4(vec3(gl_Vertex.xyz),1.0); \n\
	gl_Position =  gl_ModelViewProjectionMatrix * pos; \n\
}\n\
";

static const std::string particle_gp_src =
"#version 120 \n\
#extension GL_ARB_geometry_shader4 : enable \n\
\n\
layout(points) in; \n\
layout(points, max_vertices=1) out; \n\
\n\
void main() \n\
{ \n\
  for(int i = 0; i < gl_in.length(); i++) \n\
  { \n\
    gl_Position = gl_in[i].gl_Position; \n\
    EmitVertex(); \n\
  } \n\
  EndPrimitive(); \n\
}\n\
";

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
	m_pShader(NULL)
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

	if (m_type == CGL_PARTICLE_TEXTURE)
	{
		m_pShader = new CShader(getName()+"_SHADER");
		CVertexProgram *vp = m_pShader->glGetVertexProgram();
		res = vp->glLoadProgram(particle_vp_src);

	//	CGeometryProgram *gp = m_pShader->glGetGeometryProgram();
	//	res = res & gp->glLoadProgram(particle_gp_src);

		res = res & m_pShader->glCompileShader();
	}
	else if (m_type == CGL_PARTICLE_VOLUMETRIC)
	{
		m_pShader = new CShader(getName()+"_VOLUME_SHADER");
		CVertexProgram *vp = m_pShader->glGetVertexProgram();
		CShaderProgram::CProgramParameters params;
		params.addParameter("texDepth",CShaderProgram::ADDITIONAL_PARAM1);
		vp->setProgramParameters(params);

		res = vp->glLoadProgram(particle2_vp_src);
		res = res & m_pShader->glCompileShader();
	}


	//	Precompute data
	if (!cacheReady)
	{
        cacheReady = true;
        unsigned int i=0;

		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
		{
			cachePointer = pAllocator->allocateVertices(CACHEPOINTER_SIZE);
			pAllocator->glvkCopyPointer(cachePointer,(float*)&cache,CACHEPOINTER_SIZE);
		}
		else
			cachePointer = &cache[0].coord.x;
	}
}

CParticle::~CParticle()
{
}

void CParticle::setTexture(CTextureObject* texture) 
{
    m_pTexture = texture;
}


//////////////////////////////////////////////////////////////////////

void CParticle::usePointSprite(bool use,float size) 
{
	m_fPointSize = size;
#if defined(GL_NV_point_sprite)
	if ((Raptor::glIsExtensionSupported("GL_NV_point_sprite")) ||
        (Raptor::glIsExtensionSupported("GL_ARB_point_sprite")))
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
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif

	float oldSize = 1.0f;
	glGetFloatv(GL_POINT_SIZE,&oldSize);
	glPointSize(m_fPointSize);

	if (m_bPointSprite)
	{
		glEnable(GL_TEXTURE_2D);
		m_pTexture->glRender();
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

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glEnable(GL_POINT_SMOOTH);

	CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
	if (pAllocator->isMemoryRelocated())
	{
		CACHEELT_t* pCache = (CACHEELT_t*)cachePointer;
		glVertexPointer(3, GL_FLOAT, sizeof(CACHEELT), &pCache[0].coord);
		glColorPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].colors);

		unsigned int nbElt = 0;

		while ((nbElt+CACHE_SIZE) < m_uiQuantity)
		{
			for (size_t i=0;i<CACHE_SIZE;i++)
			{
				memcpy(&cache[i].coord,&m_attributes[nbElt+i].position,sizeof(GL_COORD_VERTEX));
				memcpy(&cache[i].colors,&m_attributes[nbElt+i].color,sizeof(GL_COORD_VERTEX));
			}

			pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);
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

			pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*sz/sizeof(float));
			glDrawArrays(GL_POINTS, 0, sz);
		}
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, sizeof(PARTICLE_ATTRIBUTE), &m_attributes[0].position);
		glColorPointer(4, GL_FLOAT, sizeof(PARTICLE_ATTRIBUTE), &m_attributes[0].color);

		glDrawArrays(GL_POINTS, 0, m_uiQuantity);
	}

    glPopClientAttrib();
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
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif

	glDisable(GL_TEXTURE_2D);
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	CACHEELT_t* pCache = (CACHEELT_t*)cachePointer;
	glVertexPointer(3, GL_FLOAT, sizeof(CACHEELT), &pCache[0].coord);
	glColorPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].colors);

	unsigned int nbElt = 0;
	CGenericVector<float> position;
	CGenericVector<float> speed;

	for(unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];

		float *c = attrs.color;

		memcpy(&cache[nbElt].colors,c,4*sizeof(float));
        memcpy(&cache[nbElt+1].colors,c,4*sizeof(float));

		float size = attrs.size;
		position = attrs.position;
		speed = attrs.speed;

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
				pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);

			glDrawArrays(GL_LINES, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}

	if (nbElt > 0)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*nbElt/sizeof(float));

		glDrawArrays(GL_LINES, 0, nbElt);
		nbElt = 0;
	}

	glPopClientAttrib();
}


void RAPTOR_FASTCALL CParticle::glRenderTextures(void)
{
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	m_pShader->glRender();

#ifdef GL_ARB_multitexture
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
	glEnable(GL_TEXTURE_2D);
	m_pTexture->glRender();

	CGenericMatrix<float> transform;
	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,transform);

	glPushMatrix();
	glLoadIdentity();

	CGenericVector<float> position;
	float halfSize = 0.0f;
	float Hx = 0.0f;
	float Hy = 0.0f;
	unsigned int nbElt = 0;

    glColor4f(1.0f,1.0f,1.0f,1.0f);

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	CACHEELT_t* pCache = (CACHEELT_t*)cachePointer;
	glVertexPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].coord);
	glColorPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].colors);

	for(unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];

		halfSize = m_fPointSize * 0.5f * attrs.size;
        float f = TO_RADIAN(attrs.angle);
        float cs = cos(f);
        float ss = sin(f);

		float *c = attrs.color;
		position = attrs.position;
        position *= m_fPointSize;   // is this correct ?
        position.H(1.0f);
		position *= transform;

		Hx = halfSize * (cs - ss);
		Hy = halfSize * (cs + ss);

#ifndef RAPTOR_SSE_CODE_GENERATION
		float *p = position.vector();
        float Hs[8] = { Hx, Hy, 0, 0, Hy, -Hx, 0, 0 };
        
		__asm
		{
			lea ebx,nbElt
			mov eax,[ebx]
			shl eax,4			// eax = 4*nbElt

			lea edi,colors
			mov esi,c
			add edi,eax			// edi = colors[4*nbElt]

			sse_loadups(XMM0_ESI)
			sse_storeups(XMM0_EDI)
			sse_storeupsofs(XMM0_EDI,16)
			sse_storeupsofs(XMM0_EDI,32)
			sse_storeupsofs(XMM0_EDI,48)

			lea edi,coords
			mov esi,p
			add dword ptr [ebx],4	// nbElt += 4
			add edi,eax				// edi = coords[4*nbElt]
            
            sse_loadups(XMM0_ESI)
            lea esi, Hs
            sse_loadups(XMM1_ESI)
            sse_loadupsofs(XMM2_ESI,16)
            sse_loadaps(XMM3_XMM0)
            sse_loadaps(XMM4_XMM0)
            sse_loadaps(XMM5_XMM0)
            sse_addps(XMM3_XMM1)
            sse_subps(XMM5_XMM2)
            sse_subps(XMM4_XMM1)
            sse_addps(XMM0_XMM2)
            sse_storeups(XMM3_EDI)
            sse_storeupsofs(XMM5_EDI,16)
            sse_storeupsofs(XMM4_EDI,32)
            sse_storeupsofs(XMM0_EDI,48)
        }
#else
		memcpy(&cache[nbElt].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+1].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+2].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+3].colors,c,4*sizeof(float));
		
		cache[nbElt].coord.x = position.X() + Hx;
		cache[nbElt].coord.y = position.Y() + Hy;
		cache[nbElt].coord.z = position.Z();
		cache[nbElt].coord.h = 0.0; //position.H();
		cache[nbElt+1].coord.x = position.X() - Hy;
		cache[nbElt+1].coord.y = position.Y() + Hx;
		cache[nbElt+1].coord.z = position.Z();
		cache[nbElt+1].coord.h = 1.0; //position.H();
		cache[nbElt+2].coord.x = position.X() - Hx;
		cache[nbElt+2].coord.y = position.Y() - Hy;
		cache[nbElt+2].coord.z = position.Z();
		cache[nbElt+2].coord.h = 2.0; //position.H();
		cache[nbElt+3].coord.x = position.X() + Hy;
		cache[nbElt+3].coord.y = position.Y() - Hx;
		cache[nbElt+3].coord.z = position.Z();
		cache[nbElt+3].coord.h = 3.0; //position.H();

		nbElt += 4;
#endif

		if (nbElt == CACHE_SIZE)
		{
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
			if (pAllocator->isMemoryRelocated())
				pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);

			glDrawArrays(GL_QUADS, 0, CACHE_SIZE);
			//glDrawArrays(GL_POINTS, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}
	if (nbElt > 0)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*nbElt/sizeof(float));

		glDrawArrays(GL_QUADS, 0, nbElt);
		//glDrawArrays(GL_POINTS, 0, nbElt);
		nbElt = 0;
	}

	m_pShader->glStop();

	glDisable(GL_TEXTURE_2D);

	glPopClientAttrib();
	glPopMatrix();
}

void RAPTOR_FASTCALL CParticle::glRenderVolumes(void)
{
#ifdef GL_EXT_texture3D
 	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	m_pShader->glRender();

#ifdef GL_ARB_multitexture
	if (pExtensions->glActiveTextureARB != NULL)
		pExtensions->glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
	glEnable(GL_TEXTURE_3D_EXT);
	m_pTexture->glRender();

	CGenericMatrix<float> transform;
	glGetTransposeFloatv(GL_MODELVIEW_MATRIX,transform);
	glPushMatrix();
	glLoadIdentity();

	CGenericVector<float> position;
	float halfSize = 0.0f;
	float Hx = 0.0f;
	float Hy = 0.0f;
	unsigned int nbElt = 0;

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	pExtensions->glEnableVertexAttribArrayARB(CShaderProgram::ADDITIONAL_PARAM1);
	
	CACHEELT_t* pCache = (CACHEELT_t*)cachePointer;
	glVertexPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].coord);
	glColorPointer(4, GL_FLOAT, sizeof(CACHEELT), &pCache[0].colors);
	pExtensions->glVertexAttribPointerARB(CShaderProgram::ADDITIONAL_PARAM1,1,GL_FLOAT,false,sizeof(CACHEELT),&pCache[0].texDepth);

	for(unsigned int i = 0; i < m_uiQuantity; i++)
	{
		CParticle::PARTICLE_ATTRIBUTE &attrs = m_attributes[i];
		halfSize = m_fPointSize * 0.5f * attrs.size;
        float f = TO_RADIAN(attrs.angle);
        float cs = cos(f);
        float ss = sin(f);

		float *c = attrs.color;
		position = attrs.position;
        position *= m_fPointSize;
        position.H(1.0f);
		position *= transform;

		// With this instruction, the compiler do not compute the last row of the previous
		// vector * matrix product. Well, this suppose it is a smart compiler
		position.H(1.0f);
		Hx = halfSize * (cs - ss);
		Hy = halfSize * (cs + ss);
		//float angle = attrs.angle / 360.0f;
		float angle = 1.0f - attrs.color.h;

		cache[nbElt].texDepth = angle;
		cache[nbElt+1].texDepth = angle;
		cache[nbElt+2].texDepth = angle;
		cache[nbElt+3].texDepth = angle;

		memcpy(&cache[nbElt].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+1].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+2].colors,c,4*sizeof(float));
		memcpy(&cache[nbElt+3].colors,c,4*sizeof(float));
		
		cache[nbElt].coord.x = position.X() + Hx;
		cache[nbElt].coord.y = position.Y() + Hy;
		cache[nbElt].coord.z = position.Z();
		cache[nbElt].coord.h = 0.0; //position.H();
		cache[nbElt+1].coord.x = position.X() - Hy;
		cache[nbElt+1].coord.y = position.Y() + Hx;
		cache[nbElt+1].coord.z = position.Z();
		cache[nbElt+1].coord.h = 1.0; //position.H();
		cache[nbElt+2].coord.x = position.X() - Hx;
		cache[nbElt+2].coord.y = position.Y() - Hy;
		cache[nbElt+2].coord.z = position.Z();
		cache[nbElt+2].coord.h = 2.0; //position.H();
		cache[nbElt+3].coord.x = position.X() + Hy;
		cache[nbElt+3].coord.y = position.Y() - Hx;
		cache[nbElt+3].coord.z = position.Z();
		cache[nbElt+3].coord.h = 3.0; //position.H();

		nbElt += 4;

		if (nbElt == CACHE_SIZE)
		{
			CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
			if (pAllocator->isMemoryRelocated())
				pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,CACHEPOINTER_SIZE);

			glDrawArrays(GL_QUADS, 0, CACHE_SIZE);
			nbElt = 0;
		}
	}
	if (nbElt > 0)
	{
		CGeometryAllocator *pAllocator = CGeometryAllocator::GetInstance();
		if (pAllocator->isMemoryRelocated())
			pAllocator->glvkCopyPointer(cachePointer,&cache[0].coord.x,sizeof(CACHEELT)*nbElt/sizeof(float));

		glDrawArrays(GL_QUADS, 0, nbElt);
		nbElt = 0;
	}

	m_pShader->glStop();

	glPopClientAttrib();
	glPopMatrix();
    glDisable(GL_TEXTURE_3D_EXT);
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
