/***************************************************************************/
/*                                                                         */
/*  3DSceneObject.cpp                                                      */
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


#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "3DSceneObject.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORVULKANDEVICE_H__2FDEDD40_444E_4CC2_96AA_CBF9E79C3ABE__INCLUDED_)
	#include "Subsys/Vulkan/VulkanDevice.h"
#endif
#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
	#include "Subsys/Vulkan/VulkanPipeline.h"
#endif
#if !defined(AFX_RAPTORVULKANCOMMANDBUFFER_H__0398BABD_747B_4DFE_94AA_B026BDBD03B1__INCLUDED_)
	#include "Subsys/Vulkan/VulkanCommandBuffer.h"
#endif
#if !defined(AFX_SHADEDGEOMETRY_H__E56C66F7_2DF6_497B_AA0F_19DDC11390F9__INCLUDED_)
	#include "GLHierarchy/ShadedGeometry.h"
#endif
#if !defined(AFX_OPENGLSHADERSTAGE_H__56B00FE3_E508_4FD6_9363_90E6E67446D9__INCLUDED_)
	#include "GLHierarchy/OpenGLShaderStage.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_SHADERBLOC_H__56C73DCA_292E_4722_8881_82DC1BF53EA5__INCLUDED_)
	#include "GLHierarchy/ShaderBloc.h"
#endif


#include <set>      // to sort the lights

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSceneObject::C3DSceneObject(CObject3D* obj)
	:m_pPipeline(NULL),z_span(-FLT_MAX), z_order(-FLT_MAX), bbox(UINT64_MAX)
{
	object = (RAPTOR_HANDLE)(*obj);

	for (unsigned int i = 0; i<NB_PASSES; i++)
	{
		visibilityQuery[i] = 0;
		passVisibility[i] = -1;
	}

	for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
         effectiveLights[i] = NULL;

	if (obj->getId().isSubClassOf(CShadedGeometry::CShadedGeometryClassID::GetClassId()))
	{
		CShadedGeometry *sg = (CShadedGeometry *)obj;
		m_pPipeline = sg->glvkCreatePipeline();
	}
}

C3DSceneObject::~C3DSceneObject()
{
	if (NULL != m_pPipeline)
		delete m_pPipeline;
}

void C3DSceneObject::vkRender(	CVulkanCommandBuffer& commandBuffer,
								VkBuffer vertexBinding,
								VkBuffer indexBinding)
{
	if (NULL != m_pPipeline)
	{
		CVulkanPipeline *pPipeline = (CVulkanPipeline*)m_pPipeline;
		VkPipeline pipe = pPipeline->getPipeline();
		commandBuffer.vkCmdBindPipeline(commandBuffer.commandBuffer,
										VK_PIPELINE_BIND_POINT_GRAPHICS,
										pipe);

		CObject3D* obj = object.ptr<CObject3D>();
		obj->vkRender(commandBuffer, vertexBinding, indexBinding);
	}
}

CObject3D* C3DSceneObject::getObject(void) const
{
	CObject3D* obj = object.ptr<CObject3D>();

#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (!obj->getId().isSubClassOf(CObject3D::CObject3DClassID::GetClassId()))
		obj = NULL;
#endif

	return obj;
}

size_t C3DSceneObject::initShaders(size_t base)
{
	CObject3D* obj = object.ptr<CObject3D>();

	std::vector<CShader*> list;
	obj->getShaders(list);

	size_t nb = 0;

	for (size_t i = 0; i < list.size(); i++)
	{
		CShader *pShader = list[i];
		CShaderBloc *pBloc = pShader->glGetShaderBloc("LightProducts");
		//if (pShader->hasShaderBloc())	// For 'user' shaders including Raptor.glsl, check appropriate bloc !
		if (NULL != pBloc)
		{
			lightShaderbloc bloc;
			bloc.uniform = 0;
			bloc.bufferOffset = 0;
			bloc.shader = pShader;
			bloc.index = base + i;
			lightShaders.push_back(bloc);
			nb++;
		}
	}

	return nb;
}

