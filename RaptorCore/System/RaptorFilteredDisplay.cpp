// RaptorFilteredDisplay.cpp: implementation of the CRaptorFilteredDisplay class.
//
//////////////////////////////////////////////////////////////////////
#include "Subsys/CodeGeneration.h"

#if !defined(AFX_RAPTOR_H__C59035E1_1560_40EC_A0B1_4867C505D93A__INCLUDED_)
	#include "Raptor.h"
#endif
#if !defined(AFX_RAPTORCONFIG_H__29B753B8_17DE_44DF_A4D2_9D19C5AC53D5__INCLUDED_)
    #include "RaptorConfig.h"
#endif
#if !defined(AFX_RAPTORERRORMANAGER_H__FA5A36CD_56BC_4AA1_A5F4_451734AD395E__INCLUDED_)
    #include "RaptorErrorManager.h"
#endif
#if !defined(AFX_RAPTORFILTEREDDISPLAY_H__CF464057_8489_43FF_954F_52ECFD722D13__INCLUDED_)
	#include "RaptorFilteredDisplay.h"
#endif
#if !defined(AFX_RAPTORBUFFERDISPLAY_H__DBB1E3EC_EAF3_4EBA_AAB4_C51FA49C5884__INCLUDED_)
	#include "RaptorBufferDisplay.h"
#endif
#if !defined(AFX_RAPTORDISPLAYFILTER_H__805D8523_96EA_427B_ABEC_C39EE1BC094C__INCLUDED_)
    #include "RaptorDisplayFilter.h"
#endif
#ifndef __GLOBAL_H__
    #include "Global.h"
#endif
#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
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
#if !defined(AFX_RENDERINGPROPERTIES_H__634BCF2B_84B4_47F2_B460_D7FDC0F3B698__INCLUDED_)
	#include "GLHierarchy/RenderingProperties.h"
#endif
#if !defined(AFX_3DENGINE_H__DB24F018_80B9_11D3_97C1_FC2841000000__INCLUDED_)
	#include "Engine/3DEngine.h"
#endif
#if !defined(AFX_RAPTOREXTENSIONS_H__E5B5A1D9_60F8_4E20_B4E1_8E5A9CB7E0EB__INCLUDED_)
  #include "System/RaptorExtensions.h"
#endif
#if !defined(AFX_GEOMETRYALLOCATOR_H__802B3C7A_43F7_46B2_A79E_DDDC9012D371__INCLUDED_)
	#include "Subsys/GeometryAllocator.h"
#endif
#if !defined(AFX_TEXELALLOCATOR_H__7C48808C_E838_4BE3_8B0E_286428BB7CF8__INCLUDED_)
	#include "Subsys/TexelAllocator.h"
#endif
#if !defined(AFX_SHADER_H__4D405EC2_7151_465D_86B6_1CA99B906777__INCLUDED_)
	#include "GLHierarchy/Shader.h"
#endif
#if !defined(AFX_ANIMATORSTREAM_H__3D03D0B9_A350_4226_8AB4_BABDD53D68B6__INCLUDED_)
	#include "Subsys/AnimatorStream.h"
#endif

