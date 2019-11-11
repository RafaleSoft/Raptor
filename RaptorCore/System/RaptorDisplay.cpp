/***************************************************************************/
/*                                                                         */
/*  RaptorDisplay.cpp                                                      */
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

#if !defined(AFX_RAPTORDISPLAY_H__9637BC66_3734_43A8_A130_87553D4379BC__INCLUDED_)
	#include "RaptorDisplay.h"
#endif
#if !defined(AFX_CONTEXTMANAGER_H__F992F5F0_D8A5_475F_9777_B0EB30E7648E__INCLUDED_)
	#include "Subsys/ContextManager.h"
#endif
#if !defined(AFX_RAPTORGLEXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorGLExtensions.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_OPENGLVIEWPOINT_H__94BDC36B_27AB_41FC_848E_DD28D1BDFC13__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLViewPoint.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_OBJECT3D_H__DB24F017_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "GLHierarchy/Object3D.h"
#endif
#if !defined(AFX_OPENGLRENDERINGPROPERTIES_H__1F0F1E67_FC84_4772_A6EE_923BD81F91D3__INCLUDED_)
	#include "Subsys/OpenGL/OpenGLRenderingProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_RAPTORINSTANCE_H__90219068_202B_46C2_BFF0_73C24D048903__INCLUDED_)
	#include "Subsys/RaptorInstance.h"
#endif


//////////////////////////////////////////////////////////////////////
// Static data
//////////////////////////////////////////////////////////////////////

RAPTOR_NAMESPACE_BEGIN

CRaptorDisplay	*CRaptorDisplay::m_pCurrentDisplay = NULL;

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

CRaptorDisplay * const CRaptorDisplay::GetCurrentDisplay(void) 
{ 
    return m_pCurrentDisplay; 
}

static CRaptorDisplay::CRaptorDisplayClassID displayID;
const CPersistence::CPersistenceClassID& CRaptorDisplay::CRaptorDisplayClassID::GetClassId(void)
{
	return displayID;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorDisplay::CRaptorDisplay(const CPersistence::CPersistenceClassID& id,const std::string& name)
	:CPersistence(id, name), 
	m_bDeleteViewPoint(true), m_bApplyViewPointModel(true),
	m_pRootScene(NULL), m_pViewPoint(NULL), m_pProperties(NULL)
{
	m_pRootScene = new C3DScene("ROOT_SCENE");
	m_pRootScene->registerDestruction(this);
	m_pScenes.push_back(m_pRootScene);

	m_pViewPoint = createViewPoint();
	m_pViewPoint->registerDestruction(this);

	m_pProperties = new COpenGLRenderingProperties();
    m_pProperties->clear(CGL_RGBA|CGL_DEPTH);
	m_pProperties->setMultisampling(IRenderingProperties::DISABLE);
}

CRaptorDisplay::~CRaptorDisplay()
{
    if (m_pProperties != NULL)
        delete m_pProperties;
	if (m_pRootScene != NULL)
		delete m_pRootScene;
	if ((m_bDeleteViewPoint) && (m_pViewPoint != NULL))
		delete m_pViewPoint;
}

IRenderingProperties *const CRaptorDisplay::createRenderingProperties(void) const
{
	return new COpenGLRenderingProperties();
}

IViewPoint *const CRaptorDisplay::createViewPoint(void) const
{
	if (NULL != m_pRootScene)
		return new COpenGLViewPoint(m_pRootScene->getName() + "_VIEWPOINT");
	else
		return new COpenGLViewPoint("RAPTORDISPLAY_VIEWPOINT");
}

void CRaptorDisplay::glReleaseResources(void)
{
    //!  Should place destructor body here,
    //! it would be better, allowing to catch errors
}

void CRaptorDisplay::unLink(const CPersistence* obj)
{
    if (static_cast<CPersistence*>(m_pRootScene) == obj)
    {
        m_pRootScene = NULL;
    }
    else if (static_cast<CPersistence*>(m_pViewPoint) == obj)
    {
        m_pViewPoint = NULL;
    }
}

IViewPoint *const CRaptorDisplay::getViewPoint(void) const
{
	return m_pViewPoint; 
}

IRenderingProperties &CRaptorDisplay::getRenderingProperties(void) const
{
	return *m_pProperties;
}

void CRaptorDisplay::addScene(C3DScene* const scene )
{
	bool duplicate = false;
	unsigned int pos= 0;
	while ((pos<m_pScenes.size()) && (!duplicate))
	{
		duplicate = (scene == m_pScenes[pos++]);
	}
	if (!duplicate)
    {
        scene->registerDestruction(this);
		m_pScenes.push_back(scene);
    }
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
		Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "Raptor Display already manages the scene in call to addScene!");
	}
#endif
}

bool CRaptorDisplay::selectScene( const std::string& sname)
{
	bool selected = false;
	unsigned int pos= 0;
	while ((pos<m_pScenes.size()) && (!selected))
	{
		C3DScene *pScene = m_pScenes[pos++];
		if (pScene->getName() == sname)
		{
			selected = true;
			m_pRootScene = pScene;
		}
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (!selected)
	{
		Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "Raptor Display cannot find the scene in call to SceneScene!");
	}
#endif

	return selected;
}

void CRaptorDisplay::addSubDisplay(CRaptorDisplay *pDisplay)
{
    if (NULL != pDisplay)
        m_pSubDisplays.push_back(pDisplay);
}

void CRaptorDisplay::setRenderingProperties(IRenderingProperties *properties)
{
	if (NULL != m_pProperties)
		delete m_pProperties;
	m_pProperties = properties;
}

