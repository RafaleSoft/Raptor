// RaptorDisplay.cpp: implementation of the CRaptorDisplay class.
//
//////////////////////////////////////////////////////////////////////
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
#ifndef __GLOBAL_H__
	#include "Global.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
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
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_RAPTORIO_H__87D52C27_9117_4675_95DC_6AD2CCD2E78D__INCLUDED_)
	#include "System/RaptorIO.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
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
	:CPersistence(id,name)
{
	m_pRootScene = new C3DScene("ROOT_SCENE");
	m_pViewPoint = createViewPoint();
	m_pProperties = new CRenderingProperties();
    m_pProperties->clear(CGL_RGBA|CGL_DEPTH);
	m_pProperties->setMultisampling(CRenderingProperties::DISABLE);

	m_bDeleteViewPoint = true;
    m_bApplyViewPointModel = true;

    m_pRootScene->registerDestruction(this);
	m_pScenes.push_back(m_pRootScene);

    m_pViewPoint->registerDestruction(this);
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
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
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
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "Raptor Display cannot find the scene in call to SceneScene!");
	}
#endif

	return selected;
}

void CRaptorDisplay::addSubDisplay(CRaptorDisplay *pDisplay)
{
    if (pDisplay != NULL)
    {
        m_pSubDisplays.push_back(pDisplay);
    }
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
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_WARNING,
											               "Raptor Display cannot setViewPoint because it is not bound!");
		}
#endif

		CATCH_GL_ERROR
	}
}

bool CRaptorDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle != CGL_NULL)
	{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		if ((m_pCurrentDisplay != NULL) && (m_pCurrentDisplay != this))
		{
			Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                           CRaptorErrorManager::RAPTOR_WARNING,
											               "Another RaptorDisplay is already bound !");
		}
#endif
		m_pCurrentDisplay = this;

        Global::GetInstance().getCurrentStatus().iRenderedObjects = 0;
		Global::GetInstance().getCurrentStatus().iRenderedTriangles = 0;
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
		    display->glUnBindDisplay();
        }
    }

	return true;
}

bool CRaptorDisplay::glUnBindDisplay(void)
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
	if ((handle.handle == 0) || (handle.hClass == 0))
	{
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Invalid Raptor Handle in call to generic Render !");
		return;
	}
#endif

	if (handle.hClass == Global::COpenGLClassID::GetClassId().ID())
	{
		if (glIsList(handle.handle))
			glCallList(handle.handle);
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
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
			getRenderingProperties()->importObject(io);
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
	Global::GetInstance().setDefaultConfig(da);

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
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
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
		Raptor::GetErrorManager()->generateRaptorError(Global::COpenGLClassID::GetClassId(),
                                                       CRaptorErrorManager::RAPTOR_WARNING,
										               "None or another RaptorDisplay is already bound !");
	}
#endif

	return state.glApplyConfig(query);
}

