#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTORRENDERBUFFERDISPLAY_H__AC23F2BD_DAC5_4B9A_9F75_AD9A2CEEAE08__INCLUDED_)
	#include "RaptorRenderBufferDisplay.h"
#endif

#ifndef __GLOBAL_H__
	#include "System/Global.h"
#endif

#if !defined(AFX_3DSCENE_H__E597E752_BAD4_415D_9C00_8C59D139D32B__INCLUDED_)
	#include "Engine/3DScene.h"
#endif

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
    #include "Raptor.h"
#endif

#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
	#include "RaptorExtensions.h"
#endif

#if !defined(AFX_TEXTUREOBJECT_H__D32B6294_B42B_4E6F_AB73_13B33C544AD0__INCLUDED_)
	#include "GLHierarchy/TextureObject.h"
#endif

#if !defined(AFX_TEXTURESET_H__26F3022D_70FE_414D_9479_F9CCD3DCD445__INCLUDED_)
	#include "GLHierarchy/TextureSet.h"
#endif

#if !defined(AFX_VIEWPOINT_H__82071851_A036_4311_81CB_01E7E25F19E1__INCLUDED_)
	#include "Engine/ViewPoint.h"
#endif

#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif


RAPTOR_NAMESPACE

vector<unsigned int> CRaptorRenderBufferDisplay::m_bindingStack;
static CRaptorRenderBufferDisplay::CRaptorRenderBufferDisplayClassID bufferID;
const CPersistence::CPersistenceClassID& CRaptorRenderBufferDisplay::CRaptorRenderBufferDisplayClassID::GetClassId(void)
{
	return bufferID;
}

CRaptorRenderBufferDisplay::CRaptorRenderBufferDisplay(const CRaptorDisplayConfig& pcs)
	:CRaptorDisplay(bufferID,pcs.caption)
{
	cs = pcs;

	//!	Equivalent to default window-system-framebuffer
	m_framebuffer = 0;
	m_colorbuffer = 0;
	m_depthbuffer  = 0;
	m_stencilbuffer = 0;

	m_pAttachments = NULL;
}

CRaptorRenderBufferDisplay::~CRaptorRenderBufferDisplay()
{
    //  Unbind, in case it is forgotten by the user.
    glUnBindDisplay();

	if (m_pAttachments != NULL)
	{
		glDetachBuffers();

		m_pAttachments->unregisterDestruction(this);
		m_pAttachments = NULL;
	}

	glDestroyBuffer();
}

void CRaptorRenderBufferDisplay::unLink(const CPersistence* obj)
{
    if (static_cast<CPersistence*>(m_pAttachments) == obj)
        m_pAttachments = NULL;
    else
        CRaptorDisplay::unLink(obj);
}

void CRaptorRenderBufferDisplay::glDestroyBuffer(void)
{
#if defined(GL_EXT_framebuffer_object)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_framebuffer != 0)
	{
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_framebuffer);

		if(m_colorbuffer != 0)
		{
			pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
														GL_COLOR_ATTACHMENT0_EXT,
														GL_RENDERBUFFER_EXT,
														0);

			pExtensions->glDeleteRenderbuffersEXT(1,&m_colorbuffer);
			m_colorbuffer = 0;
		}

		if (m_depthbuffer != 0)
		{
			pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
														GL_DEPTH_ATTACHMENT_EXT,
														GL_RENDERBUFFER_EXT,
														0);
			
			pExtensions->glDeleteRenderbuffersEXT(1,&m_depthbuffer);
			m_depthbuffer = 0;
		}

		if (m_stencilbuffer != 0)
		{
			pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
														GL_STENCIL_ATTACHMENT_EXT,
														GL_RENDERBUFFER_EXT,
														0);
			
			pExtensions->glDeleteRenderbuffersEXT(1,&m_stencilbuffer);
			m_stencilbuffer = 0;
		}

		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		pExtensions->glDeleteFramebuffersEXT(1,&m_framebuffer);
		m_framebuffer = 0;
	}

	CATCH_GL_ERROR