RAPTOR_NAMESPACE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaptorFilteredDisplay::CRaptorFilteredDisplay(const CRaptorDisplayConfig& pcs)
	:CRaptorScreenDisplay(pcs),m_pDisplay(NULL),m_pFSAADisplay(NULL),
	m_bBufferBound(false),m_pImageSet(NULL)
{
    filter_cs = pcs;
    //  float pixels are not supported in a render to window context :-(
    cs.display_mode = (filter_cs.display_mode & (CGL_RGB|CGL_RGBA)) |
                      (filter_cs.display_mode & CGL_DOUBLE) |
                      (filter_cs.display_mode & (CGL_SOFTWARE|CGL_GENERIC|CGL_HARDWARE));

    //  Impose texture rendering and single buffer. Disable multisample rendering
    if ((filter_cs.display_mode & CGL_RENDER_TEXTURE) != CGL_RENDER_TEXTURE)
        filter_cs.display_mode |= CGL_RENDER_TEXTURE;
    if ((filter_cs.display_mode & CGL_RENDER_DEPTHTEXTURE) != CGL_RENDER_DEPTHTEXTURE)
        filter_cs.display_mode |= CGL_RENDER_DEPTHTEXTURE;
    if ((filter_cs.display_mode & CGL_DOUBLE) == CGL_DOUBLE)
        filter_cs.display_mode &= ~CGL_DOUBLE;
#if !defined(GL_EXT_framebuffer_multisample) && !defined(GL_EXT_framebuffer_blit)
    if ((filter_cs.display_mode & CGL_ANTIALIAS) == CGL_ANTIALIAS)
        filter_cs.display_mode &= ~CGL_ANTIALIAS;
#endif

	// Impose a precise default float format if not specified.
	if ((filter_cs.display_mode & CGL_FLOAT) == CGL_FLOAT)
    {
        if (((filter_cs.display_mode & CGL_FLOAT_16) != CGL_FLOAT_16) &&
			(filter_cs.display_mode & CGL_FLOAT_32) != CGL_FLOAT_32)
			filter_cs.display_mode |= CGL_FLOAT_16;
	}

    filter_cs.display_mode &= ~CGL_RENDER_FILTERED;

	//	Check texture rendering capabilities:
	//	nVidia does not support separate Depth + Stencil framebuffers, except
	//	in combination with EXT_packed_depth_stencil or pixel buffers
#if !defined(GL_EXT_packed_depth_stencil)
	if (((filter_cs.display_mode & CGL_STENCIL) == CGL_STENCIL) &&
		((filter_cs.display_mode & CGL_DEPTH) == CGL_DEPTH) &&
		((filter_cs.display_mode & CGL_RENDER_BUFFER) == CGL_RENDER_BUFFER))
		filter_cs.display_mode  &= ~CGL_RENDER_BUFFER;
#endif

	CViewPoint *vp = CRaptorDisplay::getViewPoint();
	vp->setViewVolume(-1.0f,1.0f,-1.0f,1.0f,1.0f,100.0f,CViewPoint::ORTHOGRAPHIC);

    CRenderingProperties *rp = CRaptorScreenDisplay::getRenderingProperties();
    rp->setTexturing(CRenderingProperties::ENABLE);
	rp->setCullFace(CRenderingProperties::DISABLE);
	rp->setDepthTest(CRenderingProperties::DISABLE);
	rp->clear(CGL_NULL);
}

CRaptorFilteredDisplay::~CRaptorFilteredDisplay()
{
	if (m_pImageSet != NULL)
	{
		m_pImageSet->unregisterDestruction(this);
		delete m_pImageSet;
	}

	if (m_pFSAADisplay != NULL)
	{
		m_pFSAADisplay->unregisterDestruction(this);
		if (m_bBufferBound)
			m_pFSAADisplay->glUnBindDisplay();
		Raptor::glDestroyDisplay(m_pFSAADisplay);
	}

	if (m_pDisplay != NULL)
	{
		m_pDisplay->unregisterDestruction(this);
		if (m_bBufferBound)
			m_pDisplay->glUnBindDisplay();
		Raptor::glDestroyDisplay(m_pDisplay);
	}

    if (m_pFilters.size() > 0)
    {
        vector<CRaptorDisplayFilter*>::const_iterator itr = m_pFilters.begin();
        while (itr != m_pFilters.end())
        {
            CRaptorDisplayFilter *filter = (*itr++);
			filter->releaseReference();
        }
    }
}

void CRaptorFilteredDisplay::glReleaseResources(void)
{
    //!  Should place destructor body here,
    //! it would be better, allowing to catch errors
    if (m_pFilters.size() > 0)
    {
        vector<CRaptorDisplayFilter*>::const_iterator itr = m_pFilters.begin();
        while (itr != m_pFilters.end())
        {
            CRaptorDisplayFilter *filter = (*itr++);
			filter->releaseReference();
        }
    }
    m_pFilters.clear();

    CRaptorScreenDisplay::glReleaseResources();
}

