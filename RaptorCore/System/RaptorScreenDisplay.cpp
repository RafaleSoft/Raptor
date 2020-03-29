/***************************************************************************/
/*                                                                         */
/*  RaptorScreenDisplay.cpp                                                */
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

#if !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)
	#include "RaptorScreenDisplay.h"
#endif
#if !defined(AFX_RAPTORDATAMANAGER_H__114BFB19_FA00_4E3E_879E_C9130043668E__INCLUDED_)
	#include "DataManager/RaptorDataManager.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "Engine/TimeObject.h"
#endif
#if !defined(AFX_IVIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/IViewPoint.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_UNIFORMALLOCATOR_H__4DD62C99_E476_4FE5_AEE4_EEC71F7B0F38__INCLUDED_)
	#include "Subsys/UniformAllocator.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif
#if !defined(AFX_OPENGLMEMORY_H__C344567B_877F_4F43_8961_C4E59E3BBF7E__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLMemory.h"
#endif
#if !defined(AFX_TEXTUREQUAD_H__1712AF34_6723_4E39_BC72_05ED6FA28418__INCLUDED_)
	#include "GLHierarchy/TextureQuad.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif


RAPTOR_NAMESPACE

static CRaptorScreenDisplay::CRaptorScreenDisplayClassID bufferID;
const CPersistence::CPersistenceClassID& CRaptorScreenDisplay::CRaptorScreenDisplayClassID::GetClassId(void)
{
	return bufferID;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorScreenDisplay::CRaptorScreenDisplay(const CRaptorDisplayConfig& pcs)
    :CRaptorDisplay(bufferID,pcs.caption),cs(pcs),
	fps(0.0f), ftime(0.0f), rtfps(0.0f), rtime(0.0f), pLogo(NULL),
	m_context(CContextManager::INVALID_CONTEXT),
	m_layerContext(CContextManager::INVALID_CONTEXT),
	m_framerate(0), lastfreq(0), l1(0), m_streamer(NULL),
    m_pGAllocator(NULL),m_pGOldAllocator(NULL),
	m_pTAllocator(NULL),m_pTOldAllocator(NULL),
	m_pUAllocator(NULL), m_pUOldAllocator(NULL),
	m_pDeviceMemory(NULL), nbFramesPerSecond(0)
{
}

CRaptorScreenDisplay::~CRaptorScreenDisplay()
{
	if (CContextManager::INVALID_CONTEXT != m_context)
		CContextManager::GetInstance()->glDestroyContext(m_context);
}

void CRaptorScreenDisplay::unLink(const CPersistence* obj)
{
	if (obj == static_cast<CPersistence*>(pLogo))
		pLogo = NULL;
	else
		CRaptorDisplay::unLink(obj);
}

bool CRaptorScreenDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
	state.copyBaseConfig(cs);
    return CRaptorDisplay::glQueryStatus(state,query);
}

void CRaptorScreenDisplay::glGenerate(CTextureObject* t)
{
    if ((t == NULL) || (!m_bEnabled))
        return;

    int posx;
    int posy;
    int width;
    int height;

    t->getGenerationSize(   posx, posy, width, height);

    //! Target for generated texture can only be a 2D texture.
    //! Extended features such as generating cube map faces might be supported in future versions.
    //! Nevertheless, cube textures are handled by CRaptorBufferedDisplay, which is more efficient is most cases
    glCopyTexSubImage2D(GL_TEXTURE_2D,
						t->getCurrentMipMapLevel(),
                        0,0,
						posx,	posy,
						width, height);
    CATCH_GL_ERROR
}

bool CRaptorScreenDisplay::glGrab(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
								  uint8_t* &data, size_t& size) const
{
    bool res = false;
   //! If buffer is bound, read pixels directly
    const CRaptorDisplay *pDisplay = CRaptorDisplay::GetCurrentDisplay();
    if (this == pDisplay)
    {
        int grab_x = x;
        int grab_y = y;
        int grab_w = width;
        int grab_h = height;

        if ((grab_x < 0) || (grab_x > cs.width)) grab_x = 0;
        if ((grab_y < 0) || (grab_y > cs.height)) grab_y = 0;
        if ((grab_w < 0) || (grab_w > cs.width)) grab_w = cs.width - grab_x;
        if ((grab_h < 0) || (grab_h > cs.height)) grab_h = cs.height - grab_y;

        size = grab_w*grab_h*4;
		if (data == NULL)
			data = new unsigned char[size];
        glReadPixels(grab_x,grab_y,grab_w,grab_h,GL_RGBA,GL_UNSIGNED_BYTE,data);
        res = true;
    }
    else
    {
        size = 0;
        data = NULL;
    }

    CATCH_GL_ERROR

    return res;
}

