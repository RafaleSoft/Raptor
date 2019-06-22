// RaptorBufferDisplay.cpp: implementation of the CRaptorBufferDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"


#if !defined(AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_)
	#include "RaptorBufferDisplay.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
	#include "System/RaptorErrorManager.h"
#endif	
#if !defined(AFX_OPENGL_H__6C8840CA_BEFA_41DE_9879_5777FBBA7147__INCLUDED_)
	#include "Subsys/OpenGL/RaptorOpenGL.h"
#endif


RAPTOR_NAMESPACE_BEGIN

vector<unsigned int> CRaptorBufferDisplay::m_bindingStack;
static CRaptorBufferDisplay::CRaptorBufferDisplayClassID bufferID;
const CPersistence::CPersistenceClassID& CRaptorBufferDisplay::CRaptorBufferDisplayClassID::GetClassId(void)
{
	return bufferID;
}

RAPTOR_NAMESPACE_END


RAPTOR_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorBufferDisplay::CRaptorBufferDisplay(const CRaptorDisplayConfig& pcs)
	:CRaptorDisplay(bufferID,pcs.caption)
{
	m_pBuffer = 0;
	m_bBoundToTexture = false;

	m_previousContext = CContextManager::INVALID_CONTEXT;

	cs = pcs;
	
	//! P Buffer (nVidia + Win32) do not support double buffering. Maybe future versions will do so.
	cs.double_buffer = false;
}

CRaptorBufferDisplay::~CRaptorBufferDisplay()
{
    //  Unbind, in case it is forgotten by the user.
    glvkUnBindDisplay();

	if (m_pBuffer > 0)
		CContextManager::GetInstance()->glDestroyPBuffer(m_pBuffer - 1);
}

bool CRaptorBufferDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
	state.copyBaseConfig(cs);
    return CRaptorDisplay::glQueryStatus(state,query);
}

bool CRaptorBufferDisplay::glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* &data,unsigned int& size) const
{
    //! If buffer is bound, read pixels directly
	if (CContextManager::INVALID_CONTEXT != m_previousContext)
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
		data = new unsigned char[size];
        glReadPixels(grab_x,grab_y,grab_w,grab_h,GL_RGBA,GL_UNSIGNED_BYTE,data);

		CATCH_GL_ERROR

        return true;
    }
    else
    {
        data = NULL;
        size = 0;
        return false;
    }
}

bool CRaptorBufferDisplay::glBlit(	unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
									unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
									CRaptorDisplay *pDst) const
{
	return false;
}

void CRaptorBufferDisplay::glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy)
{
    if ((sx == 0) || (sy == 0))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
						"Raptor Buffer Display is requested a resize with a wrong dimension ( 0 ) !");
#endif
        return;
    }

	glvkUnBindDisplay();
    if (m_pBuffer > 0)
		CContextManager::GetInstance()->glDestroyPBuffer(m_pBuffer - 1);

    cs.x = ox;
    cs.y = oy;
    cs.width = sx;
    cs.height = sy;
    m_pBuffer = 0;

    RAPTOR_HANDLE noDevice;
	glvkBindDisplay(noDevice);
	glvkUnBindDisplay();

    CATCH_GL_ERROR
}


void CRaptorBufferDisplay::glGenerate(CTextureObject* T)
{
    if ((T == NULL) || (!m_bEnabled))
        return;

#if defined(WGL_ARB_pbuffer)
#if defined(WGL_ARB_render_texture)
	if (m_pBuffer > 0)
	{
		// Assert the buffer is not bound
		// to ensure a correct 'texture rendering'
		if (CContextManager::INVALID_CONTEXT == m_previousContext)
		{
			m_bBoundToTexture = true;
			GLuint buffer = WGL_FRONT_LEFT_ARB;

#ifdef WGL_NV_render_depth_texture
			ITextureObject::TEXEL_TYPE tt = T->getTexelType();
			if ((tt == ITextureObject::CGL_DEPTH8) ||
				(tt == ITextureObject::CGL_DEPTH16) ||
				(tt == ITextureObject::CGL_DEPTH24) ||
				(tt == ITextureObject::CGL_DEPTH32))
				buffer = WGL_DEPTH_COMPONENT_NV;
#endif
			CContextManager::GetInstance()->glBindTexImageARB(m_pBuffer - 1,buffer);
		}
		else
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                            CRaptorErrorManager::RAPTOR_WARNING,
											                "Raptor Display cannot bind a texture while buffer is still bound");
#endif
		}
	}
#endif
#endif
}