#endif
}


bool CRaptorRenderBufferDisplay::glAttachBuffers()
{
#if defined(GL_EXT_framebuffer_object)
	if (m_pAttachments != NULL)
	{
		GLint maxAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT ,&maxAttachments);
		 
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		unsigned int nbtextures = m_pAttachments->getNbTexture();

		GLuint colorAttachment = GL_COLOR_ATTACHMENT0_EXT;
		for (unsigned int i=0;i<MIN(nbtextures,(unsigned int)abs(max(0,maxAttachments)));i++)
		{
			CTextureObject *T = m_pAttachments->getTexture(i);
			unsigned int target = T->target & 0xFFFF;

			CTextureObject::TEXEL_TYPE tt = T->getTexelType();
			GLuint attachment = GL_COLOR_ATTACHMENT0_EXT;
			if ((tt == CTextureObject::CGL_DEPTH8) ||
				(tt == CTextureObject::CGL_DEPTH16) ||
				(tt == CTextureObject::CGL_DEPTH24) ||
				(tt == CTextureObject::CGL_DEPTH32) ||
				(tt == CTextureObject::CGL_DEPTH24_STENCIL8))
				attachment = GL_DEPTH_ATTACHMENT_EXT;
			else
				attachment = colorAttachment++;

			pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
													attachment,
													target, 
													T->texname,
													T->getCurrentMipMapLevel());

			if (tt == CTextureObject::CGL_DEPTH24_STENCIL8)
				pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
														GL_STENCIL_ATTACHMENT_EXT,
														target, 
														T->texname,
														T->getCurrentMipMapLevel());
		}

		return true;
	}
	else
		return false;
#else
	return false;
#endif
}

bool CRaptorRenderBufferDisplay::glDetachBuffers()
{
#if defined(GL_EXT_framebuffer_object)
	if (m_pAttachments != NULL)
	{
		GLint maxAttachments = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT ,&maxAttachments);

		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		unsigned int nbtextures = m_pAttachments->getNbTexture();

		for (unsigned int i=0;i<MIN(nbtextures,(unsigned int)abs(max(0,maxAttachments)));i++)
		{
			CTextureObject *T = m_pAttachments->getTexture(i);
			unsigned int target = T->target & 0xFFFF;

			if ((T->getWidth() != cs.width) ||
				(T->getHeight() != cs.height))
			{
				Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
																CRaptorErrorManager::RAPTOR_ERROR,
																"Raptor Render Buffer Display is attached to a texture with invalid sizes");
			}

			CTextureObject::TEXEL_TYPE tt = T->getTexelType();
			GLuint attachment = GL_COLOR_ATTACHMENT0_EXT;
			if ((tt == CTextureObject::CGL_DEPTH8) ||
				(tt == CTextureObject::CGL_DEPTH16) ||
				(tt == CTextureObject::CGL_DEPTH24) ||
				(tt == CTextureObject::CGL_DEPTH32) ||
				(tt == CTextureObject::CGL_DEPTH24_STENCIL8))
				attachment = GL_DEPTH_ATTACHMENT_EXT;

			pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
													attachment,
													target, 
													0,
													0);

			if (tt == CTextureObject::CGL_DEPTH24_STENCIL8)
				pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
														GL_STENCIL_ATTACHMENT_EXT,
														target, 
														0,
														0);
		}

		return true;
	}
	else
		return false;
#else
	return false;
#endif
}


bool CRaptorRenderBufferDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
	state.copyBaseConfig(cs);
    return CRaptorDisplay::glQueryStatus(state,query);
}

bool CRaptorRenderBufferDisplay::createFrameBuffer(void)
{
#if defined(GL_EXT_framebuffer_object)
	int maxSize = 0;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT,&maxSize);
	if ((maxSize < cs.height) || (maxSize < cs.width))
		return false;

	GLsizei nbSamples = 1;
