#include "stdafx.h"

#if !defined(AFX_SERVERIMAGERENDERER_H__C9720F3B_1B29_482C_84C8_1A990CEC0EBD__INCLUDED_)
	#include "ServerImageRenderer.h"
#endif

#if !defined(AFX_TEXTUREFACTORY_H__1B470EC4_4B68_11D3_9142_9A502CBADC6B__INCLUDED_)
	#include "GLHierarchy/TextureFactory.h"
#endif

#if !defined(AFX_RAPTORINSTANCE_H__D5297BB6_098A_4082_96AA_36A78E76F18C__INCLUDED_)
	#include "RaptorNetwork/RaptorNetwork.h"
#endif


CServerImageRenderer::CServerImageRenderer(size_t width,size_t height)
	:CObject3D(CObject3D::CObject3DClassID::GetClassId(),"SERVER_IMAGE"),
	m_pImage(NULL),m_serverWidth(width),m_serverHeight(height)
{
	//! awfull trick: make the object visible:
	//! the bbox is shifted to make occlusion visibility effective,
	//! the scene viewpoint is unmodified at origin
	setBoundingBox(	GL_COORD_VERTEX(-1.0f,-1.0f,-10.0f,1.0f),
					GL_COORD_VERTEX(1.0f,1.0f,-9.0f,1.0f));
}

CServerImageRenderer::~CServerImageRenderer(void)
{
}

bool CServerImageRenderer::hasImagesAvailable(void) const
{
	return (m_pImageDatas.size() > 0);
}

void CServerImageRenderer::glClipRender(void)
{
	glRender();
}

void CServerImageRenderer::glRender(void)
{
	if (m_pImage == NULL)
		return;
		
	{
		m_pImage->glvkRender();

		unsigned char* src = NULL;

		{
			CRaptorLock lock(tMutex);
			if (m_pImageDatas.size())
			{
				src = m_pImageDatas[0];
				m_pImageDatas.erase(m_pImageDatas.begin());
				/*
#ifdef GL_EXT_bgra
        GLuint mode = GL_BGR_EXT;
#else
        GLuint mode = GL_RGBA;
#endif
		*/
			}
		}

		if (src != NULL)
		{
			GLuint mode = GL_RGB;
			glTexSubImage2D(GL_TEXTURE_2D,
				    0,  // level
				    0,  // xoffset
				    0,  // yoffset
				    m_serverWidth,
				    m_serverHeight,
				    mode,
				    GL_UNSIGNED_BYTE,
				    src+sizeof(CRaptorNetwork::BLOC_HEADER));

			delete [] src;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
		
	glCallList(drawBuffer.handle());
}

void CServerImageRenderer::setImageData(unsigned char *in)
{
	CRaptorLock lock(tMutex);
	
	m_pImageDatas.push_back(in);
}

void CServerImageRenderer::glInitImage()
{
	drawBuffer.handle(glGenLists(1));
    glNewList(drawBuffer.handle(),GL_COMPILE);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);glVertex4f(-1.0,-1.0,-1.0f,1.0f);
            glTexCoord2f(1.0f,0.0f);glVertex4f(1.0,-1.0,-1.0f,1.0f);
            glTexCoord2f(1.0f,1.0f);glVertex4f(1.0,1.0,-1.0f,1.0f);
            glTexCoord2f(0.0f,1.0f);glVertex4f(-1.0,1.0,-1.0f,1.0f);
        glEnd();
    glEndList();

	CTextureFactory &factory = CTextureFactory::getDefaultFactory();
	m_pImage = factory.glCreateTexture(	ITextureObject::CGL_COLOR24, //_ALPHA,
										ITextureObject::CGL_BILINEAR);

	factory.glResizeTexture(m_pImage,m_serverWidth,m_serverHeight);
}