bool CRaptorScreenDisplay::glBlit(uint32_t xSrc, uint32_t ySrc, uint32_t widthSrc, uint32_t heightSrc,
								  uint32_t xDst, uint32_t yDst, uint32_t widthDst, uint32_t heightDst,
								  CRaptorDisplay *pDst) const
{
	return false;
}

void CRaptorScreenDisplay::glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy)
{
	const CRaptorDisplay *const self = this;

    if ((sx == 0) || (sy == 0))
    {
		RAPTOR_ERROR(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
						CRaptorMessages::ID_UPDATE_FAILED);
        return;
    }

	if (CRaptorDisplay::GetCurrentDisplay() == self)
	{
		glViewport(ox,oy,sx,sy);
		C3DEngine::Get3DEngine()->setClip(ox,oy,sx,sy);

		IViewPoint *pVp = getViewPoint();
		if (pVp != NULL)
			pVp->glvkRenderViewPointModel();
	}
}

RAPTOR_HANDLE CRaptorScreenDisplay::getCurrentDevice(void) const
{
    RAPTOR_HANDLE device;

    if (m_context >= 0)
        device = CContextManager::GetInstance()->getDevice(m_context);

    return device;
}

bool CRaptorScreenDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle() != 0)
	{
		if (CContextManager::INVALID_CONTEXT == m_context)
		{
            // last chance to get some valid display atributes
			if (cs.display_mode == CGL_NULL)
				cs = CRaptorInstance::GetInstance().getDefaultConfig();

			//	Initialise frame rate management
			l1 = CTimeObject::GetGlobalTime();
			m_framerate = cs.refresh_rate.fps;
			if (cs.refresh_rate.sync_to_monitor)
				m_framerate = 0;

			if (CRaptorDisplayConfig::SOFTWARE == cs.acceleration)
			{
				m_context = CContextManager::GetInstance()->glCreateContext(device, cs);

				if (cs.overlay)
				{
					int res2 = CContextManager::GetInstance()->glCreateContext(device, cs);
					if (CContextManager::INVALID_CONTEXT == res2)
					{
						RAPTOR_FATAL(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
										CRaptorMessages::ID_CREATE_FAILED);
					}
				}
			}
			else
			{
				// create the extended rendering context if possible
				m_context = CContextManager::GetInstance()->glCreateExtendedContext(device, cs);
				if (CContextManager::INVALID_CONTEXT == m_context)
				{
					RAPTOR_FATAL(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
									CRaptorMessages::ID_CREATE_FAILED);
					return false;
				}
			}

			CContextManager::GetInstance()->glMakeCurrentContext(device,m_context);

			
			//  Display shading configuration
			cs.glApplyConfig(GL_PIXEL_STATE_QUERY|GL_HINT_STATE_QUERY|GL_LIGHT_STATE_QUERY|GL_FRAME_STATE_QUERY);

			bool res = CRaptorDisplay::glvkBindDisplay(device);
			//glViewport(cs.x,cs.y,cs.width,cs.height);
			glViewport(0,0,cs.width,cs.height); // Viewport is relative to window !!!
			
			bool hasSwapControl = CContextManager::GetInstance()->glSwapVSync(m_framerate);

			//	Create rendering context resources.
			glvkAllocateResources();

            return res;
		}
        else
        {
		    CContextManager::GetInstance()->glMakeCurrentContext(device,m_context);

            m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(m_pGAllocator);
            if ((m_pGOldAllocator != m_pGAllocator) && (m_pGOldAllocator != NULL))
                m_pGOldAllocator->glvkLockMemory(false);

			m_pTOldAllocator = CTexelAllocator::SetCurrentInstance(m_pTAllocator);
            if ((m_pTOldAllocator != m_pTAllocator) && (m_pTOldAllocator != NULL))
                m_pTOldAllocator->glvkLockMemory(false);

			m_pUOldAllocator = CUniformAllocator::SetCurrentInstance(m_pUAllocator);
			if ((m_pUOldAllocator != m_pUAllocator) && (m_pUOldAllocator != NULL))
				m_pUOldAllocator->glvkLockMemory(false);
        }
	}

    CATCH_GL_ERROR

	return CRaptorDisplay::glvkBindDisplay(device);
}