#if defined(GL_EXT_framebuffer_multisample)
	if (CRaptorDisplayConfig::ANTIALIAS_NONE != cs.antialias)
		nbSamples = cs.getNbSamples();
#endif

	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
	pExtensions->glGenFramebuffersEXT(1,&m_framebuffer);
	pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_framebuffer);
	if (pExtensions->glIsFramebufferEXT(m_framebuffer))
	{
		// further intialisations : 
		// - create color render buffers if target is not a texture
		if ( (((cs.display_mode & CGL_RGBA) == CGL_RGBA) || ((cs.display_mode & CGL_RGB) == CGL_RGB)) && 
			!cs.bind_to_texture)
		{
			GLuint internalFormat = GL_NONE;
			if ((cs.display_mode & CGL_RGBA) == CGL_RGBA)
			{
				internalFormat = GL_RGBA8;	// GL_RGBA
				if ((cs.display_mode & CGL_FLOAT_16) == CGL_FLOAT_16)
#if defined(GL_ARB_texture_float)
					internalFormat = GL_RGBA16F_ARB;
#elif defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGBA16_NV;
#else
					internalFormat = GL_RGBA8;	// float buffer unsupported
#endif
				else if ((cs.display_mode & CGL_FLOAT_32) == CGL_FLOAT_32)
#if defined(GL_ARB_texture_float)
					internalFormat = GL_RGBA32F_ARB;
#elif defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGBA32_NV;
#else
					internalFormat = GL_RGBA8;	// float buffer unsupported
#endif
				else if ((cs.display_mode & CGL_FLOAT) == CGL_FLOAT)
#if defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGBA_NV;
#elif defined(GL_ARB_texture_float)
					internalFormat = GL_RGBA16F_ARB;
#else
					internalFormat = GL_RGBA8;	// float buffer unsupported
#endif
			}
			if ((cs.display_mode & CGL_RGB) == CGL_RGB)
			{
				internalFormat = GL_RGB8;	// GL_RGB
				if ((cs.display_mode & CGL_FLOAT_16) == CGL_FLOAT_16)
#if defined(GL_ARB_texture_float)
					internalFormat = GL_RGB16F_ARB;
#elif defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGB16_NV;
#else
					internalFormat = GL_RGB8;	// float buffer unsupported
#endif
				else if ((cs.display_mode & CGL_FLOAT_32) == CGL_FLOAT_32)
#if defined(GL_ARB_texture_float)
					internalFormat = GL_RGB32F_ARB;
#elif defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGB32_NV;
#else
					internalFormat = GL_RGB8;	// float buffer unsupported
#endif
				else if ((cs.display_mode & CGL_FLOAT) == CGL_FLOAT)
#if defined(GL_NV_float_buffer)
					internalFormat = GL_FLOAT_RGB_NV;
#elif defined(GL_ARB_texture_float)
					internalFormat = GL_RGB16F_ARB;
#else
					internalFormat = GL_RGB8;	// float buffer unsupported
#endif
			}

			pExtensions->glGenRenderbuffersEXT(1,&m_colorbuffer);
			pExtensions->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,m_colorbuffer);
			if (pExtensions->glIsRenderbufferEXT(m_colorbuffer))
			{
#if defined(GL_EXT_framebuffer_multisample)
				if (nbSamples > 1)
					pExtensions->glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,nbSamples,internalFormat,cs.width,cs.height);
				else
					pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,internalFormat,cs.width,cs.height);
#else
				pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,internalFormat,cs.width,cs.height);