void CRaptorFilteredDisplay::unLink(const CPersistence* obj)
{
    if (static_cast<CPersistence*>(m_pDisplay) == obj)
        m_pDisplay = NULL;
	else if (static_cast<CPersistence*>(m_pFSAADisplay) == obj)
        m_pFSAADisplay = NULL;
	else if (static_cast<CPersistence*>(m_pImageSet) == obj)
        m_pImageSet = NULL;
    else
        CRaptorScreenDisplay::unLink(obj);
}


bool CRaptorFilteredDisplay::glQueryStatus(CRaptorDisplayConfig &state,unsigned long query) const
{
    state.x = filter_cs.x;
    state.y = filter_cs.y;
    state.width = filter_cs.width;
    state.height = filter_cs.height;
    state.caption = filter_cs.caption;
    state.refresh_rate = filter_cs.refresh_rate;
    state.display_mode = filter_cs.display_mode;
    state.status_bar = filter_cs.status_bar;
	state.draw_logo = filter_cs.draw_logo;

    return CRaptorDisplay::glQueryStatus(state,query);
}

CRenderingProperties *CRaptorFilteredDisplay::getRenderingProperties(void) const
{
    if (m_pDisplay != NULL)
    {
		if (m_pFSAADisplay != NULL)
			return m_pFSAADisplay->getRenderingProperties();
		else
			return m_pDisplay->getRenderingProperties();
    }
    else
        return NULL;
}