bool CRaptorBufferDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
#if defined(WGL_ARB_pbuffer)
	if (m_pBuffer == 0)
	{
		CContextManager *manager = CContextManager::GetInstance();

		if (CContextManager::INVALID_CONTEXT == manager->glGetCurrentContext())
		{
#ifdef RAPTOR_DEBUG_MODE_GENERATION
			RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
							"Raptor Display cannot create a buffered display with no device context active");
#endif
			return false;
		}
		else
		{
			m_pBuffer = 1 + manager->glCreatePBuffer(cs);

			m_previousContext = manager->glGetCurrentContext();
			manager->glBindPBuffer(m_pBuffer - 1);

			m_bindingStack.push_back(m_pBuffer);

			//  Display shading configuration
			glApplyStatus(cs, GL_PIXEL_STATE_QUERY | GL_HINT_STATE_QUERY | GL_LIGHT_STATE_QUERY);

			CATCH_GL_ERROR

			RAPTOR_HANDLE noDevice;
			return CRaptorDisplay::glvkBindDisplay(noDevice);
		}
	}
	else
	{
		bool res = false;
		CGeometryAllocator *allocatorG = CGeometryAllocator::GetInstance();
		bool bRelocGeometry = allocatorG->isMemoryLocked();
		if (bRelocGeometry)
			allocatorG->glvkLockMemory(false);
		CTexelAllocator *allocatorT = CTexelAllocator::GetInstance();
		bool bRelocTexels = allocatorT->isMemoryLocked();
		if (bRelocTexels)
			allocatorT->glvkLockMemory(false);

		// The current rendering context is saved for restoring after pBuffer is used
		// Save the context only once, in case of using multiple Pixel Buffers
		if (CContextManager::INVALID_CONTEXT == m_previousContext)
		{
			// Remove texture binding because writing is incompatible
			// with texturing. Explicit unbinding can be avoided for
			// performance reasons, but future drivers might be faster.
            if (m_bBoundToTexture)
				glvkUnBindDisplay();
    
			CContextManager *manager = CContextManager::GetInstance();

			m_previousContext = manager->glGetCurrentContext();
			manager->glBindPBuffer(m_pBuffer - 1);

            m_bindingStack.push_back(m_pBuffer);

            CATCH_GL_ERROR

			res = CRaptorDisplay::glvkBindDisplay(device);
		}
        // Specific case for BufferDisplay :
        //  a configuration parameter can be transmitted to the underlying
        //  pixel buffer. Here under, for temporary testing, a selection of the
        //  cube face mapping of the pixel buffer ( but should also select
        //  mipmapping level ).
        //  This else case lacks some tests : already bound ? / buffer valid ? / render target compatible ? ...
        //  ... so it might disappear or remain stricly internal.
		else if ((device.hClass() >= CTextureObject::CGL_CUBEMAP_PX) &&
				 (device.hClass() <= CTextureObject::CGL_CUBEMAP_NZ))
        {
            CContextManager *manager = CContextManager::GetInstance();
            manager->glBindPBuffer(m_pBuffer - 1,(CTextureObject::CUBE_FACE)(device.hClass()));
            res = true;
        }
        // Context already bound !!! Raise warning ?
        else
		{
            res = false;
		}
		
		if (bRelocGeometry)
			allocatorG->glvkLockMemory(true);
		if (bRelocTexels)
			allocatorT->glvkLockMemory(true);

		return res;
	}
#else
	return false;
#endif
}

/*


bool CRaptorBufferDisplay::glvkBindDisplay(const RAPTOR_HANDLE& device)
{
	#if defined(WGL_ARB_pbuffer)
	bool res = false;

	CGeometryAllocator *allocatorG = CGeometryAllocator::GetInstance();
	bool bRelocGeometry = allocatorG->isMemoryLocked();
	if (bRelocGeometry)
	allocatorG->glvkLockMemory(false);
	CTexelAllocator *allocatorT = CTexelAllocator::GetInstance();
	bool bRelocTexels = allocatorT->isMemoryLocked();
	if (bRelocTexels)
	allocatorT->glvkLockMemory(false);

	if (m_pBuffer == 0)
	{
		CContextManager *manager = CContextManager::GetInstance();

		if (CContextManager::INVALID_CONTEXT == manager->glGetCurrentContext())
		{
	#ifdef RAPTOR_DEBUG_MODE_GENERATION
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_ERROR,
															"Raptor Display cannot create a buffered display with no device context active");
	#endif
			//return false;
			res = false;
		}
		else
		{
			m_pBuffer = 1 + manager->glCreatePBuffer(cs);

			m_previousContext = manager->glGetCurrentContext();
			manager->glBindPBuffer(m_pBuffer - 1);

			m_bindingStack.push_back(m_pBuffer);

			//  Display shading configuration
			glApplyStatus(cs, GL_PIXEL_STATE_QUERY | GL_HINT_STATE_QUERY | GL_LIGHT_STATE_QUERY);

			CATCH_GL_ERROR

				RAPTOR_HANDLE noDevice;
			//return CRaptorDisplay::glvkBindDisplay(noDevice);
			res = CRaptorDisplay::glvkBindDisplay(noDevice);
		}
	}
	else
	{
		//bool res = false;
		//CGeometryAllocator *allocatorG = CGeometryAllocator::GetInstance();
		//bool bRelocGeometry = allocatorG->isMemoryLocked();
		//if (bRelocGeometry)
		//allocatorG->glvkLockMemory(false);
		//CTexelAllocator *allocatorT = CTexelAllocator::GetInstance();
		//bool bRelocTexels = allocatorT->isMemoryLocked();
		//if (bRelocTexels)
		//allocatorT->glvkLockMemory(false);

		// The current rendering context is saved for restoring after pBuffer is used
		// Save the context only once, in case of using multiple Pixel Buffers
		if (CContextManager::INVALID_CONTEXT == m_previousContext)
		{
			// Remove texture binding because writing is incompatible
			// with texturing. Explicit unbinding can be avoided for
			// performance reasons, but future drivers might be faster.
			if (m_bBoundToTexture)
				glvkUnBindDisplay();

			CContextManager *manager = CContextManager::GetInstance();

			m_previousContext = manager->glGetCurrentContext();
			manager->glBindPBuffer(m_pBuffer - 1);

			m_bindingStack.push_back(m_pBuffer);

			CATCH_GL_ERROR

				res = CRaptorDisplay::glvkBindDisplay(device);
		}
		// Specific case for BufferDisplay :
		//  a configuration parameter can be transmitted to the underlying
		//  pixel buffer. Here under, for temporary testing, a selection of the
		//  cube face mapping of the pixel buffer ( but should also select
		//  mipmapping level ).
		//  This else case lacks some tests : already bound ? / buffer valid ? / render target compatible ? ...
		//  ... so it might disappear or remain stricly internal.
		else if ((device.hClass() >= CTextureObject::CGL_CUBEMAP_PX) &&
				 (device.hClass() <= CTextureObject::CGL_CUBEMAP_NZ))
		{
			CContextManager *manager = CContextManager::GetInstance();
			manager->glBindPBuffer(m_pBuffer - 1, (CTextureObject::CUBE_FACE)(device.hClass()));
			res = true;
		}
		// Context already bound !!! Raise warning ?
		else
		{
			res = false;
		}
		
		//if (bRelocGeometry)
		//allocatorG->glvkLockMemory(true);
		//if (bRelocTexels)
		//allocatorT->glvkLockMemory(true);
		//
		//return res;
	}

	if (bRelocGeometry)
		allocatorG->glvkLockMemory(true);
	if (bRelocTexels)
		allocatorT->glvkLockMemory(true);

	return res;

#else
return false;
#endif
}
*/