#endif

				pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
															GL_COLOR_ATTACHMENT0_EXT,
															GL_RENDERBUFFER_EXT,
															m_colorbuffer);
			}
			else
				return false;
		}

		CATCH_GL_ERROR

		// - create depth render buffers
		if (((cs.display_mode & CGL_DEPTH_32) != 0) && !cs.bind_to_texture)
		{
			GLenum internalFormat = GL_DEPTH_COMPONENT;
			if ((cs.display_mode & CGL_DEPTH_32) == CGL_DEPTH_16)
				internalFormat = GL_DEPTH_COMPONENT16_ARB;
			else if ((cs.display_mode & CGL_DEPTH_32) == CGL_DEPTH_24)
				internalFormat = GL_DEPTH_COMPONENT24_ARB;
			else if ((cs.display_mode & CGL_DEPTH_32) == CGL_DEPTH_32)
				internalFormat = GL_DEPTH_COMPONENT32_ARB;
#if defined(GL_EXT_packed_depth_stencil)
			if (cs.stencil_buffer)
				internalFormat = GL_DEPTH24_STENCIL8_EXT;
#endif

			pExtensions->glGenRenderbuffersEXT(1,&m_depthbuffer);
			pExtensions->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,m_depthbuffer);
			if (pExtensions->glIsRenderbufferEXT(m_depthbuffer))
			{
#if defined(GL_EXT_framebuffer_multisample)
				if (nbSamples > 1)
					pExtensions->glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,nbSamples,internalFormat,cs.width,cs.height);
				else
					pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,internalFormat,cs.width,cs.height);
#else
				pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,internalFormat,cs.width,cs.height);
#endif

				pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
															GL_DEPTH_ATTACHMENT_EXT,
															GL_RENDERBUFFER_EXT,
															m_depthbuffer);
			}
			else
				return false;
		}

		CATCH_GL_ERROR

		// - create stencil render buffers
		if ((cs.stencil_buffer) && !cs.bind_to_texture)
		{
#if defined(GL_EXT_packed_depth_stencil)
			if (((cs.display_mode & CGL_DEPTH_32) != 0) &&
				(pExtensions->glIsRenderbufferEXT(m_depthbuffer)))
			{
				pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
															GL_STENCIL_ATTACHMENT_EXT,
															GL_RENDERBUFFER_EXT,
															m_depthbuffer);
			}
			else
#endif
			{
				pExtensions->glGenRenderbuffersEXT(1,&m_stencilbuffer);	
				pExtensions->glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,m_stencilbuffer);
				if (pExtensions->glIsRenderbufferEXT(m_stencilbuffer))
				{
#if defined(GL_EXT_framebuffer_multisample)
					if (nbSamples > 1)
						pExtensions->glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,nbSamples,GL_STENCIL_INDEX8_EXT,cs.width,cs.height);
					else
						pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_STENCIL_INDEX8_EXT,cs.width,cs.height);
#else
					pExtensions->glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_STENCIL_INDEX8_EXT,cs.width,cs.height);
#endif

					pExtensions->glFramebufferRenderbufferEXT(	GL_FRAMEBUFFER_EXT,
																GL_STENCIL_ATTACHMENT_EXT,
																GL_RENDERBUFFER_EXT,
																m_stencilbuffer);
				}
				else
					return false;
			}
		}

		CATCH_GL_ERROR
	}
	else
		return false;

	//! Further initialisations : make missing attachments,
	//!	except for multisampling
	if (nbSamples == 1)
	{
		if (cs.bind_to_texture)
		{
			if (m_pAttachments == NULL)
				return false;
			if (!glAttachBuffers())
				return false;
		}
	}

	return checkBufferStatus();
#else
	return false;
#endif
}


bool CRaptorRenderBufferDisplay::glBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.hClass == CTextureSet::CTextureSetClassID::GetClassId().ID())
	{
		if (m_pAttachments != NULL)
		{
			m_pAttachments->unregisterDestruction(this);
			m_pAttachments = NULL;
		}

		if (0 != device.handle)
		{
			m_pAttachments = (CTextureSet*)device.handle;
			m_pAttachments->registerDestruction(this);
			cs.bind_to_texture = true;
			return (m_pAttachments->getNbTexture() > 0);
		}
		else
		{
			cs.bind_to_texture = false;
			return false;
		}
	}