bool CRaptorFilteredDisplay::glCreateRenderDisplay(void)
{
    if (m_pDisplay == NULL)
	{
		// Then create buffered rendering display
		CRaptorDisplayConfig rda = Global::GetInstance().getDefaultConfig();
        const CRaptorConfig& config = Global::GetInstance().getConfig();

		rda.width = cs.width * config.getFilterSizeFactor();
		rda.height = cs.height * config.getFilterSizeFactor();
		rda.caption = "Raptor Filtered Buffer";
		rda.refresh_rate.fps = CGL_MAXREFRESHRATE;

        //  prepare createStruct for buffers :
        //  render to texture for window system buffers may not be supported,
		//	try to use textures attached to application buffers instead
		if (!Raptor::glIsExtensionSupported("WGL_ARB_render_texture"))
		{
			if (Raptor::glIsExtensionSupported("GL_EXT_framebuffer_object"))
				filter_cs.display_mode |= CGL_RENDER_BUFFER;
			else
				return false;
		}

		rda.display_mode = filter_cs.display_mode;

		if ((rda.display_mode & CGL_ANTIALIAS) == CGL_ANTIALIAS)
		{
			//	remove render to texture, we will use FSAA buffer only
			CRaptorDisplayConfig FSAArda = rda;
			FSAArda.display_mode &= ~CGL_RENDER_TEXTURE;

			m_pFSAADisplay = Raptor::glCreateDisplay(FSAArda);
			if (m_pFSAADisplay == NULL)
				return false;
			m_pFSAADisplay->registerDestruction(this);

			//	'Real' display will no more need antialiasing.
			rda.display_mode &= ~CGL_ANTIALIAS;
		}

		m_pDisplay = Raptor::glCreateDisplay(rda);
		if (m_pDisplay == NULL)
			return false;
        m_pDisplay->registerDestruction(this);

		CTextureFactory &f = CTextureFactory::getDefaultFactory();
		m_pImageSet = new CTextureSet();
		m_pImageSet->registerDestruction(this);

		//	Create the texture set for render buffer targets
		//	Render to texture and render to depth texture are imposed.
		if ((filter_cs.display_mode & CGL_RENDER_BUFFER) == CGL_RENDER_BUFFER)
		{
			CTextureObject::TEXEL_TYPE texelType = CTextureObject::CGL_COLOR24_ALPHA;
			if ((filter_cs.display_mode & CGL_FLOAT) == CGL_FLOAT)
				texelType = CTextureObject::CGL_COLOR_FLOAT16_ALPHA;

			CTextureObject *T = f.glCreateTexture(	texelType,
													CTextureObject::CGL_OPAQUE,
													CTextureObject::CGL_BILINEAR);
			f.glResizeTexture(T,rda.width,rda.height);
			T->glUpdateClamping(CTextureObject::CGL_EDGECLAMP);
			m_pImageSet->addTexture(T);

			//!	Do not use bilinear filtering for depth textures :
			//!	interpolators will use a lower precision ( 8 bits )
			//!	Bilinear filtering is necessary only for PCF shadow maps
#if defined(GL_EXT_packed_depth_stencil)
			if (((filter_cs.display_mode & CGL_STENCIL) == CGL_STENCIL) &&
				((filter_cs.display_mode & CGL_DEPTH) == CGL_DEPTH))
				T = f.glCreateTexture(	CTextureObject::CGL_DEPTH24_STENCIL8,
										CTextureObject::CGL_OPAQUE,
										CTextureObject::CGL_UNFILTERED);
			else
#endif

				T = f.glCreateTexture(	CTextureObject::CGL_DEPTH24,
										CTextureObject::CGL_OPAQUE,
										CTextureObject::CGL_UNFILTERED);

			f.glResizeTexture(T,rda.width,rda.height);
			m_pImageSet->addTexture(T);

			m_pDisplay->glBindDisplay(*m_pImageSet);

			RAPTOR_HANDLE noDevice;
            if (!m_pDisplay->glBindDisplay(noDevice))
				return false;
			m_pDisplay->glUnBindDisplay();
		}
		else
		{
			//  - Create the rendering display texture, which is also the main subdisplay when there are no filters,
			//  - a spare display texture, which is used to alternate rendering with the main display when there are
			//  several filters chained ( adds ability to use the previous render in a TMU source )
			CTextureObject *T = f.glCreateDynamicTexture(	CTextureObject::CGL_COLOR24_ALPHA,
															CTextureObject::CGL_OPAQUE,
															CTextureObject::CGL_BILINEAR,
															m_pDisplay);
			m_pImageSet->addTexture(T);
		}

        if ((filter_cs.display_mode & CGL_FLOAT) == CGL_FLOAT)
        {
#if defined(GL_ARB_color_buffer_float) || defined(WGL_ATI_pixel_format_float)
            if (Raptor::glIsExtensionSupported("GL_ARB_color_buffer_float") ||
				Raptor::glIsExtensionSupported("WGL_ATI_pixel_format_float"))
            {
				CRenderingProperties* props = m_pDisplay->getRenderingProperties();
				props->setFloatClamping(CRenderingProperties::DISABLE);
				if (m_pFSAADisplay != NULL)
				{
					props = m_pFSAADisplay->getRenderingProperties();
					props->setFloatClamping(CRenderingProperties::DISABLE);
				}
            }
			else 
#endif
            {
                Raptor::GetErrorManager()->generateRaptorError( Global::COpenGLClassID::GetClassId(),
                                                                CRaptorErrorManager::RAPTOR_ERROR,
		    							                        "Float Display Buffer is not supported !");
            }
        }


        drawBuffer.handle = glGenLists(1);
        glNewList(drawBuffer.handle,GL_COMPILE);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f,0.0f);glVertex4f(-1.0,-1.0,-1.0f,1.0f);
                glTexCoord2f(1.0f,0.0f);glVertex4f(1.0,-1.0,-1.0f,1.0f);
                glTexCoord2f(1.0f,1.0f);glVertex4f(1.0,1.0,-1.0f,1.0f);
                glTexCoord2f(0.0f,1.0f);glVertex4f(-1.0,1.0,-1.0f,1.0f);
            glEnd();
        glEndList();
	}

    CATCH_GL_ERROR

    return true;
}

