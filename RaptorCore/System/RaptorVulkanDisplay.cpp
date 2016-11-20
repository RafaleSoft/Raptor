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
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "System/Raptor.h"
#endif
#if !defined(AFX_RAPTORVULKANPIPELINE_H__C2997B30_C6E2_4EF2_AFE3_FCD27AB5CBB7__INCLUDED_)
	#include "Subsys/Vulkan/VulkanPipeline.h"
#endif
#if !defined(AFX_RAPTORVULKANSHADER_H__C188550F_1D1C_4531_B0A0_727CE9FF9450__INCLUDED_)
	#include "Subsys/Vulkan/VulkanShader.h"
#endif
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
		#include "System/RaptorErrorManager.h"
	#endif
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_GEOMETRY_H__B42ABB87_80E8_11D3_97C2_DE5C28000000__INCLUDED_)
	#include "GLHierarchy/Geometry.h"
#endif


RAPTOR_NAMESPACE_BEGIN

static CRaptorVulkanDisplay::CRaptorVulkanDisplayClassID bufferID;
const CPersistence::CPersistenceClassID& CRaptorVulkanDisplay::CRaptorVulkanDisplayClassID::GetClassId(void)
{
	return bufferID;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorVulkanDisplay::CRaptorVulkanDisplay(const CRaptorDisplayConfig& pcs)
	:CRaptorDisplay(bufferID,pcs.caption),
	m_context(-1),cs(pcs),fps(0.0f),ftime(0.0f),rtfps(0.0f),rtime(0.0f)
{
}

CRaptorVulkanDisplay::~CRaptorVulkanDisplay(void)
{
	if (m_pGOldAllocator != NULL)
		CGeometryAllocator::SetCurrentInstance(m_pGOldAllocator);
	if (m_pGAllocator != NULL)
		delete m_pGAllocator;

	glUnBindDisplay();

	if (0 < m_pipelines.size())
	{
		for (unsigned int i=0;i<m_pipelines.size();i++)
		{
			m_pipelines[i]->destroyPipeline();
			delete m_pipelines[i];
		}
		m_pipelines.clear();
	}
	
	CContextManager::GetInstance()->vkDestroyContext(m_context);
}

void CRaptorVulkanDisplay::glResize(unsigned int sx,unsigned int sy,
									unsigned int ox, unsigned int oy)
{
	if (m_context != -1)
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

float *pVertices = NULL;

bool CRaptorVulkanDisplay::glRender(void)
{
	if (m_context != -1)
	{
		CContextManager *manager = CContextManager::GetInstance();
		CVulkanDevice &vk_device = manager->vkGetDevice(m_context);
		//	cs.x & cs.y are window position, not pixel origin in layer
		VkRect2D scissor = { {0, 0}, {cs.width,cs.height} };

		CTimeObject::markTime(this);

        m_pGAllocator->glvkLockMemory(true);
		//m_pTAllocator->glLockMemory(true);

		CVulkanPipeline *pipeline = m_pipelines[0];
		vk_device.vkBindPipeline(*pipeline,scissor,cs.framebufferState.colorClearValue,(VkDeviceSize)pVertices);

		//C3DScene *pScene = getRootScene();
		//pScene->vkRender();

        m_pGAllocator->glvkLockMemory(false);
		//m_pTAllocator->glLockMemory(false);

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

bool CRaptorVulkanDisplay::glBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.handle != CGL_NULL)
	{
		CContextManager *manager = CContextManager::GetInstance();

		if ((m_context == -1) && (WINDOW_CLASS == device.hClass))
		{
			unsigned int m_framerate = cs.refresh_rate.fps;
			if (cs.refresh_rate.sync_to_monitor)
				m_framerate = 0;

			manager->vkSwapVSync(m_framerate);
			m_context = manager->vkCreateContext(device,cs);
			if (m_context == -1)
			{
				Raptor::GetErrorManager()->generateRaptorError(	bufferID,
																CRaptorErrorManager::RAPTOR_FATAL,
																CRaptorMessages::ID_CREATE_FAILED);
				return false;
			}

			//	Manage vertex/pixel buffer objects.
			allocateResources();
		}
		
		manager->vkMakeCurrentContext(device,m_context);
	}

	return CRaptorDisplay::glBindDisplay(device);
}

bool CRaptorVulkanDisplay::glUnBindDisplay(void)
{
	if (m_pipelines.empty())
	{

		CContextManager *manager = CContextManager::GetInstance();
		CVulkanDevice &device = manager->vkGetDevice(m_context);


float VertexData[4*8] = 
{
	-0.7f, -0.7f, 0.0f, 1.0f	,	1.0f, 0.0f, 0.0f, 0.0f,
	-0.7f,  0.7f, 0.0f, 1.0f	,	0.0f, 1.0f, 0.0f, 0.0f,
	 0.7f, -0.7f, 0.0f, 1.0f	,	0.0f, 0.0f, 1.0f, 0.0f,
	 0.7f,  0.7f, 0.0f, 1.0f	,	0.3f, 0.3f, 0.3f, 0.0f
};

pVertices = m_pGAllocator->allocateVertices(32);
m_pGAllocator->glvkCopyPointer(pVertices,&VertexData[0],32);

		CVulkanShader *vshader = device.createShader();
		CVulkanShader *fshader = device.createShader();
		if (!vshader->loadShader("shader2.vert") ||
			!fshader->loadShader("shader2.frag"))
		{
			return false;
		}

		CVulkanPipeline *pipeline = device.createPipeline();
		m_pipelines.push_back(pipeline);
		pipeline->addShader(vshader);
		pipeline->addShader(fshader);

		
		if (!pipeline->initPipeline())
		{
			Raptor::GetErrorManager()->generateRaptorError(	bufferID,
															CRaptorErrorManager::RAPTOR_FATAL,
															CRaptorMessages::ID_CREATE_FAILED);
			return false;
		}
	}

	return CRaptorDisplay::glUnBindDisplay();
}


void CRaptorVulkanDisplay::allocateResources(void)
{
    //  Ensure no current allocator.
    m_pGOldAllocator = CGeometryAllocator::SetCurrentInstance(NULL);
    m_pGAllocator = CGeometryAllocator::GetInstance();

    const CRaptorConfig& config = Global::GetInstance().getConfig();
	if ((!config.m_bRelocation) || (0 == config.m_uiVertices))
    {
		Raptor::GetErrorManager()->generateRaptorError(	CGeometry::CGeometryClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_FATAL,
	    		        								CRaptorMessages::ID_NO_RESOURCE);
	}
	else
    {
		CContextManager *manager = CContextManager::GetInstance();
		CVulkanDevice &vk_device = manager->vkGetDevice(m_context);
		IDeviceMemoryManager* pDeviceMemory = vk_device.getMemory();

		bool initAllocator = m_pGAllocator->glvkInitMemory(pDeviceMemory,config.m_uiPolygons,config.m_uiVertices);
		if (!initAllocator)
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
        m_pGOldAllocator->glvkLockMemory(false);
}
