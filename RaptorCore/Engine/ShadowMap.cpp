// ShadowMap.cpp: implementation of the CShadowMap class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_SHADOWMAP_H__996B1CFE_3445_4FB3_AE2B_D86E55BCE769__INCLUDED_)
	#include "ShadowMap.h"
#endif
#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "ViewPoint.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif
#if !defined(AFX_SHADERLIBRARY_H__E2A8C35E_23A4_4AD1_8467_884E6B183B4F__INCLUDED_)
	#include "Subsys/ShaderLibrary.h"
#endif
#if !defined(AFX_LIGHT_H__AA8BABD6_059A_4939_A4B6_A0A036E12E1E__INCLUDED_)
	#include "GLHierarchy/Light.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_FRAGMENTSHADER_H__66B3089A_2919_4678_9273_6CDEF7E5787F__INCLUDED_)
	#include "GLHierarchy/FragmentShader.h"
#endif
#if !defined(AFX_3DSCENEOBJECT_H__96A34268_AD58_4F73_B633_F6C3E92FE0A9__INCLUDED_)
	#include "Subsys/3DSceneObject.h"
#endif
#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowMap::CShadowMap(C3DScene& rScene)
    :CEnvironment(rScene),
	m_pViewPoint(NULL),m_pShadowMap(NULL),m_pShadowTexture(NULL)
{
    m_lightProperties.setTexturing(CRenderingProperties::ENABLE);
    m_lightProperties.setLighting(CRenderingProperties::ENABLE);
    m_lightProperties.clear(0);
}

CShadowMap::~CShadowMap()
{
	unLinkEnvironment();
}

void CShadowMap::unLink(const CPersistence *object)
{
    if (object == static_cast<CPersistence*>(m_pShadowMap))
        m_pShadowMap = NULL;
    else if (object == static_cast<CPersistence*>(m_pViewPoint))
        m_pViewPoint = NULL;
	else
		CEnvironment::unLink(object);
}

void CShadowMap::unLinkEnvironment(void)
{
    if (m_pShadowMap != NULL)
    {
		Raptor::glDestroyDisplay(m_pShadowMap);
        m_pShadowMap = NULL; //redundant: done at unlink
    }

	if (m_pViewPoint != NULL)
    {
		delete m_pViewPoint;
        m_pViewPoint = NULL;  //redundant: done at unlink
    }

	if (m_pShadowTexture != NULL)
    {
		m_pShadowTexture->releaseReference();
        m_pShadowTexture = NULL;
    }

	CATCH_GL_ERROR
}

void CShadowMap::addObject(C3DSceneObject* object)
{
}

bool CShadowMap::glInitEnvironment(unsigned int width,unsigned int height)
{
    CRaptorDisplay *const pDisplay = CRaptorDisplay::GetCurrentDisplay();
    CRaptorDisplayConfig state;
    bool query = pDisplay->glQueryStatus(state,GL_CONFIG_STATE_QUERY);

    if (!query || (height < state.height) || (width < state.width))
    {
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Shadow map environment is beeing passed inconsistent or invalid dimensions !");
    }

    CShaderLibrary *lib = CShaderLibrary::GetInstance();
    lib->glInitFactory();

#if defined(GL_EXT_framebuffer_object)
	if (Raptor::glIsExtensionSupported("GL_EXT_framebuffer_object"))
		glInitRenderBuffer(width,height);
	else
#endif
#if defined(WGL_ARB_pbuffer) && defined(WGL_ARB_render_texture)
	if (Raptor::glIsExtensionSupported("WGL_ARB_pbuffer") && 
		Raptor::glIsExtensionSupported("WGL_ARB_render_texture"))
		glInitPixelBuffer(width,height);
	else
#endif
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Shadow mapping is not supported, rendering will be wrong");
		return false;
    }
    
	if (NULL == m_pShadowMap)
		return false;
	m_pShadowMap->registerDestruction(m_pObserver);

	//
	// prepare light's point of view transform
	//
    CRenderingProperties *props = m_pShadowMap->getRenderingProperties();
    props->clear(CGL_DEPTH);
    props->setTexturing(CRenderingProperties::DISABLE);
    props->setLighting(CRenderingProperties::DISABLE);

	if (m_pViewPoint == NULL)
		m_pViewPoint = new CViewPoint();
	m_pViewPoint->setViewVolume(-1.33f,1.33f,-1.0f,1.0f,1.5f,100.0f,CViewPoint::PERSPECTIVE);
    m_pViewPoint->setPosition(0.0f,0.0f,0.0f,CViewPoint::EYE);
    m_pViewPoint->setPosition(0.0f,0.0f,-10.0f,CViewPoint::TARGET);
	m_pViewPoint->registerDestruction(m_pObserver);

	RAPTOR_HANDLE display;
	m_pShadowMap->glBindDisplay(display);
	m_pShadowMap->setViewPoint(m_pViewPoint);
	m_pShadowMap->glUnBindDisplay();

	return true;
}