void CRaptorScreenDisplay::glvkAllocateResources(void)
{
    //  Ensure no current allocator.
    m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(NULL);
	m_pTOldAllocator = CTexelAllocator::SetCurrentInstance(NULL);
	m_pUOldAllocator = CUniformAllocator::SetCurrentInstance(NULL);

    m_pGAllocator = CGeometryAllocator::GetInstance();
    m_pTAllocator = CTexelAllocator::GetInstance();
	m_pUAllocator = CUniformAllocator::GetInstance();

	CRaptorInstance &instance = CRaptorInstance::GetInstance();
    const CRaptorConfig& config = instance.config;
	bool relocResource = true;
    if (config.m_bRelocation)
    {
		m_pDeviceMemory = new COpenGLMemory();

		if ((config.m_uiPolygons > 0) || (config.m_uiVertices > 0))
		{
			relocResource = m_pGAllocator->glvkInitMemory(m_pDeviceMemory,config.m_uiPolygons,config.m_uiVertices);
			if (!relocResource)
			{
				RAPTOR_FATAL(	CGeometry::CGeometryClassID::GetClassId(),
								CRaptorMessages::ID_NO_RESOURCE);
			}
		}

		if (config.m_uiTexels > 0)
		{
			relocResource = m_pTAllocator->glvkInitMemory(m_pDeviceMemory, config.m_uiTexels);
			if (!relocResource)
			{
				RAPTOR_FATAL(	CGeometry::CGeometryClassID::GetClassId(),
								CRaptorMessages::ID_NO_RESOURCE);
			}
		}

		if (config.m_uiUniforms > 0)
		{
			relocResource = m_pUAllocator->glvkInitMemory(m_pDeviceMemory, config.m_uiUniforms);
			if (!relocResource)
			{
				RAPTOR_FATAL(	CGeometry::CGeometryClassID::GetClassId(),
								CRaptorMessages::ID_NO_RESOURCE);
			}
		}
    }
	else
	{
		if ((config.m_uiPolygons > 0) || (config.m_uiVertices > 0))
			relocResource = relocResource && m_pGAllocator->glvkInitMemory(NULL,config.m_uiPolygons,config.m_uiVertices);
		if (config.m_uiTexels > 0)
			relocResource = relocResource && m_pTAllocator->glvkInitMemory(NULL, config.m_uiTexels);
		if (config.m_uiUniforms > 0)
			relocResource = relocResource && m_pUAllocator->glvkInitMemory(NULL, config.m_uiUniforms);

		if (!relocResource)
		{
			RAPTOR_FATAL(	CGeometry::CGeometryClassID::GetClassId(),
							CRaptorMessages::ID_NO_RESOURCE);
		}
	}

    //! As the newly created context is made current, we must keep the
    //! associated allocator as current, otherwise it will not be used until
    //! a UnBind/Bind sequence is performed, which is unnecessary.
    //CGeometryAllocator::SetCurrentInstance(oldAllocator);
    if ((m_pGOldAllocator != m_pGAllocator) && (m_pGOldAllocator != NULL))
        m_pGOldAllocator->glvkLockMemory(false);
	if ((m_pTOldAllocator != m_pTAllocator) && (m_pTOldAllocator != NULL))
        m_pTOldAllocator->glvkLockMemory(false);
	if ((m_pUOldAllocator != m_pUAllocator) && (m_pUOldAllocator != NULL))
		m_pUOldAllocator->glvkLockMemory(false);

	if (instance.isInitialised())
		instance.glvkInitSharedResources();
}