bool CRaptorFilteredDisplay::glBindDisplay(const RAPTOR_HANDLE& device)
{
	if (device.hClass == CShader::CShaderClassID::GetClassId().ID())
    {
        if (device.handle != 0)
        {
            CRaptorDisplayFilter *shader = static_cast<CRaptorDisplayFilter*>((void*)device.handle);
			bool useRenderBuffers = (filter_cs.display_mode & CGL_RENDER_BUFFER) == CGL_RENDER_BUFFER;

			if (useRenderBuffers)
				shader->setFilterModel(CRaptorDisplayFilter::RENDER_BUFFER);
            
            if (m_pFilters.size() == 0)
            {
				if (useRenderBuffers)
				{
					shader->setColorInput(m_pImageSet->getTexture(0));
					shader->setDepthInput(m_pImageSet->getTexture(1));
				}
				else
				{
					shader->setColorSource(m_pDisplay);
					shader->setDepthSource(m_pDisplay);		
				}
            }
            else
            {
                CRaptorDisplayFilter *lastFilter = m_pFilters.back();
				shader->setPreviousFilter(lastFilter);
            }
            if (!shader->glInitFilter())
            {
				CATCH_GL_ERROR

#ifdef RAPTOR_DEBUG_MODE_GENERATION
                Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
                                                                CRaptorErrorManager::RAPTOR_ERROR,
		    							                        "Raptor Filter Display failed to initialize filter !");
#endif
				return false;
            }
            else
			{
				CATCH_GL_ERROR
                m_pFilters.push_back(shader);
				shader->addReference();
				return true;
			}
        }
        else
            return false;
    }

    //  prepare createStruct for screen :
    // just keep colors ( but remove float : unsupported ) and hardware
	if (CRaptorScreenDisplay::glBindDisplay(device))
	{
		if (!glCreateRenderDisplay())
            return false;

		m_bBufferBound = true;
		RAPTOR_HANDLE noDevice;

		if ((filter_cs.display_mode & CGL_ANTIALIAS) == CGL_ANTIALIAS)
			return m_pFSAADisplay->glBindDisplay(noDevice);
		else
			return m_pDisplay->glBindDisplay(noDevice);
	}
	else
		return false;
}


bool CRaptorFilteredDisplay::glUnBindDisplay(void)
{
	if (m_pDisplay == NULL)
		return false;
	else
	{
		if (m_bBufferBound)
		{
			if (m_pFSAADisplay != NULL)
			{
				m_pFSAADisplay->glUnBindDisplay();
				m_pFSAADisplay->glBlit(	0,0,filter_cs.width,filter_cs.height,
										0,0,filter_cs.width,filter_cs.height,
										m_pDisplay);
			}
			else
				m_pDisplay->glUnBindDisplay();
		}
		m_bBufferBound = false;
		return CRaptorScreenDisplay::glUnBindDisplay();
	}
}

void CRaptorFilteredDisplay::glResize(unsigned int sx,unsigned int sy,unsigned int ox, unsigned int oy)
{
    if ((sx == 0) || (sy == 0))
    {
#ifdef RAPTOR_DEBUG_MODE_GENERATION
        Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_ERROR,
														"Raptor Display is requested a resize with a wrong dimension ( 0 ) !");
#endif
        return;
    }

	//	Unset render buffer to apply size to the viewport
	//	and not to a P-buffer, which is irrelevant
	if (m_bBufferBound)
	{
		if (m_pFSAADisplay != NULL)
			m_pFSAADisplay->glUnBindDisplay();
		else
			m_pDisplay->glUnBindDisplay();
	}

	glViewport(ox,oy,sx,sy);
	C3DEngine::Get3DEngine()->setClip(ox,oy,sx,sy);

	CViewPoint *pVp = CRaptorDisplay::getViewPoint();
	pVp->glRenderViewPointModel();

    //m_pDisplay->glResize(sx,sy,ox,oy);

	if (m_bBufferBound)
	{
		CViewPoint *vp = NULL;
		RAPTOR_HANDLE noDevice;

		if (m_pFSAADisplay != NULL)
		{
			m_pFSAADisplay->glBindDisplay(noDevice);
			vp = m_pFSAADisplay->getViewPoint();
		}
		else
		{
			m_pDisplay->glBindDisplay(noDevice);
			vp = m_pDisplay->getViewPoint();
		}

		vp->glRenderViewPointModel();
	}

    CATCH_GL_ERROR
}