void CRaptorDisplay::setViewPoint(IViewPoint *viewPoint)
{
	if (m_pViewPoint != viewPoint)
	{
		if ((m_bDeleteViewPoint) && (m_pViewPoint != NULL))
		{
			m_bDeleteViewPoint = false;
			delete m_pViewPoint;
		}

		m_pViewPoint = viewPoint;
	}

	if (m_pViewPoint)
	{
#if defined(RAPTOR_SMP_CODE_GENERATION)
		m_pViewPoint->prioritize();
#endif
		if (m_pCurrentDisplay != NULL)
            m_bApplyViewPointModel = true;
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_WARNING,
											               "Raptor Display cannot setViewPoint because it is not bound!");
		}
#endif

		CATCH_GL_ERROR
	}
}

bool CRaptorDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle() != CGL_NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if ((m_pCurrentDisplay != NULL) && (m_pCurrentDisplay != this))
		{
			Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_WARNING,
											               "Another RaptorDisplay is already bound !");
		}
#endif
		m_pCurrentDisplay = this;

		CRaptorInstance &instance = CRaptorInstance::GetInstance();
        instance.iRenderedObjects = 0;
		instance.iRenderedTriangles = 0;
	}

	if (m_pViewPoint != NULL)
    {
        if (m_bApplyViewPointModel)
        {
            m_pViewPoint->glvkRenderViewPointModel();
            m_bApplyViewPointModel = false;
        }
		m_pViewPoint->glvkRender();
    }
    else
		C3DEngine::Get3DEngine()->glConfigureEngine(NULL);

	if (m_pProperties != NULL)
        m_pProperties->glPushProperties();

    if (m_pSubDisplays.size() > 0)
    {
        RAPTOR_HANDLE _device;
        vector<CRaptorDisplay*>::const_iterator it = m_pSubDisplays.begin();
        while (it != m_pSubDisplays.end())
        {
            CRaptorDisplay* display = (*it++);
			display->glvkBindDisplay(_device);
            display->glRender();
		    display->glvkUnBindDisplay();
        }
    }

	return true;
}

bool CRaptorDisplay::glvkUnBindDisplay(void)
{
    if (m_pProperties != NULL)
       m_pProperties->glPopProperties();

	if (m_pCurrentDisplay == this)
	{
		m_pCurrentDisplay = NULL;
		return true;
	}
	else
		return false;
}

void CRaptorDisplay::glRender(const RAPTOR_HANDLE& handle)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if ((handle.handle() == 0) || (handle.hClass() == 0))
	{
		RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
						"Invalid Raptor Handle in call to generic Render !");
		return;
	}
#endif

	if (handle.hClass() == COpenGL::COpenGLClassID::GetClassId().ID())
	{
		if (glIsList(handle.handle()))
			glCallList(handle.handle());
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			RAPTOR_WARNING(	COpenGL::COpenGLClassID::GetClassId(),
							"Unsupported Raptor Handle in call to generic Render !");
#endif
		}
	}
}


unsigned int CRaptorDisplay::getGenerateWidth(void) const 
{
    CRaptorDisplayConfig state;

    //! This should not call gl ... with only config query
    if (glQueryStatus(state,GL_CONFIG_STATE_QUERY))
        return state.width;
    else
        return 0; 
}


unsigned int CRaptorDisplay::getGenerateHeight(void) const 
{ 
    CRaptorDisplayConfig state;

    //! This should not call gl ... with only config query
    if (glQueryStatus(state,GL_CONFIG_STATE_QUERY))
        return state.height;
    else
        return 0; 
}

//////////////////////////////////////////////////////////////////////
//
//	Display persistence implementation
//
//////////////////////////////////////////////////////////////////////
bool CRaptorDisplay::exportObject(CRaptorIO& o)
{
    return true;
}

bool CRaptorDisplay::importObject(CRaptorIO& io)
{
    string name;
	CRaptorDisplayConfig da;

	io >> name; 
	string data = io.getValueName();

	while (!data.empty())
	{
		if (data == "height")
			io >> da.height;
		else if (data == "width")
			io >> da.width;
		else if (data == "x")
			io >> da.x;
		else if (data == "y")
			io >> da.y;
		else if (data == "display_mode")
		{
			int mode;
			io >> mode;
			da.display_mode |= mode;
		}
		else if (data == "RenderingProperties")
			getRenderingProperties().importObject(io);
		else if (data == "Scene")
			m_pRootScene->importObject(io);
		else if (data == "ViewPoint")
        {
			getViewPoint()->importObject(io);
            setViewPoint(getViewPoint());
        }
	   else
			io >> name;

		data = io.getValueName();
	} 
	io >> name; 

	da.caption = "Raptor Server";
	da.display_mode = CGL_RGBA | CGL_DEPTH;
	da.acceleration = CRaptorDisplayConfig::SOFTWARE;
	da.depth_buffer = true;
	da.double_buffer = true;
	CRaptorInstance &instance = CRaptorInstance::GetInstance();
	instance.setDefaultConfig(da);

	return true;
}



//////////////////////////////////////////////////////////////////////
//
//	Open GL status state management
//
//////////////////////////////////////////////////////////////////////
bool CRaptorDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (((m_pCurrentDisplay == NULL) || (m_pCurrentDisplay != this)) &&
		 (GL_CONFIG_STATE_QUERY != query))
	{
		Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "None or another RaptorDisplay is already bound !");
	}
#endif

	return state.glQueryConfig(query);
}

bool CRaptorDisplay::glApplyStatus(const CRaptorDisplayConfig& state,unsigned long query)
{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	if (((m_pCurrentDisplay == NULL) || (m_pCurrentDisplay != this)) &&
		(GL_CONFIG_STATE_QUERY != query))
	{
		Raptor::GetErrorManager()->generateRaptorError(COpenGL::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "None or another RaptorDisplay is already bound !");
	}
#endif

	return state.glApplyConfig(query);
}

