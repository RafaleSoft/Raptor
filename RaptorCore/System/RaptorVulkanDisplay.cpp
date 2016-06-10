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
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "System/RaptorErrorManager.h"
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
	glUnBindDisplay();
	
	CContextManager::GetInstance()->vkDestroyContext(m_context);
}

void CRaptorVulkanDisplay::glResize(unsigned int sx,unsigned int sy,
									unsigned int ox, unsigned int oy)
{
}

bool CRaptorVulkanDisplay::glRender(void)
{
	if (m_context != -1)
	{
		CTimeObject::markTime(this);

        //m_pGAllocator->glLockMemory(true);
		//m_pTAllocator->glLockMemory(true);

		C3DScene *pScene = getRootScene();
		//pScene->vkRender();

        //m_pGAllocator->glLockMemory(false);
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

		CATCH_VK_ERROR

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

			RAPTOR_HANDLE device = manager->getDevice(m_context);

			VkFormat format;
			VkSampleCountFlagBits samples;
			if (((cs.display_mode & CGL_RGBA) == CGL_RGBA) ||
				((cs.display_mode & CGL_RGB) == CGL_RGB) ||
				((cs.display_mode & CGL_FLOAT_32) == CGL_FLOAT_32))
				samples = VK_SAMPLE_COUNT_32_BIT;
			else if ((cs.display_mode & CGL_FLOAT_16) == CGL_FLOAT_16)
				samples = VK_SAMPLE_COUNT_16_BIT;

			VkAttachmentDescription pAttachments = {0, // or 1 = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT
													format,samples,
													VK_ATTACHMENT_LOAD_OP_CLEAR,
													VK_ATTACHMENT_STORE_OP_STORE,
													VK_ATTACHMENT_LOAD_OP_CLEAR,
													VK_ATTACHMENT_STORE_OP_DONT_CARE,
													VK_IMAGE_LAYOUT_GENERAL, //	Is this correct ?
													VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }; // is this correct ?
			VkAttachmentReference pColorAttachments;
			VkAttachmentReference pDepthStencilAttachment;
			VkSubpassDescription pSubpasses = {	0,
												VK_PIPELINE_BIND_POINT_GRAPHICS,
												0, NULL,
												1, &pColorAttachments,
												NULL,
												&pDepthStencilAttachment,
												0, NULL };
			VkRenderPassCreateInfo pCreateInfo = {	VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
													NULL, 0,
													1, &pAttachments,
													1, &pSubpasses,
													0, NULL /*const VkSubpassDependency* pDependencies*/ };
			VkRenderPass pRenderPass;

			CRaptorErrorManager *pErrMgr = Raptor::GetErrorManager();
			VkResult res = manager->glGetExtensions()->vkCreateRenderPass((VkDevice)device.handle,&pCreateInfo,NULL,&pRenderPass);
			if (VK_SUCCESS != res)
				pErrMgr->vkGetError(res,__FILE__,__LINE__);
		}

		manager->vkMakeCurrentContext(device,m_context);
	}

	return CRaptorDisplay::glBindDisplay(device);
}

bool CRaptorVulkanDisplay::glUnBindDisplay(void)
{
	RAPTOR_HANDLE device;
	CContextManager *manager = CContextManager::GetInstance();
	manager->vkMakeCurrentContext(device,m_context);

	return CRaptorDisplay::glUnBindDisplay();
}
