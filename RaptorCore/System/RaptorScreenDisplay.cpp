// RaptorScreenDisplay.cpp: implementation of the CRaptorScreenDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORSCREENDISPLAY_H__D3165157_E39B_4770_990F_26D44A7BD1A3__INCLUDED_)
	#include "RaptorScreenDisplay.h"
#endif

#ifndef __GLOBAL_H__
	#include "Global.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "Engine/TimeObject.h"
#endif
#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/ViewPoint.h"
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
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
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
    :CRaptorDisplay(bufferID,pcs.caption)
{
	fps = 0.0f;
	ftime = 0.0f;
	rtfps = 0.0f;
	rtime = 0.0f;

	m_context = -1;
	m_layerContext = -1;

	nbFramesPerSecond = 0;
	m_framerate = 0;
	lastfreq = 0;

	cs = pcs;

    m_pGAllocator = NULL;
    m_pGOldAllocator = NULL;
	m_pTAllocator = NULL;
    m_pTOldAllocator = NULL;
}

CRaptorScreenDisplay::~CRaptorScreenDisplay()
{
    if (m_pGOldAllocator != NULL)
        CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
    if (m_pGAllocator != NULL)
        delete m_pGAllocator;

	if (m_pTOldAllocator != NULL)
        CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
    if (m_pTAllocator != NULL)
        delete m_pTAllocator;

	glUnBindDisplay();
	
	CContextManager::GetInstance()->glDestroyContext(m_context);
}

bool CRaptorScreenDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
    state.x = cs.x;
    state.y = cs.y;
    state.width = cs.width;
    state.height = cs.height;
    state.caption = cs.caption;
    state.refresh_rate = cs.refresh_rate;
    state.display_mode = cs.display_mode;
    state.status_bar = cs.status_bar;
	state.draw_logo = cs.draw_logo;

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

bool CRaptorScreenDisplay::glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height,unsigned char* &data,unsigned int& size) const
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

bool CRaptorScreenDisplay::glBlit(	unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
									unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
									CRaptorDisplay *pDst) const
{
	return false;
}

void CRaptorScreenDisplay::glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy)
{
	const CRaptorDisplay *const self = this;

    if ((sx == 0) || (sy == 0))
    {
		Raptor::GetErrorManager()->generateRaptorError(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														CRaptorMessages::ID_UPDATE_FAILED);
        return;
    }

	if (CRaptorDisplay::GetCurrentDisplay() == self)
	{
		glViewport(ox,oy,sx,sy);
		C3DEngine::Get3DEngine()->setClip(ox,oy,sx,sy);

		CViewPoint *pVp = getViewPoint();
		if (pVp != NULL)
			pVp->glRenderViewPointModel();
	}
}

RAPTOR_HANDLE CRaptorScreenDisplay::getCurrentDevice(void) const
{
    RAPTOR_HANDLE device;

    if (m_context >= 0)
    {
        device = CContextManager::GetInstance()->getDevice(m_context);
    }

    return device;
}

bool CRaptorScreenDisplay::glBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle != CGL_NULL)
	{
		if (m_context == -1)
		{
            // last chance to get some valid display atributes
			if (cs.display_mode == CGL_NULL)
				cs = Global::GetInstance().getDefaultConfig();

			//	Initialise frame rate management
			l1 = CTimeObject::GetGlobalTime();
			m_framerate = cs.refresh_rate.fps;
			if (cs.refresh_rate.sync_to_monitor)
				m_framerate = 0;

			if ((cs.display_mode & CGL_SOFTWARE) == CGL_SOFTWARE)
			{
				m_context = CContextManager::GetInstance()->glCreateContext(device, cs.display_mode);

				if (cs.display_mode & CGL_OVERLAY)
				{
					int res2 = CContextManager::GetInstance()->glCreateContext(device, cs.display_mode);
					if (res2 == -1)
					{
						Raptor::GetErrorManager()->generateRaptorError(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
																		CRaptorErrorManager::RAPTOR_FATAL,
																		CRaptorMessages::ID_CREATE_FAILED);
					}
				}
			}
			else
			{
				// create the extended rendering context if possible
				m_context = CContextManager::GetInstance()->glCreateExtendedContext(device, cs.display_mode);
				if (m_context == -1)
				{
					Raptor::GetErrorManager()->generateRaptorError(	CRaptorDisplay::CRaptorDisplayClassID::GetClassId(),
																	CRaptorErrorManager::RAPTOR_FATAL,
																	CRaptorMessages::ID_CREATE_FAILED);
					return false;
				}
			}

			CContextManager::GetInstance()->glMakeCurrentContext(device,m_context);

			
			//  Display shading configuration
			cs.glApplyConfig(	GL_PIXEL_STATE_QUERY|
								GL_HINT_STATE_QUERY|
								GL_LIGHT_STATE_QUERY|
								GL_FRAME_STATE_QUERY);

            bool res = CRaptorDisplay::glBindDisplay(device);
			//glViewport(cs.x,cs.y,cs.width,cs.height);
			glViewport(0,0,cs.width,cs.height); // Viewport is relative to window !!!
			
			bool hasSwapControl = CContextManager::GetInstance()->glSwapVSync(m_framerate);

			//	Manage vertex/pixel buffer objects.
			allocateResources();

            return res;
		}
        else
        {
		    CContextManager::GetInstance()->glMakeCurrentContext(device,m_context);

            m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(m_pGAllocator);
            if ((m_pGOldAllocator != m_pGAllocator) && (m_pGOldAllocator != NULL))
                m_pGOldAllocator->glLockMemory(false);

			m_pTOldAllocator = CTexelAllocator::SetCurrentInstance(m_pTAllocator);
            if ((m_pTOldAllocator != m_pTAllocator) && (m_pTOldAllocator != NULL))
                m_pTOldAllocator->glLockMemory(false);
        }
	}

    CATCH_GL_ERROR

	return CRaptorDisplay::glBindDisplay(device);
}