size_t C3DSceneObject::glRenderLights(CLight::R_LightProducts *buffer, uint8_t* uniform, bool proceedLights)
{
	size_t nb_shaders = 0;

	if (!proceedLights)
	{
		for (size_t i = 0; i < lightShaders.size(); i++)
		{
			lightShaderbloc &bloc = lightShaders[i];
			bloc.uniform = uniform;
			bloc.bufferOffset = 0;
		}
		return nb_shaders;
	}

	if (NULL != buffer)
	{
		for (size_t i = 0; i < lightShaders.size(); i++)
		{
			lightShaderbloc &bloc = lightShaders[i];
			CShader *shader = bloc.shader;

			CLight::R_LightProducts &products = buffer[bloc.index];

			COpenGLShaderStage *stage = shader->glGetOpenGLShader();
			CMaterial *M = shader->getMaterial();
			
			for (int j = 0, numl = 0; (j < CLightAttributes::MAX_LIGHTS) && (numl < 5); j++)
			{
				products.lights[j].enable = shader_false;
				
				CLight *pLight = effectiveLights[j];
				if (NULL != pLight)
				{
					CLight::R_LightProduct& lp = products.lights[numl++];
					lp = pLight->computeLightProduct(*M);
				}
			}
			products.scene_ambient = shader->getAmbient();
			products.shininess = M->getShininess();

			size_t size = sizeof(CLight::R_LightProducts);
			bloc.uniform = uniform;
			bloc.bufferOffset = bloc.index * size;

			nb_shaders++;
		}
	}

	return nb_shaders;
}

void C3DSceneObject::glRenderLights(GLboolean proceedLights,const std::vector<CLight*> &lights)
{
	if (proceedLights)
	{
		std::vector<CLight*>::const_iterator it = lights.begin();
		while (it != lights.end())
		{
			CLight *pLight = (*it++);
			if (pLight->isActive())
			{
				int lightPos = 0;
				while ((lightPos < CLightAttributes::MAX_LIGHTS) &&
					(pLight != effectiveLights[lightPos++]));
				if (lightPos == CLightAttributes::MAX_LIGHTS)
					pLight->glDeActivate();
			}
		}

		for (unsigned int i = 0; i < CLightAttributes::MAX_LIGHTS; i++)
		{
			CLight *pLight = effectiveLights[i];
			if (pLight != NULL)
			{
				if (!pLight->isActive())
					pLight->glActivate(true);
				pLight->glRender();
			}
			else
				break;
		}

		CLightAttributes::setLightOrder(effectiveLights);
	}
	else
	{
		CLight *noLights[CLightAttributes::MAX_LIGHTS] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		CLightAttributes::setLightOrder(noLights);
	}
}

void C3DSceneObject::glRenderBBoxOcclusion(unsigned int passNumber)
{
#if defined(GL_ARB_occlusion_query)
	const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();
	GLuint query = visibilityQuery[passNumber];

	pExtensions->glBeginQueryARB(GL_SAMPLES_PASSED_ARB,query);

    passVisibility[passNumber] = 0;

	//	Render the filled bbox with shader and bindings done in 3DSceneAttributes.
	CObject3D *obj = object.ptr<CObject3D>();
	obj->glRenderBBox(CObject3D::RAW);

	pExtensions->glEndQueryARB(GL_SAMPLES_PASSED_ARB);
#endif
}

bool C3DSceneObject::glRenderPass(	unsigned int passNumber,
									const vector<CLight*> &lights,
									GLboolean proceedLights)
{
	bool ret = true;
	bool use_gl_lights = true;
	CObject3D *obj = object.ptr<CObject3D>();
	
	for (size_t i = 0; i < lightShaders.size(); i++)
	{
		use_gl_lights = false;

		lightShaderbloc &bloc = lightShaders[i];
		CShader *shader = bloc.shader;
		CShaderBloc *B = shader->glGetShaderBloc();

		B->glvkSetUniformBuffer(bloc.uniform, sizeof(CLight::R_LightProducts), bloc.bufferOffset);
	}

	if (use_gl_lights)
		glRenderLights(proceedLights, lights);

#if defined(GL_ARB_occlusion_query)
    GLuint query = visibilityQuery[passNumber];

    if (query == 0)
    {
        obj->glClipRender();
        return ret;
    }
	
    const CRaptorGLExtensions *const pExtensions = Raptor::glGetExtensions();

    if (passVisibility[passNumber] >= 0)
	{
	    pExtensions->glGetQueryObjectivARB(query,GL_QUERY_RESULT_ARB,&passVisibility[passNumber]);
	}

    if (passVisibility[passNumber] > 0)
    {
		pExtensions->glBeginQueryARB(GL_SAMPLES_PASSED_ARB,query);
    	obj->glClipRender();
		pExtensions->glEndQueryARB(GL_SAMPLES_PASSED_ARB);

#ifdef RAPTOR_DEBUG_MODE_GENERATION
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        obj->glRenderBBox(CObject3D::WIREFRAME);
#endif
    }
	else
		ret = false;
	
#else
     obj->glClipRender();
#endif

     CATCH_GL_ERROR

	 return ret;
}