#if defined(GL_EXT_framebuffer_object)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	if (m_framebuffer == 0)
	{
		if (!createFrameBuffer())
			return false;
	}
	else
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_framebuffer);

	m_bindingStack.push_back(m_framebuffer);

	glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(cs.x,cs.y,cs.width,cs.height);
	glViewport(0,0,cs.width,cs.height); // Viewport is relative to window !!!
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	CViewPoint *vp = getViewPoint();
	if (vp != NULL)
		vp->glRenderViewPointModel();

	RAPTOR_HANDLE noDevice;
	return CRaptorDisplay::glBindDisplay(noDevice);
#else
	return false;
#endif
}


bool CRaptorRenderBufferDisplay::glUnBindDisplay(void)
{
#if defined(GL_EXT_framebuffer_object)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();

	// TODO : check last binding is self !!!
	m_bindingStack.pop_back();
	if (m_bindingStack.size() == 0)
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	else
	{
		int previousBinding = m_bindingStack.back();
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,previousBinding);
	}

	bool res = CRaptorDisplay::glUnBindDisplay();
	glPopAttrib();	// Viewport
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// Todo : 3DEngine configuration is lost here ?

	return res;
#else
	return false;
#endif
}

void CRaptorRenderBufferDisplay::glGenerate(CTextureObject* T)
{
    if ((T == NULL) || (!m_bEnabled) || (m_framebuffer == 0))
        return;

	// Remove current texture 
	// ( faster, not prone to errors and may not be wanted by buffer scene ).
	glBindTexture(GL_TEXTURE_2D,0);

#if defined(GL_EXT_framebuffer_object)
	const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();	
	pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,m_framebuffer);

	unsigned int target = T->target & 0xFFFF;
	pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
											GL_COLOR_ATTACHMENT0_EXT,
											target, 
											T->texname,
											T->getCurrentMipMapLevel());

	//	Render the display after texture is attached
	//	because rendering propoerties are applied now ( e.g. glClear )
	RAPTOR_HANDLE noDevice;
	CRaptorDisplay::glBindDisplay(noDevice);
	glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(cs.x,cs.y,cs.width,cs.height);
	glViewport(0,0,cs.width,cs.height); // Viewport is relative to window !!!

	glRender();

	glPopAttrib();
	CRaptorDisplay::glUnBindDisplay();

	pExtensions->glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
											GL_COLOR_ATTACHMENT0_EXT,
											target, 
											0,
											0);

	pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
#endif
}

void CRaptorRenderBufferDisplay::glResize(unsigned int sx,unsigned int sy,unsigned int ox,unsigned int oy)
{
    if ((sx == 0) || (sy == 0))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
		    							                "Raptor Render Buffer Display is requested a resize with a wrong dimension ( 0 ) !");
#endif
        return;
    }

#if defined(GL_EXT_framebuffer_object)
	int binding = 0;
	bool bindSelf = false;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&binding);
	if (binding != 0)
	{
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	}

	// How do I handle bound texture objects ?
	bindSelf = (binding == m_framebuffer);
	glDestroyBuffer();
	
    cs.x = ox;
    cs.y = oy;
    cs.width = sx;
    cs.height = sy;

    RAPTOR_HANDLE noDevice;
    glBindDisplay(noDevice);

	// Render buffers are recreated, now restore previous state.
	if (binding == 0)
		glUnBindDisplay();
	else if (!bindSelf)
	{
		const CRaptorExtensions *const pExtensions = Raptor::glGetExtensions();
		pExtensions->glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,binding);
	}
#endif

    CATCH_GL_ERROR
}

bool CRaptorRenderBufferDisplay::glGrab(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char* &data,unsigned int& size) const
{
	if (m_framebuffer == 0)
		return false;

#if defined(GL_EXT_framebuffer_object)
	GLint oldRead;
	glGetIntegerv(GL_READ_BUFFER,&oldRead);
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);

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

	glReadBuffer(oldRead);

	CATCH_GL_ERROR

	return true;