void CRaptorScreenDisplay::allocateResources(void)
{
    //  Ensure no current allocator.
    m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(NULL);
	m_pTOldAllocator = CTexelAllocator::SetCurrentInstance(NULL);

    m_pGAllocator = CGeometryAllocator::GetInstance();
    m_pTAllocator = CTexelAllocator::GetInstance();

    const CRaptorConfig& config = Global::GetInstance().getConfig();
    if (config.m_bRelocation)
    {
		bool relocResource = true;

		if ((config.m_uiPolygons > 0) || (config.m_uiVertices > 0))
		{
			relocResource = m_pGAllocator->glUseMemoryRelocation();
			if (relocResource)
				relocResource = m_pGAllocator->glInitMemory(config.m_uiPolygons,config.m_uiVertices);
			if (!relocResource)
			{
				Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
																CRaptorErrorManager::RAPTOR_FATAL,
	    		        										CRaptorMessages::ID_NO_RESOURCE);
			}
		}

		if (config.m_uiTexels > 0)
		{
			relocResource = m_pTAllocator->glUseMemoryRelocation();
			if (relocResource)
				relocResource = m_pTAllocator->glInitMemory(config.m_uiTexels);
			if (!relocResource)
			{
				Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
																CRaptorErrorManager::RAPTOR_FATAL,
	    		        										CRaptorMessages::ID_NO_RESOURCE);
			}
		}
    }
	else
	{
		bool relocResource = true;
		if ((config.m_uiPolygons > 0) || (config.m_uiVertices > 0))
			relocResource &= m_pGAllocator->glInitMemory(config.m_uiPolygons,config.m_uiVertices);
		if (config.m_uiTexels > 0)
			relocResource &= m_pTAllocator->glInitMemory(config.m_uiTexels);

		if (!relocResource)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_FATAL,
    		        										CRaptorMessages::ID_NO_RESOURCE);
		}
	}

    //! As the newly created context is made current, we must keep the
    //! associated allocator as current, otherwise it will not be used until
    //! a UnBind/Bind sequence is performed, which is unnecessary.
    //CGeometryAllocator::SetCurrentInstance(oldAllocator);
    if ((m_pGOldAllocator != m_pGAllocator) && (m_pGOldAllocator != NULL))
        m_pGOldAllocator->glLockMemory(false);
	if ((m_pTOldAllocator != m_pTAllocator) && (m_pTOldAllocator != NULL))
        m_pTOldAllocator->glLockMemory(false);
}

bool CRaptorScreenDisplay::glUnBindDisplay(void)
{
    CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
    m_pGOldAllocator = NULL;
	CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
    m_pTOldAllocator = NULL;

    CRaptorDisplay::glUnBindDisplay();

	RAPTOR_HANDLE device;
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
	if (m_context != -1)
	{
		CTimeObject::markTime(this);

        m_pGAllocator->glLockMemory(true);
		m_pTAllocator->glLockMemory(true);

		glRenderScene();

        m_pGAllocator->glLockMemory(false);
		m_pTAllocator->glLockMemory(false);

		rtime = CTimeObject::deltaMarkTime(this);

        if (Global::GetInstance().getConsole() != NULL)
		    Global::GetInstance().getConsole()->glRender();

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