void CRaptorFilteredDisplay::glRenderScene(void)
{
	if (m_bBufferBound)
	{
		m_bBufferBound = false;
		if (m_pFSAADisplay != NULL)
		{
			m_pFSAADisplay->glUnBindDisplay();
			m_pFSAADisplay->glBlit(	0,0,filter_cs.width,filter_cs.height,
									0,0,filter_cs.width,filter_cs.height,
									m_pDisplay);
		}
        else if (m_pDisplay != NULL)
		    m_pDisplay->glUnBindDisplay();
	}

    if (m_pFilters.size() > 0)
    {
		bool lastfilter = false;
        vector<CRaptorDisplayFilter*>::reverse_iterator ritr = m_pFilters.rbegin();
		while (ritr != m_pFilters.rend())
        {
			CRaptorDisplayFilter *filter = (*ritr++);
			filter->enableBufferOutput(lastfilter);
			if (!lastfilter)
				lastfilter = filter->isEnabled();
		}

		vector<CRaptorDisplayFilter*>::const_iterator itr = m_pFilters.begin();
        while (itr != m_pFilters.end())
        {
            CRaptorDisplayFilter *filter = (*itr++);

            //  enable/disable state must be managed by the filter
			if (filter->isEnabled())
				filter->glRender();
        }
    }
    else
    {
		CTextureObject *T = m_pImageSet->getTexture(0);
        T->glRender();

        if (drawBuffer.handle > 0)
            glCallList(drawBuffer.handle);

		glBindTexture(GL_TEXTURE_2D,0);
    }

    CATCH_GL_ERROR
}


bool CRaptorFilteredDisplay::glRender(void)
{
	if (m_pDisplay == NULL)
		return false;

	CTimeObject::markTime(this+1);

    //! Memory will be locked also in CRaptorScreenDisplay, but it is harmless,
    //! once it is locked, it is kept unchanged.
    m_pGAllocator->glLockMemory(true);
	m_pTAllocator->glLockMemory(true);

	CRaptorScreenDisplay::glRenderScene();

    //! glRender will unlock memory.
	bool render = CRaptorScreenDisplay::glRender();

	rtime = CTimeObject::deltaMarkTime(this+1);

	return render;
}



void CRaptorFilteredDisplay::setViewPoint(CViewPoint *viewPoint)
{
	if (m_pDisplay != NULL)
	{
		if (m_bBufferBound)
		{
			if (m_pFSAADisplay != NULL)
				m_pFSAADisplay->setViewPoint(viewPoint);
			else
				m_pDisplay->setViewPoint(viewPoint);
		}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
		else
		{
			Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
															CRaptorErrorManager::RAPTOR_WARNING,
															"Raptor Buffer Display cannot setViewPoint because buffer not bound!");
		}
#endif
	}
#ifdef RAPTOR_DEBUG_MODE_GENERATION
	else
	{
		Raptor::GetErrorManager()->generateRaptorError(	Global::COpenGLClassID::GetClassId(),
														CRaptorErrorManager::RAPTOR_WARNING,
														"Raptor Buffer Display cannot setViewPoint because buffer not initialised!");
	}
#endif
}

CViewPoint *const CRaptorFilteredDisplay::getViewPoint(void) const
{
	if (m_pDisplay == NULL)
		return NULL;

	if (m_pFSAADisplay != NULL)
		return m_pFSAADisplay->getViewPoint();
	else
		return m_pDisplay->getViewPoint();
}

bool CRaptorFilteredDisplay::glBlit(unsigned int xSrc, unsigned int ySrc, unsigned int widthSrc, unsigned int heightSrc,
									unsigned int xDst, unsigned int yDst, unsigned int widthDst, unsigned int heightDst,
									CRaptorDisplay *pDst) const
{
	bool res = false;

	return res;
}