#else
		return false;
#endif
}

bool CRaptorRenderBufferDisplay::glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
										unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
										CRaptorDisplay *pDst) const
{
	bool res = false;

#if defined(GL_EXT_framebuffer_blit)
	if ((m_framebuffer != 0) && 
		(pDst->getId().isSubClassOf(CRaptorRenderBufferDisplay::CRaptorRenderBufferDisplayClassID::GetClassId())))
	{
		CRaptorRenderBufferDisplay *pDstBuffer = (CRaptorRenderBufferDisplay *)pDst;
		const CRaptorExtensions * const pExtensions = Raptor::glGetExtensions(); 

		GLint readBinding = 0;
		GLint drawBinding = 0;
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING_EXT,&readBinding);
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_EXT,&drawBinding);

		pExtensions->glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT,m_framebuffer);
		pExtensions->glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT,pDstBuffer->m_framebuffer);

		pExtensions->glBlitFrameBufferEXT(	xSrc,ySrc,xSrc+widthSrc,ySrc+heightSrc,
											xDst,yDst,xDst+widthDst,yDst+heightDst,
											GL_COLOR_BUFFER_BIT,GL_LINEAR);

		GLuint buffers = 0;
		if (m_depthbuffer != 0) 
			buffers = GL_DEPTH_BUFFER_BIT;
		if (m_stencilbuffer != 0)
			buffers |= GL_STENCIL_BUFFER_BIT;

		if (buffers != 0)
		{
			pExtensions->glBlitFrameBufferEXT(	xSrc,ySrc,xSrc+widthSrc,ySrc+heightSrc,
												xDst,yDst,xDst+widthDst,yDst+heightDst,
												buffers,GL_NEAREST);
		}

		pExtensions->glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT,readBinding);
		pExtensions->glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT,drawBinding);

		CATCH_GL_ERROR

		res = true;
	}
#endif

	return res;
}

bool CRaptorRenderBufferDisplay::glRender(void)
{
	CGeometryAllocator *allocatorG = CGeometryAllocator::GetInstance();
	CTexelAllocator *allocatorT = CTexelAllocator::GetInstance();

	// If memory locked, an unlock would be necessary to 
	//	lock for this display.
	allocatorG->glvkLockMemory(true);
	allocatorT->glvkLockMemory(true);

	C3DScene *pScene = getRootScene();
	pScene->glRender();

	allocatorG->glvkLockMemory(false);
	allocatorT->glvkLockMemory(false);

    return true;
}

bool CRaptorRenderBufferDisplay::checkBufferStatus(void) const
{
#if defined(GL_EXT_framebuffer_object)
	if (m_framebuffer == 0)
		return false;

	int buffer = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&buffer);

	if ((buffer != m_framebuffer) || (buffer == 0))
		return false;

	const CRaptorExtensions * const pExtensions = Raptor::glGetExtensions(); 
	GLenum status = pExtensions->glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

#ifndef RAPTOR_DEBUG_MODE_GENERATION
	string msg = "";   
	switch(status)
	{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			msg = "Incomplete attachment.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			msg = "Missing attachment.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			msg = "Incomplete dimensions.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			msg = "Incomplete formats.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			msg = "Incomplete draw buffer.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			msg = "Incomplete read buffer.";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			msg = "Incomplete unsupported.";
			break;
#if defined(GL_EXT_framebuffer_multisample)
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
			msg = "Inconsistent number of samples.";
			break;
#endif
	}
           
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
		RAPTOR_ERROR(	Global::COpenGLClassID::GetClassId(),
						"Raptor Render Buffer Display failed to initialize: " + msg);
#endif

	return (status == GL_FRAMEBUFFER_COMPLETE_EXT);
#else
	return false;
#endif
}