void CShadowMap::glInitRenderBuffer(unsigned int width,unsigned int height)
{
	CRaptorDisplayConfig cs;
	cs.width = width;
	cs.height = height;
	cs.display_mode = CGL_RGBA | CGL_DEPTH;
	cs.bind_to_texture = true;
	cs.acceleration = CRaptorDisplayConfig::HARDWARE;
    cs.caption = "SHADOW_MAP_DISPLAY";
	cs.renderer = CRaptorDisplayConfig::RENDER_BUFFER;

	m_pShadowMap = Raptor::glCreateDisplay(cs);
	
	CTextureFactory &factory = CTextureFactory::getDefaultFactory();

#ifdef GL_ARB_shadow
    if (Raptor::glIsExtensionSupported("GL_ARB_shadow"))
    {
		m_pShadowTexture = factory.glCreateTexture( CTextureObject::CGL_DEPTH24,
													CTextureObject::CGL_OPAQUE,
													CTextureObject::CGL_BILINEAR);
		m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF_16X");
		//m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF_4X");
        //m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF");
        if (((m_pFSShadowMap != NULL) && (!m_pFSShadowMap->isValid())) ||
            (m_pFSShadowMap == NULL))
        {
			m_pShadowTexture->releaseReference();
            m_pShadowTexture = factory.glCreateTexture( CTextureObject::CGL_DEPTH24,
                                                        CTextureObject::CGL_OPAQUE,
														CTextureObject::CGL_UNFILTERED);
            m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER");
        }
	}
	else
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Shadow mapping is not supported, rendering will be wrong");
    }
#else
    m_pShadowTexture = factory.glCreateTexture(	CTextureObject::CGL_DEPTH24,
												CTextureObject::CGL_OPAQUE,
												CTextureObject::CGL_UNFILTERED);
    m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER");
#endif

	CTextureSet *pImageSet = new CTextureSet();
	CTextureObject *ShadowTexture = factory.glCreateTexture(CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR);

	factory.glResizeTexture(m_pShadowTexture,width,height);
	m_pShadowTexture->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
	factory.glResizeTexture(ShadowTexture,width,height);
	ShadowTexture->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
	
	pImageSet->addTexture(m_pShadowTexture);
	pImageSet->addTexture(ShadowTexture);

	m_pShadowMap->glBindDisplay(*pImageSet);

	CATCH_GL_ERROR
}

void CShadowMap::glInitPixelBuffer(unsigned int width,unsigned int height)
{
	CRaptorDisplayConfig cs;
	cs.width = width;
	cs.height = height;
	cs.display_mode = CGL_DEPTH;
	cs.bind_to_texture = true;
	cs.acceleration = CRaptorDisplayConfig::HARDWARE;
    cs.caption = "SHADOW_MAP_DISPLAY";
	cs.renderer = CRaptorDisplayConfig::PIXEL_BUFFER;

	m_pShadowMap = Raptor::glCreateDisplay(cs);
	
	CATCH_GL_ERROR

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();

#ifdef GL_ARB_shadow
    if (Raptor::glIsExtensionSupported("GL_ARB_shadow"))
    {
		m_pShadowTexture = factory.glCreateDynamicTexture( CTextureObject::CGL_DEPTH24,
                                                           CTextureObject::CGL_MULTIPLY,
                                                           CTextureObject::CGL_BILINEAR,
                                                           m_pShadowMap);
        m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF_16X");
        //m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF_4X");
        //m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER_PCF");
        if (((m_pFSShadowMap != NULL) && (!m_pFSShadowMap->isValid())) ||
            (m_pFSShadowMap == NULL))
        {
			m_pShadowTexture->releaseReference();
            m_pShadowTexture = factory.glCreateDynamicTexture( CTextureObject::CGL_DEPTH24,
                                                               CTextureObject::CGL_MULTIPLY,
                                                               CTextureObject::CGL_UNFILTERED,
                                                               m_pShadowMap);
            m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER");
        }
    }
    else
    {
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Shadow mapping is not supported, rendering will be wrong");
    }
#else
    m_pShadowTexture = factory.glCreateDynamicTexture(	CTextureObject::CGL_DEPTH24,
														CTextureObject::CGL_MULTIPLY,
														CTextureObject::CGL_UNFILTERED,
														m_pShadowMap);
    m_pFSShadowMap = (CFragmentShader*)CPersistence::FindObject("SHADOWMAP_TEX_SHADER");
#endif

    // check shader to switch to default base ARB_shadow rendering
    if (((m_pFSShadowMap != NULL) && 
		(!m_pFSShadowMap->isValid())) || (m_pFSShadowMap == NULL))
    {
        m_pFSShadowMap = NULL;
        m_pShadowTexture = factory.glCreateDynamicTexture( CTextureObject::CGL_DEPTH24,
                                                           CTextureObject::CGL_MULTIPLY,
                                                           CTextureObject::CGL_UNFILTERED,
                                                           m_pShadowMap);
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Shadow mapping cannot use shaders objects, rendering will be wrong");
    }

	CATCH_GL_ERROR
}

