// RaptorVulkanDisplay.cpp: implementation of the CRaptorVulkanDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORVULKANDISPLAY_H__1D39CB77_CA4D_4A8F_90FA_F9C76774CE6F__INCLUDED_)
	#include "RaptorVulkanDisplay.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_TIMEOBJECT_H__C06AC4B9_4DD7_49E2_9C5C_050EF5C39780__INCLUDED_)
	#include "Engine/TimeObject.h"
#endif
#ifndef __GLOBAL_H__
	#include "Global.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
	#include "Subsys/Vulkan/VulkanPipeline.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#if !defined(AFX_VULKANVIEWPOINT_H__08D29395_9883_45F8_AE51_5174BD6BC19B__INCLUDED_)
	#include "Subsys/Vulkan/VulkanViewPoint.h"
#endif
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
		#include "System/RaptorErrorManager.h"
	#endif
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
#if !defined(AFX_IRENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/IRenderingProperties.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static CRaptorVulkanDisplay::CRaptorVulkanDisplayClassID bufferID;
const CPersistence::CPersistenceClassID& CRaptorVulkanDisplay::CRaptorVulkanDisplayClassID::GetClassId(void)
{
	return bufferID;
}

class VulkanRP : public IRenderingProperties
{
public:
	VulkanRP() {};
	virtual void glPushProperties(void)
	{
		if ((m_pCurrent != this) && (m_pPrevious == NULL))
		{
			m_pPrevious = m_pCurrent;
			m_pCurrent = this;
		}
	};
	virtual void glPopProperties(void)
	{
		if (m_pCurrent == this)
		{
			m_pCurrent = m_pPrevious;
			m_pPrevious = NULL;
		}
	};
	virtual PROPERTY_SETTING getCurrentTexturing(void) const { return IGNORE_PROPERTY; };
	virtual PROPERTY_SETTING getCurrentLighting(void) const { return IGNORE_PROPERTY; };
};

RAPTOR_NAMESPACE_END



RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorVulkanDisplay::CRaptorVulkanDisplay(const CRaptorDisplayConfig& pcs)
	:CRaptorDisplay(bufferID,pcs.caption),
	m_context(CContextManager::INVALID_CONTEXT), cs(pcs),
	fps(0.0f),ftime(0.0f),rtfps(0.0f),rtime(0.0f),
	l1(0),lastfreq(0),nbFramesPerSecond(0),
	m_pGAllocator(NULL), m_pGOldAllocator(NULL),
	m_pTAllocator(NULL), m_pTOldAllocator(NULL),
	m_pUAllocator(NULL), m_pUOldAllocator(NULL)
{
	setViewPoint(createViewPoint());
	setRenderingProperties(new VulkanRP());
}

CRaptorVulkanDisplay::~CRaptorVulkanDisplay(void)
{
	if (m_pGOldAllocator != NULL)
		CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
	if (m_pGAllocator != NULL)
		delete m_pGAllocator;

	if (NULL != m_pTOldAllocator)
		CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
	if (NULL != m_pTAllocator)
		delete m_pTAllocator;

	if (NULL != m_pUOldAllocator)
		CUniformAllocator::SetCurrentInstance(m_pUOldAllocator);
	if (NULL != m_pUAllocator)
		delete m_pUAllocator;


	glvkUnBindDisplay();

	CContextManager::GetInstance()->vkDestroyContext(m_context);
}

IViewPoint *const CRaptorVulkanDisplay::createViewPoint(void) const
{
	if (NULL != getRootScene())
		return new CVulkanViewPoint(getRootScene()->getName() + "_VIEWPOINT");
	else
		return new CVulkanViewPoint("RAPTORVULKANDISPLAY_VIEWPOINT");
}

void CRaptorVulkanDisplay::glResize(unsigned int sx,unsigned int sy,
									unsigned int ox, unsigned int oy)
{
	if (CContextManager::INVALID_CONTEXT != m_context)
	{
		CContextManager *manager = CContextManager::GetInstance();
		if ((sx != cs.width) || (sy != cs.height))
		{
			cs.width = sx;
			cs.height = sy;
			manager->vkResize(m_context,cs);
		}
	}
}

bool CRaptorVulkanDisplay::glRender(void)
{
	if (CContextManager::INVALID_CONTEXT != m_context)
	{
		CContextManager *manager = CContextManager::GetInstance();
		CVulkanDevice &vk_device = manager->vkGetDevice(m_context);
		//	cs.x & cs.y are window position, not pixel origin in layer
		VkRect2D scissor = { {0, 0}, {cs.width,cs.height} };

		CTimeObject::markTime(this);

        m_pGAllocator->glvkLockMemory(true);
		m_pTAllocator->glvkLockMemory(true);
		m_pUAllocator->glvkLockMemory(true);
	
		
		//	Actual rendering
		C3DScene *pScene = getRootScene();
		vk_device.vkRender(pScene,scissor,cs);
		
        m_pGAllocator->glvkLockMemory(false);
		m_pTAllocator->glvkLockMemory(false);
		m_pUAllocator->glvkLockMemory(false);

		rtime = CTimeObject::deltaMarkTime(this);

        if (Global::GetInstance().getConsole() != NULL)
		    Global::GetInstance().getConsole()->glRender();

		CContextManager::GetInstance()->vkSwapBuffers(m_context);

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

		return true;
	}
	else
		return false;
}