void CRaptorScreenDisplay::glvkReleaseResources(void)
{
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	if (instance.isInitialised())
		instance.glvkReleaseSharedRsources();


	if (NULL != m_pGOldAllocator)
		CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
	if (NULL != m_pGAllocator)
		delete m_pGAllocator;

	if (NULL != m_pTOldAllocator)
		CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
	if (NULL != m_pTAllocator)
		delete m_pTAllocator;

	if (NULL != m_pUOldAllocator)
		CUniformAllocator::SetCurrentInstance(m_pUOldAllocator);
	if (NULL != m_pUAllocator)
		delete m_pUAllocator;

	if (NULL != m_pDeviceMemory)
		delete m_pDeviceMemory;

	if (NULL != pLogo)
	{
		pLogo->unregisterDestruction(this);
		delete pLogo;
	}

	CRaptorDisplay::glvkReleaseResources();
}

bool CRaptorScreenDisplay::glvkUnBindDisplay(void)
{
    CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
    m_pGOldAllocator = NULL;
	CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
    m_pTOldAllocator = NULL;
	CUniformAllocator::SetCurrentInstance(m_pUOldAllocator);
	m_pUOldAllocator = NULL;

	CRaptorDisplay::glvkUnBindDisplay();

	RAPTOR_HANDLE device(0, (void*)0);
	CContextManager::GetInstance()->glMakeCurrentContext(device,m_context);

	return true;
}


void CRaptorScreenDisplay::glRenderScene(void)
{
	C3DScene *pScene = getRootScene();
	pScene->glRender();

    CATCH_GL_ERROR
}

bool CRaptorScreenDisplay::glRender(void)
{
	if (CContextManager::INVALID_CONTEXT != m_context)
	{
        m_pGAllocator->glvkLockMemory(true);
		m_pTAllocator->glvkLockMemory(true);
		m_pUAllocator->glvkLockMemory(true);

		CTimeObject::markTime(this);
		glRenderScene();
		rtime = CTimeObject::deltaMarkTime(this);

		CRaptorInstance &instance = CRaptorInstance::GetInstance();
		if (instance.pConsole != NULL)
			instance.pConsole->glRender();
		
#ifdef SHAREWARE_RELEASE
		if (CRaptorInstance::GetInstance().runAsShareware)
			glDrawLogo();
#endif

        m_pGAllocator->glvkLockMemory(false);
		m_pTAllocator->glvkLockMemory(false);
		m_pUAllocator->glvkLockMemory(false);

		CContextManager::GetInstance()->glSwapBuffers(m_context);

		//	Frame rate management
		float l2 = CTimeObject::GetGlobalTime();
		float tmpfps = 0;

		ftime = l2 - l1;
		if (ftime > 0)
			tmpfps = 1.0f / ftime;

		rtfps = tmpfps;
		l1 = l2;

		if ((l2 - lastfreq) > 1.0)
		{
			fps = (nbFramesPerSecond+1) / (l2 - lastfreq);
			nbFramesPerSecond = 0;
			lastfreq = l2;
		}
		else
			nbFramesPerSecond++;

        CATCH_GL_ERROR

		return true;
	}
	else
		return false;
}


void CRaptorScreenDisplay::glDrawLogo(void)
{
	if (pLogo == NULL)
		pLogo = glBuildLogo();
	
	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pLogo->glRender();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	
	glPopAttrib();
	
	CATCH_GL_ERROR
}

CTextureQuad* CRaptorScreenDisplay::glBuildLogo(void)
{
	CRaptorDataManager  *dataManager = CRaptorDataManager::GetInstance();
	if (NULL == dataManager)
		return NULL;

	string filepath = dataManager->ExportFile("Raptor_logo_sml.txt");
	pLogo = new CTextureQuad();
	pLogo->glLoadTexture(filepath, true);
	pLogo->glSetQuadAttributes(GL_COORD_VERTEX(0.85f, -0.925f, 1.0f, 1.0f),
							   CColor::RGBA(0.6f, 0.9f, 1.0f, 0.4f),
							   GL_COORD_VERTEX(0.15f, 0.075f, 0.0f, 0.0f));

	pLogo->registerDestruction(this);

	CATCH_GL_ERROR

	return pLogo;
}