void C3DSceneObject::selectLights(const vector<CLight*> &lights,const CGenericMatrix<float>& transform)
{
	if (lights.size() == 0)
	{
		for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
			effectiveLights[i] = NULL;

		return;
	}

	CObject3D *obj = object.ptr<CObject3D>();
    const CBoundingBox * const bbox = obj->boundingBox();
    
    GL_COORD_VERTEX r_max;
    GL_COORD_VERTEX r_min;
    GL_COORD_VERTEX center;

	GL_MATRIX Transform;
	memcpy(Transform, transform.matrix(), sizeof(GL_MATRIX));
	bbox->get(r_min, r_max, Transform);
    bbox->getCenter(center);
	CGenericVector<float> c = CGenericVector<float>(center);
    c = c * transform; // T is transposed

    multiset<CLightObserver::lightCompare,CLightObserver::lightCompare>	sortedLights;
    vector<CLight*>::const_iterator itl = lights.begin();
    while (itl != lights.end())
    {
		CLightObserver::lightCompare lc;
        lc.light = (*itl++);

        // light position in modelview space
        const CGenericVector<float>& xx = lc.light->getLightViewPosition();
        
        //  Compare square distances instead of square roots : faster
        float dmax = lc.light->getLightDMax();
        dmax *= dmax;
		lc.intensity = lc.light->getLightAttenuation(c);
		float dd = 0.0f;

#if defined(RAPTOR_SSE_CODE_GENERATION)
		__m128 x = _mm_loadu_ps(xx.vector());
		__m128 xmin = _mm_sub_ps(_mm_loadu_ps(r_min),x);
		__m128 xmax = _mm_sub_ps(_mm_loadu_ps(r_max),x);
		xmin = _mm_min_ps(_mm_mul_ps(xmin,xmin),_mm_mul_ps(xmax,xmax));
		xmax = _mm_shuffle_ps(xmin,xmin,_MM_SHUFFLE(0,3,2,1)/*0x39*/);
		x = _mm_shuffle_ps(xmin,xmin,_MM_SHUFFLE(0,1,3,2)/*0x1e*/);
		_mm_store_ss(&dd,_mm_add_ps(x,_mm_add_ps(xmax,xmin)));
#else
		float xm = (xx.X() - r_min.x); xm *= xm;
        float ym = (xx.Y() - r_min.y); ym *= ym;
		float zm = (xx.Z() - r_min.z); zm *= zm;

		float Xm = (xx.X() - r_max.x); Xm *= Xm;
		float Ym = (xx.Y() - r_max.y); Ym *= Ym;
		float Zm = (xx.Z() - r_max.z); Zm *= Zm;

		dd = MIN(xm,Xm) + MIN(ym,Ym) + MIN(zm,Zm);
#endif
		if (dd < dmax)
            sortedLights.insert(lc);
		else
		{
#if defined(RAPTOR_SSE_CODE_GENERATION)
			__m128 d = _mm_loadu_ps(xx.vector());
			d = _mm_sub_ps(d,_mm_loadu_ps(c.vector()));
			d = _mm_mul_ps(d,d);
			__m128 d2 = _mm_shuffle_ps(d,d,_MM_SHUFFLE(0,3,2,1)/*0x39*/);
			__m128 d3 = _mm_shuffle_ps(d,d,_MM_SHUFFLE(0,1,3,2)/*0x1e*/);
			_mm_store_ss(&dd,_mm_add_ps(d,_mm_add_ps(d2,d3)));
#else
			dd = (xx.X() - c.X())*(xx.X() - c.X()) + (xx.Y() - c.Y())*(xx.Y() - c.Y()) + (xx.Z() - c.Z())*(xx.Z() - c.Z());
#endif
			if (dd < dmax)
				sortedLights.insert(lc);
		}
    }

    unsigned int flight = 0;
	multiset<CLightObserver::lightCompare, CLightObserver::lightCompare>::const_iterator it = sortedLights.begin();
	while (it != sortedLights.end() && flight < CLightAttributes::MAX_LIGHTS)
        effectiveLights[flight++] = (*it++).light;

	while (flight<CLightAttributes::MAX_LIGHTS)
        effectiveLights[flight++] = 0;
}