bool CRaptorVulkanDisplay::glGrab(	unsigned int x, unsigned int y, unsigned int width, unsigned int height,
									unsigned char* &data,unsigned int& size) const
{
	return false;
}

bool CRaptorVulkanDisplay::glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
						unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
						CRaptorDisplay *pDst) const
{
	return false;
}

void CRaptorVulkanDisplay::glGenerate(CTextureObject* )
{
}

bool CRaptorVulkanDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle != CGL_NULL)
	{
		CContextManager *manager = CContextManager::GetInstance();

		if ((CContextManager::INVALID_CONTEXT == m_context) && (WINDOW_CLASS == device.hClass))
		{
			unsigned int m_framerate = cs.refresh_rate.fps;
			if (cs.refresh_rate.sync_to_monitor)
				m_framerate = 0;

			manager->vkSwapVSync(m_framerate);
			m_context = manager->vkCreateContext(device,cs);
			if (CContextManager::INVALID_CONTEXT == m_context)
			{
				Raptor::GetErrorManager()->generateRaptorError(	bufferID,
																CRaptorErrorManager::RAPTOR_FATAL,
																CRaptorMessages::ID_CREATE_FAILED);
				return false;
			}

			//	Manage vertex/pixel buffer objects.
			allocateResources();

			//!	We need a first make current context to set the active device
			manager->vkMakeCurrentContext(device, m_context);
		}
		else
		{
			manager->vkMakeCurrentContext(device, m_context);

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

	return CRaptorDisplay::glvkBindDisplay(device);
}

bool CRaptorVulkanDisplay::glvkUnBindDisplay(void)
{
	CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
	m_pGOldAllocator = NULL;
	CTexelAllocator::SetCurrentInstance(m_pTOldAllocator);
	m_pTOldAllocator = NULL;
	CUniformAllocator::SetCurrentInstance(m_pUOldAllocator);
	m_pUOldAllocator = NULL;

	CContextManager *manager = CContextManager::GetInstance();

	RAPTOR_HANDLE device;
	manager->vkMakeCurrentContext(device, CContextManager::INVALID_CONTEXT);

	return CRaptorDisplay::glvkUnBindDisplay();
}


void CRaptorVulkanDisplay::allocateResources(void)
{
    //  Ensure no current allocator.
    m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(NULL);
	m_pTOldAllocator = CTexelAllocator::SetCurrentInstance(NULL);
	m_pUOldAllocator = CUniformAllocator::SetCurrentInstance(NULL);

    m_pGAllocator = CGeometryAllocator::GetInstance();
	m_pTAllocator = CTexelAllocator::GetInstance();
	m_pUAllocator = CUniformAllocator::GetInstance();

	CContextManager *manager = CContextManager::GetInstance();
	CVulkanDevice &vk_device = manager->vkGetDevice(m_context);
	IDeviceMemoryManager* pDeviceMemory = vk_device.getMemory();

	//!	Vulkan will not work without buffer reallocation to device memory
    const CRaptorConfig& config = Global::GetInstance().getConfig();
	if ((!config.m_bRelocation) || (0 == config.m_uiVertices) || (0 == config.m_uiPolygons))
    {
		Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
	    		        								CRaptorMessages::ID_NO_RESOURCE);
	}
	else
    {
		bool initAllocator = m_pGAllocator->glvkInitMemory(pDeviceMemory,config.m_uiPolygons,config.m_uiVertices);
		if (!initAllocator)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_FATAL,
    		        										CRaptorMessages::ID_NO_RESOURCE);
		}
    }

	if ((!config.m_bRelocation) || (0 == config.m_uiUniforms))
	{
		Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
														CRaptorMessages::ID_NO_RESOURCE);
	}
	else
	{
		bool initAllocator = m_pUAllocator->glvkInitMemory(pDeviceMemory, config.m_uiUniforms);
		if (!initAllocator)
		{
			Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_FATAL,
															CRaptorMessages::ID_NO_RESOURCE);
		}
	}

	if ((!config.m_bRelocation) || (0 == config.m_uiTexels))
	{
		Raptor::GetErrorManager()->generateRaptorError(CGeometry::CGeometryClassID::GetClassId(),
													   CRaptorErrorManager::RAPTOR_FATAL,
													   CRaptorMessages::ID_NO_RESOURCE);
	}
	else
	{
		bool initAllocator = m_pTAllocator->glvkInitMemory(pDeviceMemory, config.m_uiTexels);
		if (!initAllocator)
		{
			Raptor::GetErrorManager()->generateRaptorError(CGeometry::CGeometryClassID::GetClassId(),
														   CRaptorErrorManager::RAPTOR_FATAL,
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
}