void CShadowMap::glRender(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
    if (!isEnabled())
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(objects);
        return;
    }

	//!	Create the shadow
    vector<C3DSceneObject*> casters;
    vector<C3DSceneObject*> receivers;
    vector<C3DSceneObject*> others;
    vector<C3DSceneObject*>::const_iterator itr = objects.begin();
    while (itr != objects.end())
    {
        C3DSceneObject* const h = *itr++;
		CObject3D* obj = h->getObject();

        bool cast = obj->getProperties().isCastShadow();
        bool receive = obj->getProperties().isReceiveShadow();
        if (cast)
            casters.push_back(h);
        if (receive)
            receivers.push_back(h);
        if (!receive && !cast)
            others.push_back(h);
    }

    //! Clipping must be evaluated with this different view point.
    CObject3D::enableEarlyClip(false);
    glRenderMap(currentLight,casters);
    CObject3D::enableEarlyClip(true);

	//! Normal rendering with shadow
    m_lightProperties.glPushProperties();
    glRenderShadow(receivers);
    m_lightProperties.glPopProperties();

    //! Render 'other' objects
    if (others.size() > 0)
    {
        C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
        glRenderObjects(others);
    }
}

void CShadowMap::glRenderMap(const CLight* currentLight,const vector<C3DSceneObject*>& objects)
{
    //  TODO : should use multiple lights of the scene !!!
	GL_COORD_VERTEX coord =  currentLight->getLightPosition();
    m_pViewPoint->setPosition(coord.x,coord.y,coord.z,CViewPoint::EYE);

	GL_COORD_VERTEX coord2 =  currentLight->getLightDirection();
	m_pViewPoint->setPosition( coord.x + coord2.x,
							   coord.y + coord2.y,
							   coord.z + coord2.z,
                               CViewPoint::TARGET);

	CGenericMatrix<float> PLight;
	CGenericMatrix<float> MLight;

	m_lightProjection.Ident();
	m_lightProjection[0] = 0.5f;
	m_lightProjection[5] = 0.5f;
	m_lightProjection[10] = 0.5f;
	m_lightProjection[3] = 0.5f;
	m_lightProjection[7] = 0.5f;
	m_lightProjection[11] = 0.5f;


	// Render to p-Buffer
	RAPTOR_HANDLE display;
	m_pShadowMap->glBindDisplay(display);
	{
		glGetTransposeFloatv(GL_PROJECTION_MATRIX,PLight);
		glGetTransposeFloatv(GL_MODELVIEW_MATRIX,MLight);

		m_lightProjection *= PLight;
		m_lightProjection *= MLight;

		glPushMatrix();

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f,5.0f);
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

		C3DSceneObject::m_currentPass = C3DSceneObject::DEPTH_PASS;
		glRenderObjects(objects);

		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glDisable(GL_POLYGON_OFFSET_FILL);

		glPopMatrix();
	}
	m_pShadowMap->glUnBindDisplay();

    CATCH_GL_ERROR
}


void CShadowMap::glRenderShadow(const vector<C3DSceneObject*>& objects)
{
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	PFN_GL_ACTIVE_TEXTURE_ARB_PROC glActiveTextureARB = pExtensions->glActiveTextureARB;

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_2D);

	if (m_pShadowTexture != NULL)
	{
		m_pShadowTexture->glRender();

		glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
		glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
		glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
		glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);

		glTexGenfv(GL_S, GL_EYE_PLANE, m_lightProjection.matrix());
		glTexGenfv(GL_T, GL_EYE_PLANE, m_lightProjection.matrix()+4);
		glTexGenfv(GL_R, GL_EYE_PLANE, m_lightProjection.matrix()+8);
		glTexGenfv(GL_Q, GL_EYE_PLANE, m_lightProjection.matrix()+12);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_GEN_Q);

#ifdef GL_ARB_shadow
		glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE_ARB,GL_LUMINANCE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);
#endif
	}

    if (m_pFSShadowMap != NULL)
        m_pFSShadowMap->glRender();

    C3DSceneObject::m_currentPass = C3DSceneObject::FULL_PASS;
	glRenderObjects(objects);

	glActiveTextureARB(GL_TEXTURE2_ARB);

	if (m_pShadowTexture != NULL)
	{
		m_pShadowMap->glUnBindDisplay();

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
#ifdef GL_ARB_shadow
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
#endif
	}

	if (m_pFSShadowMap != NULL)
        m_pFSShadowMap->glStop();

	glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}


void CShadowMap::glRenderTexture(void)
{
    glEnable(GL_TEXTURE_2D);
    m_pShadowTexture->glRender();
}


