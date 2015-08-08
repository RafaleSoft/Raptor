// 3DSceneObject.cpp: implementation of the C3DSceneObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Subsys/CodeGeneration.h"

#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "3DSceneObject.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
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

#include <set>      // to sort the lights

RAPTOR_NAMESPACE

//!    Default pass is the full pass because single pass rendering needs all functionnalities
C3DSceneObject::PASS_KIND   C3DSceneObject::m_currentPass = FULL_PASS;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3DSceneObject::C3DSceneObject()
	:z_span(-FLT_MAX), z_order(-FLT_MAX)
{
	for (unsigned int i = 0; i<NB_PASSES; i++)
	{
		visibilityQuery[i] = 0;
		passVisibility[i] = -1;
	}

	for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
         effectiveLights[i] = NULL;
}

C3DSceneObject::~C3DSceneObject()
{

}

void C3DSceneObject::glRenderLights(const vector<CLight*> &lights)
{
    vector<CLight*>::const_iterator it = lights.begin();
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

	for (unsigned int i=0;i<CLightAttributes::MAX_LIGHTS;i++)
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

void C3DSceneObject::glRenderBBoxOcclusion(unsigned int passNumber)
{
#if defined(GL_ARB_occlusion_query)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	GLuint query = visibilityQuery[passNumber];

	pExtensions->glBeginQueryARB(GL_SAMPLES_PASSED_ARB,query);

    passVisibility[passNumber] = 0;

	CObject3D *obj = (CObject3D*)(object.handle);
    obj->glRenderBBox(true);

	pExtensions->glEndQueryARB(GL_SAMPLES_PASSED_ARB);
#endif
}

void C3DSceneObject::glComputeBBoxOcclusion(unsigned int passNumber,
											const vector<C3DSceneObject*> &occluded)
{
#if defined(GL_ARB_occlusion_query)
	//  actual values should be 'get' and then restored after bbox is rendered
    GLboolean cMask[4];
    glGetBooleanv(GL_COLOR_WRITEMASK ,cMask);
    GLboolean dMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK ,&dMask);
    GLboolean sMask;
    glGetBooleanv(GL_STENCIL_TEST ,&sMask);
    GLint dFunc;
    glGetIntegerv(GL_DEPTH_FUNC,&dFunc);
    GLboolean cFace;
    glGetBooleanv(GL_CULL_FACE ,&cFace);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

    glDepthMask(GL_FALSE);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_LESS);
	if (!cFace)
		glEnable(GL_CULL_FACE);

	vector<C3DSceneObject*>::const_iterator itr = occluded.begin();
	while (itr != occluded.end())
	{
        C3DSceneObject* sc = *itr++;
		sc->glRenderBBoxOcclusion(passNumber);
	}

    glDepthMask(dMask);
    glColorMask(cMask[0],cMask[1],cMask[2],cMask[3]);
    if (sMask)
        glEnable(GL_STENCIL_TEST);
    glDepthFunc(dFunc);
	if (!cFace)
        glDisable(GL_CULL_FACE);
#endif
}

bool C3DSceneObject::glRenderPass(	unsigned int passNumber,
									const vector<CLight*> &lights,
									GLboolean proceedLights)
{
	if (proceedLights)
		glRenderLights(lights);

	bool ret = true;
    CObject3D *obj = (CObject3D*)(object.handle);

#if defined(GL_ARB_occlusion_query)
    GLuint query = visibilityQuery[passNumber];

    if (query == 0)
    {
        obj->glClipRender();
        return ret;
    }
	
    const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

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
        obj->glRenderBBox(false);
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

    CObject3D *obj = (CObject3D*)(object.handle);
    const CBoundingBox * const bbox = obj->boundingBox();
    
    GL_COORD_VERTEX r_max;
    GL_COORD_VERTEX r_min;
    GL_COORD_VERTEX center;

	GL_MATRIX T;
	memcpy(T,transform.matrix(),sizeof(GL_MATRIX));
    bbox->get(r_min,r_max,T);
    bbox->getCenter(center);
    CGenericVector<float> c = center;
    c *= transform;

    multiset<CLightObserver::lightCompare,CLightObserver::lightCompare>	sortedLights;

    vector<CLight*>::const_iterator itl = lights.begin();
    while (itl != lights.end())
    {
		CLightObserver::lightCompare lc;
        lc.light = (*itl++);

        // light position in eye space
        const CGenericVector<float>& xx = lc.light->getLightEyePosition();
        
        //  Compare square distances instead of square roots : faster
        float dmax = lc.light->getLightDMax();
        dmax *= dmax;
		lc.intensity = lc.light->getLightAttenuation(c);
        
		float dd;

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