bool CRaptorBufferDisplay::glvkUnBindDisplay(void)
{
#if defined(WGL_ARB_pbuffer)
	// Buffer not created ( Binding never done )
	if (m_pBuffer == 0)
		return false;
	// Buffer bound to texture
	else if (m_bBoundToTexture)
	{
        m_bBoundToTexture = false;
// Due to heavy performance loss, keep buffer bound to texture.
/*
#if defined(WGL_ARB_render_texture)
		GLuint buffer = WGL_FRONT_LEFT_ARB;
#ifdef WGL_NV_render_depth_texture
		if (m_displayUsage == DEPTH)
			buffer = WGL_DEPTH_COMPONENT_NV;
#endif
		
		return CContextManager::GetInstance()->glReleaseTexImageARB(m_pBuffer - 1,buffer);
#endif
*/
        //CRaptorDisplay::glUnBindDisplay();
		return true;
	}
	// Buffer already unbound or error during binding
	else if (CContextManager::INVALID_CONTEXT == m_previousContext)
		return false;
	else
	{
		CGeometryAllocator *allocatorG = CGeometryAllocator::GetInstance();
		bool bRelocGeometry = allocatorG->isMemoryLocked();
		if (bRelocGeometry)
			allocatorG->glvkLockMemory(false);
		CTexelAllocator *allocatorT = CTexelAllocator::GetInstance();
		bool bRelocTexels = allocatorT->isMemoryLocked();
		if (bRelocTexels)
			allocatorT->glvkLockMemory(false);


		CRaptorDisplay::glvkUnBindDisplay();

        //  First case : there is another buffer bound in the stack,
        //  we must get back to that buffer
        if (m_bindingStack.size() > 1)
        {
            unsigned int lastBuffer = m_bindingStack.back();
            m_bindingStack.pop_back();

#ifdef RAPTOR_DEBUG_MODE_GENERATION
            if (lastBuffer != m_pBuffer)
            {
				RAPTOR_ERROR(	COpenGL::COpenGLClassID::GetClassId(),
								"Raptor Buffer Display bind/unbind mismatch");
            }
#endif

            lastBuffer = m_bindingStack.back();
            CContextManager *manager = CContextManager::GetInstance();
			manager->glBindPBuffer(lastBuffer - 1);
            m_bindingStack.push_back(lastBuffer);
        }
        //  Second case : this is the only buffer bound,
        //  return to previous screen buffer.
        else
        {
		    RAPTOR_HANDLE device(DEVICE_CONTEXT_CLASS, (void*)0);
		    CContextManager::GetInstance()->glMakeCurrentContext(device,m_previousContext);
        }

        m_bindingStack.pop_back();
		m_previousContext = CContextManager::INVALID_CONTEXT;

		if (bRelocGeometry)
			allocatorG->glvkLockMemory(true);
		if (bRelocTexels)
			allocatorT->glvkLockMemory(true);

		return true;
	}
#else
	return false;
#endif
}

bool CRaptorBufferDisplay::glRender(void)
{
	C3DScene *pScene = getRootScene();
	pScene->glRender();

    return true;
}

